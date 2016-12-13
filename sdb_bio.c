#include "sdb_config.h"

inline int sdb_nop(void)
{
    return 0;
}

#if defined(SDB_ENABLE)

#if defined(SDB_SYS_HAVE_STDIO)
static void output_std(const sdb_bio_context_t *out)
{
    char *flag;

    if ((out->flag & SDB_FLG_LV) == SDB_FLG_LV_ERR)
        flag = SDB_MARK_ERR;
    else if ((out->flag & SDB_FLG_LV) == SDB_FLG_LV_WARN)
        flag = SDB_MARK_WARN;
    else if ((out->flag & SDB_FLG_LV) == SDB_FLG_LV_INFO) {
        if ((out->flag & SDB_FLG_T) == SDB_FLG_T_INPUT_NUM)
            flag = SDB_MARK_GETNUM;
        else if ((out->flag & SDB_FLG_T) == SDB_FLG_T_INPUT_STR)
            flag = SDB_MARK_GETSTR;
        else if ((out->flag & SDB_FLG_T) == SDB_FLG_T_INPUTECHO)
            flag = SDB_MARK_ECHO;
        else if ((out->flag & SDB_FLG_T) == SDB_FLG_T_DUMP)
            flag = SDB_MARK_DUMP;
        else
            flag = SDB_MARK_INFO;
    }
    else
        flag = SDB_MARK_NONE;

    if (!(out->flag & SDB_FLG_BARE))
        printf("%16s:%04d %s", out->file, out->line, flag);
    printf("%s", out->buf);
    if (!(out->flag & SDB_FLG_NOWRAP))
        printf("\n");
    fflush(stdout);
}

static int input_std(char *buf, size_t bufsize, size_t *outlen)
{
    int ch = 0;
    int i = 0;

    while (buf) {
        if ((ch = getchar()) == EOF)
            break;
        if ((buf[i++] = ch) == '\n' || (bufsize > 0 && i >= bufsize))
            break;
    }
    if (outlen)
        *outlen = i;

    return 0;
}

const sdb_config_t sdb_cfg_std = {
    .opt        = SDB_IO,
    .input      = input_std,
    .output     = output_std,
    .ptr        = NULL,
};
#endif /* defined(SDB_SYS_HAVE_STDIO) */

int bio_output(sdb_bio_context_t *ctx)
{
    if (ctx->cfg && ctx->cfg->output)
        ctx->cfg->output(ctx);

    return 0;
}

int bio_input(const sdb_config_t *cfg,
        char *buf, size_t bufsize, size_t *outlen)
{
    if (buf == NULL)
        return SDB_RET_PARAM_ERR;
    if (cfg && cfg->input)
        return cfg->input(buf, bufsize, outlen);

    return 0;
}

#endif /* defined(SDB_ENABLE) */

