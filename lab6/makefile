# Makefile for the lab6 driver (based on that of hello).

COMPILER_TYPE= gnu

CC=gcc

PROG=	lab6
SRCS=	lab6.c test6.c RTC.c

CCFLAGS= -Wall

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+=	-ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>