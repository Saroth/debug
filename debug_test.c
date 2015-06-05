#include <stdlib.h>

#include "debug.h"

int debug_highlight_test(void)
{
    dbg_out_I(1, "Information");
    dbg_out_W(1, "Warning");
    dbg_out_E(1, "Error");
    dbg_out_H(1, "Highlight");
    dbg_outerr_I(1, "STDERR");

    return 0;
}

int debug_log_test(void)
{
    return 0;
}

// 模块测试
int debug_test(void)
{
    int i = 0;

    debug_highlight_test();

    return 0;
}

// 模块测试入口
int __entry_debug_test__(void)
{
    debug_test();
    exit(0); 
}

