DEMOS = demos/primitive
DEMO_OBJS = demos/primitive.o
DEMO_SRCS = demos/primitive.c

${DEMO_OBJS}: ${LIB_OBJS} ${HEADERS}

demos/primitive: demos/primitive.o
	${CC} -L/usr/X11R6/lib -L/usr/local/lib -o $@ demos/primitive.o ${LIB_OBJS} -lXt -lX11 ${LDFLAGS}
