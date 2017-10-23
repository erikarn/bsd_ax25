#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <event2/event.h>

#include "kiss.h"

/*
 * XXX TODO: This should just become a generic wrapper for doing
 * libevent stream socket connecting, as it abstracts out the
 * bits that unfortunately aren't done for us by libevent2.. :(
 */

struct proto_kiss *
kiss_create(struct event_base *eb)
{
	struct proto_kiss *k;

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
kiss_close(struct proto_kiss *k)
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
	return (0);
}

static void
kiss_connect_complete(struct proto_kiss *k)
{
	k->is_connecting = 0;
	k->is_connected = 1;
	event_add(k->read_ev, NULL);
	fprintf(stderr, "%s: ready!\n", __func__);
}

static void
kiss_connect_error(struct proto_kiss *k)
{
	k->is_connecting = 0;
	k->is_connected = 0;
	fprintf(stderr, "%s: error connecting!\n", __func__);
}

static void
kiss_read_cb(evutil_socket_t fd, short what, void *arg)
{
	char buf[1024];
	int ret;
	struct proto_kiss *k = arg;

	fprintf(stderr, "%s: called!\n", __func__);

	/* XXX loop */
	ret = read(k->fd, buf, 1024);
	if (ret < 0)
		return;

	fprintf(stderr, "%s: read %d bytes\n", __func__, ret);
}

static void
kiss_write_cb(evutil_socket_t fd, short what, void *arg)
{
	struct proto_kiss *k = arg;
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
			kiss_connect_error(k);
			return;
		}
		if (ret == 0 && optarg == EINTR) {
			fprintf(stderr, "%s: connect; eintr\n", __func__);
			event_add(k->write_ev, NULL);
			return;
		}
		if (ret == 0 && optarg == 0) {
			kiss_connect_complete(k);
			return;
		}
		/* issue? */
		fprintf(stderr, "%s: connect; failed; errno=%d\n", __func__, optarg);
		kiss_connect_error(k);
		return;
	}

	/* ok, if we get here, we can write normal data */
}

int
kiss_setup(struct proto_kiss *k)
{
	struct sockaddr_in *sin;
	int fd;

	fprintf(stderr, "%s: called!\n", __func__);

	if (k->fd != -1) {
		kiss_close(k);
	}

	/* For now, assume localhost:8001 */
	sin = (void *) &k->peer;
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	sin->sin_port = htons(8001);

	/* For now, assume ipv4 */
	sin = (void *) &k->lcl;
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = htonl(INADDR_ANY);
	sin->sin_port = htons(0);

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

	k->read_ev = event_new(k->eb, k->fd, EV_READ | EV_PERSIST, kiss_read_cb, k);
	k->write_ev = event_new(k->eb, k->fd, EV_WRITE, kiss_write_cb, k);

	return (0);
}

int
kiss_connect(struct proto_kiss *k)
{
	int ret;

	fprintf(stderr, "%s: called!\n", __func__);

	if (k->fd == -1) {
		fprintf(stderr, "%s: kiss_connect() called before kiss_setup()!\n", __func__);
		return (-1);
	}

	/* XXX v4 */
	ret = bind(k->fd, (void *) &k->lcl, sizeof(struct sockaddr_in));
	if (ret != 0) {
		warn("%s: bind", __func__);
		return (-1);
	}

	/* Begin the connect; handle the case where it completes immediately */
	/* XXX v4 */
	ret = connect(k->fd, (void *) &k->peer, sizeof(struct sockaddr_in));
	if (ret == 0) {
		/* Finished */
		kiss_connect_complete(k);
		return (0);
	}

	if (ret < 0 && errno == EINPROGRESS) {
		fprintf(stderr, "%s: in progress!\n", __func__);
		event_add(k->write_ev, NULL);
		k->is_connecting = 1;
		return (0);
	}

	warn("%s: connect", __func__);
	return (-1);
}

void
kiss_free(struct proto_kiss *k)
{

	kiss_close(k);

}
