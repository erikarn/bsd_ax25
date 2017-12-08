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

#include "ax25_pkt.h"
#include "ax25_plsm.h"

#include "kiss.h"
#include "plsm_kiss.h"

/*
 * Free local state.  Shouldn't be called directly.
 */
static void
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

static int
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

		/* Parse KISS frame, send it up to the owner as a raw AX25 frame  */
		ax25_buf = malloc(ax25_len);
		if (ax25_buf != NULL) {

			kiss_payload_parse(k->rx_buf->buf + ss, se - ss + 1,
			    ax25_buf, &ax25_len);

			/* If we have data, copy it into the ax25 packet */
			if (ax25_len != 0) {
				struct ax25_pkt *pkt;

				pkt = ax25_pkt_create(ax25_len);

				if (pkt != NULL) {
					/* Populate */
					(void) buf_copy(pkt->buf, ax25_buf, ax25_len);

					/*
					 * Send up, error means we need to handle
					 * the buffer
					 */
					if (ax25_plsm_data_indication(k->plsm, pkt) != 0) {
						ax25_pkt_free(pkt);
					}
				}
			}
			/* Free buffer */
			free(ax25_buf);
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

static int
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

/* PLSM client method implementations */
static int
plsm_kiss_seize_request(struct ax25_plsm *p)
{

	/* For now, confirm */
	return (ax25_plsm_seize_confirm(p));
}

static int
plsm_kiss_data_request(struct ax25_plsm *p, struct ax25_pkt *pkt)
{

	fprintf(stderr, "%s: TODO\n", __func__);
	ax25_pkt_free(pkt);
	return (0);
}

static int
plsm_kiss_release_request(struct ax25_plsm *p)
{

	return (0);
}

static int
plsm_kiss_expedited_data_request(struct ax25_plsm *p, struct ax25_pkt *pkt)
{

	fprintf(stderr, "%s: TODO\n", __func__);
	ax25_pkt_free(pkt);
	return (0);
}

int
plsm_kiss_client_free(struct ax25_plsm *p)
{
	struct plsm_kiss *k = AX25_PLSM_CLIENT_ARG(p);

	plsm_kiss_free(k);
	return (0);
}

int
plsm_kiss_client_start(struct ax25_plsm *p)
{
	struct plsm_kiss *k = AX25_PLSM_CLIENT_ARG(p);

	return (plsm_kiss_connect(k));
}

int
plsm_kiss_client_stop(struct ax25_plsm *p)
{
	struct plsm_kiss *k = AX25_PLSM_CLIENT_ARG(p);

	return (plsm_kiss_disconnect(k));
}

/*
 * This implements a connection to a KISS TNC -
 * eventually being an AX.25 protocol interface.
 *
 * For now it assumes a TCP connection but it should
 * be easy to extend to do serial, udp, i2c, etc.
 */

struct ax25_plsm *
plsm_kiss_create(struct ebase *eb, const char *host, int port)
{
	struct plsm_kiss *k;
	struct ax25_plsm *p;
	struct buf *rb;

	rb = buf_create(65536);
	if (rb == NULL) {
		warn("%s: malloc, 1024 bytes", __func__);
		return (NULL);
	}

	/* Step 1 - create a PLSM */
	p = ax25_plsm_create(eb);
	if (p == NULL) {
		buf_free(rb);
		return (NULL);
	}

	/* Step 2 - create our own implementation */
	k = calloc(1, sizeof(*k));
	if (k == NULL) {
		warn("%s: calloc", __func__);
		ax25_plsm_free(p);
		buf_free(rb);
		return (NULL);
	}

	k->eb = eb;
	k->plsm = p;
	k->rx_buf = rb;
	k->host = strdup(host);
	k->port = port;

	/*
	 * Step 3 - setup the client hooks; these are our
	 * implementations of pieces.
	 *
	 * Once this is done, unwinding state will be done
	 * through the plsm destroy path.
	 */
	p->client.arg = k;
	p->client.ph_seize_request_cb = plsm_kiss_seize_request;
	p->client.ph_data_request_cb = plsm_kiss_data_request;
	p->client.ph_release_request_cb = plsm_kiss_release_request;
	p->client.ph_expedited_data_request_cb = plsm_kiss_expedited_data_request;
	p->client.ph_client_free_cb = plsm_kiss_client_free;
	p->client.ph_client_start_cb = plsm_kiss_client_start;
	p->client.ph_client_stop_cb = plsm_kiss_client_stop;

	/* We're all ready! */

	return (p);
}

