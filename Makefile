
OPTS = -Wall -nostartfiles -e __entry_test_debug__

# DEFS = -DDS_DEBUG_MAIN

CFLAGS  = $(OPTS) $(DEFS)

LDFLAGS = $(OPTS) $(DEFS)

test_debug: $(patsubst %.c, %.o, $(wildcard *.c))

clean:
	-rm test_debug *.o

