DEMO_PRIMITIVE_BINS = demos/primitive/primitive
DEMO_PRIMITIVE_OBJS = demos/primitive/primitive.o
DEMO_PRIMITIVE_SRCS = demos/primitive/primitive.c

${DEMO_PRIMITIVE_OBJS}: include/control/PrimitiveP.h

demos/primitive/primitive: ${DEMO_PRIMITIVE_OBJS}
	${CC} -static -DXINERAMA -L./lib -L/usr/X11R6/lib -L/usr/local/lib -o $@ ${DEMO_PRIMITIVE_OBJS} -lcontrol -lXt -lSM -lICE -lX11 -lxcb -lXau -lXext -lXdmcp -lpthread ${LDFLAGS}
