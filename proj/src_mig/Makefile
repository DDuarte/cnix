COMPILER_TYPE = gnu

CC = gcc

PROG = cnix
SRCS = cnix.c vbe.c video_gr.c video_gr_a.S utilities.c
SRCS += timer.c interrupt.c list.c priority_list.c window.c
SRCS += mouse.c kbc.c keyboard.c

CFLAGS = -I./../dep/include -I/usr/local/include/freetype2 -I/usr/local/include
LDFLAGS = -L. -L./../dep/lib -L/usr/local/lib
CCFLAGS = -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys -lfreetype

MAN=

BINDIR?= /usr/sbin

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
