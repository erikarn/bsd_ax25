#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <netinet/in.h>

#include <event2/event.h>
#include <event2/dns.h>

#include "eb.h"
#include "buf.h"
#include "conn.h"
#include "ax25.h"
#include "proto_kiss.h"
#include "pkt_l3_aprs.h"
#include "proto_aprs_igate.h"

#if 1
static void
igate_read_cb(struct proto_aprs_igate *k, void *arg,
    struct pkt_l3_aprs *l)
{

	printf("SRC: %s; DST: %s; PATH: %s; PAYLOAD: %s\n",
	    l->src,
	    l->dst,
	    l->path,
	    l->payload);
	printf("====\n");

	pkt_l3_aprs_free(l);
}
#endif

#if 1
static int
kiss_read_cb(struct proto_kiss *p, void *arg,
    struct pkt_ax25 *pkt)
{

	printf("%s: called\n", __func__);
	pkt_ax25_free(pkt);
	return (0);
}
#endif

int
main(int argc, const char *argv[])
{
	struct proto_kiss *p;
	struct proto_aprs_igate *pg;
	struct ebase eb;

	eb.ebase = event_base_new();
	eb.edns = evdns_base_new(eb.ebase, 1);

#if 1
	p = proto_kiss_create(&eb);
	p->owner_cb.arg = NULL;
	p->owner_cb.read_cb = kiss_read_cb;
	proto_kiss_set_host(p, "127.0.0.1", 8001);
	proto_kiss_connect(p);
#endif

#if 1
	pg = proto_aprs_igate_create(&eb);
	proto_aprs_igate_set_login(pg, "KK6VQK", "-1");
	proto_aprs_igate_set_host(pg, "rotate.aprs2.net", 14580);
	pg->owner_cb.arg = NULL;
	pg->owner_cb.read_cb = igate_read_cb;

	pg->filter_settings.filt_lat = 37.76;
	pg->filter_settings.filt_long = -122.19;
	pg->filter_settings.filt_range = 10000;

	proto_aprs_igate_connect(pg);
#endif

	event_base_loop(eb.ebase, EVLOOP_NO_EXIT_ON_EMPTY);

	exit(0);
}
