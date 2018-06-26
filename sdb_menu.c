#include "sdb_internal.h"

static int menu_list(sdb_context *ctx, unsigned int mode,
        const sdb_menu_list *list, size_t size, const char *file, size_t line)
{
    int ret;
    int i;
    for (i = 0; i < size; i++) {
        if (list[i].info == 0) {
            break;
        }
        sdb_assert(__sdb_mcout(ctx, mode, file, line,
                    " %3d. %s", i + 1, list[i].info));
    }
    sdb_assert(__sdb_mcout(ctx, mode, file, line, "   0. return"));
}

int __sdb_menu(sdb_context *ctx, unsigned int mode,
        const sdb_menu_list *list, size_t size, const char *file, size_t line)
{
    int ret;

    sdb_assert(__sdb_mcout(ctx, mode, file, line, "#### (%d)", size));
    while (1) {
        switch (mode & SDB_TYPE_MASK) {
            case SDB_TYPE_MENU_LIST: {
                break;
            }
            case SDB_TYPE_MENU_FORM: {
                break;
            }
            case SDB_TYPE_MENU_CHAOS: {
                break;
            }
            default: {
                return __sdb_mcout(ctx, SDB_MSG_WARNING, __FILE__, __LINE__,
                        "Unknown menu type:%#x", mode & SDB_TYPE_MASK);
            }
        }
        int num;
        sdb_assert(__sdb_nmcin(ctx, &num, __FILE__, __LINE__, 0));
        if (num == 0) {
            break;
        }
        else if (num > 0 && num <= size) {
            if (list[num - 1].func) {
                list[num - 1].func(list[num - 1].param);
            }
        }
    }

    return 0;
}

