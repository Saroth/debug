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
    SDB_LINE            = (1 << 1),     //!< 标签 - 行号
    SDB_FILE            = (1 << 2),     //!< 标签 - 文件名
    SDB_FUNC            = (1 << 3),     //!< 标签 - 函数名
    SDB_TIME            = (1 << 4),     //!< 标签 - 时间戳

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

/*
 * 接口命名规则: sdb_<module>[_<flag>]
 */

#if defined(SDB_ENABLE)

/**
 * \brief       基本输出接口
 * \param       buf         输出数据的缓存
 * \param       len         输出长度
 * \return      0: Success      <0: SDB_RET_T
 */
int sdb_bio_out(char * buf, size_t len);

/**
 * \brief       基本输入接口
 * \param       buf         获取输入数据的缓存
 * \param       buflen      缓存大小
 * \param       outlen      输出数据长度
 * \return      0: Success      <0: SDB_RET_T
 * \detail      以回车或^D结束，返回结果包含换行符
 */
int sdb_bio_in(char * buf, size_t buflen, size_t *outlen);

#define sdb_conf(...)

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

#else

#define sdb_conf(...)

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

#endif


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

