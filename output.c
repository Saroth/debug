#include "config.h"

#if defined(SDB_ENABLE)

#if 0
int sdb_out_format(const char *format, ...)
{
    size_t len = 0;
    int size = 0;
    va_list list;
#if defined(SDB_SYS_HAVE_MEM_ALLOC)
    char *buf = NULL;
#else
    char buf[SDB_IO_CACHE_SIZE + 4];
#endif /* defined(SDB_SYS_HAVE_MEM_ALLOC) */

    if (format == NULL) {
        sdb_out_e(DS_SDB, "Param error.");
        return SDB_RET_PARAM_ERR;
    }
#if defined(SDB_SYS_HAVE_MEM_ALLOC)
    size = 128;                         //!< 通常一行输出数据控制在80字节左右
    if ((buf = (char *)malloc(size + 4)) == NULL) {
        sdb_err_e(DS_SDB, "malloc");
        return SDB_RET_ALLOC_FAILED;
    }
#else
    size = SDB_IO_CACHE_SIZE;
#endif /* defined(SDB_SYS_HAVE_MEM_ALLOC) */

    while (1) {
        va_start(list, format);
        len = vsnprintf(buf, size, format, list);
        va_end(list);
#if defined(SDB_SYS_HAVE_MEM_ALLOC)
        if (len > size) {
            size = len + 4;             //!< 调整为所需空间大小
            sdb_err_i(DS_SDB, "realloc size: %d", size);
            if ((buf = (char *)realloc(buf, size)) == NULL) {
                sdb_err_e(DS_SDB, "realloc");
                return SDB_RET_ALLOC_FAILED;
            }
            continue;
        }
        else
#else
        if (len > SDB_IO_CACHE_SIZE) {
            sdb_out_w(DS_SDB, "Output string too long.");
            len = SDB_IO_CACHE_SIZE;
        }
        else
#endif /* defined(SDB_SYS_HAVE_MEM_ALLOC) */
        if (len < 0) {
            sdb_err_e(DS_SDB, "vsnprintf");
        }
        break;
    }
    if (len > 0) {
        size = sdb_bio_out(buf, len);
    }
#if defined(SDB_SYS_HAVE_MEM_ALLOC)
    if (buf)
        free(buf);
#endif /* defined(SDB_SYS_HAVE_MEM_ALLOC) */

    return size;
}
#endif

int sdb_out_vprintf(SDB_OUT_PARAM_T *p, const char *format, va_list ap)
{
    int ret;

    if (format == NULL)
        return 0;
    while (1) {
#if defined(SDB_SYS_HAVE_MEM_ALLOC)
#else
        ret = vsnprintf(p->outbuf + p->outlen,
                SDB_CONF_IO_CACHE_SIZE - p->outlen, format, ap);
        if (ret > SDB_CONF_IO_CACHE_SIZE) {
            sdb_out_w(DS_SDB, "Output string too long.");
            ret = SDB_CONF_IO_CACHE_SIZE;
        }
        else
#endif /* defined(SDB_SYS_HAVE_MEM_ALLOC) */
        if (ret < 0) {
            sdb_err_e(DS_SDB, "vsnprintf");
        }
        break;
    }
    if (ret > 0)
        p->outlen += ret;

    return 0;
}

#if defined(SDB_SYS_HAVE_TIME)
int sdb_out_time(SDB_OUT_PARAM_T *p)
{
    int ret = 0;
    if (p->options & SDB_TIME) {
        time_t tim = time(NULL);
        struct tm *t;
        t = localtime(&tim);
        if ((ret = sprintf(p->outbuf + p->outlen, "%02d%02d%02d%02d%02d%02d ",
                (1900 + t->tm_year) % 100, 1 + t->tm_mon, t->tm_mday,
                t->tm_hour, t->tm_min, t->tm_sec)) < 0)
            return ret;
        p->outlen += ret;
    }
    return ret;
}
#else
#define sdb_out_time(...)
#endif /* defined(SDB_SYS_HAVE_TIME) */

#if defined(SDB_SYS_HAVE_COLOR)
int sdb_out_color(SDB_OUT_PARAM_T *p, char *color)
{
    if (p->options & SDB_NO_COLOR)
        return 0;
    int ret;
    if ((ret = sprintf(p->outbuf + p->outlen, "%s", color)) > 0)
        p->outlen += ret;
    return ret;
}
#else
#define sdb_out_color(...)
#endif /* defined(SDB_SYS_HAVE_COLOR) */

int sdb_out_style(int opt, int mode,
        char *file, const char *func, int line, const char *format, ...)
{
    int ret = 0;
    SDB_OUT_PARAM_T p;
    va_list ap;
    int ofslen = 0;
    int lbllen = 0;

    if ((~opt) & SDB_IO)
        return 0;
    memset(&p, 0x00, sizeof(SDB_OUT_PARAM_T));
    p.error     = errno;
    p.options   = opt;
    p.mode      = mode;

    do {
        // 输出标签
        if ((~opt) & SDB_NO_LABLE) {
            sdb_out_time(&p);
            if ((ret = sdb_out_color(&p, SDB_COLOR_LABEL)) < 0)
                break;

            if ((~opt) & SDB_NO_FILE) {
                if ((ret = sprintf(p.outbuf + p.outlen, "%s:", file)) < 0)
                    break;
                p.outlen += ret;
                ofslen += ret;
                lbllen += SDB_CONF_FILE_NAME_SPACE;
            }
            if (opt & SDB_FUNC) {
                if ((ret = sprintf(p.outbuf + p.outlen, "%s:", func)) < 0)
                    break;
                p.outlen += ret;
                ofslen += ret;
                lbllen += SDB_CONF_FUNC_NAME_SPACE;
            }
            if ((~opt) & SDB_NO_LINE) {
                if ((ret = sprintf(p.outbuf + p.outlen, "%d", line)) < 0)
                    break;
                p.outlen += ret;
                ofslen += ret;
                lbllen += SDB_CONF_LINE_NUM_SPACE;
            }
            *(p.outbuf + p.outlen) = ' ';
            p.outlen++;
            ofslen++;

            if ((ret = sdb_out_color(&p, SDB_COLOR_RES)) < 0)
                break;
        }
        // 填充空格
        if (ofslen < lbllen) {
            memset(p.outbuf + p.outlen, ' ', lbllen - ofslen);
            p.outlen += lbllen - ofslen;
        }
        // 输出信息
        if (format != NULL) {
            va_start(ap, format);
            sdb_out_vprintf(&p, format, ap);
            va_end(ap);
        }
        if ((ret = sprintf(p.outbuf + p.outlen, "%s",
                        SDB_CONF_WRAP_CHARACTER)) < 0)
            break;
        p.outlen += ret;
    } while (0);
    if (ret >= 0 && p.outlen > 0)
        ret = sdb_bio_out(p.outbuf, p.outlen);

    return ret;
}

#endif

