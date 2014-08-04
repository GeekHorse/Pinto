.PHONY: all fast small debug debug2 debug3 single singleSmall singleFast singleDebug profile clean

CFLAGS += -I../pintoLib
LDFLAGS += ../pintoLib/libPinto.a

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: pintoTest

fast: CFLAGS += -O3
fast: all

small: fast

debug3: debug2

debug2: CFLAGS += -g
debug2: LDFLAGS += -fprofile-arcs
debug2: all

debug: fast

single: fast
singleSmall: small
singleFast: fast

singleDebug: CFLAGS += -g
singleDebug: LDFLAGS += -fprofile-arcs
singleDebug: all

profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: fast

clean:
	rm -f pintoTest
	rm -f *.o

pintoTest : $(OBJECTS) $(HEADERS) ../pintoLib/libPinto.a
	$(CC) *.o $(LDFLAGS) -o pintoTest

%.o : %.c *.h
	$(CC) $(CFLAGS) -c $<

