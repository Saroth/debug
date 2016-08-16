#ifndef __SDB_CONFIG_H__
#define __SDB_CONFIG_H__

/**
 * \block:      Modules
 * @{ */
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

/**
 * 启用终端输入获取
 *
 * Caller: menu.c
 *
 * 如果注释该选项，输入模块和接口调用将不被编译
 */
#define SDB_INPUT_ENABLE

/**
 * 启用数据导出
 *
 * 如果注释该选项，数据导出模块和接口调用将不被编译
 */
#define SDB_DUMP_ENABLE

/**
 * 启用测试菜单
 *
 *  Required:   input.c
 *
 * 如果注释该选项 或选项 SDB_INPUT_ENABLE 未设置，测试器和接口调用将不被编译
 */
#define SDB_MENU_ENABLE

/**
 * 模块自检
 *
 * 如果注释该选项，自检测试程序将不被编译
 */
// #define SDB_SELFTEST
/** @} */

/**
 * \block:      Configs
 * @{ */
/**
 * 启用终端文本颜色
 *
 * 使用Linux的ANSI颜色控制序列
 */
#define SDB_HAVE_COLOR

/**
 * 支持通过time()函数获取时间
 *
 * 如果运行环境不支持时间获取或无法获取有效时间，则需要注释该选项
 */
#define SDB_HAVE_TIME

/**
 * 支持标准输入输出
 *
 * 如果运行环境不支持printf输出和scanf输入，则注释该选项
 */
#define SDB_HAVE_STDIO

/**
 * 系统支持多线程
 *
 * 多线程系统环境下，需要对输出信息进行互斥
 */
#define SDB_HAVE_THREAD

/** @} */

/**
 * 模块内部的全局缓存大小
 */
#define SDB_CACHE_SIZE                    0x1000

/**
 * 换行符类型
 */
#define SDB_WRAP_CHARACTER      "\n"

/// 输出颜色定义
#define SDB_COLOR_RES           "\33[0m"        //!< 恢复
#define SDB_COLOR_INFO          "\33[1m"        //!< 提示高亮
#define SDB_COLOR_WARN          "\33[1;36m"     //!< 警告高亮
#define SDB_COLOR_ERR           "\33[1;91m"     //!< 错误高亮
#define SDB_COLOR_HL            "\33[7m"        //!< 反白高亮
#define SDB_COLOR_SYMBOL        "\33[1;92m"     //!< 输入输出标记高亮

/**
 * 版本号格式: 0x00AAIISS.
 *      A:Major
 *      I:Minor
 *      S:Status, 0~0xFE:Beta, 0xFF:Release
 */
#define SDB_VERSION_NUMBER      0x00020000

#if defined(SDB_HAVE_TIME)
#include <time.h>
#endif
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
// #include <errno.h>

#include "libsdb.h"

#define DS_SDB  (SDB_IO | SDB_LINE | SDB_FILE | SDB_NO_INFO)

#endif /* __SDB_CONFIG_H__ */

