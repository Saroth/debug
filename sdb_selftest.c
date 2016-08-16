#include <libsdb.h>

#if defined(SDB_SELFTEST)

#define DS_SDB_SELFTEST (SDB_IO | SDB_LINE | SDB_FILE)

int sdb_selftest(void *p)
{
    sdb_out_entry(DS_SDB_SELFTEST);
    return 0;
}

#else
int sdb_selftest(void *p) { return 0; }
#endif /* defined(SDB_SELFTEST) */

int __entry_sdb_selftest__(int argc, char *argv[])
{
    sdb_selftest(NULL);
    exit(0); 
}

