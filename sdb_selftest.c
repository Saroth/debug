
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


static int cb_out(void *p, const char *file, size_t line, const char *str)
{
    sdb_stack_touch((sdb_context *)p);
    int ret;
    if (file) {
        ret = printf("%16s:%04ld  %s\n", strrchr(file, '/')
                ? (strrchr(file, '/') + 1) : strrchr(file, '\\')
                ? (strrchr(file, '\\') + 1) : file, line, str);
    }
    else {
        ret = printf("%s", str);
    }
    fflush(stdout);

    return ret;
}
static int cb_in(void *p, char *buf, size_t size, size_t *len)
{
    sdb_stack_touch((sdb_context *)p);
    if (size == 0) {
        return 0;
    }
    size--;

    int c;
    size_t i = 0;
    while (buf && i < size) {
        if ((c = getchar()) == EOF) {
            break;
        }
        if ((buf[i] = c) == '\n') {
            break;
        }
        i++;
    }
    buf[i] = 0;
    if (len) {
        *len = i;
    }

    return i;
}


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
        __sdb_vmcout(SDB_CTX_GLOBAL, SDB_MSG_NONE, __FILE__, __LINE__, fmt, va);
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

static int test_output(void *p)
{
    int ret = 0;
    sdb_stack_mark(SDB_CTX_GLOBAL);
    ret = sdb_out_bare("bare<\\n>\n");
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

    sdb_stack_mark(SDB_CTX_GLOBAL);
    ret = sdb_out_bare("bare, format put test:%s, %d, %#x, <\\n>\n",
            "123", 123, 123);
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

    sdb_stack_mark(SDB_CTX_GLOBAL);
    ret = sdb_out("format put test:%s, %d, %#x, <\\n>\n", "123", 123, 123);
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

    sdb_stack_mark(SDB_CTX_GLOBAL);
    ret = sdb_out_info("information");
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

    sdb_stack_mark(SDB_CTX_GLOBAL);
    ret = sdb_out_warn("warning");
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

    sdb_stack_mark(SDB_CTX_GLOBAL);
    ret = sdb_out_err("error");
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

    return 0;
}

static int test_output_stderr(void *p)
{
    int ret = 0;
    sdb_stack_mark(SDB_CTX_GLOBAL);
    errno = 2;
    ret = sdb_out_bare("bare<\\n>\n");
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

    sdb_stack_mark(SDB_CTX_GLOBAL);
    errno = 3;
    ret = sdb_out_info("information");
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

    sdb_stack_mark(SDB_CTX_GLOBAL);
    errno = 4;
    ret = sdb_out_warn("warning");
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

    sdb_stack_mark(SDB_CTX_GLOBAL);
    errno = 5;
    ret = sdb_out_err("error");
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

    sdb_stack_mark(SDB_CTX_GLOBAL);
    errno = 6;
    ret = sdb_out("wrap test: %s, %s, %s, %s, %s",
            "0123456789abcdef",
            "0123456789abcdef",
            "0123456789abcdef",
            "0123456789abcdef",
            "0123456789abcdef");
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

    return 0;
}

static int test_input(void *p)
{
    int ret = 0;
    sdb_stack_mark(SDB_CTX_GLOBAL);
    sdb_out("input number(simple)");
    ret = sdb_in();
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("return:%d", ret);

    int num = 0;
    sdb_stack_mark(SDB_CTX_GLOBAL);
    sdb_out("input a number");
    ret = sdb_in_num(&num);
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("return:%d, Get number:%d", ret, num);

    char buf[32];
    size_t len;
    sdb_stack_mark(SDB_CTX_GLOBAL);
    sdb_out("input string");
    ret = sdb_in_str(buf, 32, &len);
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("return:%d, Get string:%s(length:%d)", ret, buf, len);

    sdb_stack_mark(SDB_CTX_GLOBAL);
    ret = sdb_in_num_info(&num, "input a number");
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("return:%d, Get number:%d", ret, num);

    sdb_stack_mark(SDB_CTX_GLOBAL);
    ret = sdb_in_str_info(buf, 32, &len, "input string");
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("return:%d, Get string:%s(length:%d)", ret, buf, len);

    return 0;
}

static int test_dump(void *p)
{
    int ret = 0;
    unsigned char buf1[18] = {  /* 测试数据 */
        0x00, 0x01, 0x1f, 0x20, 0x7e, 0x7f, 0x80, 0xff,
        0x6d, 0x4b, 0x8b, 0xd5, 0x65, 0x70, 0x63, 0x6e,
        0x4c, 0x30,
    };
    sdb_stack_mark(SDB_CTX_GLOBAL);
    sdb_out("Dump buf1 bare:");
    ret = sdb_dump_bare(buf1, sizeof(buf1));
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

    unsigned char buf2[38] = {
        "# 测试数据 123\n> 测试文本 abc",
    };
    sdb_stack_mark(SDB_CTX_GLOBAL);
    sdb_out("Dump buf2:");
    ret = sdb_dump(buf2, sizeof(buf2));
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

    unsigned char buf3[44] = {
        0xD4, 0xED, 0x12, 0xDB, 0x90, 0x5D, 0x0C, 0x58,
        0x01, 0xAC, 0xC3, 0x80, 0x5C, 0xCC, 0x9E, 0x9D,
        0x70, 0xA8, 0x77, 0xE7, 0x27, 0xDE, 0x21, 0x6C,
        0xC8, 0x42, 0x0D, 0x86, 0xE2, 0xE5, 0x4A, 0x47,
        0x8A, 0x2F, 0x52, 0x2C, 0x46, 0x60, 0x2E, 0x6E,
        0xE3, 0x1E, 0xCA, 0xD5,
    };
    sdb_stack_mark(SDB_CTX_GLOBAL);
    sdb_out("Dump buf3(%p):", &buf3);
    ret = sdb_dump_addr(buf3, sizeof(buf3), &buf3);
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

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
    sdb_stack_mark(SDB_CTX_GLOBAL);
    sdb_out("Dump buf4:");
    ret = sdb_dump_addr(buf4, sizeof(buf4), (void *)0x0200f0a3);
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

    struct test_a {
        char a;
        char b;
        int c;
        char d;
        long e;
        char f;
        char g;
    };
    struct test_a a = {
        1, 2, 3, 4, 5, 6, 7,
    };
    sdb_stack_mark(SDB_CTX_GLOBAL);
    ret = sdb_dump_info(&a, sizeof(a), "Dump structure");
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

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
    sdb_stack_mark(SDB_CTX_GLOBAL);
    ret = sdb_dump_info(buf5, sizeof(buf5), "Dump buf5");
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

    unsigned char buf6[3] = {
        'A', 'B', 'C',
    };
    sdb_stack_mark(SDB_CTX_GLOBAL);
    ret = sdb_dump_addr_info(buf6, sizeof(buf6), (void *)0xe,
            "Dump buf6-1, addr:0xe");
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

    sdb_stack_mark(SDB_CTX_GLOBAL);
    ret = sdb_dump_addr_info(buf6, sizeof(buf6), (void *)0x5,
            "Dump buf6-2, addr:0x5");
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

    sdb_stack_mark(SDB_CTX_GLOBAL);
    ret = sdb_dump_info("# This is a test.", 0x10000, "Dump string");
    sdb_out("Max stack: %d", sdb_stack_max_usage(SDB_CTX_GLOBAL));
    sdb_out("Return: %d", ret);

    return 0;
}

static int test_config_color(void *p)
{
    sdb_out("0: disable, 1: enable");
    int ret = sdb_in();
    if (ret < 0) {
        sdb_out("quit");
        return 0;
    }
    sdb_config_color(SDB_CTX_GLOBAL, ret == 0 ? 0 : &sdb_color_terminal);
    return 0;
}

static int test_config_mark(void *p)
{
    sdb_out("0: disable, 1: enable");
    int ret = sdb_in();
    if (ret < 0) {
        sdb_out("quit");
        return 0;
    }
    sdb_config_mark(SDB_CTX_GLOBAL, ret == 0 ? 0 : &sdb_mark_default);
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

static int sdb_selftest(void *p)
{
    sdb_config_init(SDB_CTX_GLOBAL);
    sdb_config_bio(SDB_CTX_GLOBAL, cb_out, cb_in, SDB_CTX_GLOBAL);
    sdb_config_color(SDB_CTX_GLOBAL, &sdb_color_terminal);
    sdb_config_mark(SDB_CTX_GLOBAL, &sdb_mark_default);

    // test_check_format_output(NULL);
    // test_output(NULL);
    // test_output_stderr(NULL);
    // test_input(NULL);

    return sdb_menu(SDB_MSG_MENU_LIST,
            { "---- test", 0, 0, },
            { "check format output", test_check_format_output, 0, },
            { "output", test_output, 0, },
            { "output with system error info", test_output_stderr, 0, },
            { "input", test_input, 0, },
            { "dump", test_dump, 0, },
            { "---- config", 0, 0, },
            { "color", test_config_color, 0, },
            { "mark", test_config_mark, 0, },
            { "---- other", 0, 0, },
            { "ansi color sequences", test_ansi_color_sequences, 0, },
            { "failed return (-0x5a)", test_return_failed, (void *)-0x5a, },
            { "failed return (0xa5)", test_return_failed, (void *)0xa5, },
            { "NULL", NULL, 0, },
            { NULL, test_input, 0, },
            );
}
#endif /* defined(SDB_SELFTEST) */

#if defined(MAIN_ENTRY)
int main(int argc, char *argv[])
{
#if defined(SDB_SELFTEST)
    return sdb_selftest(NULL);
#endif
    return 0;
}
#endif /* defined(MAIN_ENTRY) */

