#ifndef __SDB_CONFIG_H__
#define __SDB_CONFIG_H__

/**
 * 启用调试器
 */
#define SDB_ENABLE

/**
 * \block:      Modules
 * @{ */
/**
 * 启用终端输入获取
 * \Caller:     menu.c
 */
#define SDB_MDL_INPUT_ENABLE

/**
 * 启用数据导出
 * \Caller:     dump.c
 */
#define SDB_MDL_DUMP_ENABLE

/**
 * 启用测试菜单
 * \Required:   SDB_MDL_INPUT_ENABLE
 * \Caller:     input.c
 */
#define SDB_MDL_MENU_ENABLE
/** @} */

/**
 * \block:      System environment
 * @{ */
/**
 * 启用终端文本颜色
 * 使用Linux的ANSI颜色控制序列
 */
#define SDB_SYS_HAVE_COLOR

/**
 * 启用标准输入输出
 * 如果运行环境不支持printf输出和scanf输入，则注释该选项
 */
#define SDB_SYS_HAVE_STDIO

/**
 * 启用标准错误信息提示
 */
#define SDB_SYS_HAVE_STDERR
/** @} */

/**
 * \block:      Configuration
 * @{ */
/**
 * 内部输出缓存大小
 */
#define SDB_CONF_BUFFER_SIZE    288

/*
 * 内部获取数值的输入缓存
 */
#define SDB_CONF_BUFFER_SIZE_GETNUM 32

/// 输出颜色定义
#define SDB_COLOR_RES           "\33[0m"    //!< 恢复: normal
#define SDB_COLOR_WARN          "\33[1;36m" //!< 警告高亮: blue, bold
#define SDB_COLOR_ERR           "\33[1;31m" //!< 错误高亮: red, bold
#define SDB_COLOR_INPUT         "\33[1;32m" //!< 输入和反馈标记高亮: green, bold
#define SDB_COLOR_HL            "\33[1m"    //!< 特殊标记高亮: bold
#define SDB_COLOR_TITLE         "\33[7m"    //!< 标题高亮: inverse
#define SDB_COLOR_LABEL         "\33[1;30m" //!< 标签高亮, black, bold
// 标记定义
#define SDB_MARK_NONE           "    "      //!< 无标记
#define SDB_MARK_INFO           " .  "      //!< 提示标记
#define SDB_MARK_WARN           " !  "      //!< 警告标记
#define SDB_MARK_ERR            " x  "      //!< 错误标记
#define SDB_MARK_GETNUM         "I.n "      //!< 获取数值输入标记
#define SDB_MARK_GETSTR         "I.s "      //!< 获取字符串输入标记
#define SDB_MARK_ECHO           "I.e "      //!< 返回数值输出标记
#define SDB_MARK_DUMP           "  | "      //!< 导出标记

/** @} */

/**
 * 模块自检
 */
// #define SDB_MDL_SELFTEST

/**
 * 版本号格式: 0x00AAIISS.
 *      A:Major
 *      I:Minor
 *      S:Status, 0 .. 0xFE:Beta, 0xFF:Release
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

#endif /* __SDB_CONFIG_H__ */

