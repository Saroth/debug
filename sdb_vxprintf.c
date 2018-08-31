#include <stdio.h>

#include "sdb_internal.h"

int sdb_vxprintf(sdb_xprintf_context *ctx)
{
#if defined(SDB_IMPLEMENT_FORMATTED_PRINT)
    int ret;

    // TODO

    return 0;
#else
    int ret;
    char buf[1024];
    if ((ret = vsnprintf(buf, sizeof(buf), ctx->fmt, ctx->va)) < 0) {
        return ret;
    }
    return ctx->f_out(ctx->p_out, buf, ret, ctx->state);
#endif /* defined(SDB_IMPLEMENT_FORMATTED_PRINT) */
}

