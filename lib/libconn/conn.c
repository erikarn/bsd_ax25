#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <sys/queue.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <event2/event.h>
#include <event2/dns.h>

#include "util.h"
#include "eb.h"
#include "buf.h"
#include "buf_list.h"
#include "conn.h"

struct conn *
conn_create(struct ebase *eb)
{
	struct conn *k;

	k = calloc(1, sizeof(*k));
	if (k == NULL) {
		warn("%s: calloc", __func__);
		return (NULL);
	}

	k->write_q = buf_list_create();
	if (k->write_q == NULL) {
		free(k);
		return (NULL);
	}

	k->eb = eb;
	k->fd = -1;

	return (k);
}

int
conn_close(struct conn *k)
{

	if (k->fd == -1)
		return (0);

	close(k->fd);
	k->fd = -1;
	k->is_setup = 0;
	k->is_connecting = 0;
	k->is_connected = 0;
	k->is_dns_done = 0;
	event_del(k->read_ev);
	event_del(k->write_ev);
	event_free(k->read_ev);
	event_free(k->write_ev);

	if (k->dns_req != NULL) {
		evdns_getaddrinfo_cancel(k->dns_req);
		k->dns_req = NULL;
	}

	if (k->cb.close_cb != NULL) {
		k->cb.close_cb(k, k->cb.cbdata, 0);
	}

	conn_write_flush(k);

	return (0);
}

int
conn_write_flush(struct conn *k)
{
	struct buf *b;

	while ((b = buf_list_pop(k->write_q)) != NULL) {
		buf_free(b);
	}

	return (0);
}

static void
conn_connect_complete(struct conn *k)
{

	k->is_connecting = 0;
	k->is_connected = 1;
	fprintf(stderr, "%s: ready!\n", __func__);

	if (k->cb.connect_cb != NULL) {
		k->cb.connect_cb(k, k->cb.cbdata, CONN_CONNECT_ERR_OK, 0);
	}

	/* If the caller started us paused then don't start reading */
	if (k->is_paused == 0)
		event_add(k->read_ev, NULL);
}

static void
conn_connect_error(struct conn *k, conn_connect_err_t connerr, int xerrno)
{
	k->is_connecting = 0;
	k->is_connected = 0;
	fprintf(stderr, "%s: error connecting!\n", __func__);

	if (k->cb.connect_cb != NULL) {
		k->cb.connect_cb(k, k->cb.cbdata, connerr, xerrno);
	}
}

static void
conn_read_cb(evutil_socket_t fd, short what, void *arg)
{
	struct conn *k = arg;
	uint8_t buf[1024];
	int ret;

	fprintf(stderr, "%s: called!\n", __func__);

	/* XXX loop */
	/* XXX zero-copy read todo? */

	ret = read(k->fd, buf, 1024);
	if (ret < 0) {
		if (errno == EWOULDBLOCK)
			return;
		if (errno == EINTR)
			return;
		fprintf(stderr, "%s: read failed; errno=%d\n", __func__, errno);
		/* notify the caller of a problem, delete the read readiness */
		if (k->cb.read_cb != NULL) {
			k->cb.read_cb(k, k->cb.cbdata, NULL, -1, errno);
			event_del(k->read_ev);
			return;
		}
		return;
	}
	if (ret == 0) {
		/* notify the caller of a problem, delete the read readiness */
		if (k->cb.read_cb != NULL) {
			k->cb.read_cb(k, k->cb.cbdata, NULL, 0, errno);
			event_del(k->read_ev);
			return;
		}
		return;
	}

	/* Read some data, return appropriate */
	if (k->cb.read_cb != NULL) {
		k->cb.read_cb(k, k->cb.cbdata, buf, ret, 0);
	}
}

static void
conn_write_cb(evutil_socket_t fd, short what, void *arg)
{
	struct conn *k = arg;
	int optarg, ret;
	socklen_t len;
	struct buf *b;

	fprintf(stderr, "%s: called!\n", __func__);

	if (k->is_setup == 0)
		return;
	/* Check if we're connecting or not */
	if (k->is_connecting == 1) {
		len = sizeof(optarg);
		ret = getsockopt(k->fd, SOL_SOCKET, SO_ERROR, &optarg, &len);
		if (ret != 0 && errno == EINTR) {
			fprintf(stderr, "%s: getsockopt; eintr\n", __func__);
			event_add(k->write_ev, NULL);
			return;
		}
		if (ret != 0) {
			/* Error out if we can't fetch the socket state */
			warn("%s: getsockopt", __func__);
			conn_connect_error(k, CONN_CONNECT_ERR_CONN_FAILURE, errno);
			return;
		}
		if (ret == 0 && optarg == EINTR) {
			fprintf(stderr, "%s: connect; eintr\n", __func__);
			event_add(k->write_ev, NULL);
			return;
		}
		if (ret == 0 && optarg == 0) {
			conn_connect_complete(k);
			/* XXX TODO: check if we have pending writeq? */
			return;
		}
		/* issue? */
		fprintf(stderr, "%s: connect; failed; errno=%d\n", __func__, optarg);
		conn_connect_error(k, CONN_CONNECT_ERR_CONN_REFUSED, optarg);
		return;
	}

	/* ok, if we get here, we can write normal data */
	if (k->is_connected == 1) {
		while ((b = buf_list_pop(k->write_q)) != NULL) {
			ret = write(k->fd, b->buf + b->write_offset, b->len - b->write_offset);
			if (ret == 0) {
				/* EOF; notify caller */
				k->cb.write_cb(k, k->cb.cbdata, b,
				    CONN_WRITE_ERR_EOF, 0);
				break;
			}
			/* Handle any other failure as temporary; should revisit */
			if (ret < 0) {
				buf_list_push(k->write_q, b);
				break;
			}

			/* Finished this buf? */
			if (ret + b->write_offset >= b->len) {
				if (ret + b->write_offset > b->len) {
					fprintf(stderr,
					     "%s: ERR: wrote too much?\n",
					     __func__);
				}
				k->cb.write_cb(k, k->cb.cbdata, b,
				    CONN_WRITE_ERR_OK, 0);
				continue;
			}

			/* Partial buffer write */
			b->write_offset += ret;
			/* Put it back at head of queue for more writing */
			buf_list_push(k->write_q, b);
		}

		/* Any more data? Not EOF? Requeue */
		if (ret != 0 && buf_list_isempty(k->write_q) == 0)
			event_add(k->write_ev, NULL);
	}
}

int
conn_set_lcl(struct conn *k, const struct sockaddr_storage *s)
{

	memcpy(&k->lcl, s, sizeof(struct sockaddr_storage));
	return (0);
}

int
conn_set_peer(struct conn *k, const struct sockaddr_storage *s)
{

	memcpy(&k->peer, s, sizeof(struct sockaddr_storage));
	k->is_dst_peer_set = 1;
	return (0);
}

int
conn_set_peer_host(struct conn *k, const char *host, int port)
{

	if (k->dst_host.host != NULL)
		free(k->dst_host.host);
	k->dst_host.host = strdup(host);
	k->dst_host.port = port;

	k->is_dst_peer_set = 0;

	return (0);
}

/*
 * Do the socket setup.  We have enough of the information required
 * to setup the socket, so do so.
 */
static int
conn_setup(struct conn *k)
{
	int fd;
	int f;

	fprintf(stderr, "%s: called!\n", __func__);

	if (k->fd != -1) {
		conn_close(k);
	}

	/*
	 * Get the address family from the peer.
	 */
	if (k->peer.ss_family == AF_INET) {
		f = PF_INET;
	} else if (k->peer.ss_family == AF_INET6) {
		f = PF_INET6;
	} else {
		fprintf(stderr, "%s: unknown address family (%d)\n",
		    __func__,
		    k->peer.ss_family);
		return (-1);
	}

	fd = socket(f, SOCK_STREAM, 0);
	if (fd < 0) {
		warn("%s: socket", __func__);
		return (-1);
	}
	evutil_make_socket_nonblocking(fd);

	k->fd = fd;
	k->is_setup = 1;
	k->is_connecting = 0;
	k->is_connected = 0;

	k->read_ev = event_new(k->eb->ebase, k->fd, EV_READ | EV_PERSIST, conn_read_cb, k);
	k->write_ev = event_new(k->eb->ebase, k->fd, EV_WRITE, conn_write_cb, k);

	return (0);
}

/*
 * All of the socket details are available; begin the connection process.
 */
static int
conn_connect_connect(struct conn *k)
{
	int ret;

	fprintf(stderr, "%s: called; beginning things!\n", __func__);
	k->is_connecting = 1;
	k->is_connected = 0;

#if 0
	ret = bind(k->fd, (void *) &k->lcl, sockaddr_len(&k->lcl));
	if (ret != 0) {
		warn("%s: bind", __func__);
		return (-1);
	}
#endif

	/* Begin the connect; handle the case where it completes immediately */
	ret = connect(k->fd, (void *) &k->peer, sockaddr_len(&k->peer));
	if (ret == 0) {
		/* Finished */
		/* XXX TODO: should defer this so it doesn't behave re-entrant! */
		conn_connect_complete(k);
		return (0);
	}

	if (ret < 0 && errno == EINPROGRESS) {
		fprintf(stderr, "%s: in progress!\n", __func__);
		event_add(k->write_ev, NULL);
		k->is_connecting = 1;
		return (0);
	}

	/* We failed; close up and schedule a notification */
	warn("%s: connect", __func__);
	close(k->fd);
	k->fd = -1;
	conn_connect_error(k, CONN_CONNECT_ERR_CONN_CONNECT_FAILURE, 0);
	return (-1);
}

static void
conn_evdns_complete_callback(int errcode, struct evutil_addrinfo *addr, void *ptr)
{
	struct conn *k = ptr;
	struct evutil_addrinfo *ai;

	/* We no longer need a reference to this */
	k->dns_req = NULL;

	fprintf(stderr, "%s: called\n", __func__);
	if (errcode) {
		fprintf(stderr, "%s: %s -> %s\n",
		    __func__,
		    k->dst_host.host,
		    evutil_gai_strerror(errcode));
		conn_connect_error(k, CONN_CONNECT_ERR_DNS_FAIL, 0);
		return;
	}

	/*
	 * ok, so for now we're just going to extract the first entry
	 * and make it the connect host.
	 */
	ai = addr;
	if (ai == NULL) {
		/* No entries available */
		fprintf(stderr, "%s: no entries available\n", __func__);
		conn_connect_error(k, CONN_CONNECT_ERR_DNS_FAIL, 0);
		return;
	}

	/*
	 * Later on I'll include the socket hint to include the port.
	 * However for now I'll just pass it in..
	 */
	sockaddr_copy(&k->peer, (void *) ai->ai_addr);
	sockaddr_set_port(&k->peer, k->dst_host.port);

	/* Free the list */
	evutil_freeaddrinfo(addr);

	/* Now we can schedule a setup and connect */
	k->is_dns_done = 1;

	if (conn_setup(k) != 0) {
		fprintf(stderr, "%s: conn_setup failed\n", __func__);
		conn_connect_error(k, CONN_CONNCET_ERR_SETUP_FAIL, 0);
		return;
	}
	if (conn_connect_connect(k) != 0) {
		conn_connect_error(k, CONN_CONNCET_ERR_CONN_FAIL, 0);
		return;
	}

	/* Ok, so now we just wait for the connection to finish .. */
}

int
conn_connect(struct conn *k)
{
	struct evutil_addrinfo hints;

	fprintf(stderr, "%s: called!\n", __func__);

	/*
	 * This is where we would do DNS lookup if required.
	 */
	k->is_setup = 0;
	k->is_connecting = 1;
	k->is_connected = 0;
	k->is_dns_done = 0;

	if (k->is_dst_peer_set == 1) {
		if (conn_setup(k) != 0)
			return (-1);
		if (conn_connect_connect(k) != 0)
			return (-1);
		return (0);
	}

	/* Do DNS setup */
	bzero(&hints, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = EVUTIL_AI_CANONNAME;

	k->dns_req = evdns_getaddrinfo(k->eb->edns, k->dst_host.host, NULL,
	    &hints, conn_evdns_complete_callback, k);
	if (k->dns_req == NULL) {
		fprintf(stderr, "%s: failed to schedule DNS request\n",
		    __func__);
		return (-1);
	}

	return (0);
}

void
conn_free(struct conn *k)
{
	conn_close(k);
	if (k->dst_host.host)
		free(k->dst_host.host);
	buf_list_free(k->write_q);
	free(k);
}

int
conn_write(struct conn *k, struct buf *b)
{

	if (buf_list_append(k->write_q, b) != 0) {
		return (-1);
	}
	event_add(k->write_ev, NULL);
	return (0);
}

int
conn_read_pause(struct conn *k)
{

	k->is_paused = 1;

	/* Only flip this off if we're connected */
	if (k->is_connected)
		event_del(k->read_ev);
	return (0);
}

int
conn_read_resume(struct conn *k)
{

	k->is_paused = 0;

	/* Only flip this on if we're connected */
	if (k->is_connected)
		event_add(k->read_ev, NULL);
	return (0);
}
