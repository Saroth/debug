#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 获取标准输入 */
int dbg_stdin(char * str, int len)
{
    int ret = 0;
    char buf[BUFFER_SIZE + 4];

    memset(buf, 0x00, sizeof(buf));
    if(str == NULL || len <= 0) {
        dbg_out_E(DS_IN_ERR, "param error.");
        return -1;
    }
    ret = dbg_bio_in(buf, len);
    if(ret == 0) {
        return 0;
    }
    // 移除末尾换行符
    while(buf[ret - 1] == '\r' || buf[ret - 1] == '\n') {
        buf[--ret] = 0;
    }
    memmove(str, buf, ret);

    return ret;
}

/** 获取输入的数值 */
int dbg_stdin_num(int * num)
{
    int len = 0;
    char str[BUFFER_SIZE + 4];

    memset(str, 0x00, sizeof(str));
    if(num == NULL) {
        dbg_out_E(DS_IN_ERR, "param error.");
        return -1;
    }
    len = dbg_stdin(str, sizeof(str));
    if(len <= 0) {
        dbg_out_W(DS_IN_ERR, "No input.");
        return -1;
    }
    *num = strtol(str, NULL, 0);
    if(*num == ERANGE) {
        *num = -1;
    }
    if(*num == 0 && str[0] != '0') {
        dbg_out_W(DS_IN_ERR, "Unrecognizable input.");
        return -1;
    }

    return 0;
}

/** 带调试标签的输入 */
int dbg_stdin_label(const char * func, int line, int mode,
        void * output, int len)
{
    int ret = 0;
    int num = 0;

#ifdef DBG_NL_HEAD
    dbg_out(1, "%s", DBG_NL_CHAR);
#endif /* DBG_NL_HEAD */
    switch(mode) {
        case DBG_STDIN_RETNUM: {
            dbg_stdout_label(__func__, __LINE__,
                    DBG_LABEL_COLOR | DBG_LABEL_COL_INPUT | DBG_LABEL_IN_RETN,
                    "");
            ret = dbg_stdin_num(&num);
            ret = ret == 0 ? num : -1;
            dbg_stdout_label(__func__, __LINE__,
                    DBG_LABEL_COLOR | DBG_LABEL_COL_INPUT | DBG_LABEL_OUT_RETN
                    | DBG_LABEL_NEWLINE,
                    "%d", ret);
            break;
        }
        case DBG_STDIN_GETNUM: {
            dbg_stdout_label(__func__, __LINE__,
                    DBG_LABEL_COLOR | DBG_LABEL_COL_INPUT | DBG_LABEL_IN_GETN,
                    "");
            ret = dbg_stdin_num((int *)output);
            dbg_stdout_label(__func__, __LINE__,
                    DBG_LABEL_COLOR | DBG_LABEL_COL_INPUT | DBG_LABEL_OUT_GETN
                    | DBG_LABEL_NEWLINE,
                    "%d", *((int *)output));
            break;
        }
        case DBG_STDIN_GETSTR: {
            dbg_stdout_label(__func__, __LINE__,
                    DBG_LABEL_COLOR | DBG_LABEL_COL_INPUT | DBG_LABEL_IN_GETS,
                    "");
            ret = dbg_stdin((char *)output, len);
            dbg_stdout_label(__func__, __LINE__,
                    DBG_LABEL_COLOR | DBG_LABEL_COL_INPUT | DBG_LABEL_OUT_GETS
                    | DBG_LABEL_NEWLINE,
                    "%s", (char *)output);
            break;
        }
        default: {
            dbg_out_W(DS_IN_ERR, "Unknown mode.");
            return -1;
        }
    }
#ifndef DBG_NL_HEAD
    dbg_out(1, "%s", DBG_NL_CHAR);
#endif /* DBG_NL_HEAD */

    return ret;
}

#ifdef __cplusplus
}
#endif

