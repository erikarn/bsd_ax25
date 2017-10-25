PROG=   bsd_packet
SRCS=   main.c
SRCS+=  conn.c
SRCS+=  util.c
SRCS+=  buf.c
SRCS+=  buf_list.c
SRCS+=	proto_kiss.c
SRCS+=	proto_aprs_igate.c

CFLAGS=         -I/usr/local/include -Wall -Werror
LDADD=-lutil -L/usr/local/lib -levent_core -levent_extra

MK_MAN=no

.include <bsd.prog.mk>
