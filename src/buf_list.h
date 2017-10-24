#ifndef	__BUF_LIST_H__
#define	__BUF_LIST_H__

struct buf_list {
	TAILQ_HEAD(bufhead, buf) bufs;
	int nitems;
};

extern	struct buf_list * buf_list_create(void);
extern	void buf_list_free(struct buf_list *);
extern	void buf_list_flush(struct buf_list *);
extern	int buf_list_append(struct buf_list *, struct buf *);
extern	int buf_list_isempty(struct buf_list *);
extern	int buf_list_push(struct buf_list *, struct buf *);
extern	struct buf * buf_list_pop(struct buf_list *);
extern	struct buf * buf_list_popend(struct buf_list *);

#endif
