#include "sdb_config.h"

#if defined(SDB_ENABLE) && defined(SDB_MDL_MENU_ENABLE)

#define __sdb_cfg cfg

int sdb_menu(const sdb_config_t *cfg,
        const char *file, const char *func, size_t line,
        sdb_item_t *list, size_t num)
{
    int i;
    int ret;

    while (1) {
        if ((ret = sdb_output(cfg, SDB_FLG_LV_INFO,
                        file, func, line, " ######## (%d)", num)))
            return ret;
        for (i = 0; i < num; i++)
            SDB_OUT_I(" #%3d.%s", i + 1, (list + i)->info);
        SDB_OUT_I(" #  0.Return");
        ret = SDB_IN_N(NULL);
        if (ret == 0)
            break;
        else if (ret > 0 && ret <= num)
            if ((list + ret - 1)->func)
                (list + ret - 1)->func((list + ret - 1)->param);
    }

    return 0;
}

#else
inline int sdb_menu(const sdb_config_t *cfg,
        const char *file, const char *func, size_t line,
        sdb_item_t *list, size_t num)
{
    return 0;
}

#endif /* defined(SDB_ENABLE) && defined(SDB_MDL_MENU_ENABLE) */

