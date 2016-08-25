CFLAGS = -Wall -O2
OBJ_FILE = bio.o output.o libsdb.o
HEAD_FILE = config.h libsdb.h sdb_internal.h

all: libsdb.a sdb_selftest

libsdb.a: $(HEAD_FILE) $(OBJ_FILE)
	ar rc libsdb.a $^

%.o: %.c $(HEAD_FILE)
	$(CC) $(CFLAGS) -c $^

sdb_selftest: $(OBJ_FILE) libsdb.h sdb_selftest.c 
	$(CC) $(CFLAGS) -DSDB_SELFTEST -I. -c -o sdb_selftest.o sdb_selftest.c
	$(CC) $(CFLAGS) -L. -nostartfiles -e __entry_sdb_selftest__\
		-o $@ sdb_selftest.o -lsdb

clean:
	-rm -f *.o libsdb.a sdb_selftest

