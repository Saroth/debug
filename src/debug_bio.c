#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "debug.h"

#if (DS_DEBUG_MAIN == 1)

static int _std_out(char * buf, int len)
{
    printf(buf);
    fflush(stdout);
    return 0;
}
static int _std_in(char * buf, int len)
{
    int ch = 0;
    char * b = buf;
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
    return 0;
}
#if (DBG_LOG_EN == 1)
static void * s_dbg_fp = NULL;          //!< 日志文件指针
static int _f_open(char * filename)
{
    s_dbg_fp = fopen(filename, "ab+");
    if(NULL == s_dbg_fp) {
        return -1;
    }
    return 0;
}
static int _f_close(void)
{
    if(NULL == s_dbg_fp) {
        return 0;
    }
    if(fclose(s_dbg_fp) && errno != EBADF) {
        return -1;
    }
    s_dbg_fp = NULL;
    return 0;
}
static int _f_write(char * buf, int len)
{
    if(NULL == s_dbg_fp) {
        return 0;
    }
    if(fwrite(buf, 1, len, s_dbg_fp) != len) {
        return -1;
    }
    return 0;
}
static int _f_sync(void)
{
    if(NULL == s_dbg_fp) {
        return 0;
    }
    fflush(s_dbg_fp);                   //!< 将数据从C库缓存写到内核缓存
#if defined(__unix__) || defined(__linux__)
#include <unistd.h>
    fsync(fileno(s_dbg_fp));            //!< 将数据从内核缓存写到磁盘
#endif
    return 0;
}
#endif /* (DBG_LOG_EN == 1) */

static DBG_BIO_T g_dbg_bio_unix = {     //!< 默认接口(类unix平台)
    .f_output   = _std_out,
    .f_input    = _std_in,
#if (DBG_LOG_EN == 1)
    .f_open     = _f_open,
    .f_close    = _f_close,
    .f_write    = _f_write,
    .f_sync     = _f_sync,
#endif /* (DBG_LOG_EN == 1) */
};

static DBG_BIO_T * g_dbg_bio_p = &g_dbg_bio_unix;

/** 输出接口 */
int dbg_bio_out(char * buf, int len)
{
    if(buf == NULL) {
        char errmsg[128];
        sprintf(errmsg, "\r\n dbg_bio_out Param error: buf=%p\r\n", buf);
        dbg_bio_out(errmsg, strlen(errmsg));
        return DBG_RET_PARAM_ERR;
    }
    if(g_dbg_bio_p->f_output) {
        g_dbg_bio_p->f_output(buf, len);
    }
    return DBG_RET_OK;
}
/** 输入接口 */
int dbg_bio_in(char * buf, int len)
{
    int ret;
    char errmsg[128];
    if(buf == NULL || len < 0) {
        sprintf(errmsg, "\r\n dbg_bio_in Param error: buf=%p, len=%d\r\n",
                buf, len);
        dbg_bio_out(errmsg, strlen(errmsg));
        return DBG_RET_PARAM_ERR;
    }
    if(g_dbg_bio_p->f_input) {
        if((ret = g_dbg_bio_p->f_input(buf, len)) == 0) {
            return strlen(buf);
        }
        sprintf(errmsg, "\r\n f_input error: %d\r\n", ret);
        dbg_bio_out(errmsg, strlen(errmsg));
    }
    return 0;
}
#if (DBG_LOG_EN == 1)
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
    if(g_dbg_bio_p->f_open && (ret = g_dbg_bio_p->f_open(file))) {
        sprintf(errmsg, "\r\n f_open: ret:%d; err:%s(%d)\r\n",
                ret, strerror(errno), errno);
        dbg_bio_out(errmsg, strlen(errmsg));
        return DBG_RET_OPEN_FILE_ERR;
    }
    return DBG_RET_OK;
}
/** 关闭日志文件 */
int dbg_bio_close(void)
{
    int ret;
    char errmsg[128];
    if(g_dbg_bio_p->f_close && (ret = g_dbg_bio_p->f_close())) {
        sprintf(errmsg, "\r\n f_close: ret:%d; err:%s(%d)\r\n",
                ret, strerror(errno), errno);
        dbg_bio_out(errmsg, strlen(errmsg));
        return DBG_RET_CLOSE_FILE_ERR;
    }
    return DBG_RET_OK;
}
/** 写日志 */
int dbg_bio_write(char * buf, int len)
{
    int ret;
    char errmsg[128];
    if(buf == NULL || len < 0) {
        sprintf(errmsg, "\r\n dbg_bio_write Param error: buf=%p, len=%d\r\n",
                buf, len);
        dbg_bio_out(errmsg, strlen(errmsg));
        return DBG_RET_PARAM_ERR;
    }
    if(g_dbg_bio_p->f_write && (ret = g_dbg_bio_p->f_write(buf, len))) {
        sprintf(errmsg, "\r\n f_write: ret:%d; err:%s(%d)\r\n",
                ret, strerror(errno), errno);
        dbg_bio_out(errmsg, strlen(errmsg));
        return DBG_RET_WRITE_FILE_ERR;
    }
    return DBG_RET_OK;
}
/** 同步数据，将缓存中的数据保存到存储设备 */
int dbg_bio_sync(void)
{
    if(g_dbg_bio_p->f_sync) {
        g_dbg_bio_p->f_sync();
    }
    return DBG_RET_OK;
}
#endif /* (DBG_LOG_EN == 1) */
/** 配置基本接口 */
int dbg_bio_conf(DBG_BIO_T * bio)
{
    dbg_bio_close();                    //!< 切换接口前先关闭日志文件
    if(NULL == bio) {
        g_dbg_bio_p = &g_dbg_bio_unix;  //!< 使用默认接口
    }
    else {
        g_dbg_bio_p = bio;
    }
    return 0;
}

#endif /* (DS_DEBUG_MAIN == 1) */

