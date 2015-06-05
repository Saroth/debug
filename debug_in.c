#include "debug.h"
#include "debug_bio.h"
#include "debug_in.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 获取输入的字符串 */
int dbg_in_str(char * str, int len)
{
    char buf[BUFFER_SIZE + 4] = { 0 };
    return 0;
}

/** 获取输入的数值 */
int dbg_in_num(int * num)
{
    return 0;
}

#ifdef __cplusplus
}
#endif

