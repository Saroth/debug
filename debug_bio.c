#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "debug.h"

#if (DS_DEBUG_MAIN == 1)
/** 输出接口 */
int dbg_bio_out(char * buf, int len)
{
    if(buf == NULL) {
        char errmsg[128];
        sprintf(errmsg, "\r\n dbg_bio_out Param error: buf=%p\r\n", buf);
        dbg_bio_out(errmsg, strlen(errmsg));
        return DBG_RET_PARAM_ERR;
    }
    printf(buf);
    fflush(stdout);

    return len;
}

/** 输入接口 */
int dbg_bio_in(char * buf, int len)
{
    int ch = 0;
    char * b = buf;

    if(buf == NULL || len < 0) {
        char errmsg[128];
        sprintf(errmsg, "\r\n dbg_bio_in Param error: buf=%p, len=%d\r\n",
                buf, len);
        dbg_bio_out(errmsg, strlen(errmsg));
        return DBG_RET_PARAM_ERR;
    }
    while(1) {
        ch = getchar();
        if(ch == EOF) {
            break;
        }
        else {
            *b = ch;
            if(*b == '\n') {
                break;
            }
            if((++b - buf) >= len) {
                break;
            }
        }
    }

    return strlen(buf);
}

#if (DBG_USE_LOG == 1)
static FILE * gdbg_log_fp = NULL;       //!< 日志文件指针

/** 打开日志文件 */
int dbg_bio_open(char * file)
{
    int ret;
    char errmsg[128];
    if(NULL == file) {
        sprintf(errmsg, "\r\n dbg_bio_open Param error: file=%p\r\n", file);
        dbg_bio_out(errmsg, strlen(errmsg));
        return DBG_RET_PARAM_ERR;
    }
    if((ret = dbg_bio_close())) {
        return ret;
    }
    if(!(gdbg_log_fp = fopen(file, "ab+"))) {
        sprintf(errmsg, "\r\n dbg_bio_open fopen: %s(%d)\r\n",
                strerror(errno), errno);
        dbg_bio_out(errmsg, strlen(errmsg));
        gdbg_log_fp = NULL;
        return DBG_RET_OPEN_FILE_ERR;
    }

    return DBG_RET_OK;
}

/** 关闭日志文件 */
int dbg_bio_close(void)
{
    if(gdbg_log_fp) {
        if(fclose(gdbg_log_fp) && errno != EBADF) {
            char errmsg[128];
            sprintf(errmsg, "\r\n dbg_bio_close fclose: %s(%d)\r\n",
                    strerror(errno), errno);
            dbg_bio_out(errmsg, strlen(errmsg));
            return DBG_RET_CLOSE_FILE_ERR;
        }
        gdbg_log_fp = NULL;
    }

    return DBG_RET_OK;
}

/** 写日志 */
int dbg_bio_write(char * buf, int len)
{
    char errmsg[128];
    if(buf == NULL || len < 0) {
        sprintf(errmsg, "\r\n dbg_bio_write Param error: buf=%p, len=%d\r\n",
                buf, len);
        dbg_bio_out(errmsg, strlen(errmsg));
        return DBG_RET_PARAM_ERR;
    }
    if(gdbg_log_fp) {
        if(fwrite(buf, 1, len, gdbg_log_fp) != len) {
            sprintf(errmsg, "\r\n dbg_bio_write fwrite: %s(%d)\r\n",
                    strerror(errno), errno);
            dbg_bio_out(errmsg, strlen(errmsg));
            return DBG_RET_WRITE_FILE_ERR;
        }
    }

    return len;
}

/** \brief       同步数据，将缓存中的数据保存到存储设备 */
int dbg_bio_sync(void)
{
    fflush(gdbg_log_fp);                //!< 将数据从C库缓存写到内核缓存
#if defined(__unix__) || defined(__linux__)
#include <unistd.h>
    fsync(fileno(gdbg_log_fp));         //!< 将数据从内核缓存写到磁盘
#endif

    return DBG_RET_OK;
}
#endif /* (DBG_USE_LOG == 1) */
#endif /* (DS_DEBUG_MAIN == 1) */

