.PHONY: usage all fast small debug debug2 debug3 single singleFast singleSmall singleDebug singleCoverage profile test vtest coverage clean pintoLib pintoTest

ARGS =

CFLAGS += -Wall -Werror -Wextra

# targets to test against c89, c99, and c11 standards
c89: CFLAGS += -std=c89 -pedantic
c89: all

c99: CFLAGS += -std=c99 -pedantic
c99: all

c11: CFLAGS += -std=c11 -pedantic
c11: all

export CFLAGS
export LDFLAGS

usage:
	@echo "targets:"
	@echo "  fast     - build fast version"
	@echo "  small    - build small version"
	@echo "  debug    - build for debug"
	@echo "  debug2   - build for debug, with necessary flags to"
	@echo "             generate test coverage."
	@echo "  debug3   - build for debug, with PARANOID checks,"
	@echo "             PINTO_ENABLE_LOGGING, and necessary flags to"
	@echo "             generate test coverage."
	@echo "  single   - build a combined pintoSingle.c file"
	@echo "  profile  - build for profiling"
	@echo "  test     - run tests"
	@echo "  vtest    - run tests in valgrind"
	@echo "  coverage - generate coverage report with gcov and lcov"
	@echo "  clean    - cleans up files"

all: pintoLib pintoTest rgbaToPinto pintoToRgba

fast: ARGS = fast
fast: all

small: ARGS = small
small: all

debug3: ARGS = debug3
debug3: CFLAGS += -DPINTO_ENABLE_LOGGING
debug3: CFLAGS += -DBE_PARANOID
debug3: CFLAGS += -DPINTO_DEBUG=3
debug3: pintoLib pintoTest

debug2: ARGS = debug2
debug2: CFLAGS += -DPINTO_DEBUG=2
debug2: pintoLib pintoTest

debug: ARGS = debug
debug: CFLAGS += -DPINTO_DEBUG=1
debug: pintoLib pintoTest

single: ARGS = single
single: all

singleFast: ARGS = singleFast
singleFast: all

singleSmall: ARGS = singleSmall
singleSmall: all

singleDebug: ARGS = singleDebug
singleDebug: CFLAGS += -DPINTO_DEBUG=1
singleDebug: pintoLib pintoTest

profile: ARGS = profile
profile: all

test: pintoTest
	pintoTest/pintoTest -t all

vtest:
	valgrind --leak-check=full pintoTest/pintoTest -t all

coverage:
	cd pintoLib && $(MAKE) coverage

singleCoverage:
	cd pintoLib && $(MAKE) singleCoverage

clean:
	rm -rf html
	rm -f gmon.out
	cd pintoLib && $(MAKE) clean
	cd pintoTest && $(MAKE) clean
	cd utilities/rgbaToPinto && $(MAKE) clean
	cd utilities/pintoToRgba && $(MAKE) clean

rgbaToPinto:
	cd utilities/rgbaToPinto && $(MAKE) $(ARGS)

pintoToRgba:
	cd utilities/pintoToRgba && $(MAKE) $(ARGS)

pintoTest:
	cd pintoTest && $(MAKE) $(ARGS)

pintoLib:
	cd pintoLib && $(MAKE) $(ARGS)

