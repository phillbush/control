include demo/primitive/include.mk

DEMO_BINS = ${DEMO_PRIMITIVE_BINS}
DEMO_OBJS = ${DEMO_PRIMITIVE_OBJS}
DEMO_SRCS = ${DEMO_PRIMITIVE_SRCS}

${DEMO_OBJS}: lib/libcontrol.so lib/libcontrol.a
