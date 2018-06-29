
#if defined(SDB_SELFTEST)
#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifndef SDB_ENABLE
#define SDB_ENABLE
#endif
#include <libsdb.h>
sdb_context sdb_ctx;
#ifdef SDB_CTX_GLOBAL
#undef SDB_CTX_GLOBAL
#endif
#define SDB_CTX_GLOBAL (&sdb_ctx)


static int test_format_output_compare(const char *fmt, ...)
{
    va_list va;

    char buf_std[2048];
    va_start(va, fmt);
    int len_std = vsnprintf(buf_std, sizeof(buf_std), fmt, va);
    va_end(va);
    if (len_std < 0) {
        int err = errno;
        printf("vsnprintf error, return:%d.\n", len_std);
        printf("stderr: (%d)%s.\n", err, strerror(err));
        return len_std;
    }
    printf("std:[%s]\n", buf_std);

    char buf_vx[2048];
    va_start(va, fmt);
    int len_vx = sdb_vsnprintf(buf_vx, sizeof(buf_vx), fmt, va);
    va_end(va);
    if (len_vx < 0) {
        int err = errno;
        printf("sdb_vsnprintf error, return:%d.\n", len_vx);
        printf("stderr: (%d)%s.\n", err, strerror(err));
        return len_vx;
    }

    if (len_std != len_vx) {
        printf("output length differ! vx:%d != std:%d\n", len_vx, len_std);
        len_vx = len_vx > len_std ? len_vx : len_std;
    }
    if (memcmp(buf_vx, buf_std, len_vx)) {
        int i;
        char ch = 0;
        for (i = 0; i < len_vx; i++) {
            if ((ch = buf_vx[i]) != buf_std[i]) {
                break;
            }
        }
        printf("vx: [%s]\n", buf_vx);

        memset(buf_vx, ' ', sizeof(buf_vx));
        buf_vx[i] = '^';
        snprintf(&buf_vx[i + 1], sizeof(buf_vx) - i - 1,
                " (%d: %#x!=%#x)", i, ch, buf_std[i]);
        printf("diff:%s\n", buf_vx);
    }
    else {
        va_start(va, fmt);
        __sdb_vmcout(&sdb_ctx, SDB_MSG_NONE, __FILE__, __LINE__, fmt, va);
        va_end(va);
        printf("==\n");
    }

    return 0;
}

static int test_check_format_output(void *p)
{
    test_format_output_compare("0 d:\t%%,%d,%d,%d,%d,%d,%d,%d,%d",
            0, 8, 32, 128, 80000, 0x7fffffff, -1, -0x123);
    test_format_output_compare("1 i:\t%i,%i,%i,%i,%i,%i,%i,%i",
            0, 8, 32, 128, 80000, 0x7fffffff, -1, -0x123);
    test_format_output_compare("2 x:\t%x,%x,%x,%x,%x,%x,%x,%x,%x",
            0, 7, 8, 15, 0x00123abc, 0x7fffffff, 0xffffffff, -1, -0x123);
    test_format_output_compare("3 X:\t%X,%X,%X,%X,%X,%X,%X,%X,%X",
            0, 7, 8, 15, 0x00123abc, 0x7fffffff, 0xffffffff, -1, -0x123);
    test_format_output_compare("4 x:\t%lx,%lx,%lx,%lx,%lx,%lx,%lx,%lx,%lx",
            0, 7, 8, 15, 0x00123abc, 0x7fffffff, 0xffffffff, -1, -0x123);
    test_format_output_compare("5 o:\t%o,%o,%o,%o,%o,%o,%o,%o,%o",
            0, 7, 8, 15, 0x00123abc, 0x7fffffff, 0xffffffff, -1, -0x123);
    test_format_output_compare("6 c:\t%c,%c,%c,%c,%c,%c,%c,%c,%c,%c",
            1, ' ', 'S', '\b', 124, 0xf6, "123", -1, -0x20, 0x55);
    test_format_output_compare(
            "7 ?d:\t%08o,%-08d,%+8d,%04i,%08d,%8d,%+u,%+-8d,",
            123, 123, 123, 123, -123, -123, 123, 123);
    test_format_output_compare("8 #:\t%#d,%#x,%p,%#o,%#X,%#i",
            123, 123, 123, 123, 123, 123);
    test_format_output_compare(
            "9 #0?:\t%#02d,%#08x,%08p,%16p,%#08o,%#08X,%#08i",
            123, 123, 123, 123, 123, 123, 123);
    test_format_output_compare("a s:\t%s,%+s,%08s,%-08s,%s,%8s,",
            "test1",
            "test2",
            "test3",
            "test4",
            NULL, NULL);
    test_format_output_compare("b ?:\t%a,%b,%O,%e,%#x",
            0x5f5f);
    test_format_output_compare("c f:\t%f,%f,%f,%#x,%f",
            3.1415, 2.16, 1.234, 0x5a5a, 3.8);

    return 0;
}

static int test_ansi_color_sequences(void *p)
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

static int test_return_failed(void *p)
{
    size_t ret = (size_t)p;
    return (int)ret;
}

#if 0

static int sdb_selftest_put(void *p)
{
    int ret = 0;

    sdb_set_stack();
    ((void) ret);
    ret = SDB_OUT("bare<\\n>\n");
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("Return: %d", ret);

    sdb_set_stack();
    ((void) ret);
    ret = SDB_OUT("bare, format put test:%s, %d, %#x, <\\n>\n",
            "123", 123, 123);
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("Return: %d", ret);

    sdb_set_stack();
    ret = SDB_OUT_I("information");
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("Return: %d", ret);

    sdb_set_stack();
    ret = SDB_OUT_W("warning");
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("Return: %d", ret);

    sdb_set_stack();
    ret = SDB_OUT_E("error");
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("Return: %d", ret);
    SDB_OUT("\n");

    return 0;
}

static int sdb_selftest_put_stderr(void *p)
{
    int ret = 0;

    sdb_set_stack();
    ((void) ret);
    errno = 2;
    ret = SDB_OUT("bare<\\n>\n");
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("Return: %d (errno:%d)", ret, errno);

    sdb_set_stack();
    errno = 3;
    ret = SDB_OUT_I("information");
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("Return: %d (errno:%d)", ret, errno);

    sdb_set_stack();
    errno = 4;
    ret = SDB_OUT_W("warning");
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("Return: %d (errno:%d)", ret, errno);

    sdb_set_stack();
    errno = 5;
    ret = SDB_OUT_E("error");
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("Return: %d (errno:%d)", ret, errno);
    SDB_OUT("\n");

    return 0;
}

static int sdb_selftest_get(void *p)
{
    int ret = 0;
    int num = 0;
    char buf[32];

    sdb_set_stack();
    ((void) buf);
    ((void) num);
    ((void) ret);
    SDB_OUT_I("get number(simple)");
    ret = SDB_IN_N(NULL);
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("return:%d", ret);

    sdb_set_stack();
    SDB_OUT_I("get a number");
    ret = SDB_IN_N(&num);
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("return:%d, Get number:%d", ret, num);

    sdb_set_stack();
    SDB_OUT_I("get string");
    ret = SDB_IN_S(buf, 32, &num);
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("return:%d, Get string:%s(size:%d)", ret, buf, num);

    sdb_set_stack();
    ret = SDB_IN_NI(&num, "get a number");
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("return:%d, Get number:%d", ret, num);

    sdb_set_stack();
    ret = SDB_IN_SI(buf, 32, &num, "get string");
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("return:%d, Get string:%s(size:%d)", ret, buf, num);

    SDB_OUT("\n");

    return 0;
}

static int sdb_selftest_dump(void *p)
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
    unsigned char buf4[64] = {
        0xAC, 0x8B, 0x40, 0x2C, 0x9C, 0xC6, 0x56, 0xC2,
        0x49, 0x02, 0x94, 0xF7, 0x4C, 0x0C, 0x19, 0x63,

        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    unsigned char buf5[368] = {
        0x42, 0x79, 0xA3, 0xDB, 0xD1, 0x03, 0xCD, 0x4B,
        0x32, 0xE4, 0x90, 0x06, 0x0C, 0xFC, 0xE4, 0xCD,

        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

        0xAC, 0x8B, 0x40, 0x2C, 0x9C, 0xC6, 0x56, 0xC2,
        0x49, 0x02, 0x94, 0xF7, 0x4C, 0x0C, 0x19, 0x63,

        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

        0x00, 0x30, 0xB1, 0xCC, 0xD7, 0xB2, 0xAD, 0xDB,
        0xE2, 0x2C, 0x4D, 0x26, 0x25, 0x01, 0x21, 0xD8,

        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,

        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

        0xF2, 0x8D, 0x83, 0xD4, 0xAD, 0x39, 0x5A, 0xFE,
        0x28, 0xE1, 0x53, 0x53, 0x17, 0x99, 0x8A, 0xC2,
        0x17, 0x69, 0xA2, 0x44, 0x84, 0x37, 0x27, 0x58,

        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    unsigned char buf6[3] = {
        'A', 'A', 'A',
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

    sdb_set_stack();
    memset(&a, 0x00, sizeof(a));
    a.a = 1;
    a.b = 2;
    a.c = 3;
    a.d = 4;
    a.e = 5;
    a.f = 6;
    a.g = 7;

    sdb_set_stack();
    SDB_OUT_I("Dump buf1:");
    ret = SDB_DMP_C(buf1, sizeof(buf1));
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("Return: %d", ret);

    sdb_set_stack();
    SDB_OUT_I("Dump buf2:");
    ret = SDB_DMP(buf2, sizeof(buf2));
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("Return: %d", ret);

    sdb_set_stack();
    SDB_OUT_I("Dump buf3:");
    ret = SDB_DMP_CA(buf3, sizeof(buf3), (unsigned long)&buf3);
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("Return: %d", ret);

    sdb_set_stack();
    SDB_OUT_I("Dump buf4:");
    ret = SDB_DMP_CA(buf4, sizeof(buf4), 0x0200f0a3);
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("Return: %d", ret);

    sdb_set_stack();
    ret = SDB_DMP_CI(&a, sizeof(a), "Dump structure");
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("Return: %d", ret);

    sdb_set_stack();
    ret = SDB_DMP_CAI(buf5, sizeof(buf5), 0, "Dump buf5");
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("Return: %d", ret);

    sdb_set_stack();
    ret = SDB_DMP_CAI(buf6, sizeof(buf6), 0xe, "Dump buf6-1");
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("Return: %d", ret);

    sdb_set_stack();
    ret = SDB_DMP_CAI(buf6, sizeof(buf6), 0x5, "Dump buf6-2");
    SDB_OUT_I("Max stack: %d", sdb_get_stack_max());
    SDB_OUT_I("Return: %d", ret);

    /* sdb_set_stack(); */
    /* ret = SDB_DMP_I("# This is a test.", 0x10000, "Dump string"); */
    /* SDB_OUT_I("Max stack: %d", sdb_get_stack_max()); */
    /* SDB_OUT_I("Return: %d", ret); */

    SDB_OUT("\n");

    return 0;
}

#endif /* 0 */

static int sdb_selftest(void *p)
{
    // test_check_format_output(NULL);
    // test_ansi_color_sequences(NULL);
    // sdb_selftest_put(NULL);
    // sdb_selftest_put_stderr(NULL);
    // sdb_selftest_get(NULL);
    // sdb_selftest_dump(NULL);

    return sdb_menu(SDB_MENU_LIST,
            { "---- standard check", 0, 0, },
            { "check format output", test_check_format_output, 0, },
            // { "put", sdb_selftest_put, 0, },
            // { "put err", 0, sdb_selftest_put_stderr, 0, },
            // { "get", sdb_selftest_get, 0, },
            // { "dump", sdb_selftest_dump, 0, },
            { "---- other", 0, 0, },
            { "ansi color sequences", test_ansi_color_sequences, 0, },
            { "failed return (-0x5a)", test_return_failed, (void *)-0x5a, },
            { "failed return (0xa5)", test_return_failed, (void *)0xa5, },
            // { NULL, sdb_selftest_put, 0, },
            // { "NULL", NULL, 0, },
            );
}
#endif /* defined(SDB_SELFTEST) */

#if defined(MAIN_ENTRY)
int main(int argc, char *argv[])
{
    sdb_config_init(&sdb_ctx);
#if defined(SDB_SELFTEST)
    return sdb_selftest(NULL);
#endif
    return 0;
}
#endif /* defined(MAIN_ENTRY) */

