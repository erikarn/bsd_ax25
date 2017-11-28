#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>

#include <sys/queue.h>

#include "ax25_pkt.h"
#include "ax25_pkt_list.h"

struct ax25_pkt_list *
ax25_pkt_list_create(int max)
{
	struct ax25_pkt_list *a;

	a = calloc(1, sizeof(*a));
	if (a == NULL) {
		warn("%s: calloc", __func__);
		return (NULL);
	}

	TAILQ_INIT(&a->list);
	a->max = max;
	return (a);
}

void
ax25_pkt_list_free(struct ax25_pkt_list *l)
{

	ax25_pkt_list_flush(l);
	free(l);
}


int
ax25_pkt_list_queue_head(struct ax25_pkt_list *l, struct ax25_pkt *p)
{

	if (l->n >= l->max)
		return (-1);

	TAILQ_INSERT_HEAD(&l->list, p, n);
	l->n++;
	return (0);
}

int
ax25_pkt_list_queue_tail(struct ax25_pkt_list *l, struct ax25_pkt *p)
{

	if (l->n >= l->max)
		return (-1);

	TAILQ_INSERT_TAIL(&l->list, p, n);
	l->n++;
	return (0);
}

struct ax25_pkt *
ax25_pkt_list_remove_head(struct ax25_pkt_list *l)
{
	struct ax25_pkt *p;

	if (l->n == 0)
		return (NULL);

	p = TAILQ_FIRST(&l->list);
	TAILQ_REMOVE(&l->list, p, n);
	l->n--;

	return (p);
}

struct ax25_pkt *
ax25_pkt_list_remove_tail(struct ax25_pkt_list *l)
{
	struct ax25_pkt *p;

	if (l->n == 0)
		return (NULL);

	p = TAILQ_LAST(&l->list, ax25_pkt_list_head);
	TAILQ_REMOVE(&l->list, p, n);
	l->n--;

	return (p);
}

int
ax25_pkt_list_num(struct ax25_pkt_list *l)
{

	return (l->n);
}

int
ax25_pkt_list_has_space(struct ax25_pkt_list *l)
{

	return (l->n < l->max);
}

int
ax25_pkt_list_flush(struct ax25_pkt_list *l)
{
	struct ax25_pkt *p;
	int i = 0;

	while ((p = TAILQ_FIRST(&l->list)) != NULL) {
		TAILQ_REMOVE(&l->list, p, n);
		l->n--;
		i++;

		ax25_pkt_free(p);
	}

	return (i);
}
