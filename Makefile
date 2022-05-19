PREFIX = /usr/local
INCPREFIX = ${PREFIX}/include
LIBPREFIX = ${PREFIX}/lib
MANPREFIX = ${PREFIX}/share/man

all: lib

include include/include.mk
include lib/include.mk
include demos/include.mk

lib: ${LIBS}
demos: ${DEMOS}

tags: ${LIB_SRCS} ${DEMO_SRCS} ${HEADERS}
	ctags ${LIB_SRCS} ${DEMO_SRCS} ${HEADERS}

.c.o:
	${CC} -Iinclude -I/usr/X11R6/include -I/usr/local/include ${CFLAGS} ${CPPFLAGS} -o $@ -c $<

clean:
	rm -f ${LIBS} ${DEMOS} ${LIB_OBJS} ${DEMO_OBJS} tags

.PHONY: all lib demos tags clean
