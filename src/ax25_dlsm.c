#include <stdio.h>
#include <stdlib.h>

/*
 * AX.25 DLSM.
 *
 * This implements both halves of the DLSM API.
 *
 * + AX.25 2.2 section 5.3 (L3 entity <-> DLSM.)
 * + AX.25 2.2 section 5.4 (DLSM <-> Link multiplexer.)
 *
 * L3 entities issue the below rrequests and can register for
 * indications.  Some are per-L3 connection; some are global.
 */

/* DL-CONNECT request */

/* DL-CONNECT indication */

/* DL-CONNECT confirm */

/* DL-DISCONNECT request */

/* DL-DISCONNECT indication */

/* DL-DISCONNECT confirm */

/* DL-DATA request */

/* DL-DATA indication */

/* DL-UNIT-DATA request */

/* DL-UNIT-DATA indication */

/* DL-ERROR indication */

/* DL-FLOW-OFF request */

/* DL-FLOW-ON request */


/*
 * And this is the LMSM communication to an underlying interface (KISS, TNC, etc.)
 */

/* LM-SEIZE request */

/* LM-SEIZE confirm */

/* LM-RELEASE request */

/* LM-EXPEDITED-DATA request */

/* LM-DATA request */

/* LM-DATA indication */
