#ifndef	__AX25_PLSM_H__
#define	__AX25_PLSM_H__

/*
 * This is the public API boundary of the AX.25 PLSM.
 *
 * It does little more than provide function pointers
 * and state pointers for an implementation of the PLSM.
 */
struct ax25_plsm {
	struct ebase *eb;
};

#endif	/* __AX25_PLSM_H__ */
