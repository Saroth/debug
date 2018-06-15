#include "sdb_internal.h"

#if defined(SDB_SYSTEM_HAS_STDERR)
#include <errno.h>
#include <string.h>
#endif

typedef struct {
    char *buf;
    size_t size;
    size_t len;
} output_to_buffer_param;
static int output_to_buffer(void *p_out, const char *str, sdb_out_state state)
{
    output_to_buffer_param *p = (output_to_buffer_param *)p_out;
    if (state == SDB_OUT_INIT) {
        p->len = 0;
    }
    while (*str && p->len < p->size) {
        p->buf[p->len++] = *str++;
    }
    if (p->len == p->size || state == SDB_OUT_FINAL) {
        p->buf[(p->len == p->size) ? (p->len - 1) : p->len] = 0;
        size_t l = p->len;
        p->len = 0;
        return l;
    }
    return 0;
}

int sdb_vsnprintf(char *buf, size_t size, const char *fmt, va_list va)
{
    output_to_buffer_param param = {
        .buf            = buf,
        .size           = size,
        .len            = 0,
    };
    sdb_xprintf_context xctx = {
        .f_out          = output_to_buffer,
        .p_out          = (void *)&param,
        .fmt            = fmt,
    };
    va_copy(xctx.va, va);
    return sdb_vxprintf(&xctx, SDB_OUT_FINAL);
}

static int output_line(void *p_out, const char *str, sdb_out_state state)
{
    sdb_cout_context *p = (sdb_cout_context *)p_out;
    do {
        if (state == SDB_OUT_INIT) {
            p->line_buf_len = 0;
        }
        if (p->line_buf_len == 0) {
#warning "TODO: + color, + mark"
        }
        while (*str && p->line_buf_len < p->ctx->out_column_limit) {
            p->line_buf[p->line_buf_len++] = *str++;
        }
#if defined(SDB_SYSTEM_HAS_STDERR)
        if (state == SDB_OUT_FINAL && p->err) {
// #warning "TODO: + color"
            str = strerror(p->err);
            state = SDB_OUT_STDERR;
            continue;
        }
#endif
        if (p->line_buf_len >= p->ctx->out_column_limit
                || state != SDB_OUT_NONE) {
            if (p->line_buf_len > p->ctx->out_column_limit) {
                p->line_buf_len = p->ctx->out_column_limit;
            }
// #warning "TODO: + color:rec"
            p->line_buf[p->line_buf_len] = 0;
            int ret = sdb_bio_out(p->ctx, p->file, p->line, p->line_buf);
            if (ret < 0) {
                return ret;
            }
            p->counter += ret;
            p->line_buf_len = 0;
            return p->counter;
        }
    } while (*str);
    return 0;
}

void __sdb_vmcout_init(sdb_cout_context *ctx,
        sdb_context *sdb_ctx, unsigned int mode, char *buf, size_t size,
        const char *file, size_t line)
{
    ctx->ctx            = sdb_ctx;
    ctx->mode           = mode;
    ctx->file           = file;
    ctx->line           = line;
#if defined(SDB_SYSTEM_HAS_STDERR)
    ctx->err            = errno;
#endif
    ctx->line_buf       = buf;
    ctx->line_buf_size  = size;
    ctx->line_buf_len   = 0;
    ctx->counter        = 0;
#if defined(SDB_SYSTEM_HAS_STDERR)
    errno               = 0;
#endif
    output_line(ctx, "", SDB_OUT_INIT);
}
int __sdb_vmcout_append(sdb_cout_context *ctx, const char *fmt, va_list va)
{
    sdb_xprintf_context xctx = {
        .f_out          = output_line,
        .p_out          = (void *)ctx,
        .fmt            = fmt,
    };
    va_copy(xctx.va, va);
    return sdb_vxprintf(&xctx, SDB_OUT_NONE);
}
int __sdb_vmcout_final(sdb_cout_context *ctx, const char *fmt, va_list va)
{
    sdb_xprintf_context xctx = {
        .f_out          = output_line,
        .p_out          = (void *)ctx,
        .fmt            = fmt,
    };
    va_copy(xctx.va, va);
    return sdb_vxprintf(&xctx, SDB_OUT_FINAL);
}

int __sdb_vmcout(sdb_context *ctx, unsigned int mode,
        const char *file, size_t line, const char *fmt, va_list va)
{
    char buf[ctx->out_column_limit + SDB_CONFIG_OUTPUT_BUFFER_RESERVE];
    sdb_cout_context param;
    __sdb_vmcout_init(&param, ctx, mode, buf, sizeof(buf), file, line);
    return __sdb_vmcout_final(&param, fmt, va);
}

