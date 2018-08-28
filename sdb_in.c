#include "sdb_internal.h"

#warning "@TODO: strtol achieve"
#include <string.h>
#include <stdlib.h>

int __sdb_vmcin(const sdb_context *ctx, unsigned int mode,
        char *buf, size_t size, size_t *len,
        const char *file, size_t line, const char *fmt, va_list va)
{
    int ret, type = mode & SDB_TYPE_MASK;
    size_t count = 0, l = 0;

    if (len == 0) {
        len = &l;
    }
    if (type == 0) {
        type = SDB_TYPE_INPUT_STR;
    }
    mode &= ~SDB_TYPE_MASK;

    sdb_assert(__sdb_vmcout(ctx, mode | type,
                file, line, (fmt == 0) ? "" : fmt, va));
    count += ret;
    sdb_assert(sdb_bio_in(ctx, buf, size, len));
    if (*len == 0) {
        sdb_assert(__sdb_mcout(ctx, SDB_MSG_WARNING,
                    __FILE__, __LINE__, "no input."));
        return SDB_ERR_NO_INPUT;
    }
    sdb_assert(__sdb_mcout(ctx, mode | SDB_TYPE_INPUT_ECHO,
                file, line, "'%s'(%d byte)", buf, *len));
    count += ret;
    return count;
}

int __sdb_mcin(const sdb_context *ctx, unsigned int mode,
        char *buf, size_t size, size_t *len,
        const char *file, size_t line, const char *fmt, ...)
{
    int ret;
    va_list va;
    va_start(va, fmt);
    ret = __sdb_vmcin(ctx, mode, buf, size, len, file, line, fmt, va);
    va_end(va);
    return ret;
}

int __sdb_vnmcin(const sdb_context *ctx, unsigned int mode, int *num,
        const char *file, size_t line, const char *fmt, va_list va)
{
    int ret;
    char in[SDB_CONFIG_INPUT_BUFFER_SIZE];
    size_t len;
    if ((ret = __sdb_vmcin(ctx, (mode & ~SDB_TYPE_MASK) | SDB_TYPE_INPUT_NUM,
                    in, sizeof(in), &len, file, line, fmt, va)) < 0) {
        return ret;
    }
    if (num) {
        *num = strtol(in, 0, 0);
        if (*num == 0 && in[0] != '0') {
            sdb_assert(__sdb_mcout(ctx, SDB_MSG_WARNING, __FILE__, __LINE__,
                        "unrecognizable input: '%s'(%d byte)", in, len));
            return SDB_ERR_UNKNOWN_INPUT;
        }
    }
    return ret;
}

int __sdb_nmcin(const sdb_context *ctx, unsigned int mode, int *num,
        const char *file, size_t line, const char *fmt, ...)
{
    int ret;
    va_list va;
    va_start(va, fmt);
    ret = __sdb_vnmcin(ctx, mode, num, file, line, fmt, va);
    va_end(va);
    return ret;
}

int __sdb_rnmcin(const sdb_context *ctx, unsigned int mode,
        const char *file, size_t line)
{
    int num = 0;
    int ret = __sdb_nmcin(ctx, mode, &num, file, line, 0);
    if (ret < SDB_ERR_NULL_BUFFER && ret > SDB_ERR_MAX) {
        return ret;
    }
    return num;
}

int __sdb_cin(const sdb_context *ctx,
        char *buf, size_t size, size_t *len)
{
    return sdb_bio_in(ctx, buf, size, len);
}

