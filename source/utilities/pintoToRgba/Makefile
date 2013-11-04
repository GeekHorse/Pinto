.PHONY: all fast small debug debug2 profile clean

CFLAGS += -I../../pintoLib
LDFLAGS += ../../pintoLib/libPinto.a

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: pintoToRgba

fast: CFLAGS += -O3
fast: all

small: fast

debug2: debug

debug: CFLAGS += -g
debug: LDFLAGS += -fprofile-arcs
debug: fast

profile: fast

clean:
	rm -f pintoToRgba
	rm -f *.o

pintoToRgba : $(OBJECTS) $(HEADERS) ../../pintoLib/libPinto.a
	$(CC) *.o $(LDFLAGS) -o pintoToRgba

%.o : %.c *.h
	$(CC) $(CFLAGS) -c $<

