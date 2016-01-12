CFLAGS = -Wall -O2 -DDS_DEBUG_MAIN=1
FILE_OBJ=$(patsubst %.c, %.o, $(wildcard src/*.c))

libdebug.a: $(FILE_OBJ)
	ar rc libdebug.a $^
	@cp src/debug.h libdebug.h

test_debug:
	@cp libdebug.a lib/libdebug.a
	@cp libdebug.h inc/libdebug.h
	$(CC) $(CFLAGS) -DDBG_MODULE_TEST=1 -I./inc -c\
		-o test/test_debug.o test/test_debug.c
	$(CC) $(CFLAGS) -L./lib -nostartfiles -e __entry_test_debug__\
		-o $@ test/test_debug.o -ldebug

all: libdebug.a test_debug

clean:
	-@rm libdebug.a libdebug.h test_debug
	-@rm src/*.o
	-@rm test/*.o
	-@rm inc/libdebug.h
	-@rm lib/libdebug.a


