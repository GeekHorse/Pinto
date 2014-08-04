.PHONY: all fast small single singleFast singleSmall profile clean

CFLAGS += -I../../pintoLib
LDFLAGS += ../../pintoLib/libPinto.a

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: pintoToRgba

fast: CFLAGS += -O3
fast: all

small: fast

single: all
singleFast: fast
singleSmall: small

profile: fast

clean:
	rm -f pintoToRgba
	rm -f *.o

pintoToRgba : $(OBJECTS) $(HEADERS) ../../pintoLib/libPinto.a
	$(CC) *.o $(LDFLAGS) -o pintoToRgba

%.o : %.c *.h
	$(CC) $(CFLAGS) -c $<

