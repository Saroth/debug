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
static int output_to_buffer(void *p_out, const char *str, size_t len,
        sdb_out_state state)
{
    output_to_buffer_param *p = (output_to_buffer_param *)p_out;
    if (state == SDB_OUT_INIT) {
        p->len = 0;
    }
    if (!str) {
        str = SDB_NULL_MARK;
    }
    while (len-- && p->len < p->size) {
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
        .buf                = buf,
        .size               = size,
        .len                = 0,
    };
    sdb_xprintf_context xctx = {
        .f_out              = output_to_buffer,
        .p_out              = (void *)&param,
        .state              = SDB_OUT_FINAL,
        .fmt                = fmt,
    };
    va_copy(xctx.va, va);
    int ret = sdb_vxprintf(&xctx);
    va_end(xctx.va);
    return ret;
}

int sdb_snprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    int ret = sdb_vsnprintf(buf, size, fmt, va);
    va_end(va);
    return ret;
}


static inline const char *get_mark(sdb_cout_context *p) {
    if (p->ctx->marks == 0) {
        return "";
    }
    switch (p->mode & SDB_TYPE_MASK) {
        case SDB_TYPE_NONE:         return p->ctx->marks->none;         break;
        case SDB_TYPE_INFO:         return p->ctx->marks->info;         break;
        case SDB_TYPE_WARNING:      return p->ctx->marks->warning;      break;
        case SDB_TYPE_ERROR:        return p->ctx->marks->error;        break;
        case SDB_TYPE_DUMP:         return p->ctx->marks->dump;         break;
        case SDB_TYPE_INPUT_STR:    return p->ctx->marks->input_string; break;
        case SDB_TYPE_INPUT_NUM:    return p->ctx->marks->input_number; break;
        case SDB_TYPE_INPUT_ECHO:   return p->ctx->marks->input_echo;   break;
        case SDB_TYPE_MENU:         return p->ctx->marks->menu;         break;
        default:                    return "";                          break;
    }
};
static inline const char *get_color(sdb_cout_context *p) {
    if (p->ctx->colors == 0) {
        return "";
    }
    switch (p->mode & SDB_TYPE_MASK) {
        case SDB_TYPE_DUMP:
        case SDB_TYPE_NONE:         return p->ctx->colors->normal;  break;
        case SDB_TYPE_MENU:
        case SDB_TYPE_INFO:         return p->ctx->colors->info;    break;
        case SDB_TYPE_WARNING:      return p->ctx->colors->warning; break;
        case SDB_TYPE_ERROR:        return p->ctx->colors->error;   break;
        case SDB_TYPE_INPUT_STR:
        case SDB_TYPE_INPUT_NUM:
        case SDB_TYPE_INPUT_ECHO:   return p->ctx->colors->input;   break;
        default:                    return "";                      break;
    }
};
static int output_decorate_append(sdb_cout_context *p, const char *str)
{
    int ret;
    size_t len = strlen(str);
    if (p->line_buf_offset + len >= p->line_buf_size) {
        sdb_assert(__sdb_mcout(p->ctx, SDB_MSG_ERROR, __FILE__, __LINE__,
                    "buffer reserve size not enouth.(%d < %d)",
                    p->line_buf_size, p->line_buf_offset + len));
        return SDB_ERR_RESERVE_NOT_ENOUGH;
    }
    strcpy(&p->line_buf[p->line_buf_offset], str);
    p->line_buf_offset += len;
    return 0;
}
static int output_decorate(sdb_cout_context *p, sdb_out_state state)
{
    if (p->mode & SDB_FLAG_NO_DECORATE) {
        return 0;
    }
    p->mode |= SDB_FLAG_NO_DECORATE;

    int ret;
    if (state == SDB_OUT_LINE_HEAD || state == SDB_OUT_STRING_HEAD) {
        sdb_assert(output_decorate_append(p, get_color(p)));
        if (state == SDB_OUT_LINE_HEAD) {
            const char *str = get_mark(p);
            sdb_assert(output_decorate_append(p, str));
            p->line_buf_len += strlen(str);
        }
    }
    else if (state == SDB_OUT_LINE_TAIL || state == SDB_OUT_STRING_TAIL) {
        sdb_assert(output_decorate_append(p, p->ctx->colors->tail));
    }

    p->mode &= ~SDB_FLAG_NO_DECORATE;
    return 0;
}
static int output_line(void *p_out, const char *str, size_t len,
        sdb_out_state state)
{
    sdb_cout_context *p = (sdb_cout_context *)p_out;
    if (state == SDB_OUT_INIT) {
        p->line_buf_len = 0;
        p->line_buf_offset = 0;
    }
    if (!str) {
        str = SDB_NULL_MARK;
        len = strlen(str);
    }
    if (len) {
        p->flags &= ~SDB_OUT_LINE_IS_WRAPPED;
    }
    int ret;
    do {
        p->flags &= ~SDB_OUT_LINE_IS_OUTPUTED;
        if (p->line_buf_offset == 0) {
            sdb_assert(output_decorate(p, SDB_OUT_LINE_HEAD));
        }
        while (len && p->line_buf_len < p->ctx->out_column_limit) {
            p->line_buf[p->line_buf_offset++] = *str++;
            p->line_buf_len++;
            len--;
        }
        if (p->line_buf_len >= p->ctx->out_column_limit) {
            p->flags |= SDB_OUT_LINE_IS_WRAPPED | SDB_OUT_LINE_IS_OUTPUTED;
        }
        else if ((state == SDB_OUT_FINAL || state == SDB_OUT_END_LINE)
                && len == 0) {
            p->flags |= SDB_OUT_LINE_IS_OUTPUTED;
            if (p->line_buf_len <= strlen(get_mark(p))
                    && p->flags & SDB_OUT_LINE_IS_WRAPPED) {
                p->flags &= ~SDB_OUT_LINE_IS_OUTPUTED;
            }
        }
        if (p->flags & SDB_OUT_LINE_IS_OUTPUTED) {
            sdb_assert(output_decorate(p, SDB_OUT_STRING_TAIL));
            p->line_buf[p->line_buf_offset] = 0;
            sdb_assert(sdb_bio_out(p->ctx, p->file, p->line, p->line_buf));
            p->counter += ret;
            p->line_buf_len = 0;
            p->line_buf_offset = 0;
        }
    } while (len);
    if (state == SDB_OUT_FINAL) {
#if defined(SDB_SYSTEM_HAS_STDERR)
        if (p->err) {
            int err_num = p->err;
            p->err = 0;
            p->mode = (p->mode & ~SDB_TYPE_MASK) | SDB_TYPE_ERROR;
            sdb_assert(__sdb_mcout_append(p, "  [stderr: %s(%d)]",
                        strerror(err_num), err_num));
            sdb_assert(__sdb_mcout_final(p));
        }
#endif
        return p->counter;
    }
    return 0;
}

void __sdb_mcout_init(sdb_cout_context *ctx, const sdb_context *sdb_ctx,
        unsigned int mode, char *buf, size_t size,
        const char *file, size_t line)
{
    ctx->ctx                = sdb_ctx;
    ctx->mode               = mode;
    ctx->file               = file;
    ctx->line               = line;
#if defined(SDB_SYSTEM_HAS_STDERR)
    ctx->err                = errno;
#endif
    ctx->line_buf           = buf;
    ctx->line_buf_size      = size;
    ctx->line_buf_len       = 0;
    ctx->line_buf_offset    = 0;
    ctx->counter            = 0;
#if defined(SDB_SYSTEM_HAS_STDERR)
    errno                   = 0;
#endif
    output_line(ctx, "", 0, SDB_OUT_INIT);
}
int __sdb_mcout_append_string(sdb_cout_context *ctx, const char *str)
{
    return output_line(ctx, str, strlen(str), SDB_OUT_NONE);
}
int __sdb_mcout_append(sdb_cout_context *ctx, const char *fmt, ...)
{
    sdb_xprintf_context xctx = {
        .f_out              = output_line,
        .p_out              = (void *)ctx,
        .state              = SDB_OUT_NONE,
        .fmt                = fmt,
    };
    va_list va;
    va_start(xctx.va, fmt);
    int ret = sdb_vxprintf(&xctx);
    va_end(va);
    return ret;
}
int __sdb_mcout_append_va(sdb_cout_context *ctx, const char *fmt, va_list va)
{
    sdb_xprintf_context xctx = {
        .f_out              = output_line,
        .p_out              = (void *)ctx,
        .state              = SDB_OUT_NONE,
        .fmt                = fmt,
    };
    va_copy(xctx.va, va);
    int ret = sdb_vxprintf(&xctx);
    va_end(xctx.va);
    return ret;
}
int __sdb_mcout_append_endline(sdb_cout_context *ctx)
{
    if (ctx->file == 0) {
        return output_line(ctx, "\r\n", 2, SDB_OUT_END_LINE);
    }
    return output_line(ctx, "", 0, SDB_OUT_END_LINE);
}
int __sdb_mcout_final(sdb_cout_context *ctx)
{
    return output_line(ctx, "", 0, SDB_OUT_FINAL);
}

int __sdb_vmcout(const sdb_context *ctx, unsigned int mode,
        const char *file, size_t line, const char *fmt, va_list va)
{
    char b[ctx->out_column_limit + SDB_CONFIG_OUTPUT_BUFFER_RESERVE];
    sdb_cout_context param = {
        .ctx                = ctx,
        .mode               = mode,
        .file               = file,
        .line               = line,
#if defined(SDB_SYSTEM_HAS_STDERR)
        .err                = errno,
#endif
        .line_buf           = b,
        .line_buf_size      = sizeof(b),
        .line_buf_len       = 0,
        .line_buf_offset    = 0,
        .counter            = 0,
    };
    sdb_xprintf_context xctx = {
        .f_out              = output_line,
        .p_out              = (void *)&param,
        .state              = SDB_OUT_FINAL,
        .fmt                = fmt,
    };
#if defined(SDB_SYSTEM_HAS_STDERR)
    errno                   = 0;
#endif
    va_copy(xctx.va, va);
    int ret = sdb_vxprintf(&xctx);
    va_end(xctx.va);
    return ret;
}

int __sdb_mcout(const sdb_context *ctx, unsigned int mode,
        const char *file, size_t line, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    int ret = __sdb_vmcout(ctx, mode, file, line, fmt, va);
    va_end(va);
    return ret;
}

