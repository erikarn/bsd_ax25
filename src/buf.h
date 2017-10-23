#ifndef	__BUF_H__
#define	__BUF_H__

struct buf;
typedef struct buf buf_t;

struct buf {
	TAILQ_ENTRY(buf) n;
	char *buf;
	int len;
	int size;
};

#endif	/* __BUF_H__ */
