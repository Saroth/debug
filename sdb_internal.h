#ifndef __SDB_INTERNAL_H__
#define __SDB_INTERNAL_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "sdb_config.h"
#include "libsdb.h"

int sdb_bio_out(sdb_context *ctx,
        const char *file, size_t line, const char *str);
int sdb_bio_in(sdb_context *ctx, char *buf, size_t size, size_t *len);

enum {
    OUT_STATE_NONE = 0,
    OUT_STATE_FINAL,
    OUT_STATE_STDERR,
};
typedef struct {
    sdb_context *ctx;
    unsigned int mode;
    const char *file;
    size_t line;
    char *buf;
    size_t len;
#if defined(SDB_SYSTEM_HAS_STDERR)
    int err;
#endif
    size_t counter;
} sdb_xprintf_context;
int sdb_vxprintf(sdb_xprintf_context *ctx, const char *fmt, va_list va);

#ifdef __cplusplus
}
#endif
#endif /* __SDB_INTERNAL_H__ */

