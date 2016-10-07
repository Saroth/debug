# CC = g++
CFLAGS = -Wall -O2
OBJ_FILE = sdb_bio.o sdb_output.o sdb_input.o sdb_dump.o
HEAD_FILE = libsdb.h sdb_config.h sdb_internal.h

all: libsdb.a sdb_selftest

libsdb.a: $(OBJ_FILE)
	ar rc libsdb.a $^

%.o: %.c $(HEAD_FILE)
	$(CC) -c -o $@ $<

sdb_selftest: $(OBJ_FILE) sdb_selftest.c
	$(CC) $(CFLAGS) -DSDB_MDL_SELFTEST -I. -c -o sdb_selftest.o sdb_selftest.c
	$(CC) $(CFLAGS) -L. -o $@ sdb_selftest.o -lsdb

.PHONY: clean
clean:
	-rm -f *.o *.d libsdb.a sdb_selftest

