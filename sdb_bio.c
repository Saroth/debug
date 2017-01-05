#include "sdb_config.h"

inline int sdb_nop(void)
{
    return 0;
}

#if defined(SDB_STACK_WATCH)
char *gstack_head = 0;
unsigned long gstack_max = 0;

void sdb_set_stack(void)
{
    char i;
    gstack_head = &i;
    gstack_max = 0;
}

int sdb_get_stack(void)
{
    char i;
    unsigned long p = (unsigned long)(gstack_head - &i);
    if (p > gstack_max)
        gstack_max = p;
    return (int)p;
}

int sdb_get_stack_max(void)
{
    return gstack_max;
}

#else
inline void sdb_set_stack(void) { }
inline int sdb_get_stack(void) { return 0; }
#endif

#if defined(SDB_ENABLE)

#if defined(SDB_SYS_SUPPORT_STDIO)
#include <stdio.h>
static int std_put(void *ptr, sdb_flag_t flag,
        const char *buf, unsigned int len)
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
            sdb_get_stack();
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

static int std_get(void *ptr, char *buf, unsigned int size, unsigned int *len)
{
    int c = 0;
    unsigned int i = 0;

    sdb_get_stack();
    while (buf) {
        if ((c = getchar()) == EOF)
            break;
        if ((buf[i++] = c) == '\n' || (size > 0 && i >= size - 1))
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
    .ptr        = 0,
};
#endif /* defined(SDB_SYS_SUPPORT_STDIO) */

int bio_put(const sdb_config_t *cfg, unsigned int flag,
        const char *buf, unsigned int len)
{
    if (cfg && cfg->put) {
        if (len == 0 && buf)
            while (buf[len++]);
        return cfg->put(cfg->ptr, flag, buf, len);
    }
    return 0;
}

int bio_get(const sdb_config_t *cfg,
        char *buf, unsigned int size, unsigned int *len)
{
    if (buf == 0)
        return SDB_RET_PARAM_ERR;
    if (cfg && cfg->get)
        return cfg->get(cfg->ptr, buf, size, len);

    return 0;
}

#endif /* defined(SDB_ENABLE) */

