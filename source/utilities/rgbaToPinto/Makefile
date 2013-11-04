.PHONY: all fast small debug debug2 profile clean

CFLAGS += -I../../pintoLib
LDFLAGS += ../../pintoLib/libPinto.a

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: rgbaToPinto

fast: CFLAGS += -O3
fast: all

small: fast

debug2: debug

debug: CFLAGS += -g
debug: LDFLAGS += -fprofile-arcs
debug: fast

profile: fast

clean:
	rm -f rgbaToPinto
	rm -f *.o

rgbaToPinto : $(OBJECTS) $(HEADERS) ../../pintoLib/libPinto.a
	$(CC) *.o $(LDFLAGS) -o rgbaToPinto

%.o : %.c *.h
	$(CC) $(CFLAGS) -c $<

