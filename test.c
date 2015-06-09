#include <stdio.h>
#include <stdlib.h> 

#include "debug.h"

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
    char buf[256] = { 0 };
    dbg_in_S(buf, sizeof(buf));
    dbg_out_I(1, "Get string: %s", buf);
    return 0;
}

int test_dbg_in_N(void *p)
{
    int num = 0;
    dbg_in_N(&num);
    dbg_out_I(1, "Get number: %d", num);
    return 0;
}

int test_output(void *p)
{
    /// 输出测试
    dbg_out(1, "格式化输出测试: %d = %#x = %c\t%s\n",
            66, 66, 66, "This is a TEST.");
    dbg_out_I(1, "带标签信息的格式化输出，显示为提示高亮");
    dbg_out_W(1, "带标签信息的格式化输出，显示为警告高亮");
    dbg_out_E(1, "带标签信息的格式化输出，显示为错误高亮");
    dbg_outerr_I(1, "带标签信息的格式化输出，附带错误信息");
    dbg_out_IT(1, "带标签信息和时间的格式化输出，显示为提示高亮");
    dbg_out_WT(1, "带标签信息和时间的格式化输出，显示为警告高亮");
    dbg_out_ET(1, "带标签信息和时间的格式化输出，显示为错误高亮");
    dbg_outerr_IT(1, "带标签信息和时间的格式化输出，附带错误信息");
    return 0;
}

int test_setlogname(void *p)
{
    
    return 0;
}

// 模块测试
int test_debug(void)
{
    dbg_test_setlist(
        { "dbg_out_*",      NULL,       test_output,                },
        { "test_getchar",   NULL,       test_getchar,               },
        { "dbg_in_S",       NULL,       test_dbg_in_S,              },
        { "dbg_in_N",       NULL,       test_dbg_in_N,              },
    );

    return 0;
}

// 模块测试入口
int __entry_test_debug__(void)
{
    test_debug();
    exit(0); 
}

