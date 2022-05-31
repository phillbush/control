# installation paths (not used yet)
PREFIX = /usr/local
INCPREFIX = ${PREFIX}/include
LIBPREFIX = ${PREFIX}/lib
MANPREFIX = ${PREFIX}/share/man

# internally used flags
XCFLAGS = -Iinclude -I/usr/X11R6/include -I/usr/local/include -I/usr/include/freetype2 -I/usr/X11R6/include/freetype2 -fPIC
XLDFLAGS = -L/usr/local/lib -L/usr/X11R6/lib -lfontconfig -lXft -lXt -lX11

DEMOS = \
	demos/prompt

LIBS = lib/libcontrol.a lib/libcontrol.so

OBJS_LIB = \
	lib/util.o \
	lib/primitive.o \
	lib/textfield.o

OBJS_DEMO = ${DEMOS:=.o}

OBJS = ${OBJS_LIB} ${OBJS_DEMO}
INCS = lib/common.h include/control.h include/control_private.h
SRCS = ${OBJS:.o=.c} ${INCS}

all: lib
lib: ${LIBS}
demos: ${DEMOS}

${OBJS_LIB}: ${INCS}
${OBJS_DEMO}: ${OBJS_LIB}

tags: ${SRCS}
	ctags ${SRCS}

lib/libcontrol.a: ${OBJS_LIB}
	${AR} rc $@ ${OBJS_LIB}
	${RANLIB} $@

lib/libcontrol.so: ${OBJS_LIB}
	${CC} -o $@ -shared ${OBJS_LIB} ${XLDFLAGS} ${LDFLAGS}

.o:
	${CC} -o $@ $< ${OBJS_LIB} ${XLDFLAGS} ${LDFLAGS}

.c.o:
	${CC} ${XCFLAGS} ${CFLAGS} ${CPPFLAGS} -o $@ -c $<

cleancore:
	rm -f *.core

cleantags:
	rm -f tags

clean: cleancore cleantags
	rm -f ${LIBS} ${DEMOS} ${OBJS}

gitadd:
	git add Makefile README doc/control.3 ${SRCS}

gitpush:
	# only do this once:
	# git remote add origin git@github.com:phillbush/control.git
	git push -u origin master

.PHONY: all lib demos tags cleancore cleantags clean gitadd gitpush
