TARGET  = LoadDebugger


OBJS    = main.o logging.o hen.o
LIBS    =
CFLAGS  = -O0 -G0 -Wall -fno-builtin-printf $(CFLAGS_EXTRA)
ASFLAGS = $(CFLAGS)
USE_KERNEL_LIBS = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
