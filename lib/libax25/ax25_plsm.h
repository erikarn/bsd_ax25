#ifndef	__AX25_PLSM_H__
#define	__AX25_PLSM_H__

/*
 * This is the public API boundary of the AX.25 PLSM.
 *
 * It does little more than provide function pointers
 * and state pointers for an implementation of the PLSM.
 *
 * The owner is intended to be a link multiplexer; it will
 * add function pointers and arg state as appropriate.
 *
 * The client is the relevant implementation of the physical
 * link state.  This can be a KISS TNC, AGWPE, a 1200 baud
 * software modem implementation, etc.
 *
 * The application should:
 *
 * + create an instance of the PLSM
 * + create a client instance of something - like PLSM KISS TNC glue
 * + create an instance of the LMSM
 * + link all the function pointers up, manually for now!
 */

struct ax25_plsm;
struct ax25_pkt;

typedef int ph_seize_request_t(struct ax25_plsm *);
typedef int ph_seize_confirm_t(struct ax25_plsm *);
typedef int ph_data_request_t(struct ax25_plsm *, struct ax25_pkt *);
typedef int ph_release_request_t(struct ax25_plsm *);
typedef int ph_expedited_data_request_t(struct ax25_plsm *, struct ax25_pkt *);
typedef int ph_data_indication_t(struct ax25_plsm *, struct ax25_pkt *);
typedef int ph_busy_indication_t(struct ax25_plsm *);
typedef int ph_quiet_indication_t(struct ax25_plsm *);

typedef int ph_client_free_t(struct ax25_plsm *);

#define	AX25_PLSM_CLIENT_ARG(a)		((a)->client.arg)
#define	AX25_PLSM_OWNER_ARG(a)		((a)->owner.arg)

struct ax25_plsm {
	struct ebase *eb;

	/* For now the implementation takes care of queue management, state, etc */
#if 0
	struct ax25_pkt_list *priority_q;
	struct ax25_pkt_list *normal_q;
#endif

	struct {
		void *arg;

		/* AX25 methods relevant to the LMSM call methods */
		ph_seize_request_t * ph_seize_request_cb;
		ph_data_request_t * ph_data_request_cb;
		ph_release_request_t * ph_release_request_cb;
		ph_expedited_data_request_t * ph_expedited_data_request_cb;

		/* extra state stuff */
		ph_client_free_t * ph_client_free_cb;
	} client;

	struct {
		void *arg;

		/* AX.25 methods relevant to the LMSM notification */
		ph_seize_confirm_t * ph_seize_confirm_cb;
		ph_data_indication_t * ph_data_indication_cb;
		ph_busy_indication_t * ph_busy_indication_cb;
		ph_quiet_indication_t * ph_quiet_indication_cb;
	} owner;
};

extern	struct ax25_plsm * ax25_plsm_create(struct ebase *eb);
extern	void ax25_plsm_free(struct ax25_plsm *p);

extern	int ax25_plsm_seize_request(struct ax25_plsm *p);
extern	int ax25_plsm_seize_confirm(struct ax25_plsm *p);

#endif	/* __AX25_PLSM_H__ */
