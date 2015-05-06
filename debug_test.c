#include <stdlib.h>

#include "debug.h"

// 模块测试
int debug_test(void)
{
    dbg_out_I(1, "run");

    return 0;
}

// 模块测试入口
int __entry_debug_test__(void)
{
    debug_test();
    exit(0); 
}

