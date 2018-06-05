#include "sdb_internal.h"

#if defined(SDB_SYSTEM_HAS_STDERR)
#include <errno.h>
#include <string.h>
#endif

int sdb_out(sdb_context *ctx, size_t mode,
        const char *file, size_t line, const char *fmt, ...);
{
    sdb_xprintf_context xctx = {
        .ctx    = ctx,
        .file   = file,
        .line   = line,
    };
    va_list va;
    va_start(va, fmt);
    int ret = sdb_vxprintf(&xctx, fmt, va);
    va_end(va);
    return ret;
}

