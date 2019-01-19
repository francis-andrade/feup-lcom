# Makefile for lab7

COMPILER_TYPE= gnu

CC= gcc

PROG= lab7
SRCS= lab7.c test7.c ser_port.c queue.c
CCFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
