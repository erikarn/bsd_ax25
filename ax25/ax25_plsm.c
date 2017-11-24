#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>

#include <sys/queue.h>

#include "eb.h"
#include "ax25_plsm.h"

struct ax25_plsm *
ax25_plsm_create(struct ebase *eb)
{
	struct ax25_plsm *p;

	p = calloc(1, sizeof(*p));
	if (p == NULL) {
		warn("%s: calloc", __func__);
		return (NULL);
	}

	p->eb = eb;

	return (p);
}

void
ax25_plsm_free(struct ax25_plsm *p)
{

	p->owner.ph_owner_free_cb(p);
	free(p);
}

int
ax25_plsm_seize_request(struct ax25_plsm *p)
{

	return (p->owner.ph_seize_request_cb(p));
}

int
ax25_plsm_seize_confirm(struct ax25_plsm *p)
{

	return (p->client.ph_seize_confirm_cb(p));
}
