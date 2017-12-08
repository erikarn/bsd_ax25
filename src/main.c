#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <netinet/in.h>

#include <event2/event.h>
#include <event2/dns.h>

#include "eb.h"
#include "buf.h"
#include "conn.h"

#include "ax25_pkt.h"
#include "ax25_plsm.h"

#include "plsm_kiss.h"

#include "pkt_l3_aprs.h"
#include "proto_aprs_igate.h"

#if 0
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

int
main(int argc, const char *argv[])
{
	struct ax25_plsm *p;
//	struct proto_aprs_igate *pg;
	struct ebase eb;

	eb.ebase = event_base_new();
	eb.edns = evdns_base_new(eb.ebase, 1);

	//pkt_ax25_log_open("ax25-pkt.log");

#if 1
	p = plsm_kiss_create(&eb, "127.0.0.1", 8001);
	ax25_plsm_start(p);
#endif

#if 0
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
