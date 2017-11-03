#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <sys/queue.h>
#include <sys/param.h>

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

int
buf_consume(struct buf *b, int len)
{
	int c;

	if (len < 0)
		return (-1);
	if (len == 0)
		return (0);

	c = MIN(len, b->len);
	memmove(b->buf, b->buf + c, b->len - c);
	b->len -= c;

	return (c);
}

/*
 * Consume a string from the given buffer.
 *
 * The returned string is a NUL terminated C string.
 *
 * The length of the string (minus the terminating NUL)
 * is returned; 0 if there's no line found, -1 if we
 * don't have a big enough buffer for the line.
 */
int
buf_gets(struct buf *b, char *buf, int buflen)
{
	char *r;
	int rr;

	/* Find an end-of-line */
	r = memchr(b->buf, '\n', b->len);

	/* Nothing? no line */
	if (r == NULL)
		return (0);

	/* Ok, find out how much data there is in the buffer */
	rr = r - b->buf + 1;

	/*
	 * Find out how much data is there; see if there's
	 * enough space in the target buffer.
	 */
	if (rr > (buflen - 1)) {
		return (-1);
	}

	/*
	 * Copy, NUL terminate.
	 */
	memcpy(buf, b->buf, rr);
	buf[rr] = '\0';

	/*
	 * Consume.
	 */
	buf_consume(b, rr);

	/*
	 * Return how many bytes were consumed in the line, including the
	 * terminating \r\n.  It's up to the consumer to strip it.
	 */
	return (rr);
}

/*
 * Trim the CRLF off the end of a buffer.
 */
int
buf_trim_crlf(struct buf *b)
{
	int r = 0;

	while ((b->len) != 0 &&
	    (b->buf[b->len - 1] == '\r' || b->buf[b->len - 1] == '\n')) {
		b->len--;
		r++;
	}

	return (r);
}
