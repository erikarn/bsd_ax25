#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>

#include <sys/queue.h>
#include <netinet/in.h>

#include <event2/event.h>

#include "buf.h"
#include "buf_list.h"
#include "conn.h"
#include "proto_kiss.h"

/*
 * This implements a connection to a KISS TNC -
 * eventually being an AX.25 protocol interface.
 *
 * For now it assumes a TCP connection but it should
 * be easy to extend to do serial, udp, i2c, etc.
 *
 * However, until the interface bits are up, this
 * will primarily just be a mostly-standalone thingy
 * to encap/decap KISS.
 */

struct proto_kiss *
proto_kiss_create(struct event_base *eb)
{
	struct proto_kiss *k;

	k = calloc(1, sizeof(*k));
	if (k == NULL) {
		warn("%s: calloc", __func__);
		goto err;
	}


	k->eb = eb;
	return (k);

err:
	if (k->conn)
		conn_free(k->conn);
	if (k->host)
		free(k->host);
	free(k);
	return (NULL);
}

void
proto_kiss_free(struct proto_kiss *k)
{
	if (k->conn)
		conn_close(k->conn);
	if (k->host)
		free(k->host);
	free(k);
}

int
proto_kiss_set_host(struct proto_kiss *k, const char *host, int port)
{
	if (k->host != NULL)
		free(k->host);
	k->host = strdup(host);
	k->port = port;

	return (0);
}

static int
proto_kiss_read_cb(struct conn *c, void *arg, char *buf, int len, int xerrno)
{

	fprintf(stderr, "%s: called\n", __func__);
	return (0);
}

static int
proto_kiss_write_cb(struct conn *c, void *arg, int xerrno)
{

	fprintf(stderr, "%s: called\n", __func__);
	return (0);
}

static int
proto_kiss_connect_cb(struct conn *c, void *arg, int xerrno)
{

	fprintf(stderr, "%s: called\n", __func__);
	return (0);
}

static int
proto_kiss_close_cb(struct conn *c, void *arg, int xerrno)
{

	fprintf(stderr, "%s: called\n", __func__);
	return (0);
}

int
proto_kiss_connect(struct proto_kiss *k)
{
	struct sockaddr_storage lcl, peer;
	struct sockaddr_in *ls, *ps;

	if (k->host == NULL || k->port == 0) {
		fprintf(stderr, "%s: no host/port set\n", __func__);
		return (-1);
	}

	if (k->conn != NULL) {
		fprintf(stderr, "%s: already connected\n", __func__);
		return (-1);
	}

	/* Create connection */
	k->conn = conn_create(k->eb);
	if (k->conn == NULL) {
		fprintf(stderr, "%s: failed to create conn\n", __func__);
		return (-1);
	}

	/* Setup information for outbound connection - for now, assume localhost:8001 */
	bzero(&lcl, sizeof(lcl));
	bzero(&peer, sizeof(peer));
	ls = (void *) &lcl;
	ps = (void *) &peer;

	/* XXX TODO - replace */
	ls->sin_family = AF_INET;
	ls->sin_addr.s_addr = htonl(INADDR_ANY);
	ls->sin_port = 0;

	/* XXX TODO - replace */
	ps->sin_family = AF_INET;
	ps->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	ps->sin_port = 8001;

	(void) conn_set_lcl(k->conn, (void *) ls);
	(void) conn_set_peer(k->conn, (void *) ps);

	/*
	 * For now, manually setup the callbacks required for
	 * interface lifecycle management.
	 */
	k->conn->cb.cbdata = k;
	k->conn->cb.read_cb = proto_kiss_read_cb;
	k->conn->cb.write_cb = proto_kiss_write_cb;
	k->conn->cb.connect_cb = proto_kiss_connect_cb;
	k->conn->cb.close_cb = proto_kiss_close_cb;

	/*
	 * Start the connect.  If it succeeds here then we
	 * wait for notification that we actually have succeeded
	 * before continuing.
	 */
	if (conn_setup(k->conn) != 0) {
		fprintf(stderr, "%s: conn_setup failed\n", __func__);
		return (-1);
	}

	if (conn_connect(k->conn) != 0) {
		fprintf(stderr, "%s: conn_connect failed\n", __func__);
		return (-1);
	}

	/* waiting for connect notification now */
	k->state = PROTO_KISS_CONN_CONNECTING;

	return (0);
}

int
proto_kiss_disconnect(struct proto_kiss *k)
{

	if (k->conn == NULL) {
		fprintf(stderr, "%s: called without being connected\n", __func__);
		return (-1);
	}

	/* Close, free the connection; we're done */
	conn_close(k->conn);
	conn_free(k->conn);
	k->conn = NULL;

	/* Idle state, need to be reconfigured */
	k->state = PROTO_KISS_CONN_IDLE;

	return (0);
}
