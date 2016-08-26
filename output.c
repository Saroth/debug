#include "config.h"

#if defined(SDB_ENABLE)

#if defined(SDB_SYS_HAVE_TIME)
static int sdb_out_time(SDB_OUT_PARAM_T *p)
{
    int ret = 0;
    if (p->options & SDB_TIME) {
        time_t tim = time(NULL);
        struct tm *t;
        t = localtime(&tim);
        if ((ret = sprintf(p->outbuf + p->outlen, "%02d%02d%02d%02d%02d%02d ",
                (1900 + t->tm_year) % 100, 1 + t->tm_mon, t->tm_mday,
                t->tm_hour, t->tm_min, t->tm_sec)) >= 0) {
            p->outlen += ret;
        }
    }
    return ret;
}
#else
#define sdb_out_time(...)   0
#endif /* defined(SDB_SYS_HAVE_TIME) */

#if defined(SDB_SYS_HAVE_COLOR)
static int sdb_out_color(SDB_OUT_PARAM_T *p, unsigned int type)
{
    if (p->options & SDB_NO_COLOR || type >= SDB_HL_MAX) {
        return 0;
    }
    char *sdb_out_hl_types[SDB_HL_MAX] = {  // SDB_HL_TYPES_T
        "",
        SDB_COLOR_HL,
        SDB_COLOR_WARN,
        SDB_COLOR_ERR,
        SDB_COLOR_TITLE,
        SDB_COLOR_INPUT,
        SDB_COLOR_LABEL,
        SDB_COLOR_RES,
    };
    int ret;
    if ((ret = sprintf(p->outbuf + p->outlen, "%s",
                    sdb_out_hl_types[type])) >= 0) {
        p->outlen += ret;
    }
    return ret;
}
#else
#define sdb_out_color(...)  0
#endif /* defined(SDB_SYS_HAVE_COLOR) */

static int sdb_out_mark(SDB_OUT_PARAM_T *p, unsigned int type)
{
    if ((p->options & SDB_NO_MARK) || type >= SDB_MODE_MARK_TYPEMAX) {
        return 0;
    }
    SDB_MARK_T sdb_out_mark_types[SDB_MODE_MARK_TYPEMAX] = {
        {   SDB_HL_NONE,    "",                 },
        {   SDB_HL_NONE,    SDB_MARK_INFO,      },
        {   SDB_HL_WARN,    SDB_MARK_WARN,      },
        {   SDB_HL_ERR,     SDB_MARK_ERR,       },
        {   SDB_HL_INPUT,   SDB_MARK_IN_GETNUM, },
        {   SDB_HL_INPUT,   SDB_MARK_IN_GETSTR, },
        {   SDB_HL_INPUT,   SDB_MARK_IN_ECHO,   },
    };
    int ret = 0;
    if ((ret = sdb_out_color(p, sdb_out_mark_types[type].hl_type)) < 0) {
        return ret;
    }
    if ((ret = sprintf(p->outbuf + p->outlen, "%s%s",
            sdb_out_mark_types[type].mark, SDB_COLOR_RES)) >= 0) {
        p->outlen += ret;
    }
    return ret;
}

static int sdb_out_stderr(SDB_OUT_PARAM_T *p)
{
    if ((~p->mode) & SDB_MODE_STDERR || p->options & SDB_NO_STDERR) {
        return 0;
    }
    int ret;
    if ((ret = sprintf(p->outbuf + p->outlen, ": %s(%d)",
                    strerror(p->error), p->error)) >= 0) {
        p->outlen += ret;
    }
    return ret;
}

static int sdb_out_vprintf(SDB_OUT_PARAM_T *p, const char *format, va_list ap)
{
    int ret = 0;

    if (format == NULL) {
        return 0;
    }
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
    if (ret > 0) {
        p->outlen += ret;
    }

    return ret;
}

int sdb_out_style(int opt, int mode,
        char *file, const char *func, int line, const char *format, ...)
{
    int ret = 0;
    SDB_OUT_PARAM_T p;
    va_list ap;
    int ofslen = 0;
    int lbllen = 0;

    if ((~opt) & SDB_IO) {
        return 0;
    }
    p.error     = errno;
    p.options   = opt;
    p.mode      = mode;
    p.outlen    = 0;
    p.mark_type = mode & SDB_MODE_MARK_CHK;
    p.hl_type   = (mode & SDB_MODE_HL_CHK) >> 3;
    memset(p.outbuf, 0x00, sizeof(p.outbuf));
    if (((opt & SDB_NO_INFO) && (p.mark_type == SDB_MODE_MARK_INFO))
            || ((opt & SDB_NO_WARNING) && (p.mark_type == SDB_MODE_MARK_WARNING))
            || ((opt & SDB_NO_ERROR) && (p.mark_type == SDB_MODE_MARK_ERROR))) {
        return 0;
    }

    do {
        // 输出标签
        if ((~opt) & SDB_NO_LABLE) {
            if ((ret = sdb_out_time(&p)) < 0) {
                break;
            }
            if ((ret = sdb_out_color(&p, SDB_HL_LABEL)) < 0) {
                break;
            }
            if ((~opt) & SDB_NO_FILE) {
                if ((ret = sprintf(p.outbuf + p.outlen, "%s:", file)) < 0) {
                    break;
                }
                p.outlen += ret;
                ofslen += ret;
                lbllen += SDB_CONF_FILE_NAME_SPACE;
            }
            if (opt & SDB_FUNC) {
                if ((ret = sprintf(p.outbuf + p.outlen, "%s:", func)) < 0) {
                    break;
                }
                p.outlen += ret;
                ofslen += ret;
                lbllen += SDB_CONF_FUNC_NAME_SPACE;
            }
            if ((~opt) & SDB_NO_LINE) {
                if ((ret = sprintf(p.outbuf + p.outlen, "%04d ", line)) < 0) {
                    break;
                }
                p.outlen += ret;
                ofslen += ret;
                lbllen += SDB_CONF_LINE_NUM_SPACE;
            }
            if ((ret = sdb_out_color(&p, SDB_HL_RES)) < 0) {
                break;
            }
            // 填充空格
            while (ofslen++ < lbllen) {
                *(p.outbuf + p.outlen++) = ' ';
            }
        }
        if ((ret = sdb_out_mark(&p, p.mark_type)) < 0) {
            break;
        }
        // 输出信息
        if (format != NULL) {
            if ((ret = sdb_out_color(&p, p.hl_type)) < 0) {
                break;
            }
            va_start(ap, format);
            if ((ret = sdb_out_vprintf(&p, format, ap)) < 0) {
                break;
            }
            va_end(ap);
            if ((ret = sdb_out_color(&p, SDB_HL_RES)) < 0) {
                break;
            }
        }
        // 输出系统错误信息
        if ((ret = sdb_out_stderr(&p)) < 0) {
            break;
        }
        // 输出换行符
        if ((~opt) & SDB_NO_WRAP) {
            if ((ret = sprintf(p.outbuf + p.outlen,
                            "%s", SDB_CONF_WRAP_CHARACTER)) < 0) {
                break;
            }
            p.outlen += ret;
        }
    } while (0);
    if (ret >= 0 && p.outlen > 0) {
        ret = sdb_bio_out(p.outbuf, p.outlen);
    }

    return ret;
}

#endif

