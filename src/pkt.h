#ifndef	__PKT_H__
#define	__PKT_H__

enum {
	PKT_TYPE_NONE,
	PKT_TYPE_L2_AX25,
	PKT_TYPE_L3_APRS_MONITOR,
};

/*
 * A single L2 packet.
 *
 * For now, no scatter/gather, so one frame is a single packet.
 * (ie, no fragments, mbuf style.)
 */
struct pkt {
	TAILQ_ENTRY(pkt) int_list;
	struct buf *buf;

	uint16_t type;
	uint16_t subtype;

	/* XXX TODO: per protocol state, etc */
};

extern	struct pkt * pkt_create(int maxlen);
extern	void pkt_free(struct pkt *);

#endif	/* __PKT_H__ */
