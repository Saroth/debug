
OPTS = -nostartfiles -e __entry_debug_test__ -std=gnu99

DEFS = -DDBG_MAIN -DDBG_DUMP -DDBG_LOG

all:
	gcc -o test debug.c debug_test.c $(DEFS) $(OPTS)

clean:
	rm test

