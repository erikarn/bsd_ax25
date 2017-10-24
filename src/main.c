#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <netinet/in.h>
#include <event2/event.h>

#include "buf.h"
#include "conn.h"
#include "proto_kiss.h"

int
main(int argc, const char *argv[])
{
	struct event_base *eb;
	struct proto_kiss *p;

	eb = event_base_new();

	p = proto_kiss_create(eb);

	proto_kiss_set_host(p, "127.0.0.1", 8001);
	proto_kiss_connect(p);

	event_base_loop(eb, EVLOOP_NO_EXIT_ON_EMPTY);

	exit(0);
}
