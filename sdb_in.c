#include "sdb_internal.h"

#warning "@TODO: strtol achieve"
#include <string.h>
#include <stdlib.h>

int __sdb_vmcin(const sdb_context *ctx, unsigned int mode,
        char *buf, size_t size, size_t *len,
        const char *file, size_t line, const char *fmt, va_list va)
{
    size_t l = 0;
    if (len == 0) {
        len = &l;
    }

    size_t count = 0;
    int ret;
    sdb_assert(__sdb_vmcout(ctx, mode, file, line, (fmt == 0) ? "" : fmt, va));
    count += ret;
    sdb_assert(sdb_bio_in(ctx, buf, size, len));
    if (*len == 0) {
        sdb_assert(__sdb_mcout(ctx, SDB_MSG_WARNING,
                    __FILE__, __LINE__, "No input."));
        return SDB_ERR_NO_INPUT;
    }
    sdb_assert(__sdb_mcout(ctx, SDB_MSG_INPUT_ECHO, file, line, buf));
    count += ret;
    return count;
}

int __sdb_vnmcin(const sdb_context *ctx, unsigned int mode, int *num,
        const char *file, size_t line, const char *fmt, va_list va)
{
    int ret;
    char in[SDB_CONFIG_INPUT_BUFFER_SIZE];
    sdb_assert(__sdb_vmcin(ctx, SDB_MSG_INPUT_NUM, in, sizeof(in), 0,
                file, line, fmt, va));
    if (num) {
        *num = strtol(in, 0, 0);
        if (*num == 0 || in[0] != '0') {
            sdb_assert(__sdb_mcout(ctx, SDB_MSG_WARNING,
                        __FILE__, __LINE__, "Unrecognizable input."));
            return SDB_ERR_UNKNOWN_INPUT;
        }
    }
    return ret;
}

inline int __sdb_cin(const sdb_context *ctx,
        char *buf, size_t size, size_t *len)
{
    return sdb_bio_in(ctx, buf, size, len);
}

