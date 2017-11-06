#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define	FEND		0xc0	// Frame end
#define	FESC		0xdb	// Frame escape
#define	TFEND		0xdc	// Transposed Frame End
#define	TFESC		0xdd	// Transposed Frame Escape

/*
 * Convert a KISS frame to a raw AX.25 frame.
 *
 * The KISS TNC escaping is pretty easy to do - look for the
 * frame beginning sequence, and then un-escape stuff as it's
 * parsed.
 */

int
kiss_payload_parse(const uint8_t *sbuf, int slen, uint8_t *dbuf, int *dlen)
{
	int l, i;
	int found_start = 0;
	int kiss_channel;

	l = 0;
	for (i = 0; i < slen; i++) {
		/* Search for start of frame */
		if (found_start == 0 && sbuf[i] == FEND) {
			found_start = 1;
			/* Consume the next byte - that's the channel identifier */
			if (i < (slen - 1)) {
				kiss_channel = sbuf[i+1];
				i++;	/* Skip this byte too */
			}
			continue;
		}
		/* Skip bytes until we see a start of frame */
		if (found_start == 0)
			continue;

		/* Everything past here is in the frame */
		if (sbuf[i] == FEND)
			break;

		if (sbuf[i] == FESC) {
			if (i < (slen - 1) && sbuf[i+1] == TFESC) {
				dbuf[l] = FESC;
				l++;
				i++;	/* Need to consume two bytes */
				continue;
			}
			if (i < (slen - 1) && sbuf[i+1] == TFEND) {
				dbuf[l] = FEND;
				l++;
				i++;	/* Need to consume two bytes */
				continue;
			}
			/* Consume the FESC */
			continue;
		}

		/* Ok, copy things */
		dbuf[l] = sbuf[i];
		l++;
	}

	*dlen = l;
	return (0);
}
