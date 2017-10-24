#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

#include <sys/queue.h>

#include <event2/event.h>

#include "buf.h"
#include "buf_list.h"
#include "conn.h"
#include "proto_kiss.h"

/*
 * This implements a connection to a KISS TNC -
 * eventually being an AX.25 protocol interface.
 *
 * For now it assumes a TCP connection but it should
 * be easy to extend to do serial, udp, i2c, etc.
 *
 * However, until the interface bits are up, this
 * will primarily just be a mostly-standalone thingy
 * to encap/decap KISS.
 */

struct proto_kiss *
proto_kiss_create(struct event_base *eb)
{
	struct proto_kiss *k;

	k = calloc(1, sizeof(*k));
	if (k == NULL) {
		warn("%s: calloc", __func__);
		goto err;
	}

	k->conn = conn_create(eb);
	if (k->conn == NULL)
		goto err;

	k->eb = eb;
	return (k);

err:
	if (k->conn)
		conn_free(k->conn);
	if (k->host)
		free(k->host);
	free(k);
	return (NULL);
}

void
proto_kiss_free(struct proto_kiss *k)
{
	if (k->conn)
		conn_close(k->conn);
	if (k->host)
		free(k->host);
	free(k);
}

int
proto_kiss_set_host(struct proto_kiss *k, const char *host, int port)
{

	return (-1);
}

int
proto_kiss_connect(struct proto_kiss *k)
{

	return (-1);
}

int
proto_kiss_disconnect(struct proto_kiss *k)
{

	return (-1);
}
