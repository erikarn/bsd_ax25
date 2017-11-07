#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>

#include <sys/queue.h>

#include "buf.h"
#include "buf_list.h"

struct buf_list *
buf_list_create(void)
{
	struct buf_list *bl;

	bl = calloc(1, sizeof(*bl));
	if (bl == NULL) {
		warn("%s: calloc", __func__);
	}

	TAILQ_INIT(&bl->bufs);
	return (bl);
}

void
buf_list_free(struct buf_list *bl)
{
	buf_list_flush(bl);
	free(bl);
}

void
buf_list_flush(struct buf_list *bl)
{
	struct buf *b, *bn;

	TAILQ_FOREACH_SAFE(b, &bl->bufs, n, bn) {
		TAILQ_REMOVE(&bl->bufs, b, n);
		buf_free(b);
	}
}

int
buf_list_append(struct buf_list *bl, struct buf *b)
{

	TAILQ_INSERT_TAIL(&bl->bufs, b, n);
	bl->nitems++;
	return (0);
}

int
buf_list_isempty(struct buf_list *bl)
{

	return (bl->nitems == 0);
}

int
buf_list_push(struct buf_list *bl, struct buf *b)
{

	TAILQ_INSERT_HEAD(&bl->bufs, b, n);
	bl->nitems++;
	return (0);
}

struct buf *
buf_list_pop(struct buf_list *bl)
{
	struct buf *b;

	if (buf_list_isempty(bl))
		return (NULL);

	b = TAILQ_FIRST(&bl->bufs);
	TAILQ_REMOVE(&bl->bufs, b, n);
	bl->nitems--;
	return (b);

}

struct buf *
buf_list_popend(struct buf_list *bl)
{
	struct buf *b;

	if (buf_list_isempty(bl))
		return (NULL);

	b = TAILQ_LAST(&bl->bufs, bufhead);
	TAILQ_REMOVE(&bl->bufs, b, n);
	bl->nitems--;
	return (b);


}
