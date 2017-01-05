#include "sdb_config.h"

#if defined(SDB_ENABLE) && defined(SDB_MDL_GET_ENABLE)

#define SDB_SET_CONFIG p->cfg

static int getchar(get_param_t *p)
{
    int ret = 0;

    if ((ret = bio_get(p->cfg, p->buf, p->bufsize, &p->len)))
        return ret;
    p->buf[p->len] = 0;
    // 移除末尾换行符
    while (p->len > 0 && (p->buf[p->len - 1] == '\r'
                || p->buf[p->len - 1] == '\n'))
        p->buf[--p->len] = 0;

    return ret;
}

static int get_proc(get_param_t *p)
{
    switch (p->flag & SDB_FLG_T) {
        case SDB_FLG_T_GET_STR: {
            int ret;

            if (p->buf == NULL || p->bufsize <= 0) {
                SDB_OUT_E("Bad param, buf:%#x, bufsize:%d", p->buf, p->bufsize);
                return SDB_RET_PARAM_ERR;
            }
            if ((ret = getchar(p)))
                return ret;
            if (p->pnum)
                *p->pnum = p->len;

            return 0;
        }
        case SDB_FLG_T_GET_NUM: {
            int ret;
            char buf[SDB_CONF_BUFFER_SIZE_GETNUM];

            p->buf = buf;
            p->bufsize = SDB_CONF_BUFFER_SIZE_GETNUM;
            if ((ret = getchar(p)))
                return ret;
            if (p->len == 0) {
                SDB_OUT_W("No get.");
                return SDB_RET_NO_INPUT;
            }
            p->num = strtol(p->buf, NULL, 0);
            if (p->num == 0 && p->buf[0] != '0') {
                SDB_OUT_W("Unrecognizable get.");
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

int sdb_get_str()

int sdb_get(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, size_t line,
        char *buf, size_t bufsize, int *pnum, const char *fmt, ...)
{
    int ret;
    va_list ap;
    get_param_t p;

    p.cfg           = cfg;
    p.flag          = flag;
    p.buf           = buf;
    p.bufsize       = bufsize;
    p.len           = 0;
    p.num           = 0;
    p.pnum          = pnum;

    if (fmt) {
        va_start(ap, fmt);
        sdb_vput(cfg, SDB_FLG_LV_INFO | flag | SDB_FLG_NOWRAP,
                file, func, line, fmt, ap);
        va_end(ap);
    }
    else
        sdb_put(cfg, SDB_FLG_LV_INFO | flag | SDB_FLG_NOWRAP
                | (fmt ? SDB_FLG_BARE : 0), file, func, line, "[>]");
    if ((ret = get_proc(&p)))
        return ret;
    sdb_put(cfg, SDB_FLG_LV_INFO | SDB_FLG_T_getECHO,
            file, func, line, "[\"%s\"(%d)]", p.buf, p.len);

    return p.num;
}

#else
inline int sdb_get(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, size_t line,
        char *buf, size_t bufsize, int *pnum, const char *fmt, ...)
{
    return 0;
}
#endif /* defined(SDB_ENABLE) && defined(SDB_MDL_GET_ENABLE) */

