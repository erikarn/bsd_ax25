#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <sys/queue.h>

#include "buf.h"

struct buf *
buf_create(int len)
{
	struct buf *b;

	b = calloc(1, sizeof(*b));
	if (b == NULL) {
		warn("%s: calloc", __func__);
		return (NULL);
	}

	b->buf = malloc(len);
	if (b->buf == NULL) {
		warn("%s: malloc (%d bytes)", __func__, len);
		free(b);
	}

	b->size = len;
	return (b);
}

void
buf_free(struct buf *b)
{
	free(b->buf);
	free(b);

}

int
buf_get_len(struct buf *b)
{

	return (b->len);
}

int
buf_get_size(struct buf *b)
{

	return (b->size);
}

int
buf_copy(struct buf *b, const char *src, int len)
{
	int cl;

	cl = len;
	if (cl > b->size)
		cl = b->size;

	memcpy(b->buf, src, cl);
	b->len = cl;
	return (cl);
}

int
buf_append(struct buf *b, const char *src, int len)
{
	int cl;

	/* Error out if the buffer is full */
	if (b->size - b->len <= 0)
		return (-1);

	/* Only copy as much as the space you have */
	cl = len;
	if ((b->size - b->len) < cl)
		cl = b->size - b->len;

	memcpy(b->buf + b->len, src, cl);
	b->len += cl;

	/* Return how much was apeended */
	return (cl);
}
