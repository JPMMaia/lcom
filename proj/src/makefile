COMPILER_TYPE= gnu

CC=gcc

PROG=	proj
SRCS=	main.c ClockClass.c DispatcherClass.c GameLogicClass.c GraphicsClass.c InputClass.c KeyboardClass.c ListClass.c MenuClass.c MouseClass.c RTCClass.c SerialPortClass.c SpriteClass.c SystemClass.c TimerClass.c WallsClass.c

CCFLAGS= -Wall

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+=	-llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
