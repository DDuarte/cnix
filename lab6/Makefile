# Makefile for the mv driver (based on that of hello).

COMPILER_TYPE= gnu

CC=gcc

PROG=	lab6
SRCS=	lab6.c test6.c interrupt.c rtc.c rtc_asm.S

CCFLAGS= -Wall

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+=	-ldriver -lsys

LDFLAGS+= -L .

MAN=

BINDIR?= /usr/sbin

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
