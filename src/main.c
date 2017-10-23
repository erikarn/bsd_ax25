#include <stdio.h>
#include <stdlib.h>

#include <event2/event.h>

#include "conn.h"

int
main(int argc, const char *argv[])
{
	struct event_base *eb;
	struct proto_conn *k;

	eb = event_base_new();

	k = conn_create(eb);
	conn_setup(k);
	conn_connect(k);

	event_base_loop(eb, EVLOOP_NO_EXIT_ON_EMPTY);

	exit(0);
}
