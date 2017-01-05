#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifndef SDB_ENABLE
#define SDB_ENABLE
#endif
#include <libsdb.h>
#define SDB_SET_CONFIG (&sdb_cfg_std)

#if defined(SDB_MDL_SELFTEST)

#include <stdarg.h>
typedef int (* put_t)(void *p, const char *buf, unsigned int len);
extern int vxprint(void *ptr, put_t put, const char *fmt, va_list va);

int _put(void *p, const char *buf, unsigned int len)
{
    printf("[%04d]  ", sdb_get_stack());
    char b[64];
    if (len == 0)
        while (buf[len++]);
    if (len > 60)
        len = 60;
    memmove(b, buf, len);
    b[len] = 0;
    printf("%s", b);
    fflush(stdout);

    return 0;
}

int _printf(const char *fmt, ...)
{
    va_list va;

    va_start(va, fmt);
    vxprint(NULL, _put, fmt, va);
    va_end(va);

    return 0;
}

int sdb_selftest_printf(void *p)
{
    typedef int (*print_t)(const char *, ...);
    print_t pp[2] = {
        printf,
        _printf,
    };
    unsigned int i;

    printf("[[%04d]]  \n", sdb_get_stack());
    for (i = 0; i < 2; i++) {
        pp[i]("----\n");
        pp[i](
                /* " 0 %%d:\t %%, %d, %d, %d, %d, %d, %d, %d, %d \n" */
                /* " 1 %%i:\t %i, %i, %i, %i, %i, %i, %i, %i \n" */
                /* " 2 %%x:\t %x, %x, %x, %x, %x, %x, %x, %x, %x \n" */
                /* " 3 %%X:\t %X, %X, %X, %X, %X, %X, %X, %X, %X \n" */
                /* " 4 %%x:\t %lx, %lx, %lx, %lx, %lx, %lx, %lx, %lx, %lx \n" */
                /* " 5 %%o:\t %o, %o, %o, %o, %o, %o, %o, %o, %o \n" */
                /* " 6 %%c:\t %c, %c, %c, %c, %c, %c, %c, %c, %c \n" */
                /* " 7 %%?d:\t %08o, %-08d, %+8d, %04i, %08d, %8d, %+u, %+-8d, \n" */
                /* " 8 %%?d:\t %#d, %#x, %p, %#o, %#X, %#i \n" */
                /* " 9 %%?d:\t %#02d, %#08x, %08p, %#08o, %#08X, %#08i \n" */
                /* "10 %%s:\t %s, %+s, %08s, %-08s, \n" */
                /* "11 %%?:\t %a, %b, %O, %e, %#x \n" */
                /* "12 %%f:\t %f, %f, %f, %#x, %f\n" */
                "%s",
                /* 0, 8, 32, 128, 80000, 0x7fffffff, -1, -0x123, */
                /* 0, 8, 32, 128, 80000, 0x7fffffff, -1, -0x123, */
                /* 0, 7, 8, 15, 0x00123abc, 0x7fffffff, 0xffffffff, -1, -0x123, */
                /* 0, 7, 8, 15, 0x00123abc, 0x7fffffff, 0xffffffff, -1, -0x123, */
                /* 0, 7, 8, 15, 0x00123abc, 0x7fffffff, 0xffffffff, -1, -0x123, */
                /* 0, 7, 8, 15, 0x00123abc, 0x7fffffff, 0xffffffff, -1, -0x123, */
                /* 0, ' ', 'S', '\b', 124, 0xf6, "123", -1, -0x20, */
                /* 123, 123, 123, 123, -123, -123, 123, 123, */
                /* 123, 123, 123, 123, 123, 123, */
                /* 123, 123, 123, 123, 123, 123, */
                /* "test1", "test2", "test3", "test4", */
                /* 0x5f5f, */
                /* 3.1415, 2.16, 1.234, 0x5a5a, 3.8, */
                "[end]\n\n");
    }

    return 0;
}

int sdb_color_demo(void *p)
{
    const char *fg[] = {
        "2",
        "30", "31", "32", "33", "34", "35", "36", "37",
        "90", "91", "92", "93", "94", "95", "96", "97",
    };
    const char *bg[] = {
        "2",
        "40", "41", "42", "43", "44", "45", "46", "47",
        "100", "101", "102", "103", "104", "105", "106", "107",
    };
    int i;
    int j;

    printf("Terminal color table:\n");
    for (i = -1; i < 17; i++) {
        if (i < 0)
            printf("fg\\bg");
        else
            printf("  %3s", fg[i]);
        for (j = 0; j < 17; j++) {
            if (i < 0)
                printf(" %3s", bg[j]);
            else
                printf("\33[0m \33[%s;%sm[x]", fg[i], bg[j]);
        }
        printf("\33[0m\n");
    }
    for (i = -1; i < 17; i++) {
        if (i < 0)
            printf(" bold");
        else
            printf("  %3s", fg[i]);
        for (j = 0; j < 17; j++) {
            if (i < 0)
                printf(" %3s", bg[j]);
            else
                printf("\33[0m \33[%s;%s;1m[x]", fg[i], bg[j]);
        }
        printf("\33[0m\n");
    }
    printf("\n");

    return 0;
}

int sdb_selftest_put(void *p)
{
    int ret = 0;

    printf("[[%04d]]  \n", sdb_get_stack());
    ((void) ret);
    ret = SDB_OUT("bare, format put test:%s, %d, %#x, <\\n>\n",
            "123", 123, 123);
    SDB_OUT_I("Return: %d", ret);
    ret = SDB_OUT_I("information");
    SDB_OUT_I("Return: %d", ret);
    ret = SDB_OUT_W("warning");
    SDB_OUT_I("Return: %d", ret);
    ret = SDB_OUT_E("error");
    SDB_OUT_I("Return: %d", ret);
    SDB_OUT("\n");

    return 0;
}

int sdb_selftest_put_stderr(void *p)
{
    int ret = 0;

    ((void) ret);
    errno = 2;
    ret = SDB_OUT("bare<\\n>\n");
    SDB_OUT_I("Return: %d (errno:%d)", ret, errno);
    errno = 3;
    ret = SDB_OUT_I("information");
    SDB_OUT_I("Return: %d (errno:%d)", ret, errno);
    errno = 4;
    ret = SDB_OUT_W("warning");
    SDB_OUT_I("Return: %d (errno:%d)", ret, errno);
    errno = 5;
    ret = SDB_OUT_E("error");
    SDB_OUT_I("Return: %d (errno:%d)", ret, errno);
    SDB_OUT("\n");

    return 0;
}

int sdb_selftest_get(void *p)
{
    int ret = 0;
    int num = 0;
    char buf[32];

    ((void) buf);
    ((void) num);
    ((void) ret);
    SDB_OUT_I("get number(simple)");
    ret = SDB_IN_N(NULL);
    SDB_OUT_I("return:%d", ret);
    SDB_OUT_I("get a number");
    ret = SDB_IN_N(&num);
    SDB_OUT_I("return:%d, Get number:%d", ret, num);
    SDB_OUT_I("get string");
    ret = SDB_IN_S(buf, 32, &num);
    SDB_OUT_I("return:%d, Get string:%s(size:%d)", ret, buf, num);

    ret = SDB_IN_NI(&num, "get a number");
    SDB_OUT_I("return:%d, Get number:%d", ret, num);
    ret = SDB_IN_SI(buf, 32, &num, "get string");
    SDB_OUT_I("return:%d, Get string:%s(size:%d)", ret, buf, num);
    SDB_OUT("\n");

    return 0;
}

#if 0
int sdb_selftest_dump(void *p)
{
    unsigned char buf1[16] = {
        0x00, 0x01, 0x1f, 0x20, 0x7e, 0x7f, 0x80, 0xff,
        0x6d, 0x4b, 0x8b, 0xd5, 0x65, 0x70, 0x63, 0x6e,     /* 测试数据 */
    };
    unsigned char buf2[38] = {
        "# 测试数据 123\n> 测试文本 abc",
    };
    unsigned char buf3[44] = {
        0xD4, 0xED, 0x12, 0xDB, 0x90, 0x5D, 0x0C, 0x58,
        0x01, 0xAC, 0xC3, 0x80, 0x5C, 0xCC, 0x9E, 0x9D,
        0x70, 0xA8, 0x77, 0xE7, 0x27, 0xDE, 0x21, 0x6C,
        0xC8, 0x42, 0x0D, 0x86, 0xE2, 0xE5, 0x4A, 0x47,
        0x8A, 0x2F, 0x52, 0x2C, 0x46, 0x60, 0x2E, 0x6E,
        0xE3, 0x1E, 0xCA, 0xD5,
    };
    unsigned char buf4[16] = {
        0xAC, 0x8B, 0x40, 0x2C, 0x9C, 0xC6, 0x56, 0xC2,
        0x49, 0x02, 0x94, 0xF7, 0x4C, 0x0C, 0x19, 0x63,
    };
    unsigned char buf5[68] = {
        0x42, 0x79, 0xA3, 0xDB, 0xD1, 0x03, 0xCD, 0x4B,
        0x32, 0xE4, 0x90, 0x06, 0x0C, 0xFC, 0xE4, 0xCD,
        0xAC, 0x8B, 0x40, 0x2C, 0x9C, 0xC6, 0x56, 0xC2,
        0x49, 0x02, 0x94, 0xF7, 0x4C, 0x0C, 0x19, 0x63,
        0xC4, 0x30, 0xB1, 0xCC, 0xD7, 0xB2, 0xAD, 0xDB,
        0xE2, 0x2C, 0x4D, 0x26, 0x25, 0x01, 0x21, 0xD8,
        0xF2, 0x8D, 0x83, 0xD4, 0xAD, 0x39, 0x5A, 0xFE,
        0x28, 0xE1, 0x53, 0x53, 0x17, 0x99, 0x8A, 0xC2,
        0x17, 0x69, 0xA2, 0x44,
    };
    struct test_a {
        char a;
        char b;
        int c;
        char d;
        long e;
        char f;
        char g;
    };
    struct test_a a;
    int ret = 0;

    memset(&a, 0x00, sizeof(a));
    a.a = 1;
    a.b = 2;
    a.c = 3;
    a.d = 4;
    a.e = 5;
    a.f = 6;
    a.g = 7;

    SDB_OUT_I("Dump buf1:");
    ret = SDB_DMP_C(buf1, sizeof(buf1));
    SDB_OUT_I("Return: %d", ret);

    SDB_OUT_I("Dump buf2:");
    ret = SDB_DMP(buf2, sizeof(buf2));
    SDB_OUT_I("Return: %d", ret);

    SDB_OUT_I("Dump buf3:");
    ret = SDB_DMP_CA(buf3, sizeof(buf3), &buf3);
    SDB_OUT_I("Return: %d", ret);

    SDB_OUT_I("Dump buf4:");
    ret = SDB_DMP_CA(buf4, sizeof(buf4), (void *)0x0200f0a3);
    SDB_OUT_I("Return: %d", ret);

    ret = SDB_DMP_CI(&a, sizeof(a), "Dump structure");
    SDB_OUT_I("Return: %d", ret);

    ret = SDB_DMP_CAI(buf5, sizeof(buf5), 0, "Dump buf5");
    SDB_OUT_I("Return: %d", ret);

    ret = SDB_DMP_I("# This is a test.", 0x10000, "Dump string");
    SDB_OUT_I("Return: %d", ret);

    SDB_OUT("\n");

    return 0;
}
#endif

int sdb_selftest(void *p)
{
    sdb_set_stack();
    sdb_selftest_printf(NULL);
    printf("Max stack: %d\n", sdb_get_stack_max());

    /* sdb_color_demo(NULL); */
    sdb_set_stack();
    sdb_selftest_put(NULL);
    printf("Max stack: %d\n", sdb_get_stack_max());

    sdb_set_stack();
    sdb_selftest_put_stderr(NULL);
    printf("Max stack: %d\n", sdb_get_stack_max());

    sdb_set_stack();
    sdb_selftest_get(NULL);
    printf("Max stack: %d\n", sdb_get_stack_max());

    sdb_set_stack();
    /* sdb_selftest_dump(NULL); */
    printf("Max stack: %d\n", sdb_get_stack_max());

    /* SDB_MENU( */
            /* { "color demo", 0, sdb_color_demo, }, */
            /* { "put", 0, sdb_selftest_put, }, */
            /* { "put err", 0, sdb_selftest_put_stderr, }, */
            /* { "get", 0, sdb_selftest_get, }, */
            /* { "dump", 0, sdb_selftest_dump, }, */
            /* { NULL, 0, sdb_selftest_put, }, */
            /* { "NULL", 0, NULL, }, */
            /* ); */

    return 0;
}

#else
int sdb_selftest(void *p) { return 0; }
#endif /* defined(SDB_MDL_SELFTEST) */

int main(int argc, char *argv[])
{
    return sdb_selftest(NULL);
}

