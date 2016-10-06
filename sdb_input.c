#include "sdb_config.h"

#if defined(SDB_ENABLE) && defined(SDB_MDL_INPUT_ENABLE)

static int sdb_in_getchar(SDB_IO_PARAM_T *p)
{
    int ret = 0;

    if ((ret = sdb_bio_in(p->cache, SDB_CONF_IO_CACHE_SIZE,
                    (size_t *)&p->length))) {
        return ret;
    }
    p->cache[p->length] = 0;
    // 移除末尾换行符
    while (p->length > 0 && (p->cache[p->length - 1] == '\r'
                || p->cache[p->length - 1] == '\n')) {
        p->cache[--(p->length)] = 0;
    }

    return ret;
}

static int sdb_in_num(SDB_IO_PARAM_T *p)
{
    int ret = 0;

    if ((ret = sdb_in_getchar(p))) {
        return ret;
    }
    if (p->length == 0) {
        sdb_out_w(DS_SDB, "No input.");
        return SDB_RET_NO_INPUT;
    }
    p->options = strtol(p->cache, NULL, 0);
    if (p->options == 0 && p->cache[0] != '0') {
        sdb_out_w(DS_SDB, "Unrecognizable input.");
        return SDB_RET_UNKNOWN_INPUT;
    }

    return 0;
}

int sdb_input(int opt, int mode, char *file, const char *func, int line,
        char *buf, int *num, const char *format, ...)
{
    int ret = 0;
    SDB_IO_PARAM_T p;

    if ((~opt) & SDB_IO) {
        return 0;
    }
    p.mark_type = mode & SDB_MODE_MARK;
    p.length = 0;

    va_start(p.ap, format);
    sdb_voutput(opt | SDB_NO_WRAP, mode, file, func, line, format, p.ap);
    va_end(p.ap);
    if (p.mark_type == SDB_MODE_MARK_GETNUM) {
        p.options = 0;
        if ((ret = sdb_in_num(&p))) {
            return ret;
        }
        if (num) {
            *num = p.options;
        }
        else {
            ret = p.options;
        }
    }
    else if (p.mark_type == SDB_MODE_MARK_GETSTR) {
        if ((ret = sdb_in_getchar(&p))) {
            return ret;
        }
        if (num) {
            *num = p.length;
        }
    }
    else {
        sdb_out_e(DS_SDB, "should never happen");
        return SDB_RET_PROCESS_ERR;
    }
    sdb_output(opt, ((mode & ~(p.mark_type) & ~(SDB_MODE_MSG_INTERVAL))
                | SDB_MODE_MARK_ECHO), file, func, line, "\"%s\"(%d)",
            p.cache, p.length);

    if (buf && (ssize_t)p.length > 0) {
        memmove(buf, p.cache, p.length + 1);    //!< 包含结束符
    }

    return ret;
}

#else
inline int sdb_input(int opt, int mode, char *file, const char *func, int line,
        char *buf, int *num, const char *format, ...)
{
    return 0;
}
#endif /* defined(SDB_MDL_INPUT_ENABLE) && defined(SDB_ENABLE) */

