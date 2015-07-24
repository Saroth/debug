#include <stdio.h>
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
        char errmsg[256];
        sprintf(errmsg, "%s %s.%s", __func__, "param error", DBG_NL_CHAR);
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
        char errmsg[256];
        sprintf(errmsg, "%s %s.%s", __func__, "param error", DBG_NL_CHAR);
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

#if(DBG_USE_LOG == 1)
static FILE * gdbg_log_fp = NULL;       //!< 文件指针存储

/** 打开日志文件 */
int dbg_bio_open(char * file)
{
    char errmsg[256];
    if(NULL == file) {
        sprintf(errmsg, "%s %s.%s", __func__, "param error", DBG_NL_CHAR);
        dbg_bio_out(errmsg, strlen(errmsg));
        return -1;
    }
    if(gdbg_log_fp) {
        if(fclose(gdbg_log_fp) && errno != EBADF) {
            sprintf(errmsg, "%s %s: %s(%d).%s", __func__,
                    "fclose failed: ", strerror(errno), errno, DBG_NL_CHAR);
            dbg_bio_out(errmsg, strlen(errmsg));
            return -1;
        }
        gdbg_log_fp = NULL;
    }
    if(!(gdbg_log_fp = fopen(file, "ab+"))) {
        sprintf(errmsg, "%s %s: %s(%d).%s", __func__,
                "open failed: ", strerror(errno), errno, DBG_NL_CHAR);
        dbg_bio_out(errmsg, strlen(errmsg));
        gdbg_log_fp = NULL;
        return -1;
    }

    return 0;
}

/** 关闭日志文件 */
int dbg_bio_close(void)
{
    if(gdbg_log_fp) {
        if(fclose(gdbg_log_fp) && errno != EBADF) {
            char errmsg[256];
            sprintf(errmsg, "%s %s: %s(%d).%s", __func__,
                    "close failed: ", strerror(errno), errno, DBG_NL_CHAR);
            dbg_bio_out(errmsg, strlen(errmsg));
            return -1;
        }
        gdbg_log_fp = NULL;
    }

    return 0;
}

/** 写日志 */
int dbg_bio_write(char * buf, int len)
{
    if(buf == NULL || len < 0) {
        char errmsg[256];
        sprintf(errmsg, "%s %s.%s", __func__, "param error", DBG_NL_CHAR);
        dbg_bio_out(errmsg, strlen(errmsg));
        return -1;
    }
    if(gdbg_log_fp) {
        if(fwrite(buf, 1, len, gdbg_log_fp) != len) {
            char errmsg[256];
            sprintf(errmsg, "%s %s: %s(%d).%s", __func__,
                    "write failed: ", strerror(errno), errno, DBG_NL_CHAR);
            dbg_bio_out(errmsg, strlen(errmsg));
            return -1;
        }
    }
    /*
     * /// 不打印错误信息
     * else {
     *     char errmsg[256];
     *     sprintf(errmsg, "%s %s.%s", __func__,
     *             "Log file is not open", DBG_NL_CHAR);
     *     dbg_bio_out(errmsg, strlen(errmsg));
     * }
     */

    return len;
}

int dbg_bio_sync(void)
{
    fflush(gdbg_log_fp);                //!< 将数据从C库缓存写到内核缓存
#if defined(__unix__) || defined(__linux__)
#include <unistd.h>
    fsync(fileno(gdbg_log_fp));         //!< 将数据从内核缓存写到磁盘
#endif

    return 0;
}
#endif /* (DBG_USE_LOG == 1) */

#ifdef __cplusplus
}
#endif

