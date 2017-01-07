#include "sdb_config.h"

#if defined(SDB_SYS_SUPPORT_STDERR)
#include <errno.h>
#include <string.h>
#endif

#if defined(SDB_ENABLE)

inline int cb_putx(void *p, const char *buf, unsigned int len)
{
    return bio_put(((bio_put_param_t *)p)->cfg,
            ((bio_put_param_t *)p)->flag, buf, len);
}

#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
void set_color(unsigned int flag, const char **head, const char **end)
{
    unsigned int type = flag & SDB_TYPE_MASK;
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

    *head = "";
    *end = hl[HL_RES];
    if (type == SDB_TYPE_ERR)
        *head = hl[HL_ERR];
    else if (type == SDB_TYPE_WARN)
        *head = hl[HL_WARN];
    else if (type == SDB_TYPE_INPUT_NUM
            || type == SDB_TYPE_INPUT_STR
            || type == SDB_TYPE_INPUT_ECHO)
        *head = hl[HL_INPUT];
    else
        *end = "";
}
#endif /* defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES) */

int put_i2s(bio_put_param_t *p, int num)
{
    print_context_t ctx;

    ctx.ptr             = p;
    ctx.put             = cb_putx;
    ctx.fmt.base        = 10;
    ctx.fmt.flag        = 0;
    ctx.fmt.width       = 0;
    ctx.fmt.precision   = 0;

    return (int)i2s(&ctx, num);
}

static int put_proc(put_param_t *p)
{
    int ret;
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
    const char *head;
    const char *end;
#endif

    p->bio.flag &= SDB_TYPE_MASK;
    PUT_STR_BLK(&p->bio, SDB_DATA_FILE, p->file, 0);
    PUT_STR_BLK(&p->bio, SDB_DATA_FUNC, p->func, 0);
    PUT_NUM_BLK(&p->bio, SDB_DATA_LINE, p->line);

    if (p->fmt) {
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
        set_color(p->bio.flag, &head, &end);
        PUT_STR(&p->bio, SDB_DATA_COLOR, head, 0);
#endif
        p->bio.flag |= SDB_DATA_INFO;
        if ((ret = vxprint((void *)&p->bio, cb_putx, p->fmt, p->va)) < 0)
            return ret;
        p->bio.flag &= SDB_TYPE_MASK;
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
        PUT_STR_BLK(&p->bio, SDB_DATA_COLOR, end, 0);
#endif
    }
#if defined(SDB_SYS_SUPPORT_STDERR)
    if (p->err) {
        errno = 0;
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
        set_color(SDB_TYPE_ERR, &head, &end);
        PUT_STR(&p->bio, SDB_DATA_COLOR, head, 0);
#endif
        PUT_STR(&p->bio, SDB_DATA_STDERR, "  [", 3);
        PUT_STR(&p->bio, SDB_DATA_STDERR, strerror(p->err), 0);
        PUT_STR(&p->bio, SDB_DATA_STDERR, "(", 1);
        PUT_NUM(&p->bio, SDB_DATA_STDERR, p->err);
        PUT_STR(&p->bio, SDB_DATA_STDERR, ")]", 2);
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
        PUT_STR_BLK(&p->bio, SDB_DATA_COLOR, end, 0);
#endif
    }
#endif

    return 0;
}

int sdb_putx(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, unsigned int line,
        const char *fmt, ...)
{
    int ret;
    put_param_t p;

    flag &= SDB_TYPE_MASK;
#if defined(SDB_SYS_SUPPORT_STDERR)
    p.err           = errno;
#endif
    p.bio.cfg       = cfg;
    p.bio.flag      = flag;
    p.file          = file;
    p.func          = func;
    p.line          = line;
    p.fmt           = fmt;

    PUT_PEND(&p.bio);
    va_start(p.va, fmt);
    ret = put_proc(&p);
    va_end(p.va);
    if (ret)
        return ret;
    if (flag != SDB_TYPE_INPUT_NUM && flag != SDB_TYPE_INPUT_STR) {
        PUT_WRAP(&p.bio);
    }
    PUT_POST(&p.bio);

    return ret;
}

int sdb_put_bare(const sdb_config_t *cfg, const char *fmt, ...)
{
    int ret;
    va_list va;
    bio_put_param_t bio;

    bio.cfg = cfg;
    bio.flag = SDB_TYPE_INFO;

    PUT_PEND(&bio);
    va_start(va, fmt);
    ret = vxprint((void *)&bio, cb_putx, fmt, va);
    va_end(va);
    if (ret)
        return ret;
    PUT_POST(&bio);

    return ret;
}

#else
inline int sdb_putx(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, unsigned int line,
        const char *fmt, ...)
{
    return 0;
}
inline int sdb_put_bare(const sdb_config_t *cfg, const char *fmt, ...)
{
    return 0;
}
#endif /* defined(SDB_ENABLE) */

