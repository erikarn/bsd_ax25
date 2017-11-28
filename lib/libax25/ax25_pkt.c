#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>
#include <sys/queue.h>

#include "buf.h"
#include "ax25_pkt.h"

struct ax25_pkt *
ax25_pkt_create(int maxlen)
{
	struct ax25_pkt *p;

	p = calloc(1, sizeof(*p));
	if (p == NULL) {
		warn("%s: calloc", __func__);
		return (NULL);
	}

	p->buf = buf_create(maxlen);
	if (p->buf == NULL) {
		free(p);
		return (NULL);
	}

	return (p);
}

void
ax25_pkt_free(struct ax25_pkt *a)
{

	if (a->buf != NULL)
		buf_free(a->buf);
	free(a);
}
