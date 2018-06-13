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
    while (*str && p->len < p->size) {
        p->buf[p->len++] = *str++;
    }
    if (p->len == p->size || state != SDB_OUT_NONE) {
        p->buf[(p->len == p->size) ? (p->len - 1) : p->len] = 0;
        return p->len;
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
    return sdb_vxprintf(&xctx);
}

typedef struct {
    sdb_context *ctx;
    unsigned int mode;
    const char *file;
    size_t line;
#if defined(SDB_SYSTEM_HAS_STDERR)
    int err;
#endif
    char *line_buf;
    size_t line_buf_size;
    size_t line_buf_len;
    size_t counter;
} output_line_param;
static int output_line(void *p_out, const char *str, sdb_out_state state)
{
    output_line_param *p = (output_line_param *)p_out;
    do {
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

int __sdb_vmcout(sdb_context *ctx, unsigned int mode,
        const char *file, size_t line, const char *fmt, va_list va)
{
    char buf[ctx->out_column_limit + SDB_CONFIG_OUTPUT_BUFFER_RESERVE];
    output_line_param param = {
        .ctx            = ctx,
        .mode           = mode,
        .file           = file,
        .line           = line,
#if defined(SDB_SYSTEM_HAS_STDERR)
        .err            = errno,
#endif
        .line_buf       = buf,
        .line_buf_size  = sizeof(buf),
        .line_buf_len   = 0,
        .counter        = 0,
    };
    sdb_xprintf_context xctx = {
        .f_out          = output_line,
        .p_out          = (void *)&param,
        .fmt            = fmt,
    };
    va_copy(xctx.va, va);
    return sdb_vxprintf(&xctx);
}

