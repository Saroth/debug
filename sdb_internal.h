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

typedef enum {
    SDB_OUT_NONE = 0,
    SDB_OUT_FINAL,
    SDB_OUT_STDERR,
} sdb_out_state;
typedef struct sdb_xprintf_context_t sdb_xprintf_context;
typedef int (*func_sdb_vxprintf)(void *, const char *, sdb_out_state);
struct sdb_xprintf_context_t {
    func_sdb_vxprintf f_out;
    void *p_out;
    const char *fmt;
    va_list va;
};
int sdb_vxprintf(sdb_xprintf_context *ctx);

#ifdef __cplusplus
}
#endif
#endif /* __SDB_INTERNAL_H__ */

