#include "sdb_internal.h"

#if defined(SDB_SYSTEM_HAS_STDIO)
#include <stdio.h>
static int std_out(void *p, const char *file, unsigned int line,
        const char *str)
{
    printf("%16s:%04d  %s\n", file, line, str);
    fflush(stdout);

    return 0;
}

static int std_in(void *p, char *buf, unsigned int size, unsigned int *len)
{
    sdb_stack_touch();
    if (size == 0) {
        return 0;
    }
    size--;

    int c;
    unsigned int i = 0;
    while (buf && i < size) {
        if ((c = getchar()) == EOF) {
            break;
        }
        if ((buf[i++] = c) == '\n') {
            break;
        }
    }
    buf[i] = 0;
    if (len) {
        *len = i;
    }

    return 0;
}
#endif /* defined(SDB_SYSTEM_HAS_STDIO) */


struct sdb_bio_config_t {
    void *p;
    sdb_bio_out_t out;
    sdb_bio_in_t in;
};
static struct sdb_bio_config_t bio_conf = {
    .p      = 0,
    .out    = 0,
    .in     = 0,
};

int sdb_bio_conf(sdb_bio_out_t out, sdb_bio_in_t in, void *p)
{
    bio_conf.out = out;
    bio_conf.in = in;
    bio_conf.p = p;
}

int bio_out(const char *file, unsigned int line, const char *str)
{
    if (bio_conf.out) {
        return bio_conf.out(bio_conf.p, file, line, str);
    }
#if defined(SDB_SYSTEM_HAS_STDIO)
    else {
        return std_out(0, file, line, str);
    }
#endif /* defined(SDB_SYSTEM_HAS_STDIO) */
    return 0;
}

int bio_in(char *buf, unsigned int size, unsigned int *len)
{
    if (buf == 0) {
        return SDB_ERR_BAD_PARAM;
    }
    if (bio_conf.in) {
        return bio_conf.in(bio_conf.p, buf, size, len);
    }
#if defined(SDB_SYSTEM_HAS_STDIO)
    else {
        return std_in(0, buf, size, len);
    }
#endif /* defined(SDB_SYSTEM_HAS_STDIO) */

    return 0;
}

