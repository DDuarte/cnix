COMPILER_TYPE = gnu

CC = gcc

PROG = cnix
SRCS = cnix.c vbe.c video_gr.c video_gr_a.S utilities.c

CFLAGS = -I./../dep/include -I/usr/local/include
LDFLAGS = -L. -L./../dep/lib -L/usr/local/lib
CCFLAGS = -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys

MAN=

BINDIR?= /usr/sbin

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
