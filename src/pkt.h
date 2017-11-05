#ifndef	__PKT_H__
#define	__PKT_H__

struct pkt;
typedef int pkt_pp_free_cb(struct pkt *p, void *cbdata);

enum {
	PKT_TYPE_NONE,
	PKT_TYPE_L2_AX25,
	PKT_TYPE_L3_APRS_MONITOR,
};

/*
 * A single packet, both the untouched payload and for at least
 * receive, an optional parsed out payload.
 *
 * For now, no scatter/gather, so one frame is a single packet.
 * (ie, no fragments, mbuf style.)
 *
 * The parsed payload makes this packet a kind of useful generic
 * representation of "stuff" that can be thrown around, rather than
 * having to try and maintain multiple queue types.  Yes, mumble
 * C++, etc, etc.
 */
struct pkt {
	TAILQ_ENTRY(pkt) int_list;

	/* Original payload */
	struct buf *buf;

	uint16_t type;
	uint16_t subtype;

	/*
	 * This is the per-packet payload.
	 *
	 * The intent is to have decoders do the bulk of the
	 * decoding earlier and throw it in here so various
	 * consumers don't have to go through the complicated
	 * steps of continuously decoding the frame.
	 *
	 * Yes, this may get out of whack with the original payload.
	 * I will likely eventually regret this and put a strict
	 * separation between original payload and parsed payloads..
	 */
	void *pp_info;

	/*
	 * This is the per-protocol state handler for the pp_info
	 * frame.
	 *
	 * Mostly it's to handle freeing potentially complicated parsed
	 * out binary data formats in case they're complicated.
	 */
	struct {
		void *arg;
		pkt_pp_free_cb *free_cb;
	} pp_cb;
};

extern	struct pkt * pkt_create(int maxlen);
extern	void pkt_free(struct pkt *);

#endif	/* __PKT_H__ */
