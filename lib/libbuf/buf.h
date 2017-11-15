#ifndef	__BUF_H__
#define	__BUF_H__

struct buf;

struct buf {
	TAILQ_ENTRY(buf) n;
	uint8_t *buf;
	int len;
	int size;
	int write_offset;
};

static inline uint8_t *
buf_get_ptr(struct buf *b)
{

	return (b->buf);
}

static inline const uint8_t *
buf_get_ptr_const(struct buf *b)
{

	return (b->buf);
}

extern	struct buf * buf_create(int len);
extern	void buf_free(struct buf *);

extern	int buf_get_len(struct buf *);
extern	int buf_get_size(struct buf *);
extern	int buf_copy(struct buf *, const uint8_t *src, int len);
extern	int buf_append(struct buf *, const uint8_t *src, int len);
extern	struct buf * buf_gets(struct buf *);
extern	int buf_trim_crlf(struct buf *);
extern	int buf_consume(struct buf *, int);

#endif	/* __BUF_H__ */
