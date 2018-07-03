#ifndef __SDB_INTERNAL_H__
#define __SDB_INTERNAL_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "sdb_config.h"
#define SDB_ENABLE
#include "libsdb.h"

int sdb_bio_out(const sdb_context *ctx,
        const char *file, size_t line, const char *str);
int sdb_bio_in(const sdb_context *ctx, char *buf, size_t size, size_t *len);

typedef enum {
    SDB_OUT_NONE = 0,
    SDB_OUT_INIT,
    SDB_OUT_FINAL,
    SDB_OUT_END_LINE,
    SDB_OUT_LINE_HEAD,
    SDB_OUT_LINE_TAIL,
    SDB_OUT_STRING_HEAD,
    SDB_OUT_STRING_TAIL,
} sdb_out_state;
typedef int (*func_sdb_vxprintf)(void *, const char *, size_t, sdb_out_state);
typedef struct {
    func_sdb_vxprintf f_out;
    void *p_out;
    unsigned int state;
    const char *fmt;
    va_list va;
} sdb_xprintf_context;
int sdb_vxprintf(sdb_xprintf_context *ctx);

#ifdef __cplusplus
}
#endif
#endif /* __SDB_INTERNAL_H__ */

