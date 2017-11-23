#ifndef	__AX25_LMSM_H__
#define	__AX25_LMSM_H__

typedef enum {
	AX25_LMSM_STATE_IDLE = 0,
	AX25_LMSM_SEIZE_PENDING = 1,
	AX25_LMSM_SEIZED = 2,
} ax25_lmsm_state_t;

struct ax25_lmsm {
	struct ebase *eb;

	/* Awaiting queue */

	/* Served queue */

	/* Served list */

	/* State - 0 (idle), 1 (seize pending), 2 (seized) */
	int state;

	/* Which DLSM is using the transmitter */
};

#endif	/* __AX25_LMSM_H__ */
