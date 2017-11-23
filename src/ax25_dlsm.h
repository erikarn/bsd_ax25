#ifndef	__AX25_DLSM_H__
#define	__AX25_DLSM_H__

/*
 * For now we're only really doing UI, so the implementation is pretty
 * minimal.
 */

struct ax25_dlsm {
	struct ebase *eb;

	struct {
		void *arg;
	} lmsm;
};


#endif	/* __AX25_DLSM_H__ */
