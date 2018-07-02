#include "sdb_internal.h"

#warning "@TODO: strcasecmp achieve"
#include <string.h>

int __sdb_vmdump(const sdb_context *ctx,
        const void *data, size_t size, void *addr,
        const char *file, size_t line, const char *fmt, va_list va)
{
    int ret;
    if (data == 0) {
        return __sdb_mcout(ctx, SDB_MSG_DUMP, __FILE__, __LINE__, data);
    }
    size_t counter = 0;
    if (size > 0xFFFF) {
        sdb_assert(__sdb_mcout(ctx, SDB_MSG_WARNING, __FILE__, __LINE__,
                "data size too large! length:%d(%#x)", size, size));
        counter += ret;
        char in[8];
        sdb_assert(__sdb_mcin(ctx, SDB_MSG_INPUT_STR, in, sizeof(in), 0,
                __FILE__, __LINE__, "continue to dump? (y/N)"));
        counter += ret;
        if (in[0] != 'y') {
            return counter;
        }
    }

    char buf[ctx->out_column_limit + SDB_CONFIG_OUTPUT_BUFFER_RESERVE];
    sdb_cout_context cout;
    __sdb_mcout_init(&cout, ctx, SDB_MSG_DUMP, buf, sizeof(buf), file, line);
    if (fmt) {
        sdb_assert(__sdb_mcout_append_va(&cout, fmt, va));
        sdb_assert(__sdb_mcout_append(&cout, " [data:%p, size:%d, addr:%p]",
                    data, size, addr));
        sdb_assert(__sdb_mcout_append_endline(&cout));
    }

    size_t offset = 0;
    size_t blank = ((size_t)addr & 0xFFFF) % ctx->dump_bytes_perline;
    unsigned char *ptr = (unsigned char *)data - blank;
    char addr_str[20];
    sdb_snprintf(addr_str, sizeof(addr_str), "%04lx:", addr);
    size += blank;
    while (size) {
        size_t bytes = ctx->dump_bytes_perline > size
            ? size : ctx->dump_bytes_perline;
        if (ctx->dump_has_addr) {
            sdb_assert(__sdb_mcout_append_string(&cout, addr_str));
        }
        if (ctx->dump_has_hex) {
            size_t i = 0;
            if (blank) {
                for (; i < blank; i++) {
                    sdb_assert(__sdb_mcout_append_string(&cout, " --"));
                }
            }
            for (; i < ctx->dump_bytes_perline; i++) {
                if (i < bytes) {
                    sdb_assert(__sdb_mcout_append(&cout, " %02x",
                                ptr[offset + i] & 0xff));
                }
                else {
                    sdb_assert(__sdb_mcout_append_string(&cout, "   "));
                }
            }
        }
        if (ctx->dump_has_ascii) {
            sdb_assert(__sdb_mcout_append_string(&cout, "  "));
            size_t i = 0;
            if (blank) {
                for (; i < blank; i++) {
                    sdb_assert(__sdb_mcout_append_string(&cout, " "));
                }
            }
            for (; i < ctx->dump_bytes_perline; i++) {
                if (i < bytes) {
                    char c = (char)ptr[offset + i];
                    if (c >= 0x20 && c < 0x80) {
                        sdb_assert(__sdb_mcout_append(&cout, "%c", c));
                    }
                    else {
                        sdb_assert(__sdb_mcout_append_string(&cout, "."));
                    }
                }
                else {
                    sdb_assert(__sdb_mcout_append_string(&cout, " "));
                }
            }
        }
        sdb_assert(__sdb_mcout_append_endline(&cout));
        size -= bytes;
        offset += bytes;
        blank = 0;
    }
    sdb_assert(__sdb_mcout_final(&cout));
    counter += ret;

    return counter;
}

int __sdb_mdump(const sdb_context *ctx,
        const void *data, size_t size, void *addr,
        const char *file, size_t line, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    int ret = __sdb_vmdump(ctx, data, size, addr, file, line, fmt, va);
    va_end(va);
    return ret;
}

