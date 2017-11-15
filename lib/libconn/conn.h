#ifndef	__CONN_H__
#define	__CONN_H__

struct conn;

typedef enum {
	CONN_CONNECT_ERR_NONE,
	CONN_CONNECT_ERR_OK,
	CONN_CONNECT_ERR_DNS_FAIL,
	CONN_CONNECT_ERR_CONN_FAILURE,
	CONN_CONNECT_ERR_CONN_CONNECT_FAILURE,
	CONN_CONNECT_ERR_CONN_REFUSED,
	CONN_CONNCET_ERR_CONN_TIMEOUT,
	CONN_CONNCET_ERR_SETUP_FAIL,		/* socket setup failed */
	CONN_CONNCET_ERR_CONN_FAIL,		/* connection setup failed */
} conn_connect_err_t;

typedef enum {
	CONN_WRITE_ERR_NONE,
	CONN_WRITE_ERR_OK,
	CONN_WRITE_ERR_EOF,
	CONN_WRITE_ERR_FAIL,
} conn_write_err_t;

typedef	int conn_read_cb_t(struct conn *c, void *arg, const uint8_t *buf,
	    int len, int xerrno);
typedef	int conn_write_cb_t(struct conn *c, void *arg, struct buf *b,
	    int rettype, int xerrno);
typedef	int conn_connect_cb_t(struct conn *c, void *arg, int rettype,
	    int xerrno);
typedef	int conn_close_cb_t(struct conn *c, void *arg, int xerrno);

struct conn {
	struct ebase *eb;
	struct sockaddr_storage lcl, peer;
	int fd;
	struct event *read_ev;
	struct event *write_ev;
	int is_setup;
	int is_dns_done;
	int is_connecting;
	int is_connected;
	int is_dst_peer_set;
	int is_paused;

	struct evdns_getaddrinfo_request *dns_req;

	struct {
		char *host;
		int port;
	} dst_host;

	struct {
		conn_read_cb_t *read_cb;
		conn_write_cb_t *write_cb;
		conn_connect_cb_t *connect_cb;
		conn_close_cb_t *close_cb;
		void *cbdata;
	} cb;

	struct buf_list *write_q;
};

extern	struct conn * conn_create(struct ebase *eb);
extern	int conn_close(struct conn *k);
extern	int conn_set_lcl(struct conn *k, const struct sockaddr_storage *);
extern	int conn_set_peer(struct conn *k, const struct sockaddr_storage *);
extern	int conn_set_peer_host(struct conn *k, const char *host, int port);
extern	int conn_connect(struct conn *k);
extern	void conn_free(struct conn *k);

extern	int conn_write(struct conn *k, struct buf *b);
extern	int conn_write_flush(struct conn *k);
extern	int conn_read_pause(struct conn *k);
extern	int conn_read_resume(struct conn *k);

#endif	/* __CONN_H__ */
