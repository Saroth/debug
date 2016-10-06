#ifndef __SDB_CONFIG_H__
#define __SDB_CONFIG_H__

/**
 * 启用调试器
 *
 * 调试器包含如下功能模块
 *  - output.c
 *  - input.c
 *  - dump.c
 *  - menu.c
 * 如果注释该选项，所有模块接口和调用将不被编译
 */
#define SDB_ENABLE

// #define SDB_SLIM

/**
 * \block:      Modules
 * @{ */

/**
 * 启用标准错误信息提示
 */
#define SDB_MDL_STDERR_ENABLE

/**
 * 启用终端输入获取
 *
 * \Caller: menu.c
 *
 * 如果注释该选项，输入模块和接口调用将不被编译
 */
#define SDB_MDL_INPUT_ENABLE

/**
 * 启用数据导出
 *
 * 如果注释该选项，数据导出模块和接口调用将不被编译
 */
#define SDB_MDL_DUMP_ENABLE

/**
 * 启用测试菜单
 *
 * \Required:   input.c
 *
 * 如果注释该选项 或选项 SDB_MDL_INPUT_ENABLE 未设置，测试器和接口调用将不被编译
 */
#define SDB_MDL_MENU_ENABLE

/**
 * 模块自检
 */
// #define SDB_MDL_SELFTEST
/** @} */

/**
 * \block:      System environment
 * @{ */
/**
 * 启用终端文本颜色
 *
 * 使用Linux的ANSI颜色控制序列
 */
#define SDB_SYS_HAVE_COLOR

/**
 * 支持通过time()函数获取时间
 *
 * 如果运行环境不支持时间获取或无法获取有效时间，则需要注释该选项
 */
#define SDB_SYS_HAVE_TIME

/**
 * 支持标准输入输出
 *
 * 如果运行环境不支持printf输出和scanf输入，则注释该选项
 */
#define SDB_SYS_HAVE_STDIO

/** @} */

/**
 * \block:      Configuration
 * @{ */
/**
 * 模块内部的输入输出缓存大小
 */
#define SDB_CONF_IO_CACHE_SIZE  0x100

/**
 * 换行符类型
 */
#define SDB_CONF_WRAP   "\n"

/**
 * 标签预留间隔
 * Label信息总长度少于设定值时，填充部分空格。
 * 时间戳长度固定为: 12字符 + 1空格。
 */
#define SDB_CONF_FILE_NAME_SPACE    16
#define SDB_CONF_FUNC_NAME_SPACE    24
#define SDB_CONF_LINE_NUM_SPACE     8

/// 输出颜色定义
#define SDB_COLOR_RES           "\33[0m"    //!< 恢复: normal
#define SDB_COLOR_HL            "\33[1m"    //!< 特殊标记高亮: bold
#define SDB_COLOR_WARN          "\33[1;36m" //!< 警告高亮: blue, bold
#define SDB_COLOR_ERR           "\33[1;31m" //!< 错误高亮: red, bold
#define SDB_COLOR_TITLE         "\33[7m"    //!< 标题高亮: inverse
#define SDB_COLOR_INPUT         "\33[1;32m" //!< 输入和反馈标记高亮: green, bold
#define SDB_COLOR_LABEL         "\33[1;30m" //!< 标签高亮, black, bold
// 标记定义
#define SDB_MARK_NONE           "    "      //!< 无标记
#define SDB_MARK_INFO           " .  "      //!< 提示标记
#define SDB_MARK_WARN           " !  "      //!< 警告标记
#define SDB_MARK_ERR            " x  "      //!< 错误标记
#define SDB_MARK_GETNUM         "I.n "      //!< 获取数值输入标记
#define SDB_MARK_GETSTR         "I.s "      //!< 获取字符串输入标记
#define SDB_MARK_ECHO           "I.e "      //!< 返回数值输出标记
#define SDB_MARK_ENTRY          ">>> "      //!< 入口标记
#define SDB_MARK_EXIT           "<<< "      //!< 出口标记
#define SDB_MARK_DUMP           " -  "      //!< 导出标记

/** @} */

/**
 * 版本号格式: 0x00AAIISS.
 *      A:Major
 *      I:Minor
 *      S:Status, 0~0xFE:Beta, 0xFF:Release
 */
#define SDB_VERSION_NUMBER      0x00020000

#if defined(SDB_SYS_HAVE_TIME)
#include <time.h>
#endif
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <errno.h>

#include "libsdb.h"
#include "sdb_internal.h"

#define DS_SDB  (SDB_IO)

#endif /* __SDB_CONFIG_H__ */

