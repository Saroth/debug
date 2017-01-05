#include "sdb_config.h"

inline int sdb_nop(void)
{
    return 0;
}

#if defined(SDB_STACK_WATCH)
char *gstack_head = 0;

void sdb_set_stack(void)
{
    char i;
    gstack_head = &i;
}

int sdb_get_stack(void)
{
    char i;
    return (gstack_head - &i);
}
#else
inline void sdb_set_stack(void) { }
inline int sdb_get_stack(void) { return 0; }
#endif

#if defined(SDB_ENABLE)

#if defined(SDB_SYS_SUPPORT_STDIO)
#include <string.h>
static int std_put(void *ptr, sdb_flag_t flag, const char *buf, size_t len)
{
    switch (flag & SDB_DATA_MASK) {
#if 0
        case SDB_DATA_PEND: {
#if defined(SDB_STACK_WATCH)
            printf("[%04d]  ", sdb_get_stack());
#endif
            break;
        }
        case SDB_DATA_FILE: printf("%16s:", buf); break;
        case SDB_DATA_FUNC: break;
        case SDB_DATA_LINE: printf("%04d  ", len); break;
        case SDB_DATA_WRAP: printf("\n"); break;
        case SDB_DATA_STDERR_STR: printf("  [%s", buf); break;
        case SDB_DATA_STDERR_NUM: printf("(%d)]", len); break;
#endif
        default: {
#if defined(SDB_STACK_WATCH)
            // printf("[%04d]  ", sdb_get_stack()); 
#endif
            // char b[64];
            // if (len > 60 && buf == NULL)
            //     break;
            // memmove(b, buf, len);
            // b[len] = 0;
            printf("%s", buf);
            break;
        }
    }
    fflush(stdout);

    return 0;
}

static int std_get(void *ptr, char *buf, size_t size, size_t *len)
{
    int c = 0;
    int i = 0;

    while (buf) {
        if ((c = getchar()) == EOF)
            break;
        if ((buf[i++] = c) == '\n' || (size > 0 && i >= size))
            break;
    }
    if (len)
        *len = i;

    return 0;
}

const sdb_config_t sdb_cfg_std = {
    .opt        = 0,
    .put        = std_put,
    .get        = std_get,
    .ptr        = NULL,
};
#endif /* defined(SDB_SYS_SUPPORT_STDIO) */

int bio_put(const sdb_config_t *cfg, size_t flag, const char *buf, size_t len)
{
    if (cfg && cfg->put) {
        if (len == 0 && buf)
            while (buf[len++]);
        return cfg->put(cfg->ptr, flag, buf, len);
    }
    return 0;
}

int bio_get(const sdb_config_t *cfg, char *buf, size_t size, size_t *len)
{
    if (buf == NULL)
        return SDB_RET_PARAM_ERR;
    if (cfg && cfg->get)
        return cfg->get(cfg->ptr, buf, size, len);

    return 0;
}

#endif /* defined(SDB_ENABLE) */

