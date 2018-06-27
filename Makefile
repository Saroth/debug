# CC = arm-none-linux-gnueabi-gcc
# CC = arm-elf-gcc
CFLAGS = -Wall -Os -pedantic # -ansi # -std=c89
OBJ_FILE = sdb_utility.o sdb_string.o sdb_stack_watch.o sdb_config.o sdb_bio.o\
		   sdb_vxprintf.o sdb_out.o sdb_in.o sdb_dump.o sdb_menu.o
HEAD_FILE = libsdb.h sdb_config.h sdb_internal.h

all: libsdb.a test_sdb

libsdb.a: $(OBJ_FILE)
	ar rc libsdb.a $^

%.o: %.c $(HEAD_FILE)
	$(CC) -c -o $@ $<

test_sdb: test_sdb.c libsdb.a
	$(CC) $(CFLAGS) -DSDB_SELFTEST -I. -c -o test_sdb.o test_sdb.c
	$(CC) $(CFLAGS) -L. -o $@ test_sdb.o -lsdb

.PHONY: clean
clean:
	-rm -f *.o *.d libsdb.a test_sdb

