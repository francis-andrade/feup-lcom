# Makefile for proj

COMPILER_TYPE= gnu

CC= gcc

PROG= proj
SRCS=  proj.c checkers.c timer.c kbc.c mouse.c vbe.c video_gr.c utilities.c ProgramLogic.c Bitmap.c rtc.c  ser_port.c publicity.c _kbc_ass.S

CCFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys  

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
