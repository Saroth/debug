#include "sdb_internal.h"

int sdb_vxprintf(sdb_xprintf_context *ctx, sdb_out_state state)
{
#warning "TODO: achieve format convert"
    return ctx->f_out(ctx->p_out, ctx->fmt, state);
}

