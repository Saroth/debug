#include "sdb_config.h"

#warning "@TODO: strtol achieve"
#include <stdlib.h>
#if defined(SDB_SYS_SUPPORT_STDERR)
#include <errno.h>
#endif

#if defined(SDB_ENABLE) && defined(SDB_MDL_GET_ENABLE)

#define SDB_SET_CONFIG cfg

int sdb_get(const sdb_config_t *cfg, char *buf, unsigned int size, int *len,
        const char *file, const char *func, unsigned int line,
        unsigned flag, const char *fmt, ...)
{
    int ret;
    unsigned int l;
    char b[SDB_CONF_BUFFER_SIZE_GETNUM];
    bio_put_param_t bio;
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
    const char *head;
    const char *end;
#endif

    flag &= SDB_TYPE_MASK;
    if (flag == SDB_TYPE_INPUT_STR) {
        if (buf == 0 || size == 0) {
#if defined(SDB_CONF_INPUT_PROMPT)
            SDB_OUT_E("Bad param, buf:%#x, bufsize:%d", buf, size);
#endif
            return SDB_RET_PARAM_ERR;
        }
    }
    else if (flag == SDB_TYPE_INPUT_NUM) {
        buf = b;
        size = SDB_CONF_BUFFER_SIZE_GETNUM;
    }
    else {
#if defined(SDB_CONF_INPUT_PROMPT)
        SDB_OUT_E("Bad flag:%#x, should never happen!", flag);
#endif
        return SDB_RET_PARAM_ERR;
    }

    bio.cfg         = cfg;
    bio.flag        = flag;
    PUT_PEND(&bio);
    PUT_STR_BLK(&bio, SDB_DATA_FILE, file, 0);
    PUT_STR_BLK(&bio, SDB_DATA_FUNC, func, 0);
    PUT_NUM_BLK(&bio, SDB_DATA_LINE, line);
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
    set_color(bio.flag, &head, &end);
    PUT_STR(&bio, SDB_DATA_COLOR, head, 0);
#endif
    if (fmt) {
        va_list va;
        bio.flag |= SDB_DATA_INFO;
        va_start(va, fmt);
        if ((ret = vxprint((void *)&bio, cb_putx, fmt, va)) < 0)
            return ret;
        va_end(va);
        bio.flag &= ~SDB_DATA_INFO;
        if (ret)
            return ret;
    }
    else {
        PUT_STR(&bio, SDB_DATA_INFO, "[>]", 3);
    }
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
    PUT_STR(&bio, SDB_DATA_COLOR, end, 0);
#endif
    PUT_POST(&bio);

    if ((ret = bio_get(cfg, buf, size, &l)))
        return ret;
    if (l >= size)
        l = size - 1;
    buf[l] = 0;
    while (l > 0 && (buf[l - 1] == '\r' || buf[l - 1] == '\n')) /* Trim */
        buf[--l] = 0;
    if (l == 0) {
#if defined(SDB_CONF_INPUT_PROMPT)
        SDB_OUT_W("No input.");
#endif
        return SDB_RET_NO_INPUT;
    }

    bio.flag        = SDB_TYPE_INPUT_ECHO;
    PUT_PEND(&bio);
    PUT_STR_BLK(&bio, SDB_DATA_FILE, file, 0);
    PUT_STR_BLK(&bio, SDB_DATA_FUNC, func, 0);
    PUT_NUM_BLK(&bio, SDB_DATA_LINE, line);
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
    set_color(bio.flag, &head, &end);
    PUT_STR(&bio, SDB_DATA_COLOR, head, 0);
#endif
    bio.flag |= SDB_DATA_INFO;
    PUT_STR(&bio, SDB_DATA_INFO, "[\"", 2);
    PUT_STR(&bio, SDB_DATA_INFO, buf, 0);
    PUT_STR(&bio, SDB_DATA_INFO, "\"(", 2);
    PUT_NUM(&bio, SDB_DATA_INFO, l);
    PUT_STR(&bio, SDB_DATA_INFO, ")]", 2);
#if defined(SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES)
    PUT_STR(&bio, SDB_DATA_COLOR, end, 0);
#endif
    PUT_WRAP(&bio);
    PUT_POST(&bio);

    if (flag == SDB_TYPE_INPUT_NUM) {
        ret = strtol(buf, 0, 0);
        if (ret == 0 && buf[0] != '0') {
#if defined(SDB_CONF_INPUT_PROMPT)
            SDB_OUT_W("Unrecognizable input.");
#endif
            return SDB_RET_UNKNOWN_INPUT;
        }
        if (len)
            *len = ret;

        return ret;
    }
    return 0;
}

#else
inline int sdb_get(const sdb_config_t *cfg,
        char *buf, unsigned int size, int *len,
        const char *file, const char *func, unsigned int line,
        unsigned flag, const char *fmt, ...)
{
    return 0;
}
#endif /* defined(SDB_ENABLE) && defined(SDB_MDL_GET_ENABLE) */

