#include "sdb_config.h"

#if defined(SDB_ENABLE) && defined(SDB_MDL_DUMP_ENABLE)

static int sdb_dump_makeline(SDB_IO_PARAM_T *p, size_t ofs, unsigned char sec)
{
    int i = 0;
    int head = 0;
    int end = sec;
    int dump_type = p->mode & SDB_MODE_DMP;
    unsigned char *buf = (unsigned char *)(p->format + ofs);
    size_t addr = (size_t)p->addr + ofs;
    char *cache = p->cache;

    if (dump_type == SDB_MODE_DMP_STR) {
        if (p->length - ofs < sec) {
            end = p->length - ofs;
        }
        memcpy(cache, buf, end);
        *(cache + end) = 0;
        return end;
    }

    if (addr % sec) {
        head -= addr % sec;
    }
    if (p->length + head - ofs < sec) {
        end = p->length + head - ofs;
    }

    cache += sprintf(cache, "%04x:", addr + head);
    if (dump_type & SDB_MODE_DMP_HEX) {
        for (i = head; i < head + sec; i++) {
            if ((addr + i) % 16 == 0) {
                *cache++ = ' ';
            }
            if (i >= 0 && i < end) {
                cache += sprintf(cache, "%02x ", *(buf + i));
            }
            else {
                cache += sprintf(cache, "   ");
            }
        }
    }
    if (dump_type & SDB_MODE_DMP_CHAR) {
        for (i = head; i < end; i++, cache++) {
            if ((addr + i) % 16 == 0) {
                *cache++ = ' ';
            }
            if (i >= 0) {
                *cache = *(buf + i);
                if (*cache < 0x20 || *cache > 0x7e) {
                    *cache = '.';
                }
            }
            else {
                *cache = ' ';
            }
        }
    }
    *cache++ = 0;

    return end;
}

static int sdb_dump_buf(SDB_IO_PARAM_T *p)
{
    int ret = 0;
    int count = 0;
    unsigned char sec = 16;

    if (p->options & SDB_DUMP_SEC_ADD16) {
        sec += 16;
    }
    if (p->options & SDB_DUMP_SEC_ADD32) {
        sec += 32;
    }

    while (count < p->length) {
        if ((ret = sdb_dump_makeline(p, count, sec)) < 0) {
            return ret;
        }
        count += ret;
        if ((ret = sdb_output(p->options, p->mode,
                        p->file, p->func, p->line, "%s", p->cache))) {
            return ret;
        }
    }

    return 0;
}

int sdb_dump(size_t opt, size_t mode,
        const char *file, const char *func, size_t line,
        void *buf, size_t len, void *addr, const char *format, ...)
{
    int ret = 0;
    SDB_IO_PARAM_T p;

    if ((~opt) & SDB_IO || opt & SDB_NO_DUMP) {
        return 0;
    }
    p.options   = opt;
    p.mode      = mode;
    p.file      = file;
    p.func      = func;
    p.line      = line;

    if (format) {
        p.mode      |= SDB_MODE_MSG_DUMPSIZE;
        p.format    = format;
        p.addr      = (void *)len;      // 临时传递数据长度
        p.length    = 0;
        va_start(p.ap, format);
        if ((ret = sdb_out_style(&p))) {
            va_end(p.ap);
            return ret;
        }
        va_end(p.ap);
        p.mode      &= ~SDB_MODE_MSG_DUMPSIZE;
    }

    p.format    = (const char *)buf;    // 用于存放数据缓存地址
    p.length    = len;                  // 用于存放数据长度
    p.addr      = addr;
    if ((ret = sdb_dump_buf(&p))) {
        return ret;
    }

    return 0;
}

#else
inline int sdb_dump(size_t opt, size_t mode,
        const char *file, const char *func, size_t line,
        void *buf, size_t len, void *addr, const char *format, ...)
{
    return 0;
}
#endif /* defined(SDB_ENABLE) && defined(SDB_MDL_DUMP_ENABLE) */

