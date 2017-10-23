#ifndef	__BUF_H__
#define	__BUF_H__

struct buf;

struct buf {
	TAILQ_ENTRY(buf) n;
	char *buf;
	int len;
	int size;
};

extern	struct buf * buf_create(int len);
extern	void buf_free(struct buf *);

extern	int buf_get_len(struct buf *);
extern	int buf_get_size(struct buf *);
extern	int buf_copy(struct buf *, const char *src, int len);

#endif	/* __BUF_H__ */