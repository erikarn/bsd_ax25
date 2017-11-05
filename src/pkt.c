#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>

#include <sys/queue.h>

#include "buf.h"
#include "pkt.h"

struct pkt *
pkt_create(int maxlen)
{
	struct pkt *p;

	p = calloc(1, sizeof(*p));
	if (p == NULL) {
		warn("%s: calloc", __func__);
		return (NULL);
	}

	p->buf = buf_create(maxlen);
	if (p->buf == NULL) {
		goto error;
	}

	return p;

error:
	if (p->buf)
		buf_free(p->buf);
	free(p);
	return (NULL);
}

/*
 * Free a packet.
 *
 * This assumes it's no longer on any interface list or owned by anyone.
 */
void
pkt_free(struct pkt *p)
{

	if (p->pp_cb.free_cb != NULL)
		p->pp_cb.free_cb(p, p->pp_cb.arg);

	if (p->buf)
		buf_free(p->buf);
	free(p);
}
