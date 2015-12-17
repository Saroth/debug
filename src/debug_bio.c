#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "debug.h"

#if (DS_DEBUG_MAIN == 1)

typedef struct {                        //!< 基本接口定义
    DBG_FUNC_OUTPUT_T f_output;         //!< 输出接口函数
    DBG_FUNC_INPUT_T f_input;           //!< 输入接口函数
    DBG_FUNC_FOPEN_T f_open;            //!< 打开文件接口函数
    DBG_FUNC_FCLOSE_T f_close;          //!< 关闭文件接口函数
    DBG_FUNC_FWRITE_T f_write;          //!< 写文件接口函数
    DBG_FUNC_FSYNC_T f_sync;            //!< 同步文件接口函数
    int open_ret_type;                  //!< 文件打开函数返回类型 0:指针; 1:句柄
    void * fp;                          //!< 日志文件指针(或存储整形值)
}DBG_BIO_T;

static DBG_BIO_T g_dbg_bio = { 0 };

/** 输出接口 */
int dbg_bio_out(char * buf, int len)
{
    if(buf == NULL) {
        char errmsg[128];
        sprintf(errmsg, "\r\n dbg_bio_out Param error: buf=%p\r\n", buf);
        dbg_bio_out(errmsg, strlen(errmsg));
        return DBG_RET_PARAM_ERR;
    }
    if(g_dbg_bio.f_output) {
        if(g_dbg_bio.f_output(buf, len) < 0) {
            return 0;
        }
        return len;
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
    char errmsg[128];

    if(buf == NULL || len < 0) {
        sprintf(errmsg, "\r\n dbg_bio_in Param error: buf=%p, len=%d\r\n",
                buf, len);
        dbg_bio_out(errmsg, strlen(errmsg));
        return DBG_RET_PARAM_ERR;
    }
    if(g_dbg_bio.f_input) {
        int ret = g_dbg_bio.f_input(buf, len);
        if(ret < 0) {
            sprintf(errmsg, "\r\n dbg_bio_in f_input error: %d\r\n", ret);
            dbg_bio_out(errmsg, strlen(errmsg));
            return 0;
        }
        return strlen(buf);
    }
    while(1) {
        ch = getchar();
        if(ch == EOF) {
            break;
        }
        else {
            *b = ch;
            if(*b == '\n' || (++b - buf) >= len) {
                break;
            }
        }
    }
    return strlen(buf);
}

/** 设置输入输出函数 */
int dbg_set_console_bio(DBG_FUNC_OUTPUT_T f_output, DBG_FUNC_INPUT_T f_input)
{
    if(f_output) {
        g_dbg_bio.f_output = f_output;
    }
    if(f_input) {
        g_dbg_bio.f_input = f_input;
    }
    return 0;
}

#if (DBG_LOG_EN == 1)

/** 打开日志文件 */
int dbg_bio_open(char * file)
{
    int ret;
    void * fp;
    char errmsg[128];
    if(NULL == file) {
        sprintf(errmsg, "\r\n dbg_bio_open Param error: file=%p\r\n", file);
        dbg_bio_out(errmsg, strlen(errmsg));
        return DBG_RET_PARAM_ERR;
    }
    if((ret = dbg_bio_close())) {
        return ret;
    }
    if(g_dbg_bio.f_open) {
        fp = g_dbg_bio.f_open(file);
        if((g_dbg_bio.open_ret_type == DBG_FOPEN_RET_POINT && fp == NULL)
                || (g_dbg_bio.open_ret_type == DBG_FOPEN_RET_FD && fp < 0)) {
            sprintf(errmsg, "\r\n dbg_bio_open f_open: %p\r\n", fp);
            dbg_bio_out(errmsg, strlen(errmsg));
            return DBG_RET_OPEN_FILE_ERR;
        }
    }
    else if(NULL == (fp = fopen(file, "ab+"))) {
        sprintf(errmsg, "\r\n dbg_bio_open fopen: %s(%d)\r\n",
                strerror(errno), errno);
        dbg_bio_out(errmsg, strlen(errmsg));
        return DBG_RET_OPEN_FILE_ERR;
    }
    g_dbg_bio.fp = fp;

    return DBG_RET_OK;
}

/** 关闭日志文件 */
int dbg_bio_close(void)
{
    char errmsg[128];
    if((g_dbg_bio.open_ret_type == DBG_FOPEN_RET_POINT && g_dbg_bio.fp == NULL)
            || (g_dbg_bio.open_ret_type == DBG_FOPEN_RET_FD
                && g_dbg_bio.fp < 0)) {
        return DBG_RET_OK;              //!< 文件未打开
    }
    if(g_dbg_bio.f_close) {
        int ret = g_dbg_bio.f_close(g_dbg_bio.fp);
        if(ret) {
            sprintf(errmsg, "\r\n dbg_bio_close f_close: %d\r\n", ret);
            dbg_bio_out(errmsg, strlen(errmsg));
            return DBG_RET_CLOSE_FILE_ERR;
        }
    }
    else if(fclose(g_dbg_bio.fp) && errno != EBADF) {
        sprintf(errmsg, "\r\n dbg_bio_close fclose: %s(%d)\r\n",
                strerror(errno), errno);
        dbg_bio_out(errmsg, strlen(errmsg));
        return DBG_RET_CLOSE_FILE_ERR;
    }
    if(g_dbg_bio.open_ret_type == DBG_FOPEN_RET_POINT) {
        g_dbg_bio.fp = NULL;
    }
    else if(g_dbg_bio.open_ret_type == DBG_FOPEN_RET_FD) {
        g_dbg_bio.fp = (void *)-1;
    }

    return DBG_RET_OK;
}

/** 写日志 */
int dbg_bio_write(char * buf, int len)
{
    char errmsg[128];
    if((g_dbg_bio.open_ret_type == DBG_FOPEN_RET_POINT && g_dbg_bio.fp == NULL)
            || (g_dbg_bio.open_ret_type == DBG_FOPEN_RET_FD
                && g_dbg_bio.fp < 0)) {
        return DBG_RET_OK;              //!< 文件未打开
    }
    if(buf == NULL || len < 0) {
        sprintf(errmsg, "\r\n dbg_bio_write Param error: buf=%p, len=%d\r\n",
                buf, len);
        dbg_bio_out(errmsg, strlen(errmsg));
        return DBG_RET_PARAM_ERR;
    }
    if(g_dbg_bio.f_write) {
        int ret = g_dbg_bio.f_write(g_dbg_bio.fp, buf, len);
        if(ret != len) {
            sprintf(errmsg, "\r\n dbg_bio_write f_write: %d!=%d\r\n", ret, len);
            dbg_bio_out(errmsg, strlen(errmsg));
            return DBG_RET_WRITE_FILE_ERR;
        }
        return len;
    }
    if(fwrite(buf, 1, len, g_dbg_bio.fp) != len) {
        sprintf(errmsg, "\r\n dbg_bio_write fwrite: %s(%d)\r\n",
                strerror(errno), errno);
        dbg_bio_out(errmsg, strlen(errmsg));
        return DBG_RET_WRITE_FILE_ERR;
    }

    return len;
}

/** 同步数据，将缓存中的数据保存到存储设备 */
int dbg_bio_sync(void)
{
    if(g_dbg_bio.f_sync) {
        g_dbg_bio.f_sync(g_dbg_bio.fp);
        return DBG_RET_OK;
    }
    fflush(g_dbg_bio.fp);               //!< 将数据从C库缓存写到内核缓存
#if defined(__unix__) || defined(__linux__)
#include <unistd.h>
    fsync(fileno(g_dbg_bio.fp));        //!< 将数据从内核缓存写到磁盘
#endif

    return DBG_RET_OK;
}

/** 设置文件操作函数接口 */
int dbg_set_file_bio(int open_ret_type,
        DBG_FUNC_FOPEN_T f_open, DBG_FUNC_FCLOSE_T f_close,
        DBG_FUNC_FWRITE_T f_write, DBG_FUNC_FSYNC_T f_sync)
{
    if(open_ret_type < DBG_FOPEN_RET_POINT || open_ret_type > DBG_FOPEN_RET_FD){
        char errmsg[128];
        sprintf(errmsg, "\r\n dbg_set_file_bio Param error: "
                "open_ret_type=%d\r\n", open_ret_type);
        dbg_bio_out(errmsg, strlen(errmsg));
        return DBG_RET_PARAM_ERR;
    }
    g_dbg_bio.open_ret_type = open_ret_type;
    if(f_open) {
        g_dbg_bio.f_open = f_open;
    }
    if(f_close) {
        g_dbg_bio.f_close = f_close;
    }
    if(f_write) {
        g_dbg_bio.f_write = f_write;
    }
    if(f_sync) {
        g_dbg_bio.f_sync = f_sync;
    }
    return 0;
}

#endif /* (DBG_LOG_EN == 1) */
#endif /* (DS_DEBUG_MAIN == 1) */

