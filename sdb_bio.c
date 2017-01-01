#include "sdb_config.h"

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

inline int sdb_nop(void)
{
    return 0;
}

#if defined(SDB_ENABLE)

#if defined(SDB_SYS_HAVE_STDIO)
static void output_std(void *ptr, sdb_flag_t flag, const char *buf, size_t len)
{
    if (flag & SDB_DATA_PEND) {
        printf("[%04d]", sdb_get_stack());
    }
    
    char b[64];
    memmove(b, buf, len);
    b[len] = 0;
    printf("%s", b);
    fflush(stdout);
}

static int input_std(void *ptr, char *buf, size_t size, size_t *len)
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
    .put        = output_std,
    .get        = input_std,
    .ptr        = NULL,
};
#endif /* defined(SDB_SYS_HAVE_STDIO) */

int bio_output(sdb_bio_context_t *ctx)
{
    if (ctx->cfg && ctx->cfg->put)
        // ctx->cfg->put(ctx->cfg->ptr, );

    return 0;
}

int bio_input(const sdb_config_t *cfg,
        char *buf, size_t bufsize, size_t *outlen)
{
    if (buf == NULL)
        return SDB_RET_PARAM_ERR;
    if (cfg && cfg->get)
        return cfg->get(cfg->ptr, buf, bufsize, outlen);

    return 0;
}

#endif /* defined(SDB_ENABLE) */

