#include <string.h>
#include <errno.h>

#include <libsdb.h>
#ifndef SDB_ENABLE
#define SDB_ENABLE
#endif
#include <libsdb.h> // reload

#if defined(SDB_MDL_SELFTEST)

#define DS_SDB_ST   SDB_IO

int sdb_color_demo(void)
{
    const char *fg[] = {
        "2",
        "30", "31", "32", "33", "34", "35", "36", "37",
        "90", "91", "92", "93", "94", "95", "96", "97",
    };
    const char *bg[] = {
        "2",
        "40", "41", "42", "43", "44", "45", "46", "47",
        "1;100", "1;101", "1;102", "1;103", "1;104", "1;105", "1;106", "1;107",
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

    return 0;
}

int sdb_selftest_output(void)
{
    int opt[] = {
        0,

        (SDB_IO | SDB_NO_COLOR | SDB_NO_MARK),
        (SDB_IO | SDB_NO_LABLE | SDB_NO_WRAP),

        (SDB_IO | SDB_NO_LINE | SDB_NO_FILE | SDB_FUNC | SDB_TIME),
        (SDB_IO | SDB_FUNC | SDB_TIME),
        (SDB_IO | SDB_NO_LINE | SDB_NO_FILE),
        (SDB_IO),
    };
    unsigned int i;
    int ret = 0;

    sdb_out_t(DS_SDB_ST, "____OUTPUT_TEST____");

    for (i = 0; i < sizeof(opt) / sizeof(int); i++) {
        sdb_out_t(DS_SDB_ST, "set opt:%#x", opt[i]);
        ret = sdb_out(opt[i], "format:(%s, %d, %#x)<\\n>\n", "123", 123, 123);
        sdb_out_i(DS_SDB_ST, "Return: %d", ret);
        ret = sdb_out_entry(opt[i]);
        sdb_out_i(DS_SDB_ST, "Return: %d", ret);
        ret = sdb_out_i(opt[i], "information");
        sdb_out_i(DS_SDB_ST, "Return: %d", ret);
        ret = sdb_out_w(opt[i], "warning");
        sdb_out_i(DS_SDB_ST, "Return: %d", ret);
        ret = sdb_out_e(opt[i], "error");
        sdb_out_i(DS_SDB_ST, "Return: %d", ret);
        ret = sdb_out_exit(opt[i]);
        sdb_out_i(DS_SDB_ST, "Return: %d", ret);
        sdb_out(opt[i], "\n");
    }
    return 0;
}

int sdb_selftest_output_stderr(void)
{
    int ret = 0;

    sdb_out_t(DS_SDB_ST, "____OUTPUT_STDERR_TEST____");

    errno = 2;
    ret = sdb_err(DS_SDB_ST, "test");
    sdb_out_i(DS_SDB_ST, "Return: %d", ret);
    ret = sdb_out(DS_SDB_ST, "\n");
    sdb_out_i(DS_SDB_ST, "Return: %d", ret);
    ret = sdb_err_i(DS_SDB_ST, "information");
    sdb_out_i(DS_SDB_ST, "Return: %d", ret);
    ret = sdb_err_w(DS_SDB_ST, "warning");
    sdb_out_i(DS_SDB_ST, "Return: %d", ret);
    ret = sdb_err_e(DS_SDB_ST, "error");
    sdb_out_i(DS_SDB_ST, "Return: %d", ret);
    ret = sdb_err_t(DS_SDB_ST, "title");
    sdb_out_i(DS_SDB_ST, "Return: %d", ret);
    sdb_out(DS_SDB_ST, "\n");
    return 0;
}

int sdb_selftest_input(void)
{
    int ret = 0;
    int num = 0;
    char buf[32];

    sdb_out_t(DS_SDB_ST, "____INPUT_TEST____");

    sdb_out_i(DS_SDB_ST, "Input number(simple)");
    if ((ret = sdb_in(DS_SDB_ST)) >= 0) {
        sdb_out_i(DS_SDB_ST, "return:%d", ret);
    }
    else {
        sdb_out_i(DS_SDB_ST, "bad input:%d", ret);
    }
    sdb_out_i(DS_SDB_ST, "Input a number");
    ret = sdb_in_n(DS_SDB_ST, &num);
    sdb_out_i(DS_SDB_ST, "return:%d, Get number:%d", ret, num);
    sdb_out_i(DS_SDB_ST, "Input string");
    ret = sdb_in_s(DS_SDB_ST, buf, &num);
    sdb_out_i(DS_SDB_ST, "return:%d, Get string:%s(size:%d)", ret, buf, num);

    ret = sdb_in_nt(DS_SDB_ST, &num, "Input a number");
    sdb_out_i(DS_SDB_ST, "return:%d, Get number:%d", ret, num);
    ret = sdb_in_st(DS_SDB_ST, buf, &num, "Input string");
    sdb_out_i(DS_SDB_ST, "return:%d, Get string:%s(size:%d)", ret, buf, num);

    return 0;
}

int sdb_selftest_dump(void)
{
    unsigned char buf1[16] = {
        0x00, 0x01, 0x1f, 0x20, 0x7e, 0x7f, 0x80, 0xff,
        0x6d, 0x4b, 0x8b, 0xd5, 0x65, 0x70, 0x63, 0x6e,     // 测试数据
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

    sdb_out_t(DS_SDB_ST, "____DUMP_TEST____");

    sdb_out_i(DS_SDB_ST, "Dump buf2:");
    ret = sdb_dmp(DS_SDB_ST, buf2, sizeof(buf2));
    sdb_out(DS_SDB_ST, "\n");
    sdb_out_i(DS_SDB_ST, "Return: %d", ret);

    sdb_out_i(DS_SDB_ST, "Dump buf1:");
    ret = sdb_dmp_hc(DS_SDB_ST, buf1, sizeof(buf1));
    sdb_out_i(DS_SDB_ST, "Return: %d", ret);

    sdb_out_i(DS_SDB_ST, "Dump buf2:");
    ret = sdb_dmp_h(DS_SDB_ST, buf2, sizeof(buf2));
    sdb_out_i(DS_SDB_ST, "Return: %d", ret);

    sdb_out_i(DS_SDB_ST, "Dump buf3:");
    ret = sdb_dmp_hca(DS_SDB_ST, buf3, sizeof(buf3), &buf3);
    sdb_out_i(DS_SDB_ST, "Return: %d", ret);

    sdb_out_i(DS_SDB_ST, "Dump buf4:");
    ret = sdb_dmp_hca(DS_SDB_ST, buf4, sizeof(buf4), (void *)0x200a3);
    sdb_out_i(DS_SDB_ST, "Return: %d", ret);

    ret = sdb_dmp_hct(DS_SDB_ST, &a, sizeof(a), "Dump structure");
    sdb_out_i(DS_SDB_ST, "Return: %d", ret);

    sdb_out(DS_SDB_ST, "\n");

    return 0;
}

int sdb_selftest(void *p)
{
    // sdb_color_demo();
    // sdb_selftest_output();
    // sdb_selftest_output_stderr();
    // sdb_selftest_input();
    sdb_selftest_dump();
    return 0;
}

#else
int sdb_selftest(void *p) { return 0; }
#endif /* defined(SDB_MDL_SELFTEST) */

int main(int argc, char *argv[])
{
    return sdb_selftest(NULL);
}

