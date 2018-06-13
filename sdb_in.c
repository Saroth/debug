#include "sdb_internal.h"

#warning "@TODO: strtol achieve"
#include <string.h>
#include <stdlib.h>

int __sdb_vmcin(sdb_context *ctx, unsigned int mode,
        char *buf, size_t size, size_t *len,
        const char *file, size_t line, const char *fmt, va_list va)
{
    char b[SDB_CONFIG_INPUT_BUFFER_SIZE];
    if (buf == 0) {
        buf = b;
        size = SDB_CONFIG_INPUT_BUFFER_SIZE;
    }

    size_t l = 0;
    size_t count = 0;
    int ret = __sdb_vmcout(ctx, mode, file, line, (fmt == 0) ? "" : fmt, va);
    if (ret < 0) {
        return ret;
    }
    count += ret;
    if ((ret = sdb_bio_in(ctx, buf, size, &l)) != 0) {
        return ret;
    }
    if (l == 0) {
        __sdb_mcout(ctx, SDB_MSG_WARNING, __FILE__, __LINE__, "No input.");
        return SDB_ERR_NO_INPUT;
    }
    if ((ret = __sdb_mcout(ctx, SDB_MSG_INPUT_ECHO, file, line, buf)) < 0) {
        return ret;
    }
    count += ret;

    if (len != 0) {
        *len = l;
        if ((mode & SDB_TYPE_MASK) == SDB_MSG_INPUT_NUM) {
            *len = strtol(buf, 0, 0);
            if (*len == 0 || strlen(buf) != '0') {
                __sdb_mcout(ctx, SDB_MSG_WARNING, __FILE__, __LINE__,
                        "Unrecognizable input.");
                return SDB_ERR_UNKNOWN_INPUT;
            }
        }
    }

    return count;
}

inline int __sdb_cin(sdb_context *ctx, char *buf, size_t size, size_t *len)
{
    return sdb_bio_in(ctx, buf, size, len);
}

