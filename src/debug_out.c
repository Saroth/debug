#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "debug.h"

typedef enum {
    DBG_MODE_STDOUT_TIME,               //!< 标签显示时间
    DBG_MODE_STDOUT_SIGN,               //!< 显示标记
    DBG_MODE_STDOUT_STDERR,             //!< 显示错误
    DBG_MODE_STDOUT_SETCOL,             //!< 设置颜色
    DBG_MODE_STDOUT_RESCOL,             //!< 恢复颜色
    DBG_MODE_STDOUT_NL,                 //!< 显示新行
}DBG_MODE_STDOUT_OPT_t;

#if (DS_DEBUG_MAIN == 1)
#if (DBG_LOG_EN == 1)

/** 输出控制(如果输出成功写文件失败，将返回成功) */
static int dbg_ioctl(char * buf, int len, int opt)
{
    int logmode = dbg_log_getmode();
    int ret = DBG_RET_OK;
    if(logmode & DBG_MODE_P) {
        ret = dbg_bio_out(buf, len);
    }
    if(logmode & DBG_MODE_W && !(opt & DBG_UNLOG)) {
        dbg_bio_write(buf, len);        //!< 写日志，不检查结果
    }
    if(logmode & DBG_MODE_S) {
        dbg_bio_sync();
    }

    return ret;
}
#else
/** 输出控制 */
static int dbg_ioctl(char * buf, int len, int opt)
{
    return dbg_bio_out(buf, len);
}
#endif /* (DBG_LOG_EN == 1) */

/** 格式化输出 */
int dbg_stdout(int opt, const char * fmt, ...)
{
    int count = 0;
    va_list marker;
    char buf[DBG_BUFFER_SIZE + 4];

    if(NULL == fmt) {
        dbg_out_E(DS_OUT_ERR, "Param error.");
        return DBG_RET_PARAM_ERR;
    }
    va_start(marker, fmt);
    count = vsnprintf(buf, DBG_BUFFER_SIZE, fmt, marker);
    va_end(marker);
    if(count > DBG_BUFFER_SIZE) {
        dbg_out_W(DS_OUT_ERR, "Output string too long.");
        count = DBG_BUFFER_SIZE;
    }
    else if(count < 0) {
        dbg_outerr_I(DS_OUT_ERR, "vsnprintf");
        return count;
    }

    return dbg_ioctl(buf, count, opt);
}

/** 标准错误信息输出 */
int dbg_stderr(int opt)
{
    char buf[DBG_BUFFER_SIZE + 4];

    int ret = sprintf(buf, "%s(%d)", strerror(errno), errno);
    if(ret <= 0) {
        dbg_outerr_I(DS_OUT_ERR, "sprintf");
        return ret;
    }

    return dbg_ioctl(buf, ret, opt);
}

#if (DBG_COLOR_EN == 1)
/** 设置终端输出颜色 */
int dbg_color_set(char * color)
{
    if(color) {
        dbg_bio_out(color, strlen(color)); //!< 仅输出，不写入日志
    }

    return DBG_RET_OK;
}
#else
inline int dbg_color_set(char * color) { return 0; }
#endif /* (DBG_COLOR_EN == 1) */

/** 标签符号输出 */
int dbg_stdout_sign(int opt, int type)
{
    switch(type) {
        case DBG_MODE_STDOUT_TIME: {
            /// 显示时间
            if(opt & DBG_LABEL_TIME) {
                time_t tim;
                struct tm * t;
                time(&tim);
                t = localtime(&tim);
                dbg_stdout(opt, "%02d%02d%02d%02d%02d%02d ",
                        (1900 + t->tm_year) % 100, 1 + t->tm_mon, t->tm_mday,
                        t->tm_hour, t->tm_min, t->tm_sec);
            }
            break;
        }
        case DBG_MODE_STDOUT_SIGN: {
            char * col = NULL;
            char * sign = "";
            if(opt & DBG_LABEL_INFO) {
                col = DBG_COLOR_INFO;
                sign = DBG_MODE_SIGN_INFO;
            }
            else if(opt & DBG_LABEL_WARN) {
                col = DBG_COLOR_WARN;
                sign = DBG_MODE_SIGN_WARN;
            }
            else if(opt & DBG_LABEL_ERR) {
                col = DBG_COLOR_ERR;
                sign = DBG_MODE_SIGN_ERR;
            }
            else if(opt & DBG_LABEL_IN_RETN) {
                col = DBG_COLOR_INPUT;
                sign = DBG_MODE_SIGN_IN_RETNUM;
            }
            else if(opt & DBG_LABEL_IN_GETS) {
                col = DBG_COLOR_INPUT;
                sign = DBG_MODE_SIGN_IN_GETNUM;
            }
            else if(opt & DBG_LABEL_IN_GETS) {
                col = DBG_COLOR_INPUT;
                sign = DBG_MODE_SIGN_IN_GETSTR;
            }
            else if(opt & DBG_LABEL_IN_ECHO) {
                col = DBG_COLOR_INPUT;
                sign = DBG_MODE_SIGN_IN_ECHO;
            }
            dbg_color_set(col);
            dbg_stdout(opt, "%s", sign);
            dbg_color_set(DBG_COLOR_RES);
            break;
        }
        case DBG_MODE_STDOUT_STDERR: {
            if(opt & DBG_LABEL_STDERR) {
                /// 显示错误信息
                dbg_stdout(opt, ": ");
                dbg_stderr(opt);
            }
            break;
        }
        case DBG_MODE_STDOUT_SETCOL: {
            char * col = NULL;
            if(opt & DBG_LABEL_COL_INFO) {
                col = DBG_COLOR_INFO;
            }
            else if(opt & DBG_LABEL_COL_WARN) {
                col = DBG_COLOR_WARN;
            }
            else if(opt & DBG_LABEL_COL_ERR) {
                col = DBG_COLOR_ERR;
            }
            else if(opt & DBG_LABEL_COL_HL) {
                col = DBG_COLOR_HL;
            }
            dbg_color_set(col);
            break;
        }
        case DBG_MODE_STDOUT_RESCOL: {
            dbg_color_set(DBG_COLOR_RES);
            break;
        }
        case DBG_MODE_STDOUT_NL: {
            if(opt & DBG_LABEL_NEWLINE) {
                dbg_stdout(opt, "%s", DBG_NL_CHAR);
            }
            break;
        }
    }

    return DBG_RET_OK;
}

/** 带调试标签的格式化输出 */
int dbg_stdout_label(const char * file, const char * func, int line,
        int opt, const char * fmt, ...)
{
    char buf[DBG_BUFFER_SIZE + 4];
    int ret;
    int argc;
    va_list argv;

    if(fmt == NULL) {
        dbg_out_E(DS_OUT_ERR, "Param error.");
        return DBG_RET_PARAM_ERR;
    }
#if (DBG_NL_HEAD == 1)
    /// 输出换行符
    dbg_stdout_sign(opt, DBG_MODE_STDOUT_NL);
#endif /* (DBG_NL_HEAD == 1) */
    /// 显示时间
    dbg_stdout_sign(opt, DBG_MODE_STDOUT_TIME);
    /// 显示标签
    argc = 0;
    if(opt & DBG_LABEL_FILE) {
        /// 显示文件名
        ret = sprintf(buf + argc, "%s:", file);
        if(ret < 20) {
            memset(buf + argc + ret, ' ', 20 - ret);
            ret = 20;
        }
        argc += ret;
    }
    if(opt & DBG_LABEL_FUNC) {
        /// 显示函数名
        ret = sprintf(buf + argc, "%s:", func);
        if(ret < 26) {
            memset(buf + argc + ret, ' ', 26 - ret);
            ret = 26;
        }
        argc += ret;
    }
    if(opt & DBG_LABEL_LINE) {
        /// 显示行号
        ret = sprintf(buf + argc, "%d:", line);
        if(ret < 6) {
            memset(buf + argc + ret, ' ', 6 - ret);
            ret = 6;
        }
        argc += ret;
    }
    buf[argc] = 0;
    dbg_ioctl(buf, argc, opt);
    /// 显示信息标记
    dbg_stdout_sign(opt, DBG_MODE_STDOUT_SIGN);
    /// 设置内容颜色
    dbg_stdout_sign(opt, DBG_MODE_STDOUT_SETCOL);
    /// 输出内容
    va_start(argv, fmt);
    argc = vsnprintf(buf, DBG_BUFFER_SIZE, fmt, argv);
    va_end(argv);
    if(argc > 0) {
        if(argc > DBG_BUFFER_SIZE) {
            argc = DBG_BUFFER_SIZE;
        }
        argc = dbg_ioctl(buf, argc, opt);
    }
    else if(argc < 0) {
        dbg_outerr_I(DS_OUT_ERR, "vsnprintf");
    }
    /// 显示错误信息
    dbg_stdout_sign(opt, DBG_MODE_STDOUT_STDERR);
    /// 恢复颜色
    dbg_stdout_sign(opt, DBG_MODE_STDOUT_RESCOL);
#if !(DBG_NL_HEAD == 1)
    /// 输出换行符
    dbg_stdout_sign(opt, DBG_MODE_STDOUT_NL);
#endif /* !(DBG_NL_HEAD == 1) */

    return argc;
}

#endif /* (DS_DEBUG_MAIN == 1) */

