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

int put_u2s(bio_put_param_t *p, unsigned char base, unsigned char flag,
        unsigned char width, unsigned long num)
{
    print_context_t ctx;

    ctx.ptr             = p;
    ctx.put             = cb_putx;
    ctx.base            = base;
    ctx.flag            = flag;
    ctx.width           = width;

    return uli2s(&ctx, num);
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

int sdb_putx(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, unsigned int line,
        const char *fmt, ...)
{
    int ret;
    int err;
    bio_put_param_t bio;
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
    const char *head;
    const char *end;
#endif

#if defined(SDB_SYS_SUPPORT_STDERR)
    err             = errno;
#endif
    flag &= SDB_TYPE_MASK;
    bio.cfg         = cfg;
    bio.flag        = flag;

    PUT_PEND(&bio);
    bio.flag &= SDB_TYPE_MASK;
    PUT_STR_BLK(&bio, SDB_DATA_FILE, file, 0);
    PUT_STR_BLK(&bio, SDB_DATA_FUNC, func, 0);
    PUT_STR_BLK(&bio, SDB_DATA_LINE, (const char *)&line, 1);

    if (fmt) {
        va_list va;
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
        set_color(bio.flag, &head, &end);
        PUT_STR(&bio, SDB_DATA_COLOR, head, 0);
#endif
        bio.flag |= SDB_DATA_INFO;
        va_start(va, fmt);
        ret = vxprint((void *)&bio, cb_putx, fmt, va);
        va_end(va);
        if (ret)
            return ret;
        bio.flag &= SDB_TYPE_MASK;
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
        PUT_STR_BLK(&bio, SDB_DATA_COLOR, end, 0);
#endif
    }
#if defined(SDB_SYS_SUPPORT_STDERR)
    if (err) {
        errno = 0;
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
        set_color(SDB_TYPE_ERR, &head, &end);
        PUT_STR(&bio, SDB_DATA_COLOR, head, 0);
#endif
        /*
         * bio.flag |= SDB_DATA_STDERR;
         * if ((ret = xprint(&bio, cb_putx, "  [\"%s\"(%d)]", strerror(err), err)))
         *     return ret;
         * bio.flag &= ~SDB_DATA_STDERR;
         */
        PUT_STR(&bio, SDB_DATA_STDERR, "  [", 3);
        PUT_STR(&bio, SDB_DATA_STDERR, strerror(err), 0);
        PUT_STR(&bio, SDB_DATA_STDERR, "(", 1);
        PUT_NUM(&bio, SDB_DATA_STDERR, 10, 0, 0, err);
        PUT_STR(&bio, SDB_DATA_STDERR, ")]", 2);
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
        PUT_STR_BLK(&bio, SDB_DATA_COLOR, end, 0);
#endif
    }
#endif
    if (flag != SDB_TYPE_INPUT_NUM && flag != SDB_TYPE_INPUT_STR) {
        PUT_WRAP(&bio);
    }
    PUT_POST(&bio);

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

