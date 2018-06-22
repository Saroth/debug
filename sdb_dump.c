#include "sdb_internal.h"

#warning "@TODO: strcasecmp achieve"
#include <string.h>

typedef struct {
    sdb_context *ctx;
    unsigned int mode;
    const char *file;
    size_t line;
#if defined(SDB_SYSTEM_HAS_STDERR)
    int err;
#endif
    char *line_buf;
    size_t line_buf_size;
    size_t line_buf_len;
    size_t counter;
} dump_line_param;
static int dump_line(void *p_out, const char *str, sdb_out_state state)
{
    dump_line_param *p = (dump_line_param *)p_out;
    do {
        if (p->line_buf_len == 0) {
#warning "TODO: + color, + mark"
        }
        while (*str && p->line_buf_len < p->ctx->out_column_limit) {
            p->line_buf[p->line_buf_len++] = *str++;
        }
#if defined(SDB_SYSTEM_HAS_STDERR)
        if (state == SDB_OUT_FINAL && p->err) {
// #warning "TODO: + color"
            str = strerror(p->err);
            state = SDB_OUT_STDERR;
            continue;
        }
#endif
        if (p->line_buf_len >= p->ctx->out_column_limit
                || state != SDB_OUT_NONE) {
            if (p->line_buf_len > p->ctx->out_column_limit) {
                p->line_buf_len = p->ctx->out_column_limit;
            }
// #warning "TODO: + color:rec"
            p->line_buf[p->line_buf_len] = 0;
            int ret = sdb_bio_out(p->ctx, p->file, p->line, p->line_buf);
            if (ret < 0) {
                return ret;
            }
            p->counter += ret;
            p->line_buf_len = 0;
            return p->counter;
        }
    } while (*str);
    return 0;
}

int __sdb_vmdump(sdb_context *ctx, const void *buf, size_t size, void *addr,
        const char *file, size_t line, const char *fmt, va_list va)
{
    int ret;
    if (fmt) {
        sdb_assert(__sdb_vmcout(ctx, SDB_MSG_DUMP, file, line, fmt, va));
    }
    if (buf == 0) {
        return __sdb_mcout(ctx, SDB_MSG_DUMP, __FILE__, __LINE__, buf);
    }
    if (size > 0xFFFF) {
        sdb_assert(__sdb_mcout(ctx, SDB_MSG_WARNING, __FILE__, __LINE__,
                "data size too large! length:%d(%#x)", size, size));
        char in[8];
        sdb_assert(__sdb_mcin(ctx, SDB_MSG_INPUT_STR, in, sizeof(in), 0,
                __FILE__, __LINE__, "continue to dump? (y/N)"));
        if (strcasecmp(in, "y")) {
            return 0;
        }
    }

    char b[ctx->out_column_limit + SDB_CONFIG_OUTPUT_BUFFER_RESERVE];
    size_t counter = 0;
    size_t offset = 0;
    char addr_str[20];
    sdb_vsnprintf(addr_str, sizeof(addr_str), "%04x: ", addr);
    sdb_cout_context param;
    __sdb_mcout_init(&param, ctx, SDB_MSG_DUMP, b, sizeof(b), file, line);
    while (size) {
        // ...
    }
    __sdb_mcout_final(&param);

    return 0;
}


#if 0

int sdb_dump(void *data, unsigned int len, unsigned long addr,
        const sdb_config_t *cfg, int opt,
        const char *file, unsigned int line, const char *fmt, ...)
{
    int ret = 0;
    va_list va;
    bio_put_param_t bp;
    unsigned char seg = 16;
    unsigned int count = 0;
    int i;
    int head = 0;
    int end;
    unsigned long a;

    opt &= cfg->opt;
    bp.cfg          = cfg;
    bp.flag         = SDB_TYPE_DUMP;

    if (data == NULL)
        return SDB_RET_PARAM_ERR;
    if (len > 0xFFFF) {
#if defined(SDB_SYS_SUPPORT_LARGE_MEM) && defined(SDB_MDL_GET_ENABLE)
        SDB_OUT_W("Data length too large! len:%d(%#x)", len, len);
        char in[8];
        SDB_IN_SI(in, 8, NULL, "Dump it? (yes/no)");
        if (strcasecmp(in, "yes"))
            return 0;
#endif
    }
    PUT_PEND(&bp);
    if (fmt) {
        PUT_STR_BLK(&bp, SDB_DATA_FILE, file, 0);
        PUT_STR_BLK(&bp, SDB_DATA_LINE, (const char *)&line, 1);
        bp.flag |= SDB_DATA_INFO;
        va_start(va, fmt);
        ret = vxprint((void *)&bp, cb_putx, fmt, va);
        va_end(va);
        if (ret)
            return ret;
        bp.flag &= ~SDB_DATA_INFO;
        PUT_STR(&bp, SDB_DATA_INFO, " [buf:", 6);
        PUT_NUM(&bp, SDB_DATA_INFO, 16, ALTERNATE_FORM, 0, (unsigned long)data);
        PUT_STR(&bp, SDB_DATA_INFO, ", len:", 6);
        PUT_NUM(&bp, SDB_DATA_INFO, 10, 0, 0, len);
        PUT_STR(&bp, SDB_DATA_INFO, "]", 6);
        PUT_WRAP(&bp);
    }
    if (opt & SDB_DUMP_SEGADD16)
        seg += 16;
    if (opt & SDB_DUMP_SEGADD32)
        seg += 32;

    if (addr % seg)
        head -= addr % seg;
    while (1) {
        end = (int)seg + head;
        a = addr + count;
        PUT_STR_BLK(&bp, SDB_DATA_FILE, file, 0);
        PUT_STR_BLK(&bp, SDB_DATA_LINE, (const char *)&line, 1);
        if (len - head - count < seg)
            end = len - count;

        if (~opt & SDB_DUMP_NONUM) {
            if (addr)
                PUT_NUM(&bp, SDB_DATA_INFO, 16, PAD_ZERO, 8, a + head);
            else
                PUT_NUM(&bp, SDB_DATA_INFO, 16, PAD_ZERO, 4, a + head);
            PUT_STR(&bp, SDB_DATA_INFO, ":", 1);

            if (end == seg && (*(unsigned char *)data == 0xFF
                        || (*(unsigned char *)data == 0x00))) {
                unsigned char c = *(unsigned char *)data;
                i = 0;
                while (1) {
                    if (*((unsigned char *)data + i++) != c)
                        break;
                }
                if (i / seg > 1) {
                    end = (i / seg) * seg;
                    if (c == 0xFF)
                        PUT_STR(&bp, SDB_DATA_INFO, " ff ...", 8);
                    else if (c == 0)
                        PUT_STR(&bp, SDB_DATA_INFO, " 00 ...", 8);
                    PUT_WRAP(&bp);
                    head = 0;
                    count += end;
                    data += end;
                    continue;
                }
            }
        }
        if (count >= len) {
            PUT_WRAP(&bp);
            break;
        }
        if (~opt & SDB_DUMP_NOHEX) {
            for (i = head; i < head + seg; i++) {
                if ((a + i) % 16 == 0)
                    PUT_STR(&bp, SDB_DATA_INFO, " ", 1);
                if (i >= 0 && i < end) {
                    PUT_NUM(&bp, SDB_DATA_INFO, 16, PAD_ZERO, 2,
                            *((unsigned char *)data + i));
                    PUT_STR(&bp, SDB_DATA_INFO, " ", 1);
                }
                else
                    PUT_STR(&bp, SDB_DATA_INFO, "   ", 1);
            }
        }
        if (~opt & SDB_DUMP_NOCHAR) {
            for (i = head; i < end; i++) {
                if ((a + i) % 16 == 0)
                    PUT_STR(&bp, SDB_DATA_INFO, " ", 1);
                if (i >= 0) {
                    unsigned char d[2];
                    d[0] = *((unsigned char *)data + i);
                    d[1] = 0;
                    if (d[0] < 0x20 || d[0] > 0x7e)
                        PUT_STR(&bp, SDB_DATA_INFO, ".", 1);
                    else
                        PUT_STR(&bp, SDB_DATA_INFO, d, 1);
                }
                else
                    PUT_STR(&bp, SDB_DATA_INFO, " ", 1);
            }
        }
        PUT_WRAP(&bp);
        head = 0;
        count += end;
        data += end;
        if (count >= len && end < seg)
            break;
    }
    PUT_POST(&bp);
    return 0;
}

#endif /* 0 */

