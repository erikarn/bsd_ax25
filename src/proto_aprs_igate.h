#ifndef	__PROTO_APRS_IGATE_H__
#define	__PROTO_APRS_IGATE_H__

struct proto_aprs_igate;
struct pkt_l3_aprs;
typedef	void proto_aprs_igate_owner_read_cb_t(struct proto_aprs_igate *,
	    void *arg, struct pkt_l3_aprs *l);
typedef	void proto_aprs_igate_owner_close_cb_t(struct proto_aprs_igate *,
	    void *arg);
typedef	void proto_aprs_igate_owner_connected_cb_t(struct proto_aprs_igate *,
	    void *arg, int status);

typedef enum {
	PROTO_APRS_IGATE_CONN_NONE,
	PROTO_APRS_IGATE_CONN_IDLE,
	PROTO_APRS_IGATE_CONN_CONNECTING,
	PROTO_APRS_IGATE_CONN_LOGIN,
	PROTO_APRS_IGATE_CONN_LOGIN_RESPONSE,
	PROTO_APRS_IGATE_CONN_ACTIVE,
	PROTO_APRS_IGATE_CONN_CLOSING,
} proto_aprs_igate_state_t;

struct proto_aprs_igate {
	struct ebase *eb;
	struct conn *conn;

	proto_aprs_igate_state_t state;

	/* Callsign / password for login */
	char *login;
	char *password;

	/* APRS host */
	char *host;
	int port;

	/* Provided APRS server info */
	char *aprs_server_info;
	char *aprs_login_response;

	struct {
		float filt_lat, filt_long;
		float filt_range;
	} filter_settings;

	struct buf *rx_buf;

	struct {
		proto_aprs_igate_owner_read_cb_t *read_cb;
		proto_aprs_igate_owner_close_cb_t *close_cb;
		proto_aprs_igate_owner_connected_cb_t *connected_cb;
		void *arg;
	} owner_cb;
};

extern	struct proto_aprs_igate * proto_aprs_igate_create(struct ebase *eb);
extern	int proto_aprs_igate_set_login(struct proto_aprs_igate *,
	    const char *login, const char *pass);
extern	int proto_aprs_igate_set_host(struct proto_aprs_igate *, const char *, int);
extern	int proto_aprs_igate_connect(struct proto_aprs_igate *);
extern	int proto_aprs_igate_disconnect(struct proto_aprs_igate *);

#endif	/* __PROTO_APRS_IGATE_H__ */
