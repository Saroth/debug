
OPTS = -Wall -nostartfiles -e __entry_test_debug__

# DEFS = -DDS_DEBUG_MAIN

CFLAGS  = $(OPTS)

LDFLAGS = $(OPTS) $(DEFS)

test: $(patsubst %.c, %.o, ${wildcard *.c})

clean:
	-rm test *.o

