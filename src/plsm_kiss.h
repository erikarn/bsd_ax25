#ifndef	__PLSM_KISS_H__
#define	__PLSM_KISS_H__

typedef enum {
	PLSM_KISS_CONN_NONE,
	PLSM_KISS_CONN_IDLE,
	PLSM_KISS_CONN_CONNECTING,
	PLSM_KISS_CONN_ACTIVE,
	PLSM_KISS_CONN_CLOSING,
} plsm_kiss_conn_state_t;

struct plsm_kiss;
typedef	int plsm_kiss_read_cb_t(struct plsm_kiss *, void *arg,
	    struct ax25_pkt *p);

struct plsm_kiss {
	struct ebase *eb;
	struct conn *conn;
	struct ax25_plsm *plsm;

	plsm_kiss_conn_state_t state;

	/* KISS terminal server */
	char *host;
	int port;

	int is_connected;
	int is_full_duplex;

	/* buffer for RX decap */
	struct buf *rx_buf;

	/* XXX TODO: outbound TX packet list to send up to the TNC. */
};

extern	struct ax25_plsm * plsm_kiss_create(struct ebase *eb, const char *host,
	    int port);

#endif	/* __PLSM_KISS_H__ */
