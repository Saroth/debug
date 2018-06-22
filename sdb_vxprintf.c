#include <stdio.h>

#include "sdb_internal.h"

int sdb_vxprintf(sdb_xprintf_context *ctx)
{
#warning "TODO: achieve format convert"
    char buf[1024];
    vsnprintf(buf, sizeof(buf), ctx->fmt, ctx->va);
    return ctx->f_out(ctx->p_out, buf, ctx->state);
}

