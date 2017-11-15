#ifndef	__AX25_PKT_H__
#define	__AX25_PKT_H__

/* Maximum number of repeaters is 2, but we will handle up to 8 */
#define	AX25_MAX_NUM_REPEATERS		8

struct ax25_address {
	char callsign[6];	/* 6 character callsign, space padded */
	uint8_t ssid;		/* SSID field, C/M bits, etc */
};

/*
 * Note - this isn't a binary representation of an ax25 packet.
 * This is designed for ease of programming, not ease of encode/decode.
 */
struct pkt_ax25 {

	/* First field is dest address */
	struct ax25_address dest_addr;

	/* Second field is source address */
	struct ax25_address source_addr;

	/* Then there are optional repeater addresses */
	struct ax25_address repeater_addr_list[AX25_MAX_NUM_REPEATERS];
	int repeater_addr_count;

	/* depending upon the frame type, ctrl may be 8 or 16 bits */
	uint16_t ctrl;

	/* depending upon the frame type, pid may actually be a value */
	uint8_t pid;

	/* TODO: info / payload field */
};

extern	struct pkt_ax25 * ax25_pkt_parse(const uint8_t *buf, int len);

extern	int ax25_addr_assign(struct ax25_address *a, const char *b,
	    uint8_t ssid);
extern	void ax25_addr_copy(struct ax25_address *dst,
	    const struct ax25_address *src);

extern	struct pkt_ax25 * pkt_ax25_create(void);
extern	void pkt_ax25_free(struct pkt_ax25 *);

#endif	/* __AX25_PKT_H__ */

