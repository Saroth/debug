#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DBG_USE_LOG
#include "debug_log.h"

/** 输出控制(如果输出成功写文件失败，将返回成功) */
static int dbg_ioctl(char * buf, int len)
{
    int mode = dbg_log_getmode();
    int ret = 0;
    if(mode & DBG_P) {
        ret = dbg_bio_out(buf, len);
    }
    if(mode & DBG_W) {
        dbg_bio_write(buf, len);        //!< 写日志，不检查结果
    }
    if(mode & DBG_S) {
        dbg_bio_sync();
    }
    return ret;
}
#else
/** 输出控制 */
static int dbg_ioctl(char * buf, int len)
{
    return dbg_bio_out(buf, len);
}
#endif /* DBG_USE_LOG */

/** 格式化输出 */
int dbg_stdout(const char * fmt, ...)
{
    int count = 0;
    va_list marker;
    char buf[BUFFER_SIZE + 4] = {};

    if(NULL == fmt) {
        dbg_out_E(DS_OUT_ERR, "param error.");
        return -1;
    }
    va_start(marker, fmt);
    count = vsnprintf(buf, BUFFER_SIZE, fmt, marker);
    va_end(marker);
    if(count > BUFFER_SIZE) {
        dbg_out_W(DS_OUT_ERR, "Output string too long.");
        count = BUFFER_SIZE;
    }
    else if(count < 0) {
        dbg_outerr_I(DS_OUT_ERR, "vsnprintf");
        return count;
    }
    return dbg_ioctl(buf, count);
}

/** 标准错误信息输出 */
int dbg_stderr(void)
{
    char buf[BUFFER_SIZE + 4] = {};

    int ret = sprintf(buf, "%s(%d)", strerror(errno), errno);
    if(ret <= 0) {
        dbg_outerr_I(DS_OUT_ERR, "sprintf");
        return ret;
    }
    return dbg_ioctl(buf, ret);
}

#ifdef DBG_USE_COLOR
/** 设置终端输出颜色 */
int dbg_color_set(char * color)
{
    if(color == NULL) {
        dbg_out_E(DS_OUT_ERR, "param error.");
        return -1;
    }
    dbg_bio_out(color, strlen(color));
    return 0;
}
#endif /* DBG_USE_COLOR */

/** 标签符号输出 */
int dbg_stdout_sign(int opt, int type)
{
    switch(type) {
        case DBG_STDOUT_TIME: {
            /// 显示时间
            if(opt & DBG_TIME) {
                time_t tim;
                struct tm * t;
                time(&tim);
                t = localtime(&tim);
                dbg_stdout("%04d-%02d-%02d %02d:%02d:%02d ",
                        1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday,
                        t->tm_hour, t->tm_min, t->tm_sec);
            }
            break;
        }
        case DBG_STDOUT_POS: {
            break;
        }
        case DBG_STDOUT_SIGN: {
            if(opt & DBG_LABEL_INFO) {
                dbg_stdout("%s", DBG_SIGN_INFO);
            }
            else if(opt & DBG_LABEL_WARN) {
                dbg_stdout("%s", DBG_SIGN_WARN);
            }
            else if(opt & DBG_LABEL_ERR) {
                dbg_stdout("%s", DBG_SIGN_ERR);
            }
            else if(opt & DBG_LABEL_IN_RETN) {
                dbg_stdout("%s", DBG_SIGN_IN_RETNUM);
            }
            else if(opt & DBG_LABEL_IN_GETN) {
                dbg_stdout("%s", DBG_SIGN_IN_GETNUM);
            }
            else if(opt & DBG_LABEL_IN_GETS) {
                dbg_stdout("%s", DBG_SIGN_IN_GETSTR);
            }
            else if(opt & DBG_LABEL_OUT_RETN) {
                dbg_stdout("%s", DBG_SIGN_OUT_RETNUM);
            }
            else if(opt & DBG_LABEL_OUT_GETN) {
                dbg_stdout("%s", DBG_SIGN_OUT_GETNUM);
            }
            else if(opt & DBG_LABEL_OUT_GETS) {
                dbg_stdout("%s", DBG_SIGN_OUT_GETSTR);
            }
            break;
        }
        case DBG_STDOUT_STDERR: {
            if(opt & DBG_LABEL_STDERR) {
                /// 显示错误信息
                dbg_stdout(": ");
                dbg_stderr();
            }
            break;
        }
        case DBG_STDOUT_NL: {
            if(opt & DBG_LABEL_NEWLINE) {
                dbg_stdout("\r\n");
            }
            break;
        }
#ifdef DBG_USE_COLOR
        case DBG_STDOUT_COLOR_SET: {
            if(opt & DBG_LABEL_COL_INFO) {
                dbg_color_set(DBG_COLOR_INFO);
            }
            else if(opt & DBG_LABEL_COL_WARN) {
                dbg_color_set(DBG_COLOR_WARN);
            }
            else if(opt & DBG_LABEL_COL_ERR) {
                dbg_color_set(DBG_COLOR_ERR);
            }
            else if(opt & DBG_LABEL_COL_HL) {
                dbg_color_set(DBG_COLOR_HL);
            }
            else if(opt & DBG_LABEL_COL_INPUT) {
                dbg_color_set(DBG_COLOR_INPUT);
            }
            break;
        }
        case DBG_STDOUT_COLOR_RES: {
            dbg_color_set(DBG_COLOR_RES);
            break;
        }
#endif /* DBG_USE_COLOR */
    }
    return 0;
}

/** 带调试标签的格式化输出 */
int dbg_stdout_label(const char * func, int line, int opt, char * fmt, ...)
{
    char buf[BUFFER_SIZE + 4] = {};
    int argc = 0;
    va_list argv;

    if(fmt == NULL) {
        dbg_out_E(DS_OUT_ERR, "param error.");
        return -1;
    }
#ifdef DBG_NL_HEAD
    /// 输出换行符
    dbg_stdout_sign(opt, DBG_STDOUT_NL);
#endif /* DBG_NL_HEAD */
    /// 显示时间
    dbg_stdout_sign(opt, DBG_STDOUT_TIME);
    if(opt & DBG_LABEL) {
        /// 显示函数名和行号
        dbg_stdout("%s:%5d:\t", func, line);
    }
    /// 设置标记颜色
    if((opt & DBG_LABEL_COLOR)) {
        dbg_stdout_sign(opt, DBG_STDOUT_COLOR_SET);
    }
    /// 显示标记
    dbg_stdout_sign(opt, DBG_STDOUT_SIGN);
    dbg_stdout_sign(opt, DBG_STDOUT_COLOR_RES);
    /// 设置文本颜色
    if((opt & DBG_LABEL_TEXTCOLOR)) {
        dbg_stdout_sign(opt, DBG_STDOUT_COLOR_SET);
    }
    /// 输出内容
    va_start(argv, fmt);
    argc = vsnprintf(buf, BUFFER_SIZE, fmt, argv);
    va_end(argv);
    if(argc > 0) {
        if(argc > BUFFER_SIZE) {
            argc = BUFFER_SIZE;
        }
        argc = dbg_ioctl(buf, argc);
    }
    else if(argc < 0) {
        dbg_outerr_I(DS_OUT_ERR, "vsnprintf");
    }
    dbg_stdout_sign(opt, DBG_STDOUT_COLOR_RES);
    /// 显示错误信息
    dbg_stdout_sign(opt, DBG_STDOUT_STDERR);
#ifndef DBG_NL_HEAD
    /// 输出换行符
    dbg_stdout_sign(opt, DBG_STDOUT_NL);
#endif /* DBG_NL_HEAD */

    return argc;
}

#ifdef __cplusplus
}
#endif

