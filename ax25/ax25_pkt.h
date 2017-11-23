#ifndef	__AX25_PKT_H__
#define	__AX25_PKT_H__

struct ax25_pkt;
struct ax25_pkt {
	TAILQ_ENTRY(ax25_pkt) n;
	struct buf *buf;
};

extern	struct ax25_pkt * ax25_pkt_create(int maxlen);
extern	void ax25_pkt_free(struct ax25_pkt *);

#endif	/* __AX25_PKT_H__ */
