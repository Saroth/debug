#include "sdb_config.h"

inline int sdb_nop(void)
{
    return 0;
}

#if defined(SDB_ENABLE)

#if defined(SDB_SYS_HAVE_STDIO)
static int _std_out(char *buf, size_t len)
{
    printf(buf);
    fflush(stdout);

    return 0;
}
static int _std_in(char *buf, size_t bufsize, size_t *outlen)
{
    int ch = 0;
    int i = 0;

    while (1) {
        if ((ch = getchar()) == EOF) {
            break;
        }
        if ((buf[i++] = ch) == '\n' || (bufsize > 0 && i >= bufsize)) {
            break;
        }
    }
    if (outlen) {
        *outlen = i;
    }

    return 0;
}
static SDB_BIO_T sdb_bio_std = {
    .f_output   = _std_out,
    .f_input    = _std_in,
};
static SDB_BIO_T * sdb_bio = &sdb_bio_std;
#else
static SDB_BIO_T * sdb_bio = NULL;
#endif /* defined(SDB_SYS_HAVE_STDIO) */

int sdb_bio_out(char *buf, size_t len)
{
    if (buf == NULL) {
        return SDB_RET_PARAM_ERR;
    }
    if (sdb_bio == NULL || sdb_bio->f_output == NULL) {
        return 0;
    }
    return sdb_bio->f_output(buf, len);
}

int sdb_bio_in(char *buf, size_t bufsize, size_t *outlen)
{
    if (buf == NULL) {
        return SDB_RET_PARAM_ERR;
    }
    if (sdb_bio == NULL || sdb_bio->f_input == NULL) {
        return 0;
    }
    return sdb_bio->f_input(buf, bufsize, outlen);
}

int sdb_bio_conf(SDB_BIO_T *bio)
{
    sdb_bio = bio;
    return 0;
}

#endif /* defined(SDB_ENABLE) */

