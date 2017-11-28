#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>

#include <sys/queue.h>

#include "buf.h"
#include "ax25_pkt.h"
#include "ax25_pkt_list.h"

#include "ax25_lmsm.h"
#include "ax25_dlsm.h"

/*
 * This implements the Link Multiplexer State Machine.
 *
 * This is the interface between the physical state machine and
 * instances of DLSMs.
 *
 * There are multiple possible DLSM instances which all share
 * the same LMSM.  They'll (for now) be on a linked list so the
 * LMSM can iterate through them to do things like correctly
 * deliver frames to the correct DLSM instance (eg in the case of
 * supporting multiple connections), and a pass through for UI
 * frames.
 *
 * This isn't the implementation of the LMSM - it is just the
 * base class information and function pointers to an implementation.
 * This allows alternate implementations of the LMSM to be tried -
 * eg during unit testing.
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

	a->await_q = ax25_pkt_list_create(100);
	if (a->await_q == NULL)
		goto error;

	a->served_q = ax25_pkt_list_create(100);
	if (a->served_q == NULL)
		goto error;

	a->served_list_q = ax25_pkt_list_create(100);
	if (a->served_list_q == NULL)
		goto error;

	return (a);
error:
	if (a->await_q != NULL)
		ax25_pkt_list_free(a->await_q);
	if (a->served_q != NULL)
		ax25_pkt_list_free(a->served_q);
	if (a->served_list_q != NULL)
		ax25_pkt_list_free(a->served_list_q);
	free(a);
	return (NULL);
}

void
ax25_lmsm_free(struct ax25_lmsm *a)
{

	/* Free frames on the queue */

	free(a);
}

/* Routines / interactions that are provided up to the DLSM */

/*
 * LM-SEIZE request
 *
 * This is called by the DLSM to force acquisition of the channel.
 */
int
ax25_lmsm_seize_request(struct ax25_lmsm *l, struct ax25_dlsm *d)
{

	/*
	 * Inform the underlying PLSM that we need to acquire the
	 * channel.
	 */
	return (-1);
}

/*
 * LM-SEIZE confirm
 *
 * This is called by the LMSM to inform the DLSM that acquisition has
 * occured.
 */
int
ax25_lmsm_seize_confirm(struct ax25_lmsm *l, struct ax25_dlsm *d)
{

	/*
	 * Inform the requesting DLSM that they now have access to the channel.
	 */
	return (-1);
}

/*
 * LM-DATA request
 *
 * This is called by the DLSM to queue frames.
 */
int
ax25_lmsm_data_request(struct ax25_lmsm *l, struct ax25_dlsm *d,
    struct ax25_pkt *p)
{

	ax25_pkt_free(p);
	return (-1);
}

/*
 * LM-DATA indication
 *
 * This is called by the LMSM to inform the DLSM that this frame
 * has been received.
 *
 * It will loop over the list of DLSMs to find one with which to
 * send frames to.  Yes, it's O(n) but it can be addressed later
 * on to be more efficient.
 */
int
ax25_lmsm_data_indication(struct ax25_lmsm *l, struct ax25_dlsm *d,
    struct ax25_pkt *p)
{

	ax25_pkt_free(p);
	return (-1);
}
