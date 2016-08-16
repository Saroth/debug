#include "config.h"

#if defined(SDB_ENABLE)

int sdb_stdout(const void * fmt, ...)
{
    int count = 0;
    va_list marker;
    char buf[SDB_CACHE_SIZE + 4];

    if (fmt == NULL) {
        sdb_out_e(DS_SDB, "Param error.");
        return SDB_RET_PARAM_ERR;
    }
    va_start(marker, fmt);
    count = vsnprintf(buf, SDB_CACHE_SIZE, fmt, marker);
    va_end(marker);
    if (count > SDB_CACHE_SIZE) {
        sdb_out_w(DS_SDB, "Output string too long.");
        count = SDB_CACHE_SIZE;
    }
    else if (count < 0) {
        sdb_err_i(DS_SDB, "vsnprintf");
        return count;
    }

    return sdb_bio_out(buf, count);
}

#endif

