#include <string.h>

#include "sdb_internal.h"

static int menu_list(sdb_cout_context *cout,
        const sdb_menu_item *list, size_t size)
{
    int ret;
    size_t i;
    const sdb_context *ctx = cout->ctx;

    sdb_assert(__sdb_mcout_append(cout, "#### [%d]", size));
    sdb_assert(__sdb_mcout_append_endline(cout));
    for (i = 0; i < size; i++) {
        if (list[i].info == 0) {
            break;
        }
        sdb_assert(__sdb_mcout_append(cout, " %3d. %s", i + 1, list[i].info));
        sdb_assert(__sdb_mcout_append_endline(cout));
    }
    sdb_assert(__sdb_mcout_append(cout, "   0. %s", sdb_last_item));
    return 0;
}

static inline size_t sdb_column_width_max(const sdb_menu_item *list,
        size_t size, size_t div, size_t idx, size_t reserve_size, size_t *rows)
{
    size_t i, width, max = 0;

    for (i = (size + div - 1) / div * idx;
            i < size && i < (size + div - 1) / div * (idx + 1); i++) {
        width = strlen((list + i)->info) + reserve_size;
        if (max < width) {
            max = width;
        }
    }
    if (rows) {
        *rows = i;
    }
    return max;
}

static size_t sdb_divide_columnar(sdb_cout_context *cout,
        const sdb_menu_item *list, size_t size, size_t reserve_size,
        size_t *columns, size_t *rows)
{
    size_t i = 0, col, len;

    while (i < size && (list + i)->info) {
        i++;
    }
    size = i;

    for (col = size; col > 1; col--) {
        len = strlen(sdb_get_mark(cout));
        for (i = 0; i < col && len <= cout->ctx->out_column_limit; i++) {
            len += sdb_column_width_max(list, size, col, i, reserve_size,
                    i == 0 ? rows : 0);
        }
        if (len <= cout->ctx->out_column_limit) {
            break;
        }
    }
    if (columns) {
        *columns = col;
    }
    return size;
}

static int menu_columnar(sdb_cout_context *cout,
        const sdb_menu_item *list, size_t size)
{
#define COLUMNAR_ITEM_WIDTH_RESERVE 8
    size_t rows, columns;

    size = sdb_divide_columnar(cout, list, size, COLUMNAR_ITEM_WIDTH_RESERVE,
            &columns, &rows);
    {
        int ret;
        size_t i, row = 0, col = 0, column_width[columns];
        const sdb_context *ctx = cout->ctx;

        for (i = 0; i < columns; i++) {
            column_width[i] = sdb_column_width_max(list, size, columns, i,
                    COLUMNAR_ITEM_WIDTH_RESERVE, 0);
        }

        sdb_assert(__sdb_mcout_append(cout, "#### [%d]", size));
        sdb_assert(__sdb_mcout_append_endline(cout));
        while (1) {
            for (col = 0; col < columns; col++) {
                i = rows * col + row;
                if (i >= size) {
                    break;
                }
                sdb_assert(__sdb_mcout_append(cout, " %3d.%-*s", i + 1,
                            column_width[col] - COLUMNAR_ITEM_WIDTH_RESERVE,
                            list[i].info));
            }
            if (++row >= rows) {
                break;
            }
            sdb_assert(__sdb_mcout_append_endline(cout));
        }
        if (cout->line_buf_len + 5 + strlen(sdb_last_item)
                > cout->ctx->out_column_limit) {
            sdb_assert(__sdb_mcout_append_endline(cout));
        }
        sdb_assert(__sdb_mcout_append(cout, "   0.%s", sdb_last_item));
    }
    return 0;
}

static int menu_pile(sdb_cout_context *cout,
        const sdb_menu_item *list, size_t size)
{
    int ret;
    size_t i;
    const sdb_context *ctx = cout->ctx;

    sdb_assert(__sdb_mcout_append(cout, "#### [%d]", size));
    sdb_assert(__sdb_mcout_append_endline(cout));
    for (i = 0; i < size; i++) {
        if (list[i].info == 0) {
            break;
        }
        if (cout->line_buf_len + strlen(list[i].info) + 8 >
                cout->ctx->out_column_limit) {
            sdb_assert(__sdb_mcout_append_endline(cout));
        }
        sdb_assert(__sdb_mcout_append(cout, "  %d.%s", i + 1, list[i].info));
    }
    if (cout->line_buf_len + 4 + strlen(sdb_last_item)
            > cout->ctx->out_column_limit) {
        sdb_assert(__sdb_mcout_append_endline(cout));
    }
    sdb_assert(__sdb_mcout_append(cout, "  0.%s", sdb_last_item));
    return 0;
}

struct sdb_menu_type {
    size_t type;
    int (*menu)(sdb_cout_context *, const sdb_menu_item *, size_t);
};
static const struct sdb_menu_type menus[SDB_MENU_MAX >> SDB_MENU_OFS] = {
    { SDB_MENU_LIST, menu_list, },
    { SDB_MENU_COLUMNAR, menu_columnar, },
    { SDB_MENU_PILE, menu_pile, },
};

int __sdb_menu(const sdb_context *ctx, unsigned int mode,
        const sdb_menu_item *list, size_t size, const char *file, size_t line)
{
    int ret, num = 0;
    unsigned int type = (mode & SDB_MENU_MASK) >> SDB_MENU_OFS;

    if (type >= (SDB_MENU_MAX >> SDB_MENU_OFS)) {
        sdb_assert(__sdb_mcout(ctx, SDB_INTERNAL_ERROR, __FILE__, __LINE__,
                    "Unknown menu type:%#x(%#x)", type, mode));
        return SDB_ERR_UNKNOWN_MENU_TYPE;
    }
    mode &= ~SDB_TYPE_MASK;

    while (1) {
        char buf[ctx->out_column_limit + SDB_CONFIG_OUTPUT_BUFFER_RESERVE];
        sdb_cout_context cout;

        sdb_assert(__sdb_mcout(ctx, mode | SDB_TYPE_NONE, file, line, ""));
        __sdb_mcout_init(&cout, ctx, mode | SDB_TYPE_MENU, buf, sizeof(buf),
                file, line);
        sdb_assert(menus[type].menu(&cout, list, size));
        sdb_assert(__sdb_mcout_final(&cout));

        if (__sdb_nmcin(ctx, mode, &num, file, line, ">") < 0) {
            continue;
        }
        if (num == 0) {
            break;
        }
        else if (num > 0 && num <= (int)size) {
            if (list[num - 1].func) {
                if ((ret = list[num - 1].func(list[num - 1].param)) != 0) {
                    sdb_assert(__sdb_mcout(ctx, SDB_INTERNAL_WARNING,
                                file, line, "above return: %d(%s%#x)", ret,
                                ret < 0 ? "-" : "", ret < 0 ? -ret : ret));
                }
            }
        }
        else {
            sdb_assert(__sdb_mcout(ctx, SDB_INTERNAL_WARNING,
                        __FILE__, __LINE__,
                        "input out of range: %d (0~%d)", num, size));
        }
    }
    return 0;
}

