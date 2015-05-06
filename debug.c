#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif

static char _G_DBG_LOGFILE[64] = { 0 };
static int _G_DBG_LOGFD = -1;
static dbg_log_modes_e _G_DBG_LOG_MODE = DBG_LOG_OFF;

/*
 * Base
 * {
 */
/*
 * 写日志
 * @In:     buf:    缓冲区
 *          len:    需要输出的字符长度
 * @Ret:    >=0:    实际输出的字符长度
 *          -1:     错误, 信息保存在errno
 */
int _write(char * buf, int len)
{
#ifdef DBG_LOG
    // Write to log
    if(_G_DBG_LOG_MODE == DBG_LOG_ON || _G_DBG_LOG_MODE == DBG_LOG_ONLY) {
        dbg_log_modes_e preval = _G_DBG_LOG_MODE;
        if(_G_DBG_LOGFD == -1) {
            _G_DBG_LOG_MODE = DBG_LOG_OFF;
            dbg_out_I(DS_DEBUG_ERR, 
                    "\r\n[Err] Log file is not open! Stop write log.");
            return -1;
        }
        if(write(_G_DBG_LOGFD, buf, len) < 0) {
            _G_DBG_LOG_MODE = DBG_LOG_OFF;
            dbg_outerr_I(DS_DEBUG_ERR, "Write log: %s", _G_DBG_LOGFILE);
            _G_DBG_LOG_MODE = preval;
            return -1;
        }
    }
#endif /* DBG_LOG */

    return len;
}

int _outchr(char chr)
{
#ifdef DBG_MAIN
#endif /* DBG_MAIN */
    return 0;
}

/*
 * 数据输出
 * @In:     buf:    缓冲区
 *          len:    需要输出的字符长度
 * @Ret:    >=0:    实际输出的字符长度
 *          <0:     Failed
 */
int _output(char * buf, int len)
{
#ifdef DBG_MAIN
    // Standard output
    if(_G_DBG_LOG_MODE != DBG_LOG_ONLY) {
        printf(buf);
        setbuf(stdout, NULL);
        // fflush(stdout);
    }
#endif /* DBG_MAIN */
    _write(buf, len);

    return len;
}

int _inchr(char * chr)
{
    return 0;
}

/*
 * 获取数据输入
 * @In:     buf:    缓冲区
 * @Ret:    >=0:    实际输入的字符长度
 *          <0:     Failed
 */
int _input(char * buf, int len)
{
    char ch;
    scanf("%s", buf);
    // 清除输入流
    // fflush(stdin);                       // Illegal seek?
    // setbuf(stdin, NULL);                 // Did not work?
    while((ch = getchar()) != '\n' && ch != EOF);   // Can't break; bug?
    errno = 0;

    return strlen(buf);
}

/*
 * }
 */

/* 
 * Out
 * {
 */
/*
 * 格式输出
 * @In:     fmt:    格式输出字符串
 * @Ret:    >=0:    实际输出的字符长度
 *          <0:     Failed
 */
int dbg_out_fmt(const char * fmt, ...)
{
    int count = 0;
    va_list marker;
    char buf[BUFFER_SIZE + 4] = { 0 };

    if(NULL == fmt) {
        dbg_out_I(DS_DEBUG_ERR, "[Err] Param error!");
        return -1;
    }
    va_start(marker, fmt);
    count = vsnprintf(buf, BUFFER_SIZE, fmt, marker);
    va_end(marker);
    if(count > BUFFER_SIZE) {
        count = BUFFER_SIZE;
    }
    else if(count < 0) {
        dbg_outerr_I(DS_DEBUG_ERR, "vsnprintf");
        return count;
    }

    return _output(buf, count);
}


/*
 * 错误信息输出
 * @Ret:    >=0:    实际输出的字符长度
 *          <0:     Failed
 */
int dbg_out_err(void)
{
    char buf[BUFFER_SIZE + 4] = { 0 };

    int ret = sprintf(buf, "%s", strerror(errno));
    if(ret <= 0) {
        return ret;
    }

    return _output(buf, ret);
}

/* }
 * In
 * {
 */

/*
 * 获取输入字符串
 * @Out:    str:    获取输入数据的缓冲区
 * @In:     len:    缓冲区大小
 * @Ret:    >=0:    输入内容长度
 *          <0:     Failed
 */
int dbg_in_str(char * str, int len)
{
    int buf_len = 0;
    char buf[BUFFER_SIZE + 4] = { 0 };

    if(NULL == buf || len <= 0) {
        dbg_out_I(DS_DEBUG_ERR, "[Err] Param error!");
        return -1;
    }
    buf_len = _input(buf, len);
    if(buf_len > len) {
        memmove(str, buf, len);
        buf_len = len;
    }
    else if(buf_len > 0) {
        strcpy(str, buf);
    }

    return buf_len;
}

/*
 * 获取输入数值
 * @Out:    num:    用于获取输入数值
 * @Ret:    0:      Success
 *          -1:     Failed
 */
int dbg_in_num(int * num)
{
    char str[BUFFER_SIZE + 4] = { 0 };

    if(NULL == num) {
        dbg_out_I(DS_DEBUG_ERR, "[Err] Param error!");
        return -1;
    }
    _input(str, BUFFER_SIZE);
    if(atoi(str) == 0) {
        if(strcmp(str, "0")) {
            dbg_out_I(DS_DEBUG_ERR, "[Err] Invalid input! \
You must input a number.");
            return -1;
        }
    }
    *num = atoi(str);

    return 0;
}

/* }
 * Log
 * {
 */

/*
 * 初始化日志调试, 设置日志路径, 以追加模式打开日志文件
 * @In:     filename:   日志文件名字符串
 * @Ret:    0:      Success
 *          -1:     Failed
 */
int dbg_log_open(char * filename)
{
    if(NULL == filename) {
        dbg_out_I(DS_DEBUG_ERR, "[Err] Param error!");
        return -1;
    }
    if(_G_DBG_LOGFD != -1) {
        if(close(_G_DBG_LOGFD)) {
            dbg_outerr_I(DS_DEBUG_ERR,
                    "Close log file: \"%s\".", _G_DBG_LOGFILE);
            return -1;
        }
        dbg_out_I(DS_DEBUG, "Close log file \"%s\".", _G_DBG_LOGFILE);
        _G_DBG_LOGFD = -1;
        memset(_G_DBG_LOGFILE, 0x00, sizeof(_G_DBG_LOGFILE));
    }
#ifdef DBG_LOG
    if((_G_DBG_LOGFD = open(filename, O_RDWR | O_APPEND | O_CREAT | O_SYNC,
                    S_IRWXU | S_IRWXG | S_IRWXO)) < 0) {
        dbg_outerr_I(DS_DEBUG_ERR, "Open file: %s", filename);
        _G_DBG_LOGFD = -1;
        return -1;
    }
    strcpy(_G_DBG_LOGFILE, filename);
    dbg_out_I(DS_DEBUG, "Open log file \"%s\".", _G_DBG_LOGFILE);
#else
    dbg_out_I(DS_DEBUG_ERR,
            "[Err] Logging closed! Add macro \"DBG_LOG\" to turn on.");
#endif /* DBG_LOG */

    return 0;
}

/*
 * 结束日志调试
 * @Ret:    0:      Success
 *          -1:     Failed
 */
int dbg_log_close(void)
{
    dbg_log_setmode(DBG_LOG_OFF);
    if(_G_DBG_LOGFD == -1) {
        dbg_out_I(DS_DEBUG, "No file opened.");
        return 0;
    }
    if(close(_G_DBG_LOGFD)) {
        if(errno == EBADF) {
            _G_DBG_LOGFD = -1;
        }
        dbg_outerr_I(DS_DEBUG_ERR, "Close file: %s", _G_DBG_LOGFILE);
        return -1;
    }
    dbg_out_I(DS_DEBUG, "Close log file \"%s\".", _G_DBG_LOGFILE);
    _G_DBG_LOGFD = -1;
    memset(_G_DBG_LOGFILE, 0x00, sizeof(_G_DBG_LOGFILE));

    return 0;
}

/*
 * 设置日志记录模式
 * @In:     dbg_log_modes_e
 * @Ret:    0:      Success
 *          -1:     Failed
 */
int dbg_log_setmode(int mode)
{
    if(mode < DBG_LOG_OFF || mode > DBG_LOG_ONLY) {
        dbg_out_I(DS_DEBUG_ERR, "[Err] Param error! mode: %d", mode);
        return -1;
    }
    _G_DBG_LOG_MODE = (dbg_log_modes_e)mode;
    return 0;
}

/*
 * 写日志
 * @In:     buf:    缓冲区
 *          len:    需要输出的字符长度
 * @Ret:    >=0:    实际输出的字符长度
 *          -1:     错误, 信息保存在errno
 */
int dbg_log_write(char * buf, int len)
{
    if(NULL == buf || len <= 0) {
        dbg_out_I(DS_DEBUG_ERR, "[Err] Param error! buf: %s len: %d", buf, len);
        return -1;
    }

    return _write(buf, len);
}

/*
 * 获取日志记录模式
 * @Ret:    dbg_log_modes_e
 */
int dbg_log_getmode(void)
{
    return _G_DBG_LOG_MODE;
}

/* }
 * Dump
 * {
 */

#ifdef DBG_DUMP
/*
 * 数据行导出
 * @In:     buf:    数据缓存
 *          len:    输出长度(实际输出长度受mode参数限制)
 *          addr:   行号(mode设置DBG_DMP_LN时有效)
 *                  或数据地址(mode设置DBG_DMP_ADDR时有效)
 *          mode:   参考dbg_dmp_modes_e
 * @Ret:    >=0:    Dump字符数
 *          <0:     参数错误
 */
int _dump_line(char *buf, unsigned int len, unsigned int addr, int mode)
{
    int i = 0;
    unsigned int chpline = 0;
    int outputchar = 0;
    int headspace = 0;
    unsigned int addr_int = addr;
    unsigned int addr_cur = addr_int;

    // 判断分割显示格式
    if(mode & DBG_DMP_SEG_8) {
        chpline = 8;
    }
    else if(mode & DBG_DMP_SEG_16) {
        chpline = 16;
    }
    else if(mode & DBG_DMP_SEG_32) {
        chpline = 32;
    }
    else {
        // 纯数据导出
        outputchar = 0;
        for(i = 0; i < len; i++) {
            dbg_out(1, "%c", buf[i]);
            outputchar++;
        }

        return outputchar;
    }

    // 显示标签
    if((mode & DBG_DMP_TAG_LN) && chpline) {
        dbg_out(1, "|");
        dbg_out(1, "%03d", addr_int / chpline);
    }
    if((mode & DBG_DMP_TAG_ADDR) && chpline) {
        // 16进制下首行自动对齐到行字符个数
        if(addr % chpline) {
            headspace = addr % chpline;
            addr_int = addr - headspace;
        }
        addr_cur = addr_int;
        dbg_out(1, "|");
        dbg_out(1, "%#08x", addr_int);
    }

    // 数据数值格式显示
    if((mode & DBG_DMP_DAT_HEX) && chpline) {
        outputchar = 0;
        dbg_out(1, "|");
        for(i = 0; i < chpline; i++, addr_cur++) {
            if(addr_cur < addr || i >= len + headspace) {
                dbg_out(1, "   ");
            }
            else {
                dbg_out(1, " %02x", buf[addr_cur - addr] & 0xff);
                outputchar++;
            }
        }
    }
    addr_cur = addr_int;
    if((mode & DBG_DMP_DAT_DEC) && chpline) {
        outputchar = 0;
        dbg_out(1, "|");
        for(i = 0; i < chpline; i++, addr_cur++) {
            if(addr_cur < addr || i >= len + headspace) {
                dbg_out(1, "   ");
            }
            else {
                dbg_out(1, " %03d", buf[addr_cur - addr]);
                outputchar++;
            }
        }
    }

    // 数据字符格式输出
    addr_cur = addr_int;
    if((mode & DBG_DMP_BUF_CHR) && chpline) {
        outputchar = 0;
        dbg_out(1, "| ");
        for(i = 0; i < chpline; i++, addr_cur++) {
            if(addr_cur < addr || i >= len + headspace) {
                dbg_out(1, " ");
            }
            else {
                if(buf[addr_cur - addr] == 0) {
                    dbg_out(1, ".");
                }
                else if(buf[addr_cur - addr] == 10
                        || buf[addr_cur - addr] == 13) {
                    dbg_out(1, "~");
                }
                /* else if(buf[addr_cur - addr] < 32
                        || buf[addr_cur - addr] >= 128) {
                    dbg_out(1, "?");
                } */
                else {
                    dbg_out(1, "%c", buf[addr_cur - addr]);
                }
                outputchar++;
            }
        }
    }

    dbg_out(1, "\r\n");

    return outputchar;
}
#endif /* DBG_DUMP */

/*
 * 数据格式化导出
 * @In:     buf:    数据缓存
 *          len:    输出长度
 *          addr:   行号(mode设置DBG_DMP_LN时有效)
 *                  或数据地址(mode设置DBG_DMP_ADDR时有效)
 *          mode:   参考dbg_dmp_modes_e
 * @Ret:    0:      Success
 *          <0:     参数错误
 */
int dump_data(char *buf, unsigned int len, unsigned int addr, int mode)
{
#ifdef DBG_DUMP
    int ret = 0;

    if(NULL == buf || len < 0) {
        dbg_out_I(DS_DEBUG_ERR, "[Err] Param error!");
        return -1;
    }
    while(len > 0) {
        ret = _dump_line(buf, len, addr, mode);
        len -= ret;
        addr += ret;
        buf += ret;
    }
#endif /* DBG_DUMP */

    return 0;
}

/*
 * }
 */

#ifdef __cplusplus
}
#endif

