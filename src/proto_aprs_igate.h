#ifndef	__PROTO_APRS_IGATE_H__
#define	__PROTO_APRS_IGATE_H__

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

	struct {
		float filt_lat, filt_long;
		float filt_range;
	} filter_settings;

	struct buf *rx_buf;
};

extern	struct proto_aprs_igate * proto_aprs_igate_create(struct ebase *eb);
extern	int proto_aprs_igate_set_login(struct proto_aprs_igate *,
	    const char *login, const char *pass);
extern	int proto_aprs_igate_set_host(struct proto_aprs_igate *, const char *, int);
extern	int proto_aprs_igate_connect(struct proto_aprs_igate *);
extern	int proto_aprs_igate_disconnect(struct proto_aprs_igate *);

#endif	/* __PROTO_APRS_IGATE_H__ */
