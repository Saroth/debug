#include "sdb_config.h"

#warning "@TODO: strtol achieve"
#include <stdlib.h>

#if defined(SDB_ENABLE) && defined(SDB_MDL_GET_ENABLE)

const char *sdb_get_input = "[>]";

#define SDB_SET_CONFIG cfg

int sdb_get_str(const sdb_config_t *cfg,
        const char *file, const char *func, unsigned int line,
        char *buf, unsigned int size, unsigned int *len)
{
    int ret;
    unsigned int l;

    if (buf == 0 || size == 0) {
        SDB_OUT_E("Bad param, buf:%#x, bufsize:%d", buf, size);
        return SDB_RET_PARAM_ERR;
    }
    if ((ret = bio_get(cfg, buf, size, &l)))
        return ret;
    if (l >= size)
        l = size - 1;
    buf[l] = 0;
    while (l > 0 && (buf[l - 1] == '\r' || buf[l - 1] == '\n')) /* Trim */
        buf[--l] = 0;
    if (len)
        *len = l;
    if ((ret = sdb_putx(cfg, SDB_TYPE_INPUT_ECHO,
                    file, func, line, "[\"%s\"(%d)]", buf, l)))
        return ret;

    return 0;
}

int sdb_get_num(const sdb_config_t *cfg,
        const char *file, const char *func, unsigned int line, int *num)
{
    int ret;
    unsigned int l;
    int n = 0;
    char buf[SDB_CONF_BUFFER_SIZE_GETNUM];

    if ((ret = bio_get(cfg, buf, SDB_CONF_BUFFER_SIZE_GETNUM, &l)))
        return ret;
    if (l >= SDB_CONF_BUFFER_SIZE_GETNUM)
        l = SDB_CONF_BUFFER_SIZE_GETNUM - 1;
    buf[l] = 0;
    while (l > 0 && (buf[l - 1] == '\r' || buf[l - 1] == '\n')) /* Trim */
        buf[--l] = 0;

    if (l <= 0) {
        SDB_OUT_W("No input.");
        return SDB_RET_NO_INPUT;
    }
    n = strtol(buf, 0, 0);
    if (num)
        *num = n;
    if (n == 0 && buf[0] != '0') {
        SDB_OUT_W("Unrecognizable input.");
        return SDB_RET_UNKNOWN_INPUT;
    }
    if ((ret = sdb_putx(cfg, SDB_TYPE_INPUT_ECHO,
                    file, func, line, "[\"%s\"(%d)]", buf, l)))
        return ret;

    return n;
}

#else
inline int sdb_get_str(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, unsigned int line,
        char *buf, unsigned int size, unsigned int *len)
{
    return 0;
}
inline int sdb_get_num(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, unsigned int line, int *num)
{
    return 0;
}
#endif /* defined(SDB_ENABLE) && defined(SDB_MDL_GET_ENABLE) */

