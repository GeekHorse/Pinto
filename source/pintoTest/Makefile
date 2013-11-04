.PHONY: all fast small debug debug2 profile clean

CFLAGS += -I../pintoLib
LDFLAGS += ../pintoLib/libPinto.a

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: pintoTest

fast: CFLAGS += -O3
fast: all

small: fast

debug2: debug

debug: CFLAGS += -g
debug: LDFLAGS += -fprofile-arcs
debug: all

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

