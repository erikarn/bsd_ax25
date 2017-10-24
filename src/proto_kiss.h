#ifndef	__PROTO_KISS_H__
#define	__PROTO_KISS_H__

typedef enum {
	PROTO_KISS_CONN_NONE,
	PROTO_KISS_CONN_IDLE,
	PROTO_KISS_CONN_CONNECTING,
	PROTO_KISS_CONN_ACTIVE,
	PROTO_KISS_CONN_CLOSING,
} proto_kiss_conn_state_t;

struct proto_kiss {
	struct event_base *eb;
	struct conn *conn;

	proto_kiss_conn_state_t state;

	/* KISS terminal server */
	char *host;
	int port;

	int is_connected;
	int is_full_duplex;

	struct {
		void *cbdata;
	} cb;

	/*
	 * Incoming data buffer and state for a single packet to
	 * search for start/end fields for.
	 */
	struct {
		char *buf;
		int size;
		int len;
	} rx_decap;

	/* XXX TODO: outbound TX packet list to send up to the TNC. */
};

extern	struct proto_kiss * proto_kiss_create(struct event_base *eb);
extern	void proto_kiss_free(struct proto_kiss *);
extern	int proto_kiss_set_host(struct proto_kiss *, const char *host,
	    int port);
extern	int proto_kiss_connect(struct proto_kiss *);
extern	int proto_kiss_disconnect(struct proto_kiss *);

#endif	/* __PROTO_KISS_H__ */
