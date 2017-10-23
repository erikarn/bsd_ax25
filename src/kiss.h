#ifndef	__KISS_H__
#define	__KISS_H__

struct proto_kiss {
	struct event_base *eb;
	struct sockaddr_storage lcl, peer;
	int fd;
	struct event *read_ev;
	struct event *write_ev;
	int is_setup;
	int is_connecting;
	int is_connected;
};

extern	struct proto_kiss * kiss_create(struct event_base *eb);
extern	int kiss_close(struct proto_kiss *k);
extern	int kiss_setup(struct proto_kiss *k);
extern	int kiss_connect(struct proto_kiss *k);
extern	void kiss_free(struct proto_kiss *k);

#endif	/* __KISS_H__ */
