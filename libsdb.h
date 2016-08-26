/**
 * Copyright(C) For free. All right reserved.
 *
 * Slim Debugger, a simple code tracer for C/C++.
 */

#ifndef __SDB_H__
#define __SDB_H__

#include <stdio.h>
#include <stdlib.h>

typedef enum {                          //!< 调试输入输出选项
    SDB_IO              = (1 << 0),     //!< 输入输出使能
    //!< 标签相关
    SDB_NO_LINE         = (1 << 1),     //!< 标签 - 不输出行号
    SDB_NO_FILE         = (1 << 2),     //!< 标签 - 不输出文件名
    SDB_FUNC            = (1 << 3),     //!< 标签 - 输出函数名，默认关闭
    SDB_TIME            = (1 << 4),     //!< 标签 - 输出时间戳，默认关闭
    //!< 信息类型选择
    SDB_NO_INFO         = (1 << 8),     //!< 不输出所有信息
    SDB_NO_WARNING      = (1 << 9),     //!< 不输出所有警告
    SDB_NO_ERROR        = (1 << 10),    //!< 不输出所有错误
    SDB_NO_DUMP         = (1 << 11),    //!< 不输出所有dump信息
    //!< 输出样式控制
    SDB_NO_STDERR       = (1 << 12),    //!< 不输出标准错误信息
    SDB_NO_COLOR        = (1 << 13),    //!< 不输出高亮色
    SDB_NO_MARK         = (1 << 14),    //!< 不输出指示标记
    SDB_NO_LABLE        = (1 << 15),    //!< 不输出标签
    SDB_NO_WRAP         = (1 << 16),    //!< 不输出换行
} SDB_OPTION_T;

typedef enum {
    SDB_MODE_MARK_NONE      = (0),      //!< 无标记
    SDB_MODE_MARK_INFO      = (1),      //!< 添加高亮提示标记
    SDB_MODE_MARK_WARNING   = (2),      //!< 添加高亮警告标记
    SDB_MODE_MARK_ERROR     = (3),      //!< 添加高亮错误标记
    SDB_MODE_MARK_GETNUM    = (4),      //!< 添加高亮获取数值标记
    SDB_MODE_MARK_GETSTR    = (5),      //!< 添加高亮获取字符串标记
    SDB_MODE_MARK_ECHO      = (6),      //!< 添加高亮输入反馈标记
    SDB_MODE_MARK_TYPEMAX   = (7),      //!< 标记类型数量
    SDB_MODE_MARK_CHK       = (0x07),   //!< 标记类型检查

    SDB_MODE_HL_NONE    = (0 << 3),     //!< 信息显示为标题高亮
    SDB_MODE_HL_SIGN    = (1 << 3),     //!< 信息显示为特殊标记高亮
    SDB_MODE_HL_WARN    = (2 << 3),     //!< 信息显示为警告高亮
    SDB_MODE_HL_ERR     = (3 << 3),     //!< 信息显示为错误高亮
    SDB_MODE_HL_TITLE   = (4 << 3),     //!< 信息显示为标题高亮
    SDB_MODE_HL_INPUT   = (5 << 3),     //!< 信息显示为输入和反馈标记高亮
    SDB_MODE_HL_LABEL   = (6 << 3),     //!< 信息显示为标签高亮
    SDB_MODE_HL_RES     = (7 << 3),     //!< 信息显示为默认高亮
    SDB_MODE_HL_CHK     = (0x0F << 3),  //!< 标记类型检查

    SDB_MODE_MSG_NONE   = (0 << 7),     //!< 无特殊信息
    SDB_MODE_MSG_ENTRY  = (1 << 7),     //!< 入口标记信息
    SDB_MODE_MSG_EXIT   = (2 << 7),     //!< 出口标记信息
    SDB_MODE_MSG_CHK    = (0x03 << 7),  //!< 信息类型检查

    SDB_MODE_STDERR     = (1 << 9),     //!< 添加系统错误信息
} SDB_MODE_T;

typedef enum {                          //!< 返回值
    SDB_RET_OK              = 0,        //!< 成功
    SDB_RET_PARAM_ERR       = -11,      //!< 参数错误
    SDB_RET_NO_INPUT        = -12,      //!< 无输入
    SDB_RET_UNKNOWN_INPUT   = -13,      //!< 未知输入
    SDB_RET_ALLOC_FAILED    = -14,      //!< 内存申请失败
} SDB_RET_T;

/**
 * \brief       输出接口函数类型
 * \param       buf         输出数据的缓存
 * \param       len         输出长度
 * \return      0: Success      <0: Error
 */
typedef int (* SDB_FUNC_OUTPUT_T)(char * buf, size_t len);
/**
 * \brief       输入接口函数类型
 * \param       buf         获取输入数据的缓存
 * \param       len         缓存大小
 * \return      0: Success      <0: Error
 * \detail      输入过程不退出，直到收到回车或缓存超出才返回，包含换行符
 */
typedef int (* SDB_FUNC_INPUT_T)(char * buf, size_t len);

typedef struct {                        //!< 基本IO配置结构体
    SDB_FUNC_OUTPUT_T f_output;         //!< 输出接口函数
    SDB_FUNC_INPUT_T f_input;           //!< 输入接口函数
} SDB_BIO_T;

#endif /* __SDB_H__ */

/**
 * \block:      Output
 * @{ */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief       设置基本IO
 * \param       bio         基本IO配置结构体指针
 * \return      0:Success; <0:Error
 */
int sdb_bio_conf(SDB_BIO_T *bio);

/**
 * \brief       样式输出
 * \param       opt
 * \param       mode
 * \param       file
 * \param       func
 * \param       line
 * \param       format
 * \param       SDB_ENABLE
 * \param       ...
 * \return      
 */
int sdb_out_style(int opt, int mode,
        char *file, const char *func, int line, const char *format, ...);

#ifdef __cplusplus
}
#endif

#if defined(SDB_ENABLE)

/** \brief      格式化输出 */
#define sdb_out(__sdb_opt, ...) ({\
        if ((__sdb_opt & SDB_IO) && ((~__sdb_opt) & SDB_NO_INFO)) {\
        sdb_out_style(__sdb_opt | SDB_NO_STDERR | SDB_NO_COLOR | SDB_NO_MARK\
                | SDB_NO_LABLE | SDB_NO_WRAP, 0,\
                __FILE__, __func__, __LINE__, __VA_ARGS__);\
        }})
/** \brief      带标签的格式化输出 */
#define sdb_out_i(__sdb_opt, ...) ({\
        if ((__sdb_opt & SDB_IO) && ((~__sdb_opt) & SDB_NO_INFO)) {\
        sdb_out_style(__sdb_opt, SDB_MODE_MARK_INFO,\
                __FILE__, __func__, __LINE__, __VA_ARGS__);\
        }})
/** \brief      带标签的格式化输出，带警告标记 */
#define sdb_out_w(__sdb_opt, ...) ({\
        if ((__sdb_opt & SDB_IO) && ((~__sdb_opt) & SDB_NO_WARNING)) {\
        sdb_out_style(__sdb_opt, SDB_MODE_MARK_WARNING,\
                __FILE__, __func__, __LINE__, __VA_ARGS__);\
        }})
/** \brief      带标签的格式化输出，带错误标记 */
#define sdb_out_e(__sdb_opt, ...) ({\
        if ((__sdb_opt & SDB_IO) && ((~__sdb_opt) & SDB_NO_ERROR)) {\
        sdb_out_style(__sdb_opt, SDB_MODE_MARK_ERROR,\
                __FILE__, __func__, __LINE__, __VA_ARGS__);\
        }})
/** \brief      带标签的格式化输出，标题高亮 */
#define sdb_out_t(__sdb_opt, ...) ({\
        if ((__sdb_opt & SDB_IO) && ((~__sdb_opt) & SDB_NO_INFO)) {\
        sdb_out_style(__sdb_opt, SDB_MODE_HL_TITLE,\
                __FILE__, __func__, __LINE__, __VA_ARGS__);\
        }})
/** \brief      带标签的入口标记输出 */
#define sdb_out_entry(__sdb_opt) ({\
        if ((__sdb_opt & SDB_IO) && ((~__sdb_opt) & SDB_NO_INFO)) {\
        sdb_out_style(__sdb_opt, SDB_MODE_HL_SIGN,\
                __FILE__, __func__, __LINE__, ">>> %s {", __func__);\
        }})
/** \brief      带标签的出口标记输出 */
#define sdb_out_exit(__sdb_opt) ({\
        if ((__sdb_opt & SDB_IO) && ((~__sdb_opt) & SDB_NO_INFO)) {\
        sdb_out_style(__sdb_opt, SDB_MODE_HL_SIGN,\
                __FILE__, __func__, __LINE__, "<<< %s }", __func__);\
        }})

#define sdb_err(__sdb_opt, ...)
#define sdb_err_i(__sdb_opt, ...)
#define sdb_err_w(__sdb_opt, ...)
#define sdb_err_e(__sdb_opt, ...)
#define sdb_err_t(__sdb_opt, ...)

#else /* defined(SDB_ENABLE) */

#define sdb_bio_conf(...)

#define sdb_out(...)
#define sdb_out_i(...)
#define sdb_out_w(...)
#define sdb_out_e(...)
#define sdb_out_t(...)
#define sdb_out_entry(...)
#define sdb_out_exit(...)

#define sdb_err(...)
#define sdb_err_i(...)
#define sdb_err_w(...)
#define sdb_err_e(...)
#define sdb_err_t(...)

#endif /* defined(SDB_ENABLE) */

/** @} */

#define sdb_in()
#define sdb_in_n(...)
#define sdb_in_s(...)
#define sdb_in_nt(...)
#define sdb_in_st(...)

#define sdb_dmp(...)
#define sdb_dmp_h(...)
#define sdb_dmp_hc(...)
#define sdb_dmp_hca(...)
#define sdb_dmp_ht(...)
#define sdb_dmp_hct(...)
#define sdb_dmp_hcat(...)

#define sdb_menu(...)


/**
 * \block:      Code is far away from bug with the animal protecting
 * @{ */
/** normal {
 *     ┏━┓       ┏━┓
 *   ┏━┛ ┻━━━━━━━┛ ┻━┓
 *   ┃               ┃
 *   ┃      ━        ┃
 *   ┃ ┳━┛     ┗━┳   ┃
 *   ┃               ┃
 *   ┃     ┻         ┃
 *   ┃               ┃
 *   ┗━━━┳━      ┏━━━┛
 *       ┃       ┃
 *       ┃       ┃    神兽护体, 代码永无bug
 *       ┃       ┗━━━━━━━┓
 *       ┃               ┣━┓
 *       ┃               ┏━┛
 *       ┗━┓ ┓ ┏━━━┳━┓ ┏━┛
 *         ┃ ┫ ┫   ┃ ┫ ┫
 *         ┗━┻━┛   ┗━┻━┛
}*/
/** happy {
 *     ┏━┓       ┏━┓
 *   ┏━┛ ┻━━━━━━━┛ ┻━┓
 *   ┃               ┃
 *   ┃      ━        ┃
 *   ┃  >       <    ┃
 *   ┃               ┃
 *   ///   v   ///   ┃
 *   ┃               ┃
 *   ┗━━━┳━      ┏━━━┛
 *       ┃       ┃
 *       ┃       ┃    神兽护体, 代码永无bug
 *       ┃       ┗━━━━━━━┓
 *       ┃               ┣━┓
 *       ┃               ┏━┛
 *       ┗━┓ ┓ ┏━━━┳━┓ ┏━┛
 *         ┃ ┫ ┫   ┃ ┫ ┫
 *         ┗━┻━┛   ┗━┻━┛
}*/
/** cool {
 *     ┏━┓       ┏━┓  + +
 *   ┏━┛ ┻━━━━━━━┛ ┻━┓  + +
 *   ┃               ┃
 *   ┃      ━        ┃  + + + + +
 *   #####--#####    ┃  +
 *   ┃               ┃  +
 *   ┃     ┻         ┃
 *   ┃               ┃  + +
 *   ┗━━━┳━      ┏━━━┛
 *       ┃       ┃  + + + +
 *       ┃       ┃  + 神兽护体, 代码永无bug
 *       ┃       ┗━━━━━━━┓  + +
 *       ┃               ┣━┓
 *       ┃               ┏━┛
 *       ┗━┓ ┓ ┏━━━┳━┓ ┏━┛  + + + +
 *         ┃ ┫ ┫   ┃ ┫ ┫
 *         ┗━┻━┛   ┗━┻━┛  + + + +
}*/
/** @} */

