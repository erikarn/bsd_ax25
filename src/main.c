#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <event2/event.h>

#include "conn.h"

int
main(int argc, const char *argv[])
{
	struct event_base *eb;
	struct proto_conn *k;
	struct sockaddr_storage s;
	struct sockaddr_in *sin;

	eb = event_base_new();

	k = conn_create(eb);
	/* For now, assume localhost:8001 */
	sin = (void *) &s;
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	sin->sin_port = htons(8001);

	conn_set_peer(k, (void *) sin);

	/* For now, assume ipv4 */
	sin = (void *) &s;
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = htonl(INADDR_ANY);
	sin->sin_port = htons(0);

	conn_set_lcl(k, (void *) sin);

	conn_setup(k);
	conn_connect(k);

	event_base_loop(eb, EVLOOP_NO_EXIT_ON_EMPTY);

	exit(0);
}
