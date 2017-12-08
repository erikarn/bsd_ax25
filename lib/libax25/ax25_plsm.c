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

	if (p->client.ph_client_free_cb != NULL)
		p->client.ph_client_free_cb(p);
	free(p);
}

int
ax25_plsm_seize_request(struct ax25_plsm *p)
{

	return (p->client.ph_seize_request_cb(p));
}

int
ax25_plsm_seize_confirm(struct ax25_plsm *p)
{

	return (p->owner.ph_seize_confirm_cb(p));
}

int
ax25_plsm_data_request(struct ax25_plsm *p, struct ax25_pkt *pkt)
{

	return (p->client.ph_data_request_cb(p, pkt));
}

int
ax25_plsm_release_request(struct ax25_plsm *p)
{

	return (p->client.ph_release_request_cb(p));
}

int
ax25_plsm_expedited_data_request(struct ax25_plsm *p, struct ax25_pkt *pkt)
{

	return (p->client.ph_expedited_data_request_cb(p, pkt));
}

int
ax25_plsm_data_indication(struct ax25_plsm *p, struct ax25_pkt *pkt)
{

	return (p->owner.ph_data_indication_cb(p, pkt));
}

int
ax25_plsm_busy_indication(struct ax25_plsm *p)
{

	return (p->owner.ph_busy_indication_cb(p));
}

int
ax25_plsm_quiet_indication(struct ax25_plsm *p)
{

	return (p->owner.ph_quiet_indication_cb(p));
}

int
ax25_plsm_start(struct ax25_plsm *p)
{

	return (p->client.ph_client_start_cb(p));
}

int
ax25_plsm_stop(struct ax25_plsm *p)
{

	return (p->client.ph_client_stop_cb(p));
}
