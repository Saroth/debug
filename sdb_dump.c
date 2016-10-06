#include "sdb_config.h"

#if defined(SDB_ENABLE) && defined(SDB_MDL_DUMP_ENABLE)

int sdb_dump(int opt, int mode, char *file, const char *func, int line,
        char *buf, int len, void *addr, const char *format, ...)
{
    int ret = 0;
    SDB_IO_PARAM_T p;

    if ((~opt) & SDB_IO || (~opt) & SDB_NO_DUMP) {
        return 0;
    }
    p.options   = opt;
    p.mode      = mode;
    p.file      = file;
    p.func      = func;
    p.line      = line;
    if (format) {
        va_start(p.ap, format);
        sdb_voutput(opt, mode, file, func, line, format, p.ap);
        va_end(p.ap);
    }

    return 0;
}

#endif /* defined(SDB_ENABLE) && defined(SDB_MDL_DUMP_ENABLE) */

