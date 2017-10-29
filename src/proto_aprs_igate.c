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
#include "proto_aprs_igate.h"

/*
 * This implements a simple APRS I-Gate client.
 */

struct proto_aprs_igate *
proto_aprs_igate_create(struct ebase *eb)
{
	struct proto_aprs_igate *k;

	k = calloc(1, sizeof(*k));
	if (k == NULL) {
		warn("%s: calloc", __func__);
		goto err;
	}

	/* XXX TODO: may want to make this configurable */
	k->rx_buf = buf_create(32768);
	if (k->rx_buf == NULL) {
		warn("%s: buf", __func__);
		goto err;
	}

	k->eb = eb;
	return (k);

err:
	if (k->conn)
		conn_free(k->conn);
	if (k->host)
		free(k->host);
	if (k->rx_buf)
		buf_free(k->rx_buf);
	free(k);
	return (NULL);
}

void
proto_aprs_igate_free(struct proto_aprs_igate *k)
{
	if (k->conn)
		conn_close(k->conn);
	if (k->host)
		free(k->host);
	if (k->login)
		free(k->login);
	if (k->password)
		free(k->password);
	if (k->rx_buf)
		buf_free(k->rx_buf);
	free(k);
}

int
proto_aprs_igate_set_host(struct proto_aprs_igate *k, const char *host, int port)
{
	if (k->host != NULL)
		free(k->host);
	k->host = strdup(host);
	k->port = port;

	return (0);
}

static int
proto_aprs_igate_read_cb(struct conn *c, void *arg, char *buf, int len, int xerrno)
{
	int i;

	fprintf(stderr, "%s: called\n", __func__);

	/* XXX TODO: notify owner */
	if (len == 0) {
		fprintf(stderr, "%s: conn is EOF\n", __func__);
		return (0);
	}

	/* XXX TODO: notify owner */
	if (len < 0) {
		fprintf(stderr, "%s: conn read fail, errno+%d\n", __func__,
		    xerrno);
		return (0);
	}


	fprintf(stderr, "%s: read %d bytes\n", __func__, len);

	for (i = 0; i < len; i++) {
		if (i % 16 == 0)
			fprintf(stderr, "0x%.4x: ", i);
		fprintf(stderr, "%.2x ", buf[i] & 0xff);
		if (i % 16 == 15)
			fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n");

	return (0);
}

static int
proto_aprs_igate_write_cb(struct conn *c, void *arg, int xerrno)
{

	fprintf(stderr, "%s: called\n", __func__);
	return (0);
}

static int
proto_aprs_igate_connect_cb(struct conn *c, void *arg, int rettype, int xerrno)
{

	fprintf(stderr, "%s: called\n", __func__);
	return (0);
}

static int
proto_aprs_igate_close_cb(struct conn *c, void *arg, int xerrno)
{

	fprintf(stderr, "%s: called\n", __func__);
	return (0);
}

int
proto_aprs_igate_connect(struct proto_aprs_igate *k)
{

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

	(void) conn_set_peer_host(k->conn, k->host, k->port);

	/*
	 * For now, manually setup the callbacks required for
	 * interface lifecycle management.
	 */
	k->conn->cb.cbdata = k;
	k->conn->cb.read_cb = proto_aprs_igate_read_cb;
	k->conn->cb.write_cb = proto_aprs_igate_write_cb;
	k->conn->cb.connect_cb = proto_aprs_igate_connect_cb;
	k->conn->cb.close_cb = proto_aprs_igate_close_cb;

	/*
	 * Start the connect.  If it succeeds here then we
	 * wait for notification that we actually have succeeded
	 * before continuing.
	 */
	if (conn_connect(k->conn) != 0) {
		fprintf(stderr, "%s: conn_connect failed\n", __func__);
		return (-1);
	}

	/* waiting for connect notification now */
	k->state = PROTO_APRS_IGATE_CONN_CONNECTING;

	return (0);
}

int
proto_aprs_igate_disconnect(struct proto_aprs_igate *k)
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
	k->state = PROTO_APRS_IGATE_CONN_IDLE;

	return (0);
}

int
proto_aprs_igate_set_login(struct proto_aprs_igate *k,
    const char *callsign, const char *pass)
{
	if (k->login != NULL)
		free(k->login);
	if (k->password != NULL)
		free(k->password);
	k->login = strdup(callsign);
	k->password = strdup(pass);

	return (0);
}
