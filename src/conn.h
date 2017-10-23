#ifndef	__CONN_H__
#define	__CONN_H__

struct proto_conn {
	struct event_base *eb;
	struct sockaddr_storage lcl, peer;
	int fd;
	struct event *read_ev;
	struct event *write_ev;
	int is_setup;
	int is_connecting;
	int is_connected;
};

extern	struct proto_conn * conn_create(struct event_base *eb);
extern	int conn_close(struct proto_conn *k);
extern	int conn_setup(struct proto_conn *k);
extern	int conn_set_lcl(struct proto_conn *k, const struct sockaddr_storage *);
extern	int conn_set_peer(struct proto_conn *k, const struct sockaddr_storage *);
extern	int conn_connect(struct proto_conn *k);
extern	void conn_free(struct proto_conn *k);

#endif	/* __CONN_H__ */
