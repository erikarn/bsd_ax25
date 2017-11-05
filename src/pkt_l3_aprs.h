#ifndef	__PKT_L3_APRS_MONITOR_H__
#define	__PKT_L3_APRS_MONITOR_H__

/*
 * This is the representation of a parsed L3 APRS monitor frame.
 */
struct pkt_l3_aprs_monitor {
	/* The initial payload */
	char *src;
	char *path;
	char *payload;
};

#endif	/* __PKT_L3_APRS_MONITOR_H__ */
