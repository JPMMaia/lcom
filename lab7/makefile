# Makefile for the lab7 driver (based on that of hello).

COMPILER_TYPE= gnu

CC=gcc

PROG=	lab7
SRCS=	lab7.c test7.c SP.c

CCFLAGS= -Wall

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+=	-ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>