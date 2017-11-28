LIB=   ax25

# AX.25
SRCS+=  ax25_pkt.c
SRCS+=  ax25_pkt_list.c

SRCS+=  ax25_plsm.c
SRCS+=  ax25_lmsm.c

# SRCS+=  ax25_dlsm.c

CFLAGS=         -I/usr/local/include -I../lib/libbuf -I../lib/libconn -I../lib/libaxutil -I../lib/libstr -Wall -Werror
LDADD=-lutil -L/usr/local/lib -levent_core -levent_extra -L../lib/libbuf -L../lib/libconn -lconn -lbuf -L../lib/libaxutil -laxutil -L../lib/libstr -lstr

MK_MAN=no

.include <bsd.lib.mk>
