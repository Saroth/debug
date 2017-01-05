#include "sdb_config.h"

#include <stdarg.h>
#if defined(SDB_SYS_SUPPORT_STDERR)
#include "errno.h"
#include "string.h"
#endif

#if defined(SDB_ENABLE)

typedef struct {                        /* 内部输出参数传递结构体 */
    const sdb_config_t *cfg;            /* 配置结构体 */
    const char *file;                   /* 文件名 */
    const char *func;                   /* 函数名 */
    size_t line;                        /* 行号 */
    size_t flag;                        /* 输出标记定义, sdb_flag_t */
#if defined(SDB_SYS_SUPPORT_STDERR)
    int err;                            /* 错误码 */
#endif
    const char *fmt;                    /* 格式化字符串 */
    va_list va;                         /* 参数列表 */
} put_param_t;

static inline  int cb_putx(void *p, const char *buf, size_t len)
{
    return bio_put(((const put_param_t *)p)->cfg,
            ((put_param_t *)p)->flag, buf, len);
}

static inline int cb_put(void *p, const char *buf, size_t len)
{
    return bio_put((const sdb_config_t *)p, SDB_DATA_INFO, buf, len);
}

#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
static void set_color(size_t flag, const char **head, const char **end)
{
    size_t type = flag & SDB_TYPE_MASK;
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

#define BIO_PUTS(__data_flag, __buf, __len) do {\
    if ((ret = bio_put(p->cfg, p->flag | __data_flag, __buf, __len)))\
    return ret;\
    if ((ret = bio_put(p->cfg, p->flag | SDB_DATA_BLANK, " ", 1)))\
    return ret;\
} while (0)

static int put_proc(put_param_t *p)
{
    int ret = 0;
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
    const char *head;
    const char *end;
#endif

    p->flag &= SDB_TYPE_MASK;
    BIO_PUTS(SDB_DATA_FILE, p->file, 0);
    BIO_PUTS(SDB_DATA_FUNC, p->func, 0);
    BIO_PUTS(SDB_DATA_LINE, "_line_", p->line);

    if (p->fmt) {
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
        set_color(p->flag, &head, &end);
        BIO_PUTS(SDB_DATA_COLOR, head, 0);
#endif
        p->flag |= SDB_DATA_INFO;
        if ((ret = vxprint((void *)p, cb_putx, p->fmt, p->va)) < 0)
            return ret;
        p->flag &= SDB_TYPE_MASK;
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
        BIO_PUTS(SDB_DATA_COLOR, end, 0);
#endif
    }
#if defined(SDB_SYS_SUPPORT_STDERR)
    if (p->err) {
        errno = 0;
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
        set_color(SDB_TYPE_ERR, &head, &end);
        BIO_PUTS(SDB_DATA_COLOR, head, 0);
#endif
        BIO_PUTS(SDB_DATA_STDERR_STR, strerror(p->err), 0);
        BIO_PUTS(SDB_DATA_STDERR_NUM, "_errno_", p->err);
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
        BIO_PUTS(SDB_DATA_COLOR, end, 0);
#endif
    }
#endif

    return 0;
}

int sdb_vput(const sdb_config_t *cfg, const char *fmt, va_list va)
{
    return vxprint((void *)cfg, cb_put, fmt, va);
}

int sdb_putx(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, size_t line, const char *fmt, ...)
{
    int ret;
    put_param_t p;

#if defined(SDB_SYS_SUPPORT_STDERR)
    p.err           = errno;
#endif
    p.cfg           = cfg;
    p.flag          = flag & SDB_TYPE_MASK;
    p.file          = file;
    p.func          = func;
    p.line          = line;
    p.fmt           = fmt;

    bio_put(cfg, flag | SDB_DATA_PEND, "^", 0);
    va_start(p.va, fmt);
    ret = put_proc(&p);
    va_end(p.va);
    bio_put(cfg, flag | SDB_DATA_WRAP, "\n", 0);
    bio_put(cfg, flag | SDB_DATA_POST, "$", 0);

    return ret;
}

int sdb_put(const sdb_config_t *cfg, const char *fmt, ...)
{
    int ret;
    va_list va;

    bio_put(cfg, SDB_TYPE_INFO | SDB_DATA_PEND, "^", 1);
    va_start(va, fmt);
    ret = vxprint((void *)cfg, cb_put, fmt, va);
    va_end(va);
    bio_put(cfg, SDB_TYPE_INFO | SDB_DATA_POST, "$", 1);

    return ret;
}

#else
inline int sdb_putx(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, size_t line, const char *fmt, ...)
{
    return 0;
}
inline int sdb_put(const sdb_config_t *cfg, const char *fmt, ...)
{
    return 0;
}
#endif /* defined(SDB_ENABLE) */

