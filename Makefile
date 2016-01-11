CFLAGS = -Wall -O2 -DDS_DEBUG_MAIN=1
FILE_OBJ=$(patsubst %.c, %.o, $(wildcard src/*.c))

build/libdebug.a: $(FILE_OBJ)
	ar rc build/libdebug.a $^
	@cp src/debug.h build/libdebug.h
	@cp build/libdebug.a lib/libdebug.a
	@cp build/libdebug.h inc/libdebug.h

build/test_debug:
	$(CC) $(CFLAGS) -DDBG_MODULE_TEST=1 -I./inc -c\
		-o test/test_debug.o test/test_debug.c
	$(CC) $(CFLAGS) -L./lib -nostartfiles -e __entry_test_debug__\
		-o $@ test/test_debug.o -ldebug

all: build/libdebug.a build/test_debug

clean:
	-@rm build/*
	-@rm src/*.o
	-@rm test/*.o
	-@rm inc/libdebug.h
	-@rm lib/libdebug.a


