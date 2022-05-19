LIBS = lib/libcontrol.a lib/libcontrol.so

LIB_OBJS = lib/Primitive.o \
           lib/ControlI.o
LIB_SRCS = lib/Primitive.c \
           lib/ControlI.c \
           lib/ControlI.h

lib/Primitive.o:        include/control/PrimitiveP.h include/control/Primitive.h
${LIB_OBJS}:            lib/ControlI.h

lib/libcontrol.a: ${LIB_OBJS}
	${AR} rc $@ ${LIB_OBJS}
	${RANLIB} $@

lib/libcontrol.so: ${LIB_OBJS}
	${CC} -L/usr/X11R6/lib -L/usr/local/lib -o $@ -shared ${LIB_OBJS} -lXt -lX11 ${LDFLAGS}
