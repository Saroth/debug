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

    SDB_NO_LINE         = (1 << 1),     //!< 标签 - 不输出行号
    SDB_NO_FILE         = (1 << 2),     //!< 标签 - 不输出文件名
    SDB_FUNC            = (1 << 3),     //!< 标签 - 输出函数名，默认关闭
    SDB_TIME            = (1 << 4),     //!< 标签 - 输出时间戳，默认关闭

    SDB_NO_INFO         = (1 << 8),     //!< 不输出所有信息
    SDB_NO_WARNING      = (1 << 9),     //!< 不输出所有警告
    SDB_NO_ERROR        = (1 << 10),    //!< 不输出所有错误
    SDB_NO_TITLE        = (1 << 11),    //!< 不输出所有title信息
    SDB_NO_DUMP         = (1 << 12),    //!< 不输出所有dump信息
    SDB_NO_STDERR       = (1 << 13),    //!< 不输出所有标准错误信息
    SDB_NO_COLOR        = (1 << 14),    //!< 不输出所有高亮色
    SDB_NO_SYMBOL       = (1 << 15),    //!< 不输出所有指示标记
    SDB_NO_LABLE        = (1 << 16),    //!< 不输出所有标签
} SDB_IO_OPTION_T;

typedef enum {                          //!< 返回值
    SDB_RET_OK              = 0,        //!< 成功
    SDB_RET_PARAM_ERR       = -11,      //!< 参数错误
    SDB_RET_NO_INPUT        = -12,      //!< 无输入
    SDB_RET_UNKNOWN_INPUT   = -13,      //!< 未知输入
    SDB_RET_ALLOC_FAILED    = -14,      //!< 内存申请失败
} SDB_RET_T;

/// 输出颜色定义
#define SDB_COLOR_RES           "\33[0m"    //!< 恢复: normal
#define SDB_COLOR_SIGN          "\33[1m"    //!< 特殊标记高亮: bold
#define SDB_COLOR_WARN          "\33[1;36m" //!< 警告高亮: blue, bold
#define SDB_COLOR_ERR           "\33[1;31m" //!< 错误高亮: red, bold
#define SDB_COLOR_TITLE         "\33[7m"    //!< 标题高亮: inverse
#define SDB_COLOR_INPUT         "\33[1;32m" //!< 输入和反馈标记高亮: green, bold
#define SDB_COLOR_LABEL         "\33[1;30m" //!< 标签高亮, black, bold

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
        sdb_out_style(__sdb_opt, 0, __FILE__, __func__, __LINE__, __VA_ARGS__);\
        }})\
/** \brief      带标签的格式化输出 */
#define sdb_out_i(__sdb_opt, ...) ({\
        if ((__sdb_opt & SDB_IO) && ((~__sdb_opt) & SDB_NO_INFO)) {\
        sdb_out_style(__sdb_opt, 0, __FILE__, __func__, __LINE__, __VA_ARGS__);\
        }})\

#define sdb_out_w(__sdb_opt, ...)
#define sdb_out_e(__sdb_opt, ...)
#define sdb_out_t(__sdb_opt, ...)
#define sdb_out_entry(__sdb_opt)
#define sdb_out_exit(__sdb_opt)

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

