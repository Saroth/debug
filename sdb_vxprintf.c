#include <stdio.h>

#include "sdb_internal.h"

int sdb_vxprintf(sdb_xprintf_context *ctx)
{
#warning "TODO: achieve format convert"
    int ret;
    char buf[1024];
    if ((ret = vsnprintf(buf, sizeof(buf), ctx->fmt, ctx->va)) < 0) {
        return ret;
    }
    return ctx->f_out(ctx->p_out, buf, ret, ctx->state);
}

