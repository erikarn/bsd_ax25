#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <netinet/in.h>

#include <event2/event.h>
#include <event2/dns.h>

#include "eb.h"
#include "buf.h"
#include "conn.h"
#include "proto_kiss.h"
#include "proto_aprs_igate.h"

int
main(int argc, const char *argv[])
{
//	struct proto_kiss *p;
	struct proto_aprs_igate *pg;
	struct ebase eb;

	eb.ebase = event_base_new();
	eb.edns = evdns_base_new(eb.ebase, 1);

#if 0
	p = proto_kiss_create(&eb);
	proto_kiss_set_host(p, "127.0.0.1", 8001);
	proto_kiss_connect(p);
#endif

	pg = proto_aprs_igate_create(&eb);
	proto_aprs_igate_set_login(pg, "KK6VQK", "-1");
	proto_aprs_igate_set_host(pg, "rotate.aprs2.net", 14580);

	pg->filter_settings.filt_lat = 37.76;
	pg->filter_settings.filt_long = -122.19;
	pg->filter_settings.filt_range = 1000;

	proto_aprs_igate_connect(pg);

	event_base_loop(eb.ebase, EVLOOP_NO_EXIT_ON_EMPTY);

	exit(0);
}
