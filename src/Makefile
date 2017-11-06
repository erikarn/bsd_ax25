PROG=   bsd_packet

# Base
SRCS=   main.c
SRCS+=  pkt.c
SRCS+=  str.c

# APRS
SRCS+=  pkt_l3_aprs.c

# Protocols
SRCS+=	proto_kiss.c
SRCS+=	proto_aprs_igate.c

CFLAGS=         -I/usr/local/include -I../lib/libbuf -I../lib/libconn -I../lib/libaxutil -Wall -Werror
LDADD=-lutil -L/usr/local/lib -levent_core -levent_extra -L../lib/libbuf -L../lib/libconn -lconn -lbuf -L../lib/libaxutil -laxutil

MK_MAN=no

.include <bsd.prog.mk>
