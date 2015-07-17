#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 输出接口 */
int dbg_bio_out(char * buf, int len)
{
    if(buf == NULL) {
        char errmsg[256] = {};
        sprintf(errmsg, "%s %s.\n", __func__, "param error");
        dbg_bio_out(errmsg, strlen(errmsg));
        return -1;
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
        char errmsg[256] = {};
        sprintf(errmsg, "%s %s.\n", __func__, "param error");
        dbg_bio_out(errmsg, strlen(errmsg));
        return -1;
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

#ifdef DBG_USE_LOG
static int gdbg_log_fd = -1;            //!< 文件描述符存储

/** 打开日志文件 */
int dbg_bio_open(char * file)
{
    char errmsg[256] = {};
    if(NULL == file) {
        sprintf(errmsg, "%s %s.\n", __func__, "param error");
        dbg_bio_out(errmsg, strlen(errmsg));
        return -1;
    }
    if(gdbg_log_fd != -1) {
        if(close(gdbg_log_fd) && errno != EBADF) {
            sprintf(errmsg, "%s %s: %s(%d).\n", __func__,
                    "close failed: ", strerror(errno), errno);
            dbg_bio_out(errmsg, strlen(errmsg));
            return -1;
        }
        gdbg_log_fd = -1;
    }
    if((gdbg_log_fd = open(file, O_RDWR | O_APPEND | O_CREAT
                    /*| O_SYNC*/,           // 速度太慢
            S_IRWXU | S_IRWXG | S_IRWXO)) < 0) {
        sprintf(errmsg, "%s %s: %s(%d).\n", __func__,
                "open failed: ", strerror(errno), errno);
        dbg_bio_out(errmsg, strlen(errmsg));
        gdbg_log_fd = -1;
        return -1;
    }

    return 0;
}

/** 关闭日志文件 */
int dbg_bio_close(void)
{
    if(gdbg_log_fd != -1) {
        if(close(gdbg_log_fd) && errno != EBADF) {
            char errmsg[256] = {};
            sprintf(errmsg, "%s %s: %s(%d).\n", __func__,
                    "close failed: ", strerror(errno), errno);
            dbg_bio_out(errmsg, strlen(errmsg));
            return -1;
        }
        gdbg_log_fd = -1;
    }

    return 0;
}

/** 写日志 */
int dbg_bio_write(char * buf, int len)
{
    if(buf == NULL || len < 0) {
        char errmsg[256] = {};
        sprintf(errmsg, "%s %s.\n", __func__, "param error");
        dbg_bio_out(errmsg, strlen(errmsg));
        return -1;
    }
    if(gdbg_log_fd != -1) {
        if(write(gdbg_log_fd, buf, len) != len) {
            char errmsg[256] = {};
            sprintf(errmsg, "%s %s: %s(%d).\n", __func__,
                    "write failed: ", strerror(errno), errno);
            dbg_bio_out(errmsg, strlen(errmsg));
            return -1;
        }
    }
    /*
     * /// 不打印错误信息
     * else {
     *     char errmsg[256] = {};
     *     sprintf(errmsg, "%s %s.\n", __func__, "Log file is not open");
     *     dbg_bio_out(errmsg, strlen(errmsg));
     * }
     */

    return len;
}

int dbg_bio_sync(void)
{
    fsync(gdbg_log_fd);         // 手动同步，调试速度较慢

    return 0;
}
#endif /* DBG_USE_LOG */

#ifdef __cplusplus
}
#endif

