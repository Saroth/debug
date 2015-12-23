
OPTS = -Wall -O2

DEFS = -DDS_DEBUG_MAIN=1

CFLAGS  = $(OPTS) $(DEFS)

LDFLAGS = $(OPTS) $(DEFS)

test_debug: $(patsubst %.c, %.o, $(wildcard *.c))
	$(CC) $(OPTS) $(DEFS) -DDBG_MODULE_TEST=1 -c -o test_debug.o test_debug.c
	$(CC) $(OPTS) -nostartfiles -e __entry_test_debug__ $(DEFS) -o $@ $^

lib: $(patsubst %.c, %.o, $(wildcard *.c))
	ar rc lib_debug.a $^

clean:
	-rm test_debug *.o *.a

