#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>

#include <sys/queue.h>
#include <netinet/in.h>

#include <event2/event.h>

#include "str.h"
#include "buf.h"
#include "buf_list.h"
#include "conn.h"
#include "pkt_l3_aprs.h"
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
	if (k->aprs_server_info)
		free(k->aprs_server_info);
	if (k->aprs_login_response)
		free(k->aprs_login_response);
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
proto_aprs_igate_read_login(struct proto_aprs_igate *k, const char *buf,
    int len)
{
	struct buf *b;
	char bb[1024];
	int r;

	/* Save the server info */
	if (k->aprs_server_info)
		free(k->aprs_server_info);
	k->aprs_server_info = strndup(buf, len);

	/* Send our login string */
	r = snprintf(bb, 1024, "user %s pass %s vers %s %s filter r/%.2f/%.2f/%d\n",
	    k->login,
	    k->password,
	    "bsd_ax25",
	    "0.1",
	    k->filter_settings.filt_lat,
	    k->filter_settings.filt_long,
	    (int) k->filter_settings.filt_range);

	b = buf_create(1024);
	if (b == NULL) {
		fprintf(stderr, "%s: failed to allocate buf\n", __func__);
		return (-1);
	}

	if (buf_append(b, bb, r) != r) {
		fprintf(stderr, "%s: couldn't add login string to buf\n",
		    __func__);
		buf_free(b);
		return (-1);
	}

	k->state = PROTO_APRS_IGATE_CONN_LOGIN_RESPONSE;

	if (conn_write(k->conn, b) != 0) {
		fprintf(stderr, "%s: couldn't queue buffer for writing\n",
		    __func__);
		buf_free(b);
		return (-1);
	}

	return (0);
}

/*
 * Read in active responses.
 *
 * This state is when we receive normal APRS igate lines.
 * They may be actual payloads or server responses preceded with
 * a '#' character.
 */
static int
proto_aprs_igate_read_active(struct proto_aprs_igate *k,
    const char *buf, int len)
{
	struct pkt_l3_aprs *l;
	const char *b, *s, *pt, *py;
	int ns, npt, npy;
	int pl;

	/* Skip blank lines */
	if (len == 0)
		return (0);

	/* Skip comments / server responses for now */
	if (buf[0] == '#')
		return (0);

	/*
	 * Ok, TNC2 monitor line.
	 *
	 * eg: TG9AOR-D>APDG02,TCPIP*,qAC,TG9AOR-DS:!1431.68ND09027.24W&RNG0001 2m Voice 145.03000MHz +0.0000MHz
	 *
	 * The format is:
	 * SRC, DST path:payload
	 *
	 * The destination path is .. more complicated for APRS, but we
	 * are not the ones parsing this string out right now.
	 * That will be handled by some shared APRS code that sits
	 * above us.
	 */

	/* simple parsing - XXX should be replaced */
	pl = len;
	s = b = buf;

	/* Find the end of the ssid */
	pt = memchr(buf, '>', pl);
	if (pt == NULL) {
		return (0);
	}
	ns = pt - buf;
	/* Skip src + '>' */
	pl -= ns + 1;
	pt++;
	buf += ns;

	/* find the end of the path */
	py = memchr(buf, ':', pl);
	if (py == NULL) {
		return (0);
	}
	npt = py - buf - 1;
	/* Skip path + ":" */
	pl -= npt + 1;
	py++;
	buf += npt;

	/* Rest of the string is payload */
	npy = pl;

	/* Create an L3 APRS piece */
	l = pkt_l3_aprs_create();
	if (l == NULL)
		return (0);

	/* Populate the parsed frame */
	pkt_l3_aprs_set_src(l, s, ns);
	pkt_l3_aprs_set_path(l, pt, npt);
	pkt_l3_aprs_set_payload(l, py, npy);

	/* Call the owner with this */
	k->owner_cb.read_cb(k, k->owner_cb.arg, l);

	return (0);
}

/*
 * Read login response.
 *
 * Here's where we would check for an error and tell the owner
 * whether we're connected or not.
 */
static int
proto_aprs_igate_read_login_response(struct proto_aprs_igate *k,
    const char *buf, int len)
{
	if (k->aprs_login_response)
		free(k->aprs_login_response);
	k->aprs_login_response = strndup(buf, len);

	k->state = PROTO_APRS_IGATE_CONN_ACTIVE;

	return (0);
}

/*
 * Read callback from libconn.
 *
 * This is called whenever more data is available.
 *
 * (For now there's no flow control, so we can't tell the caller
 * to stop sending us data..)
 */
static int
proto_aprs_igate_read_cb(struct conn *c, void *arg, char *buf, int len, int xerrno)
{
	char rbuf[1024];
	struct proto_aprs_igate *k = arg;
	int r;

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

#if 0
	fprintf(stderr, "%s: read %d bytes\n", __func__, len);
	fprintf(stderr, "%s: --> %.*s\n", __func__, len, buf);
#endif

	/* Append into incoming buffer */
	r = buf_append(k->rx_buf, buf, len);
	if (r < len) {
		fprintf(stderr, "%s: error appending to buffer (r = %d)\n",
		    __func__,
		    r);
		/* XXX TODO: notify owner */
		return (0);
	}

	/* Extract out lines until we can't! */
	while ((r = buf_gets(k->rx_buf, rbuf, 1024)) > 0) {
		/* 0? We're ok. -1? Error */
		if (r < 0) {
			fprintf(stderr, "%s: error buf_gets; should close\n", __func__);
			return (0);
		}
		r = str_trim(rbuf, r);

#if 0
		fprintf(stderr, "%s: buf: '%.*s'\n", __func__, r, rbuf);
#endif

		/* Call per-state handler */
		switch (k->state) {
		case PROTO_APRS_IGATE_CONN_LOGIN:		/* Waiting for server hello string */
			proto_aprs_igate_read_login(k, rbuf, r);
			break;
		case PROTO_APRS_IGATE_CONN_LOGIN_RESPONSE:	/* Waiting for login attempt */
			proto_aprs_igate_read_login_response(k, rbuf, r);
			break;
		case PROTO_APRS_IGATE_CONN_ACTIVE:	/* Running */
			proto_aprs_igate_read_active(k, rbuf, r);
			break;
		default:
			break;
		}
	}

	return (0);
}

static int
proto_aprs_igate_write_cb(struct conn *c, void *arg, struct buf *b,
    int rettype, int xerrno)
{

	fprintf(stderr, "%s: called; buf=%p, rettype=%d, errno=%d\n", __func__,
	    b, rettype, xerrno);
	buf_free(b);
	return (0);
}

static int
proto_aprs_igate_connect_cb(struct conn *c, void *arg, int rettype, int xerrno)
{
	struct proto_aprs_igate *k = arg;

	fprintf(stderr, "%s: called\n", __func__);
	/* XXX owner notification */

	if (rettype != CONN_CONNECT_ERR_OK) {
		fprintf(stderr,
		    "%s: didn't connect successfully!; rettype=%d, errno=%d\n",
		    __func__,
		    rettype,
		    xerrno);
		return (0);
	}

	/*
	 * Ok, connected - so advance to the login state, wait for the
	 * login prompt.
	 */
	k->state = PROTO_APRS_IGATE_CONN_LOGIN;
	return (0);
}

static int
proto_aprs_igate_close_cb(struct conn *c, void *arg, int xerrno)
{

	fprintf(stderr, "%s: called\n", __func__);
	/* XXX owner notification */
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
