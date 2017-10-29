#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <sys/queue.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <event2/event.h>

#include "util.h"
#include "eb.h"
#include "buf.h"
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
	event_del(k->read_ev);
	event_del(k->write_ev);
	event_free(k->read_ev);
	event_free(k->write_ev);

	if (k->cb.close_cb != NULL) {
		k->cb.close_cb(k, k->cb.cbdata, 0);
	}

	return (0);
}

static void
conn_connect_complete(struct conn *k)
{

	k->is_connecting = 0;
	k->is_connected = 1;
	event_add(k->read_ev, NULL);
	fprintf(stderr, "%s: ready!\n", __func__);

	if (k->cb.connect_cb != NULL) {
		k->cb.connect_cb(k, k->cb.cbdata, CONN_CONNECT_ERR_OK, 0);
	}
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
	char buf[1024];
	int ret;

	fprintf(stderr, "%s: called!\n", __func__);

	/* XXX loop */
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
			return;
		}
		/* issue? */
		fprintf(stderr, "%s: connect; failed; errno=%d\n", __func__, optarg);
		conn_connect_error(k, CONN_CONNECT_ERR_CONN_REFUSED, optarg);
		return;
	}

	/* ok, if we get here, we can write normal data */
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
	return (0);
}

int
conn_set_peer_host(struct conn *k, const char *host, int port)
{

	if (k->dst_host.host != NULL)
		free(k->dst_host.host);
	k->dst_host.host = strdup(host);
	k->dst_host.port = port;

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

	fprintf(stderr, "%s: called!\n", __func__);

	if (k->fd != -1) {
		conn_close(k);
	}

	fd = socket(PF_INET, SOCK_STREAM, 0);
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

	ret = bind(k->fd, (void *) &k->lcl, sockaddr_len(&k->lcl));
	if (ret != 0) {
		warn("%s: bind", __func__);
		return (-1);
	}

	/* Begin the connect; handle the case where it completes immediately */
	/* XXX v4 */
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

int
conn_connect(struct conn *k)
{

	fprintf(stderr, "%s: called!\n", __func__);

	/*
	 * This is where we would do DNS lookup if required.
	 */

	/*
	 * Assume that we now have enough details, so do it!
	 */
	conn_setup(k);
	conn_connect_connect(k);

	/* For now; notifications will eventually be deferred */
	return (0);
}

void
conn_free(struct conn *k)
{
	conn_close(k);
	if (k->dst_host.host)
		free(k->dst_host.host);
	free(k);
}
