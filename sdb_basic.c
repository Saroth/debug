#include <string.h>

#include "sdb_internal.h"

size_t sdb_config_size(void)
{
    return sizeof(sdb_context);
}

void sdb_config_init(sdb_context *ctx)
{
    memset(ctx, 0, sizeof(sdb_context));
    sdb_stack_mark(ctx);
}

