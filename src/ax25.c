#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "ax25.h"
#include "kiss.h"
#include "ax25_pkt_examples.h"

/*
 * Print an ax.25 address field - callsign, ssid field, C/H bits.
 *
 * return 1 if there's another address following, 0 if this is the
 * final address field.  -1 if there's an error.
 */
static int
ax25_pkt_address_parse(struct ax25_address *a, const uint8_t *buf, int len)
{
	char callsign[8];
	int i;

	if (len < 7)
		return (-1);

	memset(callsign, 0, sizeof(callsign));

	for (i = 0; i < 6; i++) {
		callsign[i] = buf[i] >> 1;
		/* we shouldn't get an end of address here */
		if (buf[i] & 0x1)
			return (-1);
	}

	ax25_addr_assign(a, callsign, buf[7]);

	printf("%s-%d", callsign, buf[7] & 0xf);

	return ((buf[7] & 0x01) == 0);
}

/*
 * Note: this is the modulo-8 version of the control parser.
 * Modulo-128 (a 16 bit control field) allows for more outstanding
 * frames to be acked, however it is negotiated between TNCs.
 *
 * From what I can tell, this is negotiated between two TNCs.
 * I can't see anything in the frame that tells us explicitly which
 * mode to decode as; it looks like it's something specific to a
 * pair of TNCs talking.
 *
 * Which makes parsing a pain in the ass, because you can't go
 * and parse out the the frame entirely until you query some higher
 * session management layer to know whether data from this particular
 * source has negotiated certain options.  Sigh.
 *
 * For KISS TNCs (eg direwolf), I'm not sure whether direwolf is
 * doing the data-link layer negotiation or not.  I have a feeling
 * that it isn't, and it's up to this layer to do all of the heavy
 * lifting.
 *
 * ... which is fine, as for now we can just reject negotiating
 * modulo 128 and worry about how to teach this code about it later.
 *
 * TODO: go re-re-read the ax25 specification to better understand
 * when/how to flip up to modulo 128.
 */
static int
ax25_pkt_control_print(uint8_t ctrl)
{
	switch (ctrl & 0x3) {
		case 0:		/* low bit is 0, rest is i-frame */
		case 2:
			printf("I-frame; N(S)=%d, P=%d, N(R)=%d",
			    (ctrl & 0xe) >> 1,
			    (ctrl & 0x10) >> 4,
			    (ctrl & 0xe0) >> 5);
			break;
		case 1:		/* S-frame */
			printf("S-frame; S=%d, P/F=%d, N(R)=%d",
			    (ctrl & 0xc) >> 2,
			    (ctrl & 0x10) >> 4,
			    (ctrl & 0xe0) >> 5);
			break;
		case 3:		// U-frame */
			printf("U-frame: M-field: 0x%.2x", ctrl);
			break;
	}
	return (0);
}

/*
 * Parse an AX.25 payload.
 */
struct pkt_ax25 *
ax25_pkt_parse(const uint8_t *buf, int len)
{
	int i, r;
	i = 0;
	struct pkt_ax25 *pkt;
	int naddr = 0;

	printf("%s: total length: %d\n", __func__, len);
	if (len == 0)
		return (NULL);

	pkt = pkt_ax25_create();
	if (pkt == NULL) {
		return (NULL);
	}

	/* First up are the address field(s) - note, SSID is the 7th byte */
	/*
	 * And note there's a 'H' bit in SSID indicating it's been repeated
	 * through a repeater.
	 */
	do {
		struct ax25_address a;

		/* Parse out an address */
		r = ax25_pkt_address_parse(&a, buf + i, len - i);
		if (r < 0) {
			/* XXX parser error */
			break;
		}

		if (naddr == 0) {
			ax25_addr_copy(&pkt->dest_addr, &a);
		} else if (naddr == 1) {
			ax25_addr_copy(&pkt->source_addr, &a);
		} else if (pkt->repeater_addr_count > AX25_MAX_NUM_REPEATERS) {
			/* Too many addresses to parse */
			fprintf(stderr, "%s: too many addresses\n", __func__);
			goto fail;
		} else {
			ax25_addr_copy(&pkt->repeater_addr_list[pkt->repeater_addr_count], &a);
			pkt->repeater_addr_count++;
		}
		naddr++;

		printf(" ");
		i = i + 7;
		if (r < 1)
			break;
	} while (i < len);
	printf(": ");
	if (i >= len) {
		goto end;
	}

	/* Following byte is the control byte */
	/* XXX TODO: should handle PID escape sequences, multi-byte protocols */
	pkt->ctrl = buf[i];
	i++;

	/*
	 * Only I-frames have a PID.
	 */
	if (AX25_CTRL_FRAME_I(pkt->ctrl)) {
		if (i >= len)
			goto fail;
		/* Note this isn't strictly true - PID can be multi-byte! */
		pkt->pid = buf[i];
		if (pkt->pid == 0xff) {
			fprintf(stderr, "%s: multi-byte PID not supported yet\n",
			    __func__);
			goto fail;
		}
		i++;
		if (i >= len)
			goto fail;
	}

	/* Rest of the payload is info */

end:
	printf("\n");
	return (pkt);
fail:
	if (pkt)
		pkt_ax25_free(pkt);
	return (NULL);
}

int
ax25_addr_assign(struct ax25_address *a, const char *b, uint8_t ssid)
{

	memcpy(a->callsign, b, 6);
	a->ssid = ssid;
	return (0);
}

void
ax25_addr_copy(struct ax25_address *dst, const struct ax25_address *src)
{

	memcpy(dst, src, sizeof(*dst));
}


struct pkt_ax25 *
pkt_ax25_create(void)
{
	struct pkt_ax25 *p;

	p = calloc(1, sizeof(*p));
	if (p == NULL) {
		warn("%s: calloc", __func__);
		return (NULL);
	}
	return (p);
}

void
pkt_ax25_free(struct pkt_ax25 *p)
{

	free(p);
}

void
pkt_ax25_print(struct pkt_ax25 *p)
{
	int i;

	printf("  source: %.*s-%d (flags 0x%.2x)\n",
	    6, p->source_addr.callsign,
	    p->source_addr.ssid & 0xf,
	    p->source_addr.ssid);

	printf("  destination: %.*s-%d (flags 0x%.2x)\n",
	    6, p->dest_addr.callsign,
	    p->dest_addr.ssid & 0xf,
	    p->dest_addr.ssid);

	for (i = 0; i < p->repeater_addr_count; i++) {
		printf("  repeater[%d]: %.*s-%d (flags 0x%.2x)\n",
		    i,
		    6, p->repeater_addr_list[i].callsign,
		    p->repeater_addr_list[i].ssid & 0xf,
		    p->repeater_addr_list[i].ssid);
	}

	printf("  Control: ");
	ax25_pkt_control_print(p->ctrl);
	if (AX25_CTRL_FRAME_I(p->ctrl)) {
		/* Note this isn't strictly true - PID can be multi-byte! */
		printf("  PID: %.2x", p->pid);
		printf("\n");
	}
	printf("\n");
}

#ifdef STANDALONE
int
main(int argc, const char *argv[])
{
	uint8_t kbuf[1024];
	int kbuf_len;
	struct pkt_ax25 *pkt;

	kiss_payload_parse(kiss_ax25_aprs_example_1, sizeof(kiss_ax25_aprs_example_1),
	    kbuf, &kbuf_len);
	pkt = ax25_pkt_parse(kbuf, kbuf_len);
	if (pkt) {
		pkt_ax25_print(pkt);
		pkt_ax25_free(pkt);
	}

	kiss_payload_parse(kiss_ax25_aprs_example_2, sizeof(kiss_ax25_aprs_example_2),
	    kbuf, &kbuf_len);
	pkt = ax25_pkt_parse(kbuf, kbuf_len);
	if (pkt) {
		pkt_ax25_print(pkt);
		pkt_ax25_free(pkt);
	}
	kiss_payload_parse(kiss_ax25_aprs_example_3, sizeof(kiss_ax25_aprs_example_3),
	    kbuf, &kbuf_len);
	pkt = ax25_pkt_parse(kbuf, kbuf_len);
	if (pkt) {
		pkt_ax25_print(pkt);
		pkt_ax25_free(pkt);
	}


}
#endif


