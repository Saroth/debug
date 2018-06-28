#include <stdio.h>

#include "sdb_internal.h"

int sdb_vxprintf(sdb_xprintf_context *ctx)
{
#warning "TODO: achieve format convert"
    char buf[1024];
    int ret = vsnprintf(buf, sizeof(buf), ctx->fmt, ctx->va);
    if (ret < 0) {
        return ret;
    }
    return ctx->f_out(ctx->p_out, buf, ret, ctx->state);
}

