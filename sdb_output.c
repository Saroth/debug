#include "sdb_config.h"

#if defined(SDB_ENABLE)

#define SDB_SET_CONFIG p->ctx->cfg

#if defined(SDB_SYS_HAVE_COLOR)
static void get_color(int flag, const char **head, const char **end)
{
    int ret;
    int lv = flag & SDB_FLG_LV;
    enum {
        HL_RES,
        HL_ERR,
        HL_WARN,
        HL_INPUT,
    };
    const char *hl[] = {
        SDB_COLOR_RES,
        SDB_COLOR_ERR,
        SDB_COLOR_WARN,
        SDB_COLOR_INPUT,
    };

    *head = NULL;
    *end = hl[HL_RES];
    if (lv == SDB_FLG_LV_ERR)
        *head = hl[HL_ERR];
    else if (lv == SDB_FLG_LV_WARN)
        *head = hl[HL_WARN];
    else if (flag & SDB_FLG_T_INPUT_NUM
            || flag & SDB_FLG_T_INPUT_STR
            || flag & SDB_FLG_T_INPUTECHO)
        *head = hl[HL_INPUT];
    else if (flag & SDB_FLG_STDERR)
        *head = hl[HL_ERR];
    else
        *end = NULL;
}
static void str_append(output_param_t *p, const char *str)
{
    if (str) {
        strncpy(p->ctx->buf + p->ctx->len, str,
                SDB_CONF_BUFFER_SIZE - p->ctx->len);
        p->ctx->len += strlen(str);
    }
}
#endif /* defined(SDB_SYS_HAVE_COLOR) */

#if defined(SDB_SYS_HAVE_STDERR)
static int output_stderr(output_param_t *p)
{
    if (p->errnum) {
        int ret;

#if defined(SDB_SYS_HAVE_COLOR)
        const char *head;
        const char *end;
        get_color(SDB_FLG_STDERR, &head, &end);
        str_append(p, head);
#endif
        ret = snprintf(p->ctx->buf + p->ctx->len,
                SDB_CONF_BUFFER_SIZE - p->ctx->len,
                " [%s(%d)]", strerror(p->errnum), p->errnum);
        if (ret > SDB_CONF_BUFFER_SIZE) {
            SDB_OUT_W("Output string too long.");
            ret = SDB_CONF_BUFFER_SIZE - p->ctx->len;
        }
        errno = 0;
        p->ctx->len += ret;
        p->ctx->flag |= SDB_FLG_STDERR;
#if defined(SDB_SYS_HAVE_COLOR)
        str_append(p, end);
#endif
    }

    return 0;
}
#endif /* defined(SDB_SYS_HAVE_STDERR) */

static int output_vprintf(output_param_t *p)
{
    int ret;

#if defined(SDB_SYS_HAVE_COLOR)
    const char *head;
    const char *end;
    get_color(p->ctx->flag, &head, &end);
    str_append(p, head);
#endif
    ret = vsnprintf(p->ctx->buf + p->ctx->len,
            SDB_CONF_BUFFER_SIZE - p->ctx->len, p->fmt, p->ap);
    if (ret > SDB_CONF_BUFFER_SIZE) {
        SDB_OUT_W("Output string too long.");
        ret = SDB_CONF_BUFFER_SIZE - p->ctx->len;
    }
    else if (ret < 0) {
        SDB_OUT_E("vsnprintf");
        return ret;
    }
    p->ctx->len += ret;
#if defined(SDB_SYS_HAVE_COLOR)
    str_append(p, end);
#endif

    return 0;
}

static int output_proc(output_param_t *p)
{
    int ret = 0;

    if ((~p->ctx->cfg->opt) & SDB_IO)
        return 0;
    if (p->fmt)
        if ((ret = output_vprintf(p)) < 0)
            return ret;
#if defined(SDB_SYS_HAVE_STDERR)
    if ((ret = output_stderr(p)) < 0)
        return ret;
#endif

    return bio_output(p->ctx);
}

int sdb_output_v(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, int line,
        const char *fmt, va_list ap)
{
    int ret;
    output_param_t p;
    sdb_bio_context_t ctx;
    char buf[SDB_CONF_BUFFER_SIZE];

#if defined(SDB_SYS_HAVE_STDERR)
    p.errnum        = errno;
#endif
    p.fmt           = fmt;
    p.ctx           = &ctx;
    va_copy(p.ap, ap);
    ctx.cfg         = cfg;
    ctx.flag        = flag;
    ctx.file        = file;
    ctx.func        = func;
    ctx.line        = line;
    ctx.buf         = buf;
    ctx.len         = 0;

    return output_proc(&p);
}

int sdb_output(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, size_t line, const char *fmt, ...)
{
    int ret;
    output_param_t p;
    sdb_bio_context_t ctx;
    char buf[SDB_CONF_BUFFER_SIZE];

#if defined(SDB_SYS_HAVE_STDERR)
    p.errnum        = errno;
#endif
    p.fmt           = fmt;
    p.ctx           = &ctx;
    ctx.cfg         = cfg;
    ctx.flag        = flag;
    ctx.file        = file;
    ctx.func        = func;
    ctx.line        = line;
    ctx.buf         = buf;
    ctx.len         = 0;

    va_start(p.ap, fmt);
    ret = output_proc(&p);
    va_end(p.ap);

    return ret;
}

#else
inline int sdb_output(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, size_t line, const char *fmt, ...)
{
    return 0;
}
#endif /* defined(SDB_ENABLE) */

