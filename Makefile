
OPTS = -Wall -nostartfiles -e __entry_test_debug__

DEFS = # -DDS_DEBUG_MAIN

SRCS = debug_bio.c\
	   debug_out.c\
	   debug_in.c\
	   debug_log.c\
	   debug_dump.c\
	   debug_test.c\
	   test.c

all:
	gcc -o test $(SRCS) $(DEFS) $(OPTS)

clean:
	rm test

