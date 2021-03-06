#ifndef	__AX25_PKT_EXAMPLES_H__
#define	__AX25_PKT_EXAMPLES_H__

/*
 * These are KISS TNC formatted frames, complete with start/end markers.
 */
static const uint8_t kiss_ax25_aprs_example_1[] = {
	0xc0, 0x00, 0x82, 0xa0, 0x88, 0xae, 0x62, 0x66, 0xe0, 0x9c, 0xaa, 0x6c, 0xb0, 0x84, 0x40, 0xea,
	0x9c, 0x6c, 0xb4, 0xb0, 0x40, 0x40, 0xe6, 0xae, 0x92, 0x88, 0x8a, 0x62, 0x40, 0xe0, 0x84, 0x96,
	0x8a, 0x98, 0x8a, 0xb2, 0xe0, 0xae, 0x92, 0x88, 0x8a, 0x64, 0x40, 0xe1, 0x03, 0xf0, 0x21, 0x33,
	0x37, 0x35, 0x32, 0x2e, 0x35, 0x30, 0x4e, 0x53, 0x31, 0x32, 0x32, 0x31, 0x35, 0x2e, 0x34, 0x33,
	0x57, 0x23, 0x50, 0x48, 0x47, 0x32, 0x33, 0x34, 0x30, 0x55, 0x43, 0x20, 0x42, 0x65, 0x72, 0x6b,
	0x65, 0x6c, 0x65, 0x79, 0x20, 0x45, 0x45, 0x43, 0x53, 0x20, 0x44, 0x65, 0x70, 0x61, 0x72, 0x74,
	0x6d, 0x65, 0x6e, 0x74, 0x2c, 0x20, 0x52, 0x61, 0x73, 0x50, 0x69, 0x20, 0x2b, 0x20, 0x44, 0x69,
	0x72, 0x65, 0x77, 0x6f, 0x6c, 0x66, 0x20, 0x2b, 0x20, 0x42, 0x61, 0x6f, 0x66, 0x65, 0x6e, 0x67,
	0xc0
};

static const uint8_t kiss_ax25_aprs_example_2[] = {
	0xc0, 0x00, 0x82, 0xa0, 0x9e, 0xa8, 0x66, 0x60, 0xe0, 0xa6, 0xa0, 0x88, 0xa4, 0x98, 0x96, 0xe0,
	0xae, 0x82, 0xa4, 0x88, 0x40, 0x40, 0xe0, 0xae, 0x92, 0x88, 0x8a, 0x62, 0x40, 0xe0, 0x9c, 0x6c,
	0xb4, 0xb0, 0x40, 0x40, 0xe6, 0xae, 0x92, 0x88, 0x8a, 0x64, 0x40, 0xe1, 0x03, 0xf0, 0x21, 0x33,
	0x39, 0x30, 0x30, 0x2e, 0x38, 0x36, 0x4e, 0x2f, 0x31, 0x32, 0x30, 0x31, 0x36, 0x2e, 0x31, 0x39,
	0x57, 0x23, 0x4e, 0x36, 0x59, 0x42, 0x48, 0x20, 0x53, 0x70, 0x69, 0x64, 0x65, 0x72, 0x20, 0x4c,
	0x61, 0x6b, 0x65, 0x20, 0x44, 0x49, 0x47, 0x49, 0xc0
};

static const uint8_t kiss_ax25_aprs_example_3[] = {
	0xc0, 0x00, 0x82, 0xa0, 0x9a, 0x92, 0x60, 0x6c, 0x60, 0xae, 0x6c, 0x92, 0x82, 0x40, 0x40, 0x60,
	0x96, 0x6c, 0xa4, 0xa0, 0xa8, 0x40, 0xe0, 0x9c, 0x6c, 0xb4, 0xb0, 0x40, 0x40, 0xe6, 0xae, 0x92,
	0x88, 0x8a, 0x64, 0x40, 0xe1, 0x03, 0xf0, 0x40, 0x30, 0x36, 0x30, 0x35, 0x32, 0x33, 0x7a, 0x33,
	0x37, 0x31, 0x36, 0x2e, 0x34, 0x37, 0x4e, 0x49, 0x31, 0x32, 0x31, 0x35, 0x34, 0x2e, 0x37, 0x31,
	0x57, 0x26, 0x50, 0x48, 0x47, 0x32, 0x30, 0x36, 0x30, 0x53, 0x61, 0x6e, 0x20, 0x4a, 0x6f, 0x73,
	0x65, 0x2c, 0x20, 0x43, 0x41, 0xc0
};

#endif	/* __AX25_PKT_EXAMPLES_H__ */

