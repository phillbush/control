# installation paths (not used yet)
PREFIX = /usr/local
INCPREFIX = ${PREFIX}/include
LIBPREFIX = ${PREFIX}/lib
MANPREFIX = ${PREFIX}/share/man

# internally used flags
XCFLAGS = -Iinclude -I/usr/X11R6/include -I/usr/local/include -I/usr/include/freetype2 -I/usr/X11R6/include/freetype2 -fPIC
XLDFLAGS = -L/usr/local/lib -L/usr/X11R6/lib -lfontconfig -lXft -lXt -lX11

# executable files (for demo programs)
BINS = demos/primitive \
       demos/textfield

# library object files
LIBS = lib/libcontrol.a lib/libcontrol.so

# object files for libraries
OBJS_LIB = lib/util.o \
           lib/translations.o \
           lib/primitive.o \
           lib/textfield.o

# object and source files
OBJS_DEMO = ${BINS:=.o}

# object, header, and source files
OBJS = ${OBJS_LIB} ${OBJS_DEMO}
INCS = lib/common.h include/control.h include/control_private.h
SRCS = ${OBJS:.o=.c} ${INCS}

# main targets
all: lib
lib: ${LIBS}
demos: ${BINS}

# dependencies for object files
${OBJS_LIB}: ${INCS}
${OBJS_DEMO}: ${OBJS_LIB}

# I use tags file on vi(m)
tags: ${SRCS}
	ctags ${SRCS}

# static library
lib/libcontrol.a: ${OBJS_LIB}
	${AR} rc $@ ${OBJS_LIB}
	${RANLIB} $@

# shared library
lib/libcontrol.so: ${OBJS_LIB}
	${CC} -o $@ -shared ${OBJS_LIB} ${XLDFLAGS} ${LDFLAGS}

# object to executable (for demo programs)
.o:
	${CC} -o $@ $< ${OBJS_LIB} ${XLDFLAGS} ${LDFLAGS}

# source to object
.c.o:
	${CC} ${XCFLAGS} ${CFLAGS} ${CPPFLAGS} -o $@ -c $<

clean:
	rm -f ${LIBS} ${BINS} ${OBJS} tags

.PHONY: all lib demos tags clean
