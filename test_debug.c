#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "debug.h"

#if (DBG_MODULE_TEST == 1)

#define DS_DEBUG_MODULE (DBG_INFO | DBG_LABEL_FUNC | DBG_LABEL_LINE)

int test_output(void *p)
{
    /// 输出测试
    int debug_switch = 1;
    int i = 0;

    for(i = 0; i <= 4; i++) {
        debug_switch |= (1 << i);
        dbg_out_entry(debug_switch);
        dbg_out(debug_switch, "\n格式化输出测试: %d = %#x = %c\t%s\n",
                66, 66, 66, "This is a TEST.");
        dbg_out_I(debug_switch, "带提示标签的格式化输出，标签为提示高亮");
        dbg_out_W(debug_switch, "带警告标签的格式化输出，标签为警告高亮");
        dbg_out_E(debug_switch, "带错误标签的格式化输出，标签为错误高亮");
        dbg_out_H(debug_switch, "高亮测试");
        dbg_outerr_I(debug_switch,
                "带错误标签的格式化输出，标签为错误高亮，附带错误信息");
        dbg_out_exit(debug_switch);
    }

    return 0;
}

int test_getchar(void * p)
{
    char ch = 0;
    int breakflag = 0;

    while(1) {
        ch = getchar();
        printf("Get character: %d - %x - %c\n", ch, ch, ch);
        if(ch == 'q') {
            if(breakflag) {
                getchar();
                break;
            }
            breakflag = 1;
        }
    }

    return 0;
}

int test_dbg_in_S(void *p)
{
    char buf[256];
    memset(buf, 0x00, sizeof(buf));
    dbg_in_S(buf, sizeof(buf));
    dbg_out_I(DS_DEBUG_MODULE, "Get string: %s", buf);
    return 0;
}

int test_dbg_in_N(void *p)
{
    int num = 0;
    dbg_in_N(&num);
    dbg_out_I(DS_DEBUG_MODULE, "Get number: %d", num);
    return num;
}

int test_setlogname(void *p)
{
    dbg_log_setname("log_test.txt");
    return 0;
}

int test_setlog_off(void *p)
{
    dbg_log_off();
    return 0;
}

int test_setlog_on(void *p)
{
    dbg_log_on();
    return 0;
}

int test_setlog_on_s(void *p)
{
    dbg_log_on_s();
    return 0;
}

int test_setlog_only(void *p)
{
    dbg_log_only();
    return 0;
}

int test_setlog_only_s(void *p)
{
    dbg_log_only_s();
    return 0;
}

int test_dump(void *p)
{
    char buf[1024];
    int debug_switch = 1;
    int i = 0;

    for(i = 0; i <= 4; i++) {
        debug_switch |= (1 << i);
        strcpy(buf, "#### This is a TEST.\r\n ####");
        dbg_out_I(debug_switch, "buf address: %p", buf);
        dbg_dmp_H(debug_switch, buf, strlen(buf));
        dbg_dmp_HC(debug_switch, buf, strlen(buf));
        dbg_dmp_HCA(debug_switch, buf, strlen(buf), buf);
        dbg_dmp_HL(debug_switch, buf, strlen(buf), "test1");
        dbg_dmp_HCL(debug_switch, buf, strlen(buf), "test2");
        dbg_dmp_HCAL(debug_switch, buf, strlen(buf), buf, "test3");
        dbg_dmp_HL(debug_switch, buf, strlen(buf), NULL);
        dbg_dmp_HCL(debug_switch, buf, strlen(buf), NULL);
        dbg_dmp_HCAL(debug_switch, buf, strlen(buf), buf, NULL);
        dbg_dmp_C(debug_switch, buf, strlen(buf));
    }

    return 0;
}


// 模块测试
int test_debug(void *p)
{
    dbg_test_setlist(
        { "dbg_out_*",          NULL,   test_output,            },
        { "test_getchar",       NULL,   test_getchar,           },
        { "dbg_in_S",           NULL,   test_dbg_in_S,          },
        { "dbg_in_N",           NULL,   test_dbg_in_N,          },
        { "dbg_log_setname",    NULL,   test_setlogname,        },
        { "dbg_log_off",        NULL,   test_setlog_off,        },
        { "dbg_log_on",         NULL,   test_setlog_on,         },
        { "dbg_log_on_s",       NULL,   test_setlog_on_s,       },
        { "dbg_log_only",       NULL,   test_setlog_only,       },
        { "dbg_log_only_s",     NULL,   test_setlog_only_s,     },
        { "dbg_dmp_*",          NULL,   test_dump,              },
    );

    return 0;
}
#else
int test_debug(void *p) { return 0; }
#endif /* (DBG_MODULE_TEST == 1) */

// 模块测试入口
int __entry_test_debug__(void)
{
    test_debug(NULL);
    exit(0); 
}

