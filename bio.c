#include "config.h"

#if defined(SDB_ENABLE)
#if defined(SDB_SYS_HAVE_STDIO)

static int _std_out(char *buf, size_t len)
{
    printf(buf);
    fflush(stdout);

    return 0;
}

static int _std_in(char *buf, size_t len)
{
    int ch = 0;
    char *b = buf;

    while (1) {
        ch = getchar();
        if (ch == EOF) {
            break;
        }
        *b = ch;
        if (*b == '\n' || (++b - buf) >= len) {
            break;
        }
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
    int ret = 0;
    char errmsg[128];

    if (buf == NULL) {
        sprintf(errmsg, "%s:%d Param error, buf:%p\r\n",
                __FILE__, __LINE__, buf);
        sdb_bio_out(errmsg, strlen(errmsg));
        return SDB_RET_PARAM_ERR;
    }
    if (sdb_bio == NULL || sdb_bio->f_output == NULL) {
        return 0;
    }
    if ((ret = sdb_bio->f_output(buf, len)) == 0) {
        return 0;
    }

    sprintf(errmsg, "%s:%d Output failed, ret:%d(%#x)\r\n",
            __FILE__, __LINE__, ret, -ret);
    sdb_bio_out(errmsg, strlen(errmsg));
    return ret;
}

int sdb_bio_in(char *buf, size_t buflen, size_t *outlen)
{
    int ret = 0;
    char errmsg[128];

    if (buf == NULL || buflen < 0 || outlen == NULL) {
        sprintf(errmsg, "%s:%d Param error, buf:%p, buflen:%ld, outlen:%p\r\n",
                __FILE__, __LINE__, buf, buflen, outlen);
        sdb_bio_out(errmsg, strlen(errmsg));
        return SDB_RET_PARAM_ERR;
    }
    if (sdb_bio == NULL && sdb_bio->f_input == NULL) {
        return 0;
    }
    if ((ret = sdb_bio->f_input(buf, buflen)) == 0) {
        *outlen = strlen(buf);
        return 0;
    }

    sprintf(errmsg, "%s:%d Input failed, ret:%d(%#x)\r\n",
            __FILE__, __LINE__, ret, -ret);
    sdb_bio_out(errmsg, strlen(errmsg));
    return ret;
}

int sdb_bio_conf(SDB_BIO_T *bio)
{
#warning "@TODO: Multi-thread support"
    sdb_bio = bio;
    return 0;
}

#endif /* defined(SDB_ENABLE) */

