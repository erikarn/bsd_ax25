#ifndef	__AX25_PKT_LIST_H__
#define	__AX25_PKT_LIST_H__

struct ax25_pkt;

struct ax25_pkt_list {
	TAILQ_HEAD(ax25_pkt_list_head, ax25_pkt) list;
	int n;
	int max;
};

extern	struct ax25_pkt_list * ax25_pkt_list_create(int max);
extern	void ax25_pkt_list_free(struct ax25_pkt_list *);

extern	int ax25_pkt_list_queue_head(struct ax25_pkt_list *, struct ax25_pkt *);
extern	int ax25_pkt_list_queue_tail(struct ax25_pkt_list *, struct ax25_pkt *);

extern	struct ax25_pkt * ax25_pkt_list_remove_head(struct ax25_pkt_list *);
extern	struct ax25_pkt * ax25_pkt_list_remove_tail(struct ax25_pkt_list *);

extern	int ax25_pkt_list_num(struct ax25_pkt_list *);
extern	int ax25_pkt_list_has_space(struct ax25_pkt_list *);

extern	int ax25_pkt_list_flush(struct ax25_pkt_list *);

#endif	/* __AX25_PKT_LIST_H__ */
