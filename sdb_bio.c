#include "sdb_config.h"

int sdb_nop(void) { return 0; }

#if defined(SDB_STACK_WATCH)
static unsigned long stack_top = 0;
static unsigned long stack_max = 0;

void sdb_set_stack(void)
{
    char i;

    stack_top = &i;
    stack_max = 0;
}

long sdb_get_stack(void)
{
    unsigned long p = (unsigned long)(stack_top - &p);

    if (p > stack_max) {
        stack_max = p;
    }

    return (int)p;
}

long sdb_get_stack_max(void)
{
    return stack_max;
}

#else
void sdb_set_stack(void)        { }
long sdb_get_stack(void)        { return 0; }
long sdb_get_stack_max(void)    { return 0; }
#endif

#if defined(SDB_ENABLE)

#if defined(SDB_ENV_STDIO)
#include <stdio.h>

static int std_puts(sdb_bio_puts_param_t *p)
{
#if defined(SDB_STACK_WATCH)
    printf("%04d ", sdb_get_stack());
#endif
    printf("%16s:%04d %s\r\n", p->file, p->line, p->str);

    return 0;
}

static int std_gets(sdb_bio_gets_param_t *p)
{
    int c = 0;
    unsigned int i = 0;

#if defined(SDB_STACK_WATCH)
    sdb_get_stack();
#endif
    while (p->buf) {
        if ((c = getchar()) == EOF) {
            break;
        }
        if ((p->buf[i++] = c) == '\n' || (p->size > 0 && i >= p->size - 1)) {
            break;
        }
    }

    return 0;
}

const sdb_config_t sdb_cfg_std = {
    .opt        = 0,
    .puts       = std_puts,
    .gets       = std_gets,
    .ptr        = 0,
};
#endif /* defined(SDB_ENV_STDIO) */

int bio_put(bio_put_param_t *p)
{
    if (p->cfg && p->cfg->puts) {
        return p->cfg->puts(p->p);
    }

    return 0;
}

int bio_get(bio_get_param_t *p)
{
    if (p->cfg && p->cfg->gets) {
        if (p->p->buf == 0) {
            return SDB_RET_PARAM_ERR;
        }
        return p->cfg->gets(p->p);
    }

    return 0;
}

#endif /* defined(SDB_ENABLE) */

