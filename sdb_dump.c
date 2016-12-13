#include "sdb_config.h"

#if defined(SDB_ENABLE) && defined(SDB_MDL_DUMP_ENABLE)

#define SDB_SET_CONFIG p.ctx->cfg

static int dump_line(dump_param_t *p, size_t ofs, unsigned char sec)
{
    int ret;
    int i;
    int head = 0;
    int end = sec;
    int opt = p->opt & p->ctx->cfg->opt;
    unsigned char *data = p->data + ofs;
    size_t addr = (size_t)p->addr + ofs;
    char *buf = p->ctx->buf;

    if (addr % sec)
        head -= addr % sec;
    if (p->len + head - ofs < sec)
        end = p->len + head - ofs;

    if (~opt & SDB_DUMP_NONUM)
        buf += sprintf(buf, (p->addr ? "%08x:" : "%04x:"), addr + head);
    if (~opt & SDB_DUMP_NOHEX)
        for (i = head; i < head + sec; i++) {
            if ((addr + i) % 16 == 0)
                *buf++ = ' ';
            if (i >= 0 && i < end)
                buf += sprintf(buf, "%02x ", *(data + i));
            else
                buf += sprintf(buf, "   ");
        }
    if (~opt & SDB_DUMP_NOCHAR)
        for (i = head; i < end; i++, buf++) {
            if ((addr + i) % 16 == 0)
                *buf++ = ' ';
            if (i >= 0) {
                *buf = *(data + i);
                if (*buf < 0x20 || *buf > 0x7e)
                    *buf = '.';
            }
            else
                *buf = ' ';
        }
    *buf++ = 0;
    p->ctx->len = buf - p->ctx->buf - 1;
    if ((ret = bio_output(p->ctx)))
        return ret;

    return end;
}

static int dump_proc(dump_param_t *p)
{
    int ret;
    size_t count = 0;
    unsigned char seg = 16;
    unsigned char buf[SDB_CONF_BUFFER_SIZE];

    p->ctx->buf = buf;
    if (p->opt & SDB_DUMP_SEGADD16)
        seg += 16;
    if (p->opt & SDB_DUMP_SEGADD32)
        seg += 32;
    while (count < p->len) {
        if ((ret = dump_line(p, count, seg)) < 0)
            return ret;
        count += ret;
    }

    return 0;
}

int sdb_dump(const sdb_config_t *cfg, int opt,
        const char *file, const char *func, size_t line,
        void *data, size_t len, void *addr, const char *fmt, ...)
{
    int ret = 0;
    va_list ap;
    dump_param_t p;
    sdb_bio_context_t ctx;

    p.opt           = opt;
    p.data          = data;
    p.len           = len;
    p.addr          = addr;
    p.ctx           = &ctx;
    ctx.cfg         = cfg;
    ctx.flag        = SDB_FLG_LV_INFO | SDB_FLG_T_DUMP;
    ctx.file        = file;
    ctx.func        = func;
    ctx.line        = line;

    if (data == NULL)
        return SDB_RET_PARAM_ERR;
    if (len < 0 || len > 0xFFFF) {
        SDB_OUT_W("Data length too large! len:%d(%#x)", len, len);
#if defined(SDB_MDL_INPUT_ENABLE)
        char in[32];
        SDB_IN_SI(in, 32, NULL, "Dump it? (yes/no)");
        if (strcasecmp(in, "yes"))
            return 0;
#else
        return SDB_RET_PARAM_ERR;
#endif
    }
    if (fmt) {
        va_start(ap, fmt);
        sdb_output_v(cfg, ctx.flag | SDB_FLG_NOWRAP,
                file, func, line, fmt, ap);
        va_end(ap);
        sdb_output(cfg, ctx.flag | SDB_FLG_BARE,
                file, func, line, " [len:%d]", p.len);
    }
    return dump_proc(&p);
}

#else
inline int sdb_dump(const sdb_config_t *cfg, int opt,
        const char *file, const char *func, size_t line,
        void *data, size_t len, void *addr, const char *fmt, ...)
{
    return 0;
}
#endif /* defined(SDB_ENABLE) && defined(SDB_MDL_DUMP_ENABLE) */

