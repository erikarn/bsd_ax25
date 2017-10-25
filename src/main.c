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

int
main(int argc, const char *argv[])
{
	struct proto_kiss *p;
	struct ebase eb;

	eb.ebase = event_base_new();
	eb.edns = evdns_base_new(eb.ebase, 1);

	p = proto_kiss_create(&eb);

	proto_kiss_set_host(p, "127.0.0.1", 8001);
	proto_kiss_connect(p);

	event_base_loop(eb.ebase, EVLOOP_NO_EXIT_ON_EMPTY);

	exit(0);
}
