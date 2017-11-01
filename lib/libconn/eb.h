#ifndef	__EB_H__
#define	__EB_H__

struct ebase {
	struct event_base *ebase;
	struct evdns_base *edns;
};

#endif	/* __EB_H__ */
