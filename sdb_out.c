#include "sdb_internal.h"

#if defined(SDB_SYSTEM_HAS_STDERR)
#include <errno.h>
#include <string.h>
#endif
static int cb_out(sdb_xprintf_context *ctx, const char *str,
        unsigned int state)
{
    do {
        if (ctx->len == 0) {
            /* + color + mark + color */
        }
        while (*str && ctx->len < SDB_CONFIG_COLUMN_LIMIT) {
            ctx->buf[ctx->len++] = *str++;
        }
#if defined(SDB_SYSTEM_HAS_STDERR)
        if (state == OUT_STATE_FINAL && ctx->err) {
            /* + color */
            str = strerror(ctx->err);
            state = OUT_STATE_STDERR;
            continue;
        }
#endif
        if (ctx->len == SDB_CONFIG_COLUMN_LIMIT || state != OUT_STATE_NONE) {
            /* + color:rec */
            ctx->buf[ctx->len] = 0;
            int ret = sdb_bio_out(ctx->ctx, ctx->file, ctx->line, ctx->buf);
            if (ret < 0) {
                return ret;
            }
            ctx->counter += ret;
            ctx->len = 0;
        }
    } while (*str);
    return 0;
}

int sdb_out(sdb_context *ctx, unsigned int mode,
        const char *file, size_t line, const char *fmt, ...)
{
    char buf[SDB_CONFIG_OUTPUT_BUFFER_SIZE];
    sdb_xprintf_context xctx = {
        .ctx    = ctx,
        .mode   = mode,
        .file   = file,
        .line   = line,
        .buf    = buf,
        .len    = 0,
#if defined(SDB_SYSTEM_HAS_STDERR)
        .err    = errno,
#endif
    };
    va_list va;
    va_start(va, fmt);
    int ret = sdb_vxprintf(&xctx, fmt, va);
    va_end(va);
    return ret;
}

