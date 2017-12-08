PROG=   bsd_packet

# Base
SRCS=   main.c
SRCS+=  pkt.c

# AX.25
# SRCS+=  ax25.c

# AX.25 physical layers
SRCS+=	plsm_kiss.c

# APRS
SRCS+=  pkt_l3_aprs.c

# Higher level protocol clients
SRCS+=	proto_aprs_igate.c

CFLAGS=         -I/usr/local/include -I../lib/libbuf -I../lib/libconn -I../lib/libaxutil -I../lib/libstr -I../lib/libkiss -I../lib/libax25 -Wall -Werror
LDADD=-lutil -L/usr/local/lib -levent_core -levent_extra -L../lib/libax25 -lax25 -L../lib/libbuf -L../lib/libkiss -lkiss -L../lib/libconn -lconn -lbuf -L../lib/libaxutil -laxutil -L../lib/libstr -lstr

MK_MAN=no

.include <bsd.prog.mk>
