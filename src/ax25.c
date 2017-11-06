#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "ax25.h"
#include "kiss.h"
#include "ax25_pkt_examples.h"

/*
 * Parse an AX.25 payload.
 */
int
ax25_pkt_parse(const uint8_t *buf, int len)
{
	int i;
	i = 0;

	printf("%s: total length: %d\n", __func__, len);

	/* First up are the address field(s) - note, SSID is the 7th byte */
	/*
	 * And note there's a 'H' bit in SSID indicating it's been repeated
	 * through a repeater.
	 */
	do {
		printf("[%.2x) %c]",
		    (buf[i] & 0xff),
		    (buf[i] & 0xff) >> 1);
	} while ((buf[i++] & 0x1) == 0);
	printf("\n");

	/* Following byte is the control byte */

	return (0);
}

int
main(int argc, const char *argv[])
{
	uint8_t kbuf[1024];
	int kbuf_len;

	kiss_payload_parse(kiss_ax25_aprs_example_1, sizeof(kiss_ax25_aprs_example_1),
	    kbuf, &kbuf_len);
	ax25_pkt_parse(kbuf, kbuf_len);
}
