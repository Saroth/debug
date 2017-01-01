#include "sdb_config.h"

#if 0 //defined(SDB_ENABLE) && defined(SDB_MDL_INPUT_ENABLE)

#define SDB_SET_CONFIG p->cfg

static int input_getchar(input_param_t *p)
{
    int ret = 0;

    if ((ret = bio_input(p->cfg, p->buf, p->bufsize, &p->len)))
        return ret;
    p->buf[p->len] = 0;
    // 移除末尾换行符
    while (p->len > 0 && (p->buf[p->len - 1] == '\r'
                || p->buf[p->len - 1] == '\n'))
        p->buf[--p->len] = 0;

    return ret;
}

static int input_proc(input_param_t *p)
{
    switch (p->flag & SDB_FLG_T) {
        case SDB_FLG_T_INPUT_STR: {
            int ret;

            if (p->buf == NULL || p->bufsize <= 0) {
                SDB_OUT_E("Bad param, buf:%#x, bufsize:%d", p->buf, p->bufsize);
                return SDB_RET_PARAM_ERR;
            }
            if ((ret = input_getchar(p)))
                return ret;
            if (p->pnum)
                *p->pnum = p->len;

            return 0;
        }
        case SDB_FLG_T_INPUT_NUM: {
            int ret;
            char buf[SDB_CONF_BUFFER_SIZE_GETNUM];

            p->buf = buf;
            p->bufsize = SDB_CONF_BUFFER_SIZE_GETNUM;
            if ((ret = input_getchar(p)))
                return ret;
            if (p->len == 0) {
                SDB_OUT_W("No input.");
                return SDB_RET_NO_INPUT;
            }
            p->num = strtol(p->buf, NULL, 0);
            if (p->num == 0 && p->buf[0] != '0') {
                SDB_OUT_W("Unrecognizable input.");
                return SDB_RET_UNKNOWN_INPUT;
            }
            if (p->pnum)
                *p->pnum = p->num;

            return 0;
        }
        default: {
            SDB_OUT_E("Should never happen!");
        }
    }
    return 0;
}

int sdb_input(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, size_t line,
        char *buf, size_t bufsize, int *pnum, const char *fmt, ...)
{
    int ret;
    va_list ap;
    input_param_t p;

    p.cfg           = cfg;
    p.flag          = flag;
    p.buf           = buf;
    p.bufsize       = bufsize;
    p.len           = 0;
    p.num           = 0;
    p.pnum          = pnum;

    if (fmt) {
        va_start(ap, fmt);
        sdb_output_v(cfg, SDB_FLG_LV_INFO | flag | SDB_FLG_NOWRAP,
                file, func, line, fmt, ap);
        va_end(ap);
    }
    else
        sdb_output(cfg, SDB_FLG_LV_INFO | flag | SDB_FLG_NOWRAP
                | (fmt ? SDB_FLG_BARE : 0), file, func, line, "[>]");
    if ((ret = input_proc(&p)))
        return ret;
    sdb_output(cfg, SDB_FLG_LV_INFO | SDB_FLG_T_INPUTECHO,
            file, func, line, "[\"%s\"(%d)]", p.buf, p.len);

    return p.num;
}

#else
inline int sdb_input(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, size_t line,
        char *buf, size_t bufsize, int *pnum, const char *fmt, ...)
{
    return 0;
}
#endif /* defined(SDB_MDL_INPUT_ENABLE) && defined(SDB_ENABLE) */

