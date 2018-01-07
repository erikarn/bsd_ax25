#ifndef	__AX25_LMSM_H__
#define	__AX25_LMSM_H__

typedef enum {
	AX25_LMSM_STATE_IDLE = 0,
	AX25_LMSM_SEIZE_PENDING = 1,
	AX25_LMSM_SEIZED = 2,
} ax25_lmsm_state_t;

struct ax25_plsm;
struct ax25_dlsm;
struct ax25_lmsm;

struct ax25_lmsm {
	struct ebase *eb;

	/* Awaiting queue */
	struct ax25_pkt_list *await_q;

	/* Served queue */
	struct ax25_pkt_list *served_q;

	/* Served list */
	struct ax25_pkt_list *served_list_q;

	/* State - 0 (idle), 1 (seize pending), 2 (seized) */
	int state;

	/* Which DLSM is using the transmitter */
	struct ax25_dlsm *dlsm_active;

	/* An array of DLSM entities which have been seen */
	/* (yes, it's a static array for now) */
	struct {
		int n;
		int max;
		struct ax25_dlsm *list;
	} dlsm_served_list;

	/* PLSM link */
	struct {
		struct ax25_plsm *pl;
	} plsm;

	/*
	 * If no DLSM has been found (eg UI, unknown source)
	 * then this DLSM receives the data frames in question.
	 *
	 * It will then take care of figuring out what to do -
	 * eg, passing up UI frames to layer 3; handling
	 * incoming requests and creating a DLSM instance
	 * just for that peer, etc, etc.
	 */
	struct ax25_dlsm *dlsm_default;
};

extern	int ax25_lmsm_seize_request(struct ax25_lmsm *, struct ax25_dlsm *);
extern	int ax25_lmsm_seize_confirm(struct ax25_lmsm *, struct ax25_dlsm *);
extern	int ax25_lmsm_data_request(struct ax25_lmsm *, struct ax25_dlsm *,
	    struct ax25_pkt *);
extern	int ax25_lmsm_data_indication(struct ax25_lmsm *, struct ax25_dlsm *,
	    struct ax25_pkt *);

/* Temporary non-standard methods - Adding/removing DLSM instances as appropriate */
extern	int ax25_lmsm_add_dlsm(struct ax25_lmsm *, struct ax25_dlsm *);
extern	int ax25_lmsm_remove_dlsm(struct ax25_lmsm *, struct ax25_dlsm *);

/* And these add a default DLSM for handling UI and incoming connection requests */
extern	int ax25_lmsm_add_default_dlsm(struct ax25_lmsm *, struct ax25_dlsm *);
extern	int ax25_lmsm_remove_default_dlsm(struct ax25_lmsm *, struct ax25_dlsm *);

#endif	/* __AX25_LMSM_H__ */
