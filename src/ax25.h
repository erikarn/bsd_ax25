#ifndef	__AX25_PKT_H__
#define	__AX25_PKT_H__

struct pkt_ax25 {
	/* This is the original packet payload; yes should be buf_t */
	char *buf;
	int len;

	/* Offset inside the buffer for the beginning of things */
	int da_offset; /* offset for destination address */
	int sa_offset; /* offset for source address (single, list, etc) */
	int ctrl_offset;	/* offset of control field */

	int pid_offset;		/* offset of PID field, or -1 */
	int info_offset;	/* offset for start of info, or -1 */

	/* other things */
	int num_srcs_addresses;	/* how many source addresses are there */
};

extern	int ax25_pkt_parse(const uint8_t *buf, int len);

#endif	/* __AX25_PKT_H__ */

