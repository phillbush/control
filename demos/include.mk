DEMOS = demos/primitive
DEMO_OBJS = ${DEMOS:=.o}
DEMO_SRCS = ${DEMO_OBJS:.o=.c}

${DEMO_OBJS}: ${LIB_OBJS} ${HEADERS}

demos/primitive: demos/primitive.o
	${CC} -o $@ demos/primitive.o ${LIB_OBJS} ${XLDFLAGS} ${LDFLAGS}
