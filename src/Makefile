PROG=   bsd_packet
SRCS=   main.c
SRCS+=  kiss.c

CFLAGS=         -I/usr/local/include -Wall -Werror
LDADD=-lutil -L/usr/local/lib -levent_core

MK_MAN=no

.include <bsd.prog.mk>
