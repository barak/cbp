all: cbp

CPPFLAGS += -Wall
CFLAGS += -O2
CPPFLAGS += -Ilibcs/include
LOADLIBES += libcs/libcs.a
LOADLIBES += -lm

cbp: cbp.c cbp.h $(LIBCS)
