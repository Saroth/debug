
OPTS = -Wall -nostartfiles -e __entry_debug_test__

DEFS = -DDBG_MAIN -DDBG_DUMP -DDBG_LOG

SRCS = debug_bio.c\
	   debug_log.c\
	   debug_out.c\
	   debug_in.c\
	   debug_test.c

all:
	gcc -o test $(SRCS) $(DEFS) $(OPTS)

clean:
	rm test

