.PHONY: all fast small debug2 debug profile coverage clean

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: libPinto.a

fast: CFLAGS += -O3
fast: all

small: CFLAGS += -Os
small: all

debug2: CFLAGS += -DPRINT_ERR=1
debug2: debug

debug: CFLAGS += -DPINTO_DEBUG=1 -fprofile-arcs -ftest-coverage -g
debug: all

profile: CFLAGS += -pg
profile: fast

coverage:
	gcov -o . *.c
	mkdir lcov
	lcov --directory . --capture --output-file ./lcov/pinto.info
	genhtml ./lcov/pinto.info -o ./lcov
	@echo "Load ./pintoLib/lcov/index.html in your browser for lcov data."

clean:
	rm -f libPinto.a
	rm -f *.o
	rm -f *.gcda
	rm -f *.gcno
	rm -rf lcov

libPinto.a: $(OBJECTS) $(HEADERS)
	ar rcs libPinto.a *.o

%.o : %.c *.h
	$(CC) $(CFLAGS) -c $<

