#include "sdb_internal.h"

#warning "@TODO: strcasecmp achieve"
#include <string.h>

int __sdb_vmdump(const sdb_context *ctx,
        const void *data, size_t size, void *addr,
        const char *file, size_t line, const char *fmt, va_list va)
{
    int ret;
    size_t i, counter = 0, bytes, blank;
    char buf[ctx->out_column_limit + SDB_CONFIG_OUTPUT_BUFFER_RESERVE];
    unsigned char c;
    unsigned char *ptr;
    sdb_cout_context cout;

    if (data == 0) {
        return __sdb_mcout(ctx, SDB_MSG_DUMP, __FILE__, __LINE__, data);
    }
    if (size > 0xFFFF) {
        char in[8];
        sdb_assert(__sdb_mcout(ctx, SDB_MSG_WARNING, __FILE__, __LINE__,
                "data size too large! length:%d(%#x)", size, size));
        counter += ret;
        if ((ret = __sdb_mcin(ctx, SDB_MSG_INPUT, in, sizeof(in), 0,
                        __FILE__, __LINE__, "continue to dump? (y/N)")) < 0) {
            return ret;
        }
        counter += ret;
        if (in[0] != 'y') {
            return counter;
        }
    }

    __sdb_mcout_init(&cout, ctx, SDB_MSG_DUMP, buf, sizeof(buf), file, line);
    if (fmt) {
        sdb_assert(__sdb_mcout_append_va(&cout, fmt, va));
        sdb_assert(__sdb_mcout_append(&cout, " [data:%p, size:%d, addr:%p]",
                    data, size, addr));
        sdb_assert(__sdb_mcout_append_endline(&cout));
    }

    blank = ((size_t)addr & 0xFFFF) % ctx->dump_bytes_perline;
    ptr = (unsigned char *)data - blank;
    size += blank;
    addr -= blank;
    while (1) {
        bytes = ctx->dump_bytes_perline > size
            ? size : ctx->dump_bytes_perline;
        if (ctx->dump_has_addr) {
            sdb_assert(__sdb_mcout_append(&cout, "%04lx:", addr));
        }
        if (ctx->dump_has_hex) {
            i = 0;
            if (blank) {
                for (; i < blank; i++) {
                    sdb_assert(__sdb_mcout_append_string(&cout, " --"));
                }
            }
            for (; i < ctx->dump_bytes_perline; i++) {
                if (i < bytes) {
                    sdb_assert(__sdb_mcout_append(&cout, " %02x",
                                ptr[i] & 0xff));
                }
                else {
                    sdb_assert(__sdb_mcout_append_string(&cout, "   "));
                }
            }
        }
        if (ctx->dump_has_ascii) {
            i = 0;
            sdb_assert(__sdb_mcout_append_string(&cout, "  "));
            if (blank) {
                for (; i < blank; i++) {
                    sdb_assert(__sdb_mcout_append_string(&cout, " "));
                }
            }
            for (; i < ctx->dump_bytes_perline; i++) {
                if (i < bytes) {
                    c = ptr[i];
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

        if ((size -= bytes) == 0) {
            break;
        }
        ptr += bytes;
        addr += bytes;
        blank = 0;
        sdb_assert(__sdb_mcout_append_endline(&cout));
    }
    sdb_assert(__sdb_mcout_final(&cout));
    counter += ret;

    return counter;
}

int __sdb_mdump(const sdb_context *ctx,
        const void *data, size_t size, void *addr,
        const char *file, size_t line, const char *fmt, ...)
{
    int ret;
    va_list va;
    va_start(va, fmt);
    ret = __sdb_vmdump(ctx, data, size, addr, file, line, fmt, va);
    va_end(va);
    return ret;
}

