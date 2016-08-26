#ifndef SDB_ENABLE
#define SDB_ENABLE
#endif
#include "config.h"
#include <libsdb.h>

#if defined(SDB_SELFTEST)

#define DS_SDB_ST ( SDB_IO /* | SDB_FUNC | SDB_TIME */ )

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

int sdb_selftest(void *p)
{
    sdb_out_t(DS_SDB_ST, "输出测试");
    sdb_out(DS_SDB_ST, "无标签格式化输出:(%s, %d, %#x)\n", "123", 123, 123);
    sdb_out_i(DS_SDB_ST, "带标签的格式化输出");
    sdb_out_w(DS_SDB_ST, "带标签的格式化输出，带警告标记");
    sdb_out_e(DS_SDB_ST, "带标签的格式化输出，带错误标记");
    sdb_out_entry(DS_SDB_ST);
    sdb_out_exit(DS_SDB_ST);
    // sdb_color_demo();
    return 0;
}

#else
int sdb_selftest(void *p) { return 0; }
#endif /* defined(SDB_SELFTEST) */

int __entry_sdb_selftest__(int argc, char *argv[])
{
    sdb_selftest(NULL);
    exit(0); 
}
