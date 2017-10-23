#include <stdio.h>
#include <stdlib.h>

#include <event2/event.h>

#include "kiss.h"

int
main(int argc, const char *argv[])
{
	struct event_base *eb;
	struct proto_kiss *k;

	eb = event_base_new();

	k = kiss_create(eb);
	kiss_setup(k);
	kiss_connect(k);

	event_base_loop(eb, EVLOOP_NO_EXIT_ON_EMPTY);

	exit(0);
}
