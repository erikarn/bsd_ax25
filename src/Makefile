PROG=   bsd_packet

# Base
SRCS=   main.c
SRCS+=  pkt.c

# AX.25
SRCS+=  ax25_plsm.c
SRCS+=  ax25_lmsm.c
SRCS+=  ax25_dlsm.c
SRCS+=  ax25.c

# KISS
SRCS+=  kiss.c

# APRS
SRCS+=  pkt_l3_aprs.c

# Protocol clients
SRCS+=	proto_kiss.c
SRCS+=	proto_aprs_igate.c

CFLAGS=         -I/usr/local/include -I../lib/libbuf -I../lib/libconn -I../lib/libaxutil -I../lib/libstr -Wall -Werror
LDADD=-lutil -L/usr/local/lib -levent_core -levent_extra -L../lib/libbuf -L../lib/libconn -lconn -lbuf -L../lib/libaxutil -laxutil -L../lib/libstr -lstr

MK_MAN=no

.include <bsd.prog.mk>
