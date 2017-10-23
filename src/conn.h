#ifndef	__CONN_H__
#define	__CONN_H__

struct proto_conn;

typedef	int conn_read_cb_t(struct proto_conn *c, void *arg, char *buf,
	    int len, int xerrno);
typedef	int conn_write_cb_t(struct proto_conn *c, void *arg, int xerrno);
typedef	int conn_connect_cb_t(struct proto_conn *c, void *arg, int xerrno);
typedef	int conn_close_cb_t(struct proto_conn, void *arg, int xerrno);

struct proto_conn {
	struct event_base *eb;
	struct sockaddr_storage lcl, peer;
	int fd;
	struct event *read_ev;
	struct event *write_ev;
	int is_setup;
	int is_connecting;
	int is_connected;

	struct {
		conn_read_cb_t *read_cb;
		conn_write_cb_t *write_cb;
		conn_connect_cb_t *connect_cb;
		conn_close_cb_t *close_cb;
		void *cbdata;
	} cb;
};

extern	struct proto_conn * conn_create(struct event_base *eb);
extern	int conn_close(struct proto_conn *k);
extern	int conn_setup(struct proto_conn *k);
extern	int conn_set_lcl(struct proto_conn *k, const struct sockaddr_storage *);
extern	int conn_set_peer(struct proto_conn *k, const struct sockaddr_storage *);
extern	int conn_connect(struct proto_conn *k);
extern	void conn_free(struct proto_conn *k);

#endif	/* __CONN_H__ */
