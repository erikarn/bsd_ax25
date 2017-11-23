#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include "ax25_lmsm.h"

/*
 * This implements the Link Multiplexer State Machine.
 *
 * This is the interface between the physical state machine and
 * the upper layer Data Link State Machine.
 */

/*
 * For BSD-AX25 this is a generic interface for queuing and dequeuing
 * AX.25 frames from a TNC (eg a KISS TNC) which doesn't provide a full
 * AX.25 stack.
 */

struct ax25_lmsm *
ax25_lmsm_create(struct ebase *eb)
{
	struct ax25_lmsm *a;

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
ax25_lmsm_free(struct ax25_lmsm *a)
{

	/* Free frames on the queue */
	free(a);
}

/* Routines / interactions that are provided up to the DLSM */

/* LM-SEIZE request */
/* LM-SEIZE confirm */
/* LM-DATA request */
/* LM-DATA indication */

/* .. and what it calls in the physical layer handler */

/* PH-SEIZE request */
/* PH-SEIZE confirm */
/* PH-RELEASE request */
/* PH-EXPEDITED-DATA request */
/* PH-DATA request */
/* PH-DATA indication */
