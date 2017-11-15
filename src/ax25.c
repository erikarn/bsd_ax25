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
ax25_pkt_address_parse(const uint8_t *buf, int len)
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

	printf("%s-%d", callsign, buf[7] & 0xf);

	return ((buf[7] & 0x01) == 0);
}

static int
ax25_pkt_control_parse(uint8_t ctrl)
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
	uint8_t ctrl, pid;
	i = 0;
	struct pkt_ax25 *pkt;

	printf("%s: total length: %d\n", __func__, len);

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
		r = ax25_pkt_address_parse(buf + i, len - i);
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
	ax25_pkt_control_parse(buf[i]);
	ctrl = buf[i];
	i++;

	printf(": ");

	/* XXX hack, yes, for i-frames and payload */
	if (((buf[i] & 0x3) == 0) || ((buf[i] & 0x3) == 2)) {
		if (i >= len)
			goto end;

		pid = buf[i];
		i++;

		printf("PID: 0x%.2x: ", pid);
		if (i >= len)
			goto end;

		printf("%.*s", len - i, &buf[i]);
	}

	/* PID, info, etc if appropriate */

end:
	printf("\n");

	return (pkt);
}

int
ax25_addr_assign(struct ax25_address *a, const char *b, uint8_t ssid)
{

	memcpy(a->callsign, b, 6);
	a->ssid = ssid;
	return (0);
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
	pkt_ax25_free(pkt);
}
#endif


