#include "sdb_internal.h"

static int menu_list(sdb_cout_context *cout,
        const sdb_menu_item *list, size_t size)
{
    int ret;
    int i;
    sdb_assert(__sdb_mcout_append_endline(cout));
    sdb_assert(__sdb_mcout_append(cout, "#### (%d)", size));
    sdb_assert(__sdb_mcout_append_endline(cout));
    for (i = 0; i < size; i++) {
        if (list[i].info == 0) {
            break;
        }
        sdb_assert(__sdb_mcout_append(cout, " %3d. %s", i + 1, list[i].info));
        sdb_assert(__sdb_mcout_append_endline(cout));
    }
    sdb_assert(__sdb_mcout_append_string(cout, "   0. return"));
    return 0;
}

static int menu_form(sdb_cout_context *cout,
        const sdb_menu_item *list, size_t size)
{
    return 0;
}

static int menu_pile(sdb_cout_context *cout,
        const sdb_menu_item *list, size_t size)
{
    return 0;
}

struct menu_type {
    size_t type;
    int (*menu)(sdb_cout_context *, const sdb_menu_item *, size_t);
};
static const struct menu_type menus[SDB_MENU_MAX >> SDB_MENU_OFS] = {
    { SDB_MENU_LIST, menu_list, },
    { SDB_MENU_FORM, menu_form, },
    { SDB_MENU_PILE, menu_pile, },
};

int __sdb_menu(const sdb_context *ctx, unsigned int mode,
        const sdb_menu_item *list, size_t size, const char *file, size_t line)
{
    unsigned int type = (mode & SDB_MENU_MASK) >> SDB_MENU_OFS;
    if (type >= SDB_MENU_MAX >> SDB_MENU_OFS) {
        return __sdb_mcout(ctx, SDB_MSG_WARNING, __FILE__, __LINE__,
                "Unknown menu type:%#x(%#x)", type, mode);
    }

    int ret;
    int num = 0;
    while (1) {
        char buf[ctx->out_column_limit + SDB_CONFIG_OUTPUT_BUFFER_RESERVE];
        sdb_cout_context cout;
        __sdb_mcout_init(&cout, ctx, mode, buf, sizeof(buf), file, line);
        sdb_assert(menus[type].menu(&cout, list, size));
        sdb_assert(__sdb_mcout_final(&cout));

        if (__sdb_nmcin(ctx, SDB_MSG_INPUT_NUM, &num, file, line, ">") < 0) {
            continue;
        }
        if (num == 0) {
            break;
        }
        else if (num > 0 && num <= size) {
            if (list[num - 1].func) {
                if ((ret = list[num - 1].func(list[num - 1].param)) != 0) {
                    sdb_assert(__sdb_mcout(ctx, SDB_MSG_WARNING, file, line,
                                "above return: %d(%s%#x)", ret,
                                ret < 0 ? "-" : "", ret < 0 ? -ret : ret));
                }
            }
        }
        else {
            sdb_assert(__sdb_mcout(ctx, SDB_MSG_WARNING, __FILE__, __LINE__,
                        "input out of range: %d (0~%d)", num, size));
        }
    }
    return 0;
}

