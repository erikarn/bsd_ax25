#ifndef	__PROTO_KISS_H__
#define	__PROTO_KISS_H__

struct proto_kiss {
	struct event_base *eb;
	struct conn *conn;

	/* KISS terminal server */
	char *host;
	int port;

	int is_connected;
	int is_full_duplex;

	struct {
		void *cbdata;
	} cb;
};

extern	struct proto_kiss * proto_kiss_create(struct event_base *eb);
extern	void proto_kiss_free(struct proto_kiss *);
extern	int proto_kiss_set_host(struct proto_kiss *, const char *host,
	    int port);
extern	int proto_kiss_connect(struct proto_kiss *);
extern	int proto_kiss_disconnect(struct proto_kiss *);

#endif	/* __PROTO_KISS_H__ */
