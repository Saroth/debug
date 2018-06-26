#include "sdb_internal.h"

#if defined(SDB_SYSTEM_HAS_STDIO)
#include <stdio.h>
#include <string.h>

static int std_out(void *p, const char *file, size_t line, const char *str)
{
    sdb_stack_touch((sdb_context *)p);
    int ret;
    if (file) {
        ret = printf("%16s:%04d  %s\n", strrchr(file, '/')
                ? (strrchr(file, '/') + 1) : strrchr(file, '\\')
                ? (strrchr(file, '\\') + 1) : file, line, str);
    }
    else {
        ret = printf("%s", str);
    }
    fflush(stdout);

    return ret;
}

static int std_in(void *p, char *buf, size_t size, size_t *len)
{
    sdb_stack_touch((sdb_context *)p);
    if (size == 0) {
        return 0;
    }
    size--;

    int c;
    size_t i = 0;
    while (buf && i < size) {
        if ((c = getchar()) == EOF) {
            break;
        }
        if ((buf[i++] = c) == '\n') {
            break;
        }
    }
    buf[i] = 0;
    if (len) {
        *len = i;
    }

    return i;
}
#endif /* defined(SDB_SYSTEM_HAS_STDIO) */

int sdb_bio_out(sdb_context *ctx,
        const char *file, size_t line, const char *str)
{
    if (ctx->bio_out) {
        return ctx->bio_out(ctx->bio_param, file, line, str);
    }
#if defined(SDB_SYSTEM_HAS_STDIO)
    else {
        return std_out(ctx, file, line, str);
    }
#endif /* defined(SDB_SYSTEM_HAS_STDIO) */
    return 0;
}

int sdb_bio_in(sdb_context *ctx, char *buf, size_t size, size_t *len)
{
    if (buf == 0) {
        return SDB_ERR_BAD_PARAM;
    }
    if (ctx->bio_in) {
        return ctx->bio_in(ctx->bio_param, buf, size, len);
    }
#if defined(SDB_SYSTEM_HAS_STDIO)
    else {
        return std_in(ctx, buf, size, len);
    }
#endif /* defined(SDB_SYSTEM_HAS_STDIO) */
    return 0;
}

