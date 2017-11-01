#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <time.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>

#include "os/endian.h"

#include "util.h"

uint64_t
timespec_to_nsec(const struct timespec ts)
{
	uint64_t t;

	t = ts.tv_sec * 1000000000ULL;
	t = t + ts.tv_nsec;

	return (t);
}

void
nsec_to_timespec(uint64_t ns, struct timespec *ts)
{

	ts->tv_sec = ns / 1000000000ULL;
	ts->tv_nsec = ns % 1000000000ULL;
}

/*
 * Compare address fields only.
 * (the port field doesn't matter.)
 */
int
sockaddr_comp_addr(const struct sockaddr_storage *a,
    const struct sockaddr_storage *b)
{
	struct sockaddr_in *a4, *b4;
	struct sockaddr_in6 *a6, *b6;

	a4 = (void *) a;
	switch (a4->sin_family) {
	case AF_INET:
		a4 = (void *) a;
		b4 = (void *) b;
		if (b4->sin_family != AF_INET)
			return (0);
		/* Only need to match on IPv4 address */
		if (memcmp(&a4->sin_addr, &b4->sin_addr, sizeof(struct in_addr)) == 0)
			return (1);
		return (0);
	case AF_INET6:
		a6 = (void *) a;
		b6 = (void *) b;
		if (b6->sin6_family != AF_INET6)
			return (0);

		/* Need to match on address and scopeid */
		if (memcmp(&a6->sin6_addr, &b6->sin6_addr, sizeof(struct in6_addr)) != 0)
			return (0);
		/* XXX what about flowid too? */
		if (a6->sin6_scope_id != b6->sin6_scope_id)
			return (0);
		return (1);
	default:
		return (0);
	}

	return (0);
}

int
sockaddr_copy(struct sockaddr_storage *dst,
    const struct sockaddr_storage *src)
{
	switch (src->ss_family) {
	case AF_INET:
		memcpy(dst, src, sizeof(struct sockaddr_in));
		return (1);
	case AF_INET6:
		memcpy(dst, src, sizeof(struct sockaddr_in6));
		return (1);
	default:
		return (0);
	}
	return (0);
}

int
sockaddr_len(const struct sockaddr_storage *s)
{
	switch (s->ss_family) {
	case AF_INET:
		return (sizeof(struct sockaddr_in));
	case AF_INET6:
		return (sizeof(struct sockaddr_in6));
	default:
		return (0);
	}
}

/*
 * convert a host string to a given addrinfo.
 *
 * This is to try and avoid having to do the getaddrinfo() hijinx
 * for each place we just want to convert a host or host:port to
 * a sockaddr.
 *
 * Returns 1 if it was completed and addrinfo has been
 * initialised, 0 otherwise.
 *
 */
int
sockaddr_host_to_sockaddr(const char *host, const char *port,
    int ai_family, int ai_socktype, int ai_passive,
    struct sockaddr_storage *ra)
{
	struct addrinfo hints;
	struct addrinfo *aih;
	int ret;

	bzero(&hints, sizeof(hints));
	hints.ai_family = ai_family;
	hints.ai_socktype = ai_socktype;
	hints.ai_flags = AI_NUMERICHOST;
	if (port) {
		hints.ai_flags |= AI_NUMERICSERV;
	}
	if (ai_passive)
		hints.ai_flags |= AI_PASSIVE;

	ret = getaddrinfo(host, NULL, &hints, &aih);
	if (ret)
		return (0);

	/* Just return the first entry */
	/* XXX should use MIN() of sizeof(ai_addr) and sockaddr_storage? */
	memcpy(ra, aih->ai_addr, aih->ai_addrlen);
	freeaddrinfo(aih);
	return (1);
}

const char *
sockaddr_to_str(const struct sockaddr_storage *s, char *buf)
{
	/* XXX pointer aliasing, sigh */
	const struct sockaddr_in *s4 = (const void *) s;
	const struct sockaddr_in6 *s6 = (const void *) s;

	switch (s->ss_family) {
	case AF_INET:
		return inet_ntop(AF_INET, &s4->sin_addr, buf,
		    sockaddr_len(s));
	case AF_INET6:
		return inet_ntop(AF_INET6, &s6->sin6_addr, buf,
		    sockaddr_len(s));
	default:
		return (NULL);
	}
}

int
sockaddr_get_port(const struct sockaddr_storage *s)
{
	/* XXX pointer aliasing, sigh */
	const struct sockaddr_in *s4 = (const void *) s;
	const struct sockaddr_in6 *s6 = (const void *) s;

	switch (s->ss_family) {
	case AF_INET:
		return ntohs(s4->sin_port);
	case AF_INET6:
		return ntohs(s6->sin6_port);
	default:
		return (-1);
	}
}

int
sockaddr_set_port(struct sockaddr_storage *s, uint16_t port)
{
	/* XXX pointer aliasing, sigh */
	struct sockaddr_in *s4 = (void *) s;
	struct sockaddr_in6 *s6 = (void *) s;

	switch (s->ss_family) {
	case AF_INET:
		s4->sin_port = htons(port);
		return (1);
	case AF_INET6:
		s6->sin6_port = htons(port);
		return (1);
	default:
		return (0);
	}
}

char *
util_int_to_str(int val)
{
	char *s;

	s = malloc(32);
	if (s == NULL)
		return (NULL);
        (void) snprintf(s, 32, "%d", val);
	return (s);
}

char *
util_int_to_hex(int val)
{
	char *s;

	s = malloc(32);
	if (s == NULL)
		return (NULL);
        (void) snprintf(s, 32, "0x%x", val);
	return (s);
}
