PREFIX = /usr/local
INCPREFIX = ${PREFIX}/include
LIBPREFIX = ${PREFIX}/lib
MANPREFIX = ${PREFIX}/share/man

all: lib

include lib/include.mk
include demo/include.mk
include include/include.mk

lib: ${LIBS}
demos: ${DEMO_BINS}

tags: ${LIB_SRCS} ${CMD_SRCS} ${INCLUDE_SRCS} ${DEMO_SRCS}
	ctags ${LIB_SRCS} ${CMD_SRCS} ${INCLUDE_SRCS} ${DEMO_SRCS}

.c.o:
	${CC} -Iinclude -I/usr/X11R6/include -I/usr/local/include ${CFLAGS} ${CPPFLAGS} -o $@ -c $<

clean:
	@rm -f ${LIB_OBJS} ${DEMO_OBJS} ${BINS} ${LIBS} ${TAGS}
	@rm -f ${DEMO_OBJS} ${DEMO_BINS} ${DEMO_TAGS}
	@rm -f *.core
	@rm -f tags

.PHONY: all lib demos tags clean
