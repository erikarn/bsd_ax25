#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>

#include <sys/queue.h>
#include <netinet/in.h>

#include <event2/event.h>

#include "eb.h"
#include "buf.h"
#include "buf_list.h"
#include "conn.h"
#include "ax25.h"
#include "kiss.h"
#include "plsm_kiss.h"

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

struct plsm_kiss *
plsm_kiss_create(struct ebase *eb)
{
	struct plsm_kiss *k;

	k = calloc(1, sizeof(*k));
	if (k == NULL) {
		warn("%s: calloc", __func__);
		goto err;
	}

	/* XXX TODO: may want to make this configurable */
	k->rx_buf = buf_create(65536);
	if (k->rx_buf == NULL) {
		warn("%s: malloc, 1024 bytes", __func__);
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
plsm_kiss_free(struct plsm_kiss *k)
{
	if (k->conn)
		conn_close(k->conn);
	if (k->host)
		free(k->host);
	if (k->rx_buf)
		buf_free(k->rx_buf);
	free(k);
}

int
plsm_kiss_set_host(struct plsm_kiss *k, const char *host, int port)
{
	if (k->host != NULL)
		free(k->host);
	k->host = strdup(host);
	k->port = port;

	return (0);
}

static int
plsm_kiss_read_cb(struct conn *c, void *arg, const uint8_t *buf, int len,
    int xerrno)
{
	struct plsm_kiss *k = arg;
	int r, i;
	int ss, se;
	uint8_t *ax25_buf;
	int ax25_len;

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

	/* Append data into the receive buffer */
	r = buf_append(k->rx_buf, buf, len);
	if (r != len) {
		fprintf(stderr, "%s: buf full, erk\n", __func__);
		/* XXX error, pass up to caller, stop reading, etc */
		return (0);
	}

#if 1
	for (i = 0; i < len; i++) {
		if (i % 16 == 0)
			fprintf(stderr, "0x%.4x: ", i);
		fprintf(stderr, "%.2x ", buf[i] & 0xff);
		if (i % 16 == 15)
			fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n");
#endif
	fflush(stderr);

	while (1) {
		/* Start looking for 0xc0 .. 0xc0 to delimit a packet */
		ss = -1;
		se = -1;
		for (i = 0; i < k->rx_buf->len; i++) {
			if (ss == -1 && k->rx_buf->buf[i] == 0xc0) {
				ss = i;
				continue;
			}
			if (ss != -1 && k->rx_buf->buf[i] == 0xc0) {
				se = i;
				break;
			}
		}
//		printf("ss=%d, se=%d\n", ss, se);

		if (ss == -1 || se == -1)
			break;

		/* Parse - for now, do both kiss and ax25 decap for debugging! */
		ax25_buf = malloc(ax25_len);
		if (ax25_buf != NULL) {
			struct pkt_ax25 *pkt;

			kiss_payload_parse(k->rx_buf->buf + ss, se - ss + 1,
			    ax25_buf, &ax25_len);

			/* Create an AX25 packet, pass it up to the owner */
			pkt = ax25_pkt_parse(ax25_buf, ax25_len);

			/* Pass it to the owner */
			if (pkt != NULL && k->owner_cb.read_cb == NULL) {
				pkt_ax25_free(pkt);
			} else if (pkt != NULL) {
				k->owner_cb.read_cb(k, k->owner_cb.arg, pkt);
			}
		}

		/* Consume everything until second 0xc0 */
		buf_consume(k->rx_buf, se);
	}


	return (0);
}

static int
plsm_kiss_write_cb(struct conn *c, void *arg, struct buf *b, int rettype,
    int xerrno)
{

	fprintf(stderr, "%s: called\n", __func__);
	buf_free(b);
	return (0);
}

static int
plsm_kiss_connect_cb(struct conn *c, void *arg, int rettype, int xerrno)
{

	fprintf(stderr, "%s: called\n", __func__);
	return (0);
}

static int
plsm_kiss_close_cb(struct conn *c, void *arg, int xerrno)
{

	fprintf(stderr, "%s: called\n", __func__);
	return (0);
}

int
plsm_kiss_connect(struct plsm_kiss *k)
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
	ps->sin_port = htons(8001);

	(void) conn_set_lcl(k->conn, (void *) ls);
	(void) conn_set_peer(k->conn, (void *) ps);

	/*
	 * For now, manually setup the callbacks required for
	 * interface lifecycle management.
	 */
	k->conn->cb.cbdata = k;
	k->conn->cb.read_cb = plsm_kiss_read_cb;
	k->conn->cb.write_cb = plsm_kiss_write_cb;
	k->conn->cb.connect_cb = plsm_kiss_connect_cb;
	k->conn->cb.close_cb = plsm_kiss_close_cb;

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
	k->state = PLSM_KISS_CONN_CONNECTING;

	return (0);
}

int
plsm_kiss_disconnect(struct plsm_kiss *k)
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
	k->state = PLSM_KISS_CONN_IDLE;

	return (0);
}
