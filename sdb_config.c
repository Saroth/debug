#include "sdb_internal.h"

#include <string.h>

const sdb_color_codes sdb_color_terminal = {
    .recovery           = "\33[0m",     /* 恢复: normal */
    .warning            = "\33[1;36m",  /* 警告高亮: blue, bold */
    .error              = "\33[1;31m",  /* 错误高亮: red, bold */
    .input              = "\33[1;32m",  /* 输入和反馈标记高亮: green, bold */
    .title              = "\33[7m",     /* 标题高亮: inverse */
};

const sdb_mark_codes sdb_mark_default = {
    .none               = "  ",
    .info               = ". ",
    .warning            = "! ",
    .error              = "X ",
    .input_number       = "N>",
    .input_string       = "=>",
    .input_echo         = "E>",
    .dump               = " |",
    .menu               = " #",
};

const sdb_context sdb_ctx_default = {
    .stack_mark         = 0,
    .stack_top          = 0,
    .stack_max_usage    = 0,

    .bio_out            = 0,
    .bio_in             = 0,
    .bio_param          = 0,

    .out_column_limit   = SDB_CONFIG_COLUMN_LIMIT_DEF,
    .dump_bytes_perline = SDB_CONFIG_DUMP_BYTE_PERLINE_DEF,
    .dump_has_addr      = 1,
    .dump_has_hex       = 1,
    .dump_has_ascii     = 1,

    .colors             = 0,
    .marks              = 0,
};

void sdb_config_init(sdb_context *ctx)
{
    memset(ctx, 0, sizeof(sdb_context));

    sdb_stack_mark(ctx);
    ctx->out_column_limit       = SDB_CONFIG_COLUMN_LIMIT_DEF;
    ctx->dump_bytes_perline     = SDB_CONFIG_DUMP_BYTE_PERLINE_DEF;
    ctx->dump_has_addr          = 1;
    ctx->dump_has_hex           = 1;
    ctx->dump_has_ascii         = 1;
    ctx->colors                 = &sdb_color_terminal;
    ctx->marks                  = &sdb_mark_default;
}

void sdb_config_bio(sdb_context *ctx,
        func_sdb_bio_out out, func_sdb_bio_in in, void *p)
{
    ctx->bio_out = out;
    ctx->bio_in = in;
    ctx->bio_param = p;
}

void sdb_config_color(sdb_context *ctx, const sdb_color_codes *colors)
{
    ctx->colors = colors;
}

void sdb_config_mark(sdb_context *ctx, const sdb_mark_codes *marks)
{
    ctx->marks = marks;
}

void sdb_config_out_column_limit(sdb_context *ctx, size_t limit)
{
    if (limit < SDB_CONFIG_COLUMN_LIMIT_MINIMUM) {
        __sdb_mcout(ctx, SDB_MSG_WARNING, __FILE__, __LINE__,
                "column limit less than minimum(%d < %d), reset to minimum.",
                limit, SDB_CONFIG_COLUMN_LIMIT_MINIMUM);
        limit = SDB_CONFIG_COLUMN_LIMIT_MINIMUM;
    }

    size_t min = 0;
    min += 16 + 2;  /* address + space, e.g.: "0123456789abcdef: " */
    min += ctx->dump_bytes_perline * 3; /* hex + space, e.g.: "31 32 33 ..." */
    min += 1 + ctx->dump_bytes_perline; /* space + ascii, e.g.: " 123..." */
    if (limit < min) {
        __sdb_mcout(ctx, SDB_MSG_WARNING, __FILE__, __LINE__,
                "column limit less than dump minimum(%d < %d)", limit, min);
    }

    ctx->out_column_limit = limit;
}

void sdb_config_dump_bytes_perline(sdb_context *ctx, size_t size)
{
    if (size < SDB_CONFIG_DUMP_BYTE_PERLINE_MINIMUM) {
        __sdb_mcout(ctx, SDB_MSG_WARNING, __FILE__, __LINE__,
                "dump bytes per-line less than minimum(%d < %d), reset to"
                " minimum.", size, SDB_CONFIG_DUMP_BYTE_PERLINE_MINIMUM);
        size = SDB_CONFIG_COLUMN_LIMIT_MINIMUM;
    }

    size_t limit = ctx->out_column_limit;
    limit -= 16 + 2;
    limit -= 1;
    limit /= 4;
    if (size > limit) {
        __sdb_mcout(ctx, SDB_MSG_WARNING, __FILE__, __LINE__,
                "set dump bytes per-line large than limit(%d > %d(%d)).",
                size, limit, ctx->out_column_limit);
    }
    ctx->dump_bytes_perline = size;
}

void sdb_config_dump_format(sdb_context *ctx, unsigned int has_addr,
        unsigned int has_hex, unsigned int has_ascii)
{
    ctx->dump_has_addr = has_addr ? 1 : 0;
    ctx->dump_has_hex = has_hex ? 1 : 0;
    ctx->dump_has_ascii = has_ascii ? 1 : 0;
}


