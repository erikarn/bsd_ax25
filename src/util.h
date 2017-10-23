#ifndef	__UTIL_H__
#define	__UTIL_H__

extern	uint64_t timespec_to_nsec(const struct timespec ts);
extern	void nsec_to_timespec(uint64_t t, struct timespec *ts);

extern	int sockaddr_comp_addr(const struct sockaddr_storage *a,
	    const struct sockaddr_storage *b);
extern int sockaddr_copy(struct sockaddr_storage *dst,
	    const struct sockaddr_storage *src);
extern int sockaddr_len(const struct sockaddr_storage *dst);
struct addrinfo;
extern	int sockaddr_host_to_sockaddr(const char *host, const char *port,
	    int ai_family, int ai_socktype, int ai_passive,
	    struct sockaddr_storage *ai);
extern	const char * sockaddr_to_str(const struct sockaddr_storage *s,
	    char *buf);
extern	int sockaddr_get_port(const struct sockaddr_storage *s);
extern	int sockaddr_set_port(struct sockaddr_storage *s, uint16_t port);

extern	char * util_int_to_str(int val);
extern	char * util_int_to_hex(int val);

#endif
