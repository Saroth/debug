#ifndef __SDB_INTERNAL_H__
#define __SDB_INTERNAL_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "sdb_config.h"
#define SDB_ENABLE
#include "libsdb.h"

#define sdb_assert(_f) do {\
    if ((ret = (_f)) < 0) {\
        __sdb_mcout(ctx, SDB_MSG_WARNING, __FILE__, __LINE__,\
                "  [assert: \"%s\" (%s%#x)]", #_f,\
                ret < 0 ? "-" : "", ret < 0 ? -ret : ret);\
        return ret;\
    }\
} while (0)

int sdb_bio_out(const sdb_context *ctx,
        const char *file, size_t line, const char *str);
int sdb_bio_in(const sdb_context *ctx, char *buf, size_t size, size_t *len);

const char *sdb_get_mark(sdb_cout_context *p);
const char *sdb_get_color(sdb_cout_context *p);
extern const char *sdb_last_item;

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
typedef enum {
    SDB_OUT_LINE_IS_WRAPPED     = (1 << 0),
    SDB_OUT_LINE_IS_OUTPUTED    = (1 << 1),
} sdb_out_flags;
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

