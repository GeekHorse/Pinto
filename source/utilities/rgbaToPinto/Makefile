.PHONY: all fast small single singleFast singleSmall profile clean

CFLAGS += -I../../pintoLib
LDFLAGS += ../../pintoLib/libPinto.a

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: rgbaToPinto

fast: CFLAGS += -O3
fast: all

small: fast

single: all
singleFast: fast
singleSmall: small

profile: fast

clean:
	rm -f rgbaToPinto
	rm -f *.o

rgbaToPinto : $(OBJECTS) $(HEADERS) ../../pintoLib/libPinto.a
	$(CC) *.o $(LDFLAGS) -o rgbaToPinto

%.o : %.c *.h
	$(CC) $(CFLAGS) -c $<

