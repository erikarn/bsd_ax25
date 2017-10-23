#ifndef	__CONN_H__
#define	__CONN_H__

struct conn;

typedef	int conn_read_cb_t(struct conn *c, void *arg, char *buf,
	    int len, int xerrno);
typedef	int conn_write_cb_t(struct conn *c, void *arg, int xerrno);
typedef	int conn_connect_cb_t(struct conn *c, void *arg, int xerrno);
typedef	int conn_close_cb_t(struct conn, void *arg, int xerrno);

struct conn {
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

extern	struct conn * conn_create(struct event_base *eb);
extern	int conn_close(struct conn *k);
extern	int conn_setup(struct conn *k);
extern	int conn_set_lcl(struct conn *k, const struct sockaddr_storage *);
extern	int conn_set_peer(struct conn *k, const struct sockaddr_storage *);
extern	int conn_connect(struct conn *k);
extern	void conn_free(struct conn *k);

extern	int conn_write(struct conn *k, struct buf *b);

#endif	/* __CONN_H__ */
