/* {
 * Code is far away from bug with the animal protecting.
 *     ┏┓     ┏┓
 *    ┏┛┻━━━━━┛┻┓
 *    ┃         ┃
 *    ┃    ━    ┃
 *    ┃ ┳┛   ┗┳ ┃
 *    ┃         ┃
 *    ┃    ┻    ┃
 *    ┃         ┃
 *    ┗━┓     ┏━┛
 *      ┃     ┃神兽保佑, 远离BUG!
 *      ┃     ┗━━━┓
 *      ┃         ┣┓
 *      ┃         ┏┛
 *      ┗┓┓┏━━━┳┓┏┛
 *       ┃┫┫   ┃┫┫
 *       ┗┻┛   ┗┻┛
}*/

/*
 * @file:       debug.h
 * @Synopsis:   Debug module;
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DS_DEBUG
#define DS_DEBUG 1                      // debug模块调试
#endif /* DS_DEBUG */
#ifndef DS_DEBUG_ERR
#define DS_DEBUG_ERR 1                  // debug模块错误输出
#endif /* DS_DEBUG_ERR */
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 0x1000              // 缓存大小
#endif /* BUFFER_SIZE */

/*
 * Out
 * {
 */
// 格式化输出
int dbg_out_fmt(const char * fmt, ...);
// 标准错误信息输出
int dbg_out_err(void);

#ifdef DBG_MAIN
// 格式化输出
// @In:     __debug_switch__:   调试开关
#define dbg_out(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_out_fmt(__VA_ARGS__); \
    } \
}
// 带标签信息的格式化输出
// @In:     __debug_switch__:   调试开关
#define dbg_out_I(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_out_fmt("%s:%5d:\t", __func__, __LINE__); \
        dbg_out_fmt(__VA_ARGS__); \
        dbg_out_fmt("\n"); \
    } \
}
// 带标签信息的格式化输出, 附带错误信息
// @In:     __debug_switch__:   调试开关
#define dbg_outerr_I(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_out_fmt("%s:%5d:\t[Err] ", __func__, __LINE__); \
        dbg_out_fmt(__VA_ARGS__); \
        dbg_out_fmt(": "); \
        dbg_out_err(); \
        dbg_out_fmt("\n"); \
    } \
}
#else
#define dbg_out(...)
#define dbg_out_I(...)
#define dbg_outerr_I(...)
#endif /* DBG_MAIN */

/* }
 * In
 * {
 */

// 获取输入的字符串
int dbg_in_str(char * str, int len);
// 获取输入的数值
int dbg_in_num(int * num);

// 返回输入的数值, 如果输入值非数字开头则返回-1
#define dbg_in() ({ \
        int __num__ = 0; \
        int __ret__ = 0; \
        dbg_out_fmt("[I-n]"); \
        __ret__ = dbg_in_num(&__num__); \
        dbg_out_fmt("[O]%d\n", __num__); \
        __ret__ == 0 ? __num__ : -1; \
        })
// 获取输入的数值
#define dbg_in_N(pnum) ({ \
        int __ret__ = 0; \
        dbg_out_fmt("[I-n]"); \
        __ret__ = dbg_in_num(pnum); \
        dbg_out_fmt("[O]%d\n", *pnum); \
        __ret__; \
        })
// 获取输入的字符串
#define dbg_in_S(pstr, len) ({ \
        int __ret__ = 0; \
        dbg_out_fmt("[I-s]"); \
        __ret__ = dbg_in_str(pstr, len); \
        dbg_out_fmt("[O]%s\n", pstr); \
        __ret__; \
        })

/* }
 * Log
 * {
 */
typedef enum {
    DBG_LOG_OFF = 0,                    // 关闭日志记录
    DBG_LOG_ON,                         // 打开日志记录, 输出调试同时追加到日志
    DBG_LOG_ONLY,                       // 只将调试输出到日志
}dbg_log_modes_e;

// 初始化日志调试, 设置日志路径, 以追加模式打开日志文件
int dbg_log_open(char * filename);
// 结束日志调试
int dbg_log_close(void);
// 设置日志记录模式
int dbg_log_setmode(int mode);
// 写日志
int dbg_log_write(char * buf, int len);
// 获取日志记录模式
int dbg_log_getmode(void);

/* }
 * Dump
 * {
 */
typedef enum {
    DBG_DMP_SEG_8       = (1 << 0),     // [0]=1:   8字节分割
    DBG_DMP_SEG_16      = (1 << 1),     // [1]=1:   16字节分割
    DBG_DMP_SEG_32      = (1 << 2),     // [2]=1:   32字节分割
                                        // [0:2]=0: 无格式纯数据导出
    DBG_DMP_TAG_LN      = (1 << 3),     // [3]=1:   显示行号
    DBG_DMP_TAG_ADDR    = (1 << 4),     // [4]=1:   显示地址
                                        // [3:4]=0: 不显示标签
    DBG_DMP_DAT_HEX     = (1 << 5),     // [5]=1:   16进制显示
    DBG_DMP_DAT_DEC     = (1 << 6),     // [6]=1:   10进制显示
    DBG_DMP_BUF_CHR     = (1 << 7),     // [7]=1:   字符显示
                                        // [5:7]=0: 不显示数据
}dbg_dmp_modes_e;

// 数据格式化导出
int dump_data(char *buf, unsigned int len, unsigned int addr, int mode);

#ifdef DBG_DUMP
// 16进制导出, 带行号, 每行16字节
// @In:     __debug_switch__:   调试开关
//          buf:    数据缓存
//          len:    数据长度
#define dbg_dmp_H(__debug_switch__, buf, len) { \
    if(__debug_switch__) { \
        dbg_out_fmt("________________________________________________\r\n"); \
        dump_data(buf, len, 0, \
                DBG_DMP_SEG_16 | DBG_DMP_TAG_LN | DBG_DMP_DAT_HEX); \
        dbg_out_fmt("#### buffer size: %d\r\n", len); \
    } \
}
// 16进制导出, 同时输出字符, 带行号, 每行16字节
// @In:     __debug_switch__:   调试开关
//          buf:    数据缓存
//          len:    数据长度
#define dbg_dmp_HC(__debug_switch__, buf, len) { \
    if(__debug_switch__) { \
        dbg_out_fmt("________________________________________________\r\n"); \
        dump_data(buf, len, 0, \
                DBG_DMP_SEG_16 | DBG_DMP_TAG_LN | DBG_DMP_DAT_HEX | DBG_DMP_BUF_CHR); \
        dbg_out_fmt("#### buffer size: %d\r\n", len); \
    } \
}
// 16进制导出, 同时输出字符, 带地址, 每行16字节
// @In:     __debug_switch__:   调试开关
//          buf:    数据缓存
//          len:    数据长度
//          addr:   数据内存地址
#define dbg_dmp_HCA(__debug_switch__, buf, len, addr) { \
    if(__debug_switch__) { \
        dbg_out_fmt("________________________________________________\r\n"); \
        dump_data(buf, len, addr, \
                DBG_DMP_SEG_16 | DBG_DMP_TAG_ADDR | DBG_DMP_DAT_HEX | DBG_DMP_BUF_CHR); \
        dbg_out_fmt("#### buffer size: %d\r\n", len); \
    } \
}
// 纯字符导出, 无任何格式(可用于将内存或Flash数据导出到文件)
// @In:     __debug_switch__:   调试开关
//          buf:    数据缓存
//          len:    数据长度
#define dbg_dmp_Orig(__debug_switch__, buf, len) { \
    if(__debug_switch__) { \
        dump_data(buf, len, 0, 0); \
    } \
}
#else
#define dbg_dmp_H(...)
#define dbg_dmp_HC(...)
#define dbg_dmp_HCA(...)
#define dbg_dmp_Orig(...)
#endif /* DBG_DUMP */

/*
 * }
 */

// 模块测试
int debug_test(void);

#ifdef __cplusplus
}
#endif

#endif

