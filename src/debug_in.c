#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "debug.h"

#if ((DBG_INPUT_EN == 1) && (DS_DEBUG_MAIN == 1))

/** 获取标准输入 */
int dbg_stdin(char * str, int len)
{
    int ret = 0;
    char buf[DBG_BUFFER_SIZE + 4];

    memset(buf, 0x00, sizeof(buf));
    if(str == NULL || len <= 0) {
        dbg_out_E(DS_IN_ERR, "Param error.");
        return DBG_RET_PARAM_ERR;
    }
    if((ret = dbg_bio_in(buf, len))) {
        // 移除末尾换行符
        while(buf[ret - 1] == '\r' || buf[ret - 1] == '\n') {
            buf[--ret] = 0;
        }
        memmove(str, buf, ret);
    }

    return ret;
}

/** 获取输入的数值 */
int dbg_stdin_num(int * num)
{
    int len = 0;
    char str[DBG_BUFFER_SIZE + 4];

    memset(str, 0x00, sizeof(str));
    if(num == NULL) {
        dbg_out_E(DS_IN_ERR, "Param error.");
        return DBG_RET_PARAM_ERR;
    }
    len = dbg_stdin(str, sizeof(str));
    if(len <= 0) {
        dbg_out_W(DS_IN_ERR, "No input.");
        return DBG_RET_NO_INPUT;
    }
    *num = strtol(str, NULL, 0);
    if(*num == ERANGE) {
        *num = -1;
    }
    if(*num == 0 && str[0] != '0') {
        dbg_out_W(DS_IN_ERR, "Unrecognizable input.");
        return DBG_RET_UNKNOWN_INPUT;
    }

    return DBG_RET_OK;
}

/** 带调试标签的输入 */
int dbg_stdin_label(const char * file, const char * func, int line,
        int mode, void * output, int len)
{
    int ret = 0;
    int num = 0;

#if (DBG_NL_HEAD == 1)
    dbg_out(1, "%s", DBG_NL_CHAR);
#endif /* (DBG_NL_HEAD == 1) */
    switch(mode) {
        case DBG_MODE_STDIN_RETNUM: {
            dbg_stdout_label(file, func, line, DBG_LABEL_IN_RETN, "");
            ret = dbg_stdin_num(&num);
            ret = ret == 0 ? num : -1;
            dbg_stdout_label(file, func, line, DBG_LABEL_IN_ECHO, "%d", ret);
            break;
        }
        case DBG_MODE_STDIN_GETNUM: {
            dbg_stdout_label(file, func, line, DBG_LABEL_IN_GETN, "");
            ret = dbg_stdin_num((int *)output);
            dbg_stdout_label(file, func, line, DBG_LABEL_IN_ECHO,
                    "%d", *((int *)output));
            break;
        }
        case DBG_MODE_STDIN_GETSTR: {
            dbg_stdout_label(file, func, line, DBG_LABEL_IN_GETS, "");
            ret = dbg_stdin((char *)output, len);
            dbg_stdout_label(file, func, line, DBG_LABEL_IN_ECHO,
                    "%s", (char *)output);
            break;
        }
        default: {
            dbg_out_W(DS_IN_ERR, "Unknown mode.");
            return DBG_RET_PARAM_ERR;
        }
    }
#if !(DBG_NL_HEAD == 1)
    dbg_out(1, "%s", DBG_NL_CHAR);
#endif /* !(DBG_NL_HEAD == 1) */

    return ret;
}

#endif /* ((DBG_INPUT_EN == 1) && (DS_DEBUG_MAIN == 1)) */

