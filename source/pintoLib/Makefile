.PHONY: all fast small debug debug2 debug3 single singleFast singleSmall singleDebug singleCoverage profile coverage clean

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: libPinto.a

fast: CFLAGS += -O3
fast: all

small: CFLAGS += -Os
small: all

debug3: debug2

debug2: CFLAGS += -fprofile-arcs -ftest-coverage -g
debug2: all

debug: fast

singleFast: CFLAGS += -O3
singleFast: single

singleSmall: CFLAGS += -Os
singleSmall: single

singleDebug: CFLAGS += -fprofile-arcs -ftest-coverage -g
singleDebug: single

single:
	rm -f pintoSingle.c.temp
	rm -f pintoSingle.c
	cat pintoInternal.h > pintoSingle.c.temp
	cat *.c >> pintoSingle.c.temp
	cat pintoSingle.c.temp | grep -v pintoInternal.h > pintoSingle.c
	rm pintoSingle.c.temp
	$(CC) $(CFLAGS) -c pintoSingle.c
	ar rcs libPinto.a pintoSingle.o

profile: CFLAGS += -pg
profile: fast

coverage:
	gcov -o . *.c
	mkdir lcov
	lcov --directory . --capture --output-file ./lcov/pinto.info
	genhtml ./lcov/pinto.info -o ./lcov
	@echo "Load ./pintoLib/lcov/index.html in your browser for lcov data."

singleCoverage:
	gcov -o . ./pintoSingle.c
	mkdir lcov
	lcov --directory . --capture --output-file ./lcov/pinto.info
	genhtml ./lcov/pinto.info -o ./lcov
	@echo "Load ./pintoLib/lcov/index.html in your browser for lcov data."

clean:
	rm -f pintoSingle.c
	rm -f libPinto.a
	rm -f *.o
	rm -f *.gcda
	rm -f *.gcno
	rm -rf lcov

libPinto.a: $(OBJECTS) $(HEADERS)
	ar rcs libPinto.a *.o

%.o : %.c *.h
	$(CC) $(CFLAGS) -c $<

