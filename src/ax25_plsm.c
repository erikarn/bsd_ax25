#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include "ax25_plsm.h"

/*
 * This implements the Physical Link State Machine.
 */

/*
 * For BSD-AX25 this is a generic interface for queuing and dequeuing
 * AX.25 frames from a TNC (eg a KISS TNC) which doesn't provide a full
 * AX.25 stack.
 */

struct ax25_plsm *
ax25_plsm_create(struct ebase *eb)
{
	struct ax25_plsm *a;

	a = calloc(1, sizeof(*a));
	if (a == NULL) {
		warn("%s: calloc", __func__);
		return (NULL);
	}
	a->eb = eb;

	/* here's where the queues would be created */

	return (a);
}

void
ax25_plsm_free(struct ax25_plsm *a)
{

	/* Free frames on the queue */
	free(a);
}

/* PH-SEIZE request */
/* PH-SEIZE confirm */
/* PH-RELEASE request */
/* PH-EXPEDITED-DATA request */
/* PH-DATA request */
/* PH-DATA indication */
