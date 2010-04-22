all: cbp

CPPFLAGS += -Wall
CFLAGS += -O2
LOADLIBES += libcs/libcs.a
LOADLIBES += -lm

cbp: cbp.c cbp.h $(LIBCS)
