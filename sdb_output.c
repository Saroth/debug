#include "sdb_config.h"

#if defined(SDB_ENABLE)

#if defined(SDB_SYS_HAVE_TIME)
static int sdb_out_time(SDB_IO_PARAM_T *p)
{
    if (p->options & SDB_TIME) {
        time_t tim = time(NULL);
        struct tm *t;
        t = localtime(&tim);
        p->length += sprintf(p->cache + p->length, "%02d%02d%02d%02d%02d%02d ",
                t->tm_year % 100, 1 + t->tm_mon, t->tm_mday,
                t->tm_hour, t->tm_min, t->tm_sec);
    }

    return 0;
}
#else
#define sdb_out_time(...)   0
#endif /* defined(SDB_SYS_HAVE_TIME) */

#if defined(SDB_SYS_HAVE_COLOR)
static int sdb_out_color(SDB_IO_PARAM_T *p, unsigned int type)
{
    if (p->options & SDB_NO_COLOR || type >= SDB_MODE_HL_MAX) {
        return 0;
    }
    const char *sdb_out_hl_types[SDB_MODE_HL_MAX >> SDB_MODE_HL_OFS] = {
        "",
        SDB_COLOR_HL,
        SDB_COLOR_WARN,
        SDB_COLOR_ERR,
        SDB_COLOR_TITLE,
        SDB_COLOR_INPUT,
        SDB_COLOR_LABEL,
        SDB_COLOR_RES,
    };
    if (type == 0) {
        type = SDB_MODE_HL_RES;
    }
    p->length += sprintf(p->cache + p->length, "%s",
            sdb_out_hl_types[type >> SDB_MODE_HL_OFS]);

    return 0;
}
#else
#define sdb_out_color(...)  0
#endif /* defined(SDB_SYS_HAVE_COLOR) */

static int sdb_out_mark(SDB_IO_PARAM_T *p)
{
    unsigned int mark_type = p->mode & SDB_MODE_MARK;

    if ((p->options & SDB_NO_MARK) || mark_type >= SDB_MODE_MARK_MAX) {
        return 0;
    }
    SDB_MARK_T sdb_marks[SDB_MODE_MARK_MAX >> SDB_MODE_MARK_OFS] = {
        {   SDB_MODE_HL_NONE,   SDB_MARK_NONE,      },
        {   SDB_MODE_HL_NONE,   SDB_MARK_INFO,      },
        {   SDB_MODE_HL_WARN,   SDB_MARK_WARN,      },
        {   SDB_MODE_HL_ERR,    SDB_MARK_ERR,       },
        {   SDB_MODE_HL_INPUT,  SDB_MARK_GETNUM,    },
        {   SDB_MODE_HL_INPUT,  SDB_MARK_GETSTR,    },
        {   SDB_MODE_HL_INPUT,  SDB_MARK_ECHO,      },
        {   SDB_MODE_HL_SIGN,   SDB_MARK_ENTRY,     },
        {   SDB_MODE_HL_SIGN,   SDB_MARK_EXIT,      },
        {   SDB_MODE_HL_NONE,   SDB_MARK_DUMP,      },
    };
    int ret = 0;
    int t = mark_type >> SDB_MODE_MARK_OFS;

    if (sdb_marks[t].hl_type != SDB_MODE_HL_NONE
            && (ret = sdb_out_color(p, sdb_marks[t].hl_type)) < 0) {
        return ret;
    }
    p->length += sprintf(p->cache + p->length, "%s", sdb_marks[t].mark);
    if (sdb_marks[t].hl_type != SDB_MODE_HL_NONE
            && (ret = sdb_out_color(p, 0)) < 0) {
        return ret;
    }

    return 0;
}

static int sdb_out_message(SDB_IO_PARAM_T *p)
{
    unsigned int msg_type = p->mode & SDB_MODE_MSG;

    if (msg_type == 0) {
        return 0;
    }
    else
#if defined(SDB_MDL_STDERR_ENABLE)
    if (msg_type == SDB_MODE_MSG_STDERR) {
        if (p->options & SDB_NO_STDERR) {
            return 0;
        }
        p->length += sprintf(p->cache + p->length, ": %s(%d)",
                strerror(p->error), p->error);
    }
    else
#endif /* defined(SDB_MDL_STDERR_ENABLE) */
    if (msg_type == SDB_MODE_MSG_ENTRY) {
        p->length += sprintf(p->cache + p->length, "%s {", p->func);
    }
    else if (msg_type == SDB_MODE_MSG_EXIT) {
        p->length += sprintf(p->cache + p->length, "%s }", p->func);
    }
    else if (msg_type == SDB_MODE_MSG_INPUTFLAG) {
        p->length += sprintf(p->cache + p->length, " => ");
    }
    else if (msg_type == SDB_MODE_MSG_DUMPSIZE) {
        p->length += sprintf(p->cache + p->length, " (size:%d)",
                (size_t)p->addr);
    }
    else {
        sdb_out_e(DS_SDB, "should never happen");
        return SDB_RET_PROCESS_ERR;
    }

    return 0;
}

static int sdb_out_vprintf(SDB_IO_PARAM_T *p)
{
    int ret = 0;

    if (p->format == NULL) {
        return 0;
    }
    while (1) {
        ret = vsnprintf(p->cache + p->length,
                SDB_CONF_IO_CACHE_SIZE - p->length, p->format, p->ap);
        if (ret > SDB_CONF_IO_CACHE_SIZE) {
            sdb_out_w(DS_SDB, "Output string too long.");
            ret = SDB_CONF_IO_CACHE_SIZE;
        }
        else if (ret < 0) {
            sdb_err_e(DS_SDB, "vsnprintf");
        }
        break;
    }
    if (ret > 0) {
        p->length += ret;
    }

    return ret;
}

int sdb_out_style(SDB_IO_PARAM_T *p)
{
    int ret = 0;
    int ofslen = 0;
    int lbllen = 0;
    unsigned int hl_type = p->mode & SDB_MODE_HL;
    unsigned int mark_type = p->mode & SDB_MODE_MARK;

    if ((~p->options) & SDB_IO
            || (p->options & SDB_NO_INFO
                && mark_type == SDB_MODE_MARK_INFO)
            || (p->options & SDB_NO_WARNING
                && mark_type == SDB_MODE_MARK_WARNING)
            || (p->options & SDB_NO_ERROR
                && mark_type == SDB_MODE_MARK_ERROR)) {
        return 0;
    }
    // 标签
    if ((~p->options) & SDB_NO_LABLE) {
        if ((ret = sdb_out_time(p)) < 0) {
            return ret;
        }
        if ((ret = sdb_out_color(p, SDB_MODE_HL_LABEL)) < 0) {
            return ret;
        }
        if ((~p->options) & SDB_NO_FILE) {
            ret = sprintf(p->cache + p->length, "%s:", p->file);
            p->length += ret;
            ofslen += ret;
            lbllen += SDB_CONF_FILE_NAME_SPACE;
        }
        if (p->options & SDB_FUNC) {
            ret = sprintf(p->cache + p->length, "%s:", p->func);
            p->length += ret;
            ofslen += ret;
            lbllen += SDB_CONF_FUNC_NAME_SPACE;
        }
        if ((~p->options) & SDB_NO_LINE) {
            ret = sprintf(p->cache + p->length, "%04d ", p->line);
            p->length += ret;
            ofslen += ret;
            lbllen += SDB_CONF_LINE_NUM_SPACE;
        }
        if ((ret = sdb_out_color(p, 0)) < 0) {
            return ret;
        }
        // 填充空格
        while (ofslen++ < lbllen) {
            *(p->cache + p->length++) = ' ';
        }
    }
    // 标记
    if ((ret = sdb_out_mark(p)) < 0) {
        return ret;
    }
    if (hl_type && (ret = sdb_out_color(p, hl_type)) < 0) {
        return ret;
    }
    // 字符串格式化处理
    if (p->format) {
        if ((ret = sdb_out_vprintf(p)) < 0) {
            return ret;
        }
    }
    // 特殊信息
    if ((ret = sdb_out_message(p)) < 0) {
        return ret;
    }
    if (hl_type && (ret = sdb_out_color(p, 0)) < 0) {
        return ret;
    }
    // 换行符
    if ((~p->options) & SDB_NO_WRAP) {
        p->length += sprintf(p->cache + p->length, "%s", SDB_CONF_WRAP);
    }

    if (ret >= 0 && p->length > 0) {
        ret = sdb_bio_out(p->cache, p->length);
    }

    return ret;
}

int sdb_output(int opt, int mode, const char *file, const char *func, int line,
        const char *format, ...)
{
    int ret = 0;
    SDB_IO_PARAM_T p;

    p.options   = opt;
    p.mode      = mode;
    p.file      = file;
    p.func      = func;
    p.line      = line;
    p.error     = errno;
    p.format    = format;
    p.length    = 0;
    va_start(p.ap, format);
    ret = sdb_out_style(&p);
    va_end(p.ap);

    return ret;
}

#else
inline int sdb_output(int opt, int mode,
        const char *file, const char *func, int line, const char *format, ...)
{
    return 0;
}
#endif /* defined(SDB_ENABLE) */

