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
    int i;

    sdb_out_t(DS_SDB_ST, "____OUTPUT_TEST____");
    for (i = 0; i < sizeof(opt) / sizeof(int); i++) {
        sdb_out_t(DS_SDB_ST, "set opt:%#x", opt[i]);
        sdb_out(opt[i], "format:(%s, %d, %#x)<\\n>\n", "123", 123, 123);
        sdb_out_entry(opt[i]);
        sdb_out_i(opt[i], "information");
        sdb_out_w(opt[i], "warning");
        sdb_out_e(opt[i], "error");
        sdb_out_exit(opt[i]);
        sdb_out(opt[i], "\n");
    }
    return 0;
}

int sdb_selftest_output_stderr(void)
{
    sdb_out_t(DS_SDB_ST, "____OUTPUT_STDERR_TEST____");
    errno = 2;
    sdb_err(DS_SDB_ST, "test");
    sdb_out(DS_SDB_ST, "\n");
    sdb_err_i(DS_SDB_ST, "information");
    sdb_err_w(DS_SDB_ST, "warning");
    sdb_err_e(DS_SDB_ST, "error");
    sdb_err_t(DS_SDB_ST, "title");
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

int __entry_sdb_selftest__(int argc, char *argv[])
{
    sdb_selftest(NULL);
    exit(0); 
}

