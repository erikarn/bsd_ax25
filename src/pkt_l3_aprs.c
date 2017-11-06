#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "pkt_l3_aprs.h"

struct pkt_l3_aprs *
pkt_l3_aprs_create(void)
{
	struct pkt_l3_aprs *l;

	l = calloc(1, sizeof(*l));
	if (l == NULL) {
		warn("%s: calloc", __func__);
		return (NULL);
	}

	return (l);
}

void
pkt_l3_aprs_free(struct pkt_l3_aprs *l)
{

	if (l->src)
		free(l->src);
	if (l->path)
		free(l->path);
	if (l->payload)
		free(l->payload);
	free(l);
}

int
pkt_l3_aprs_set_src(struct pkt_l3_aprs *l, const char *buf, int len)
{

	if (l->src)
		free(l->src);

	l->src = strndup(buf, len);
	if (l->src == NULL) {
		warn("%s: strndup; %d bytes", __func__, len);
		return (-1);
	}

	return (0);
}

int
pkt_l3_aprs_set_path(struct pkt_l3_aprs *l, const char *buf, int len)
{

	if (l->path)
		free(l->path);

	l->path = strndup(buf, len);
	if (l->path == NULL) {
		warn("%s: strndup; %d bytes", __func__, len);
		return (-1);
	}

	return (0);
}

int
pkt_l3_aprs_set_payload(struct pkt_l3_aprs *l, const char *buf, int len)
{

	if (l->payload)
		free(l->payload);

	l->payload = strndup(buf, len);
	if (l->payload == NULL) {
		warn("%s: strndup; %d bytes", __func__, len);
		return (-1);
	}

	return (0);
}
