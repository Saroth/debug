#ifndef __SDB_INTERNAL_H__
#define __SDB_INTERNAL_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>
#include "sdb_config.h"

typedef struct {
#if defined(SDB_MODULE_STACK_WATCH)
    size_t *stack_mark;
    size_t stack_top;
    size_t stack_max_usage;
#endif
} sdb_context;

size_t sdb_stack_mark(sdb_context *ctx);
size_t sdb_stack_touch(sdb_context *ctx);
size_t sdb_stack_max_usage(sdb_context *ctx);

#ifdef __cplusplus
}
#endif
#endif /* __SDB_INTERNAL_H__ */

