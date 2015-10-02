
OPTS = -Wall -nostartfiles -e __entry_test_debug__

# DEFS = -DDS_DEBUG_MAIN

CFLAGS  = $(OPTS) $(DEFS)

LDFLAGS = $(OPTS) $(DEFS)

test_debug: $(patsubst %.c, %.o, $(wildcard *.c))

lib: $(patsubst %.c, %.o, $(wildcard *.c))
	ar rc lib_debug.a $^

clean:
	-rm test_debug *.o *.a

