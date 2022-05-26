LIBS = lib/libcontrol.a lib/libcontrol.so

LIB_OBJS = lib/util.o \
           lib/translations.o \
           lib/primitive.o \
           lib/textfield.o

LIB_SRCS = ${LIB_OBJS:.o=.c} lib/control.h

${LIB_OBJS}:            lib/control.h
lib/primitive.o:        include/control/PrimitiveP.h include/control/Primitive.h
lib/textfield.o:        include/control/TextFieldP.h include/control/TextField.h

lib/libcontrol.a: ${LIB_OBJS}
	${AR} rc $@ ${LIB_OBJS}
	${RANLIB} $@

lib/libcontrol.so: ${LIB_OBJS}
	${CC} -o $@ -shared ${LIB_OBJS} ${XLDFLAGS} ${LDFLAGS}
