#include "sdb_config.h"

#if defined(SDB_ENABLE) && defined(SDB_MDL_MENU_ENABLE)

int sdb_menu(const sdb_config_t *cfg,
        const char *file, const char *func, unsigned int line,
        sdb_item_t *list, unsigned int num)
{
    int i;
    int ret;

    while (1) {
        sdb_output(cfg, SDB_FLG_LV_INFO, file, func, line,
                "######## (%d)", num);
        for (i = 0; i < num; i++)
            sdb_output(cfg, SDB_FLG_LV_INFO, file, func, line,
                    "#%3d.%s", i + 1, (list + i)->info);
        sdb_output(cfg, SDB_FLG_LV_INFO, file, func, line, "#  0.Return");
        ret = sdb_input(cfg, SDB_FLG_T_INPUT_NUM, file, func, line,
                NULL, 0, NULL, NULL);
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
        const char *file, const char *func, unsigned int line,
        sdb_item_t *list, unsigned int num)
{
    return 0;
}

#endif /* defined(SDB_ENABLE) && defined(SDB_MDL_MENU_ENABLE) */

