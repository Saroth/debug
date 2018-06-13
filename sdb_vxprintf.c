#include "sdb_internal.h"

int sdb_vxprintf(sdb_xprintf_context *ctx)
{
    return ctx->f_out(ctx->p_out, ctx->fmt, SDB_OUT_FINAL);
}


