#ifndef	__PKT_L3_APRS_MONITOR_H__
#define	__PKT_L3_APRS_MONITOR_H__

/*
 * This is the representation of a parsed L3 APRS monitor frame.
 */
struct pkt_l3_aprs {

	/* aprs source */
	char *src;

	/* aprs path */
	char *path;

	/* the rest of the payload */
	char *payload;
};

extern	struct pkt_l3_aprs * pkt_l3_aprs_create(void);
extern	void pkt_l3_aprs_free(struct pkt_l3_aprs *);

extern	int pkt_l3_aprs_set_src(struct pkt_l3_aprs *l,
	    const char *buf, int len);
extern	int pkt_l3_aprs_set_path(struct pkt_l3_aprs *l,
	    const char *buf, int len);
extern	int pkt_l3_aprs_set_payload(struct pkt_l3_aprs *l,
	    const char *buf, int len);

#endif	/* __PKT_L3_APRS_MONITOR_H__ */
