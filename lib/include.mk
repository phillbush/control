LIBS = lib/libcontrol.a lib/libcontrol.so

LIB_OBJS = lib/ControlI.o \
           lib/Primitive.o \
           lib/TextField.o

LIB_SRCS = ${LIB_OBJS:.o=.c} lib/ControlI.h

${LIB_OBJS}:            lib/ControlI.h
lib/Primitive.o:        include/control/PrimitiveP.h include/control/Primitive.h
lib/TextField.o:        include/control/TextFieldP.h include/control/TextField.h

lib/libcontrol.a: ${LIB_OBJS}
	${AR} rc $@ ${LIB_OBJS}
	${RANLIB} $@

lib/libcontrol.so: ${LIB_OBJS}
	${CC} -o $@ -shared ${LIB_OBJS} ${XLDFLAGS} ${LDFLAGS}
