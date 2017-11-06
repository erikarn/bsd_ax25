#ifndef	__KISS_H__
#define	__KISS_H__

extern	int kiss_payload_parse(const uint8_t *sbuf, int slen, uint8_t *dbuf,
	    int *dlen);

#endif	/* __KISS_H__ */
