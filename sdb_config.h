#ifndef __SDB_CONFIG_H__
#define __SDB_CONFIG_H__

/** 启用调试器 */
#define SDB_ENABLE

/**
 * \block:      Modules
 * @{ */
/** 启用终端输入获取 */
#define SDB_MDL_GET_ENABLE

/** 启用数据导出 */
#define SDB_MDL_DUMP_ENABLE

/** 启用测试菜单 */
// #define SDB_MDL_MENU_ENABLE

/** 启用栈调试 */
#define SDB_STACK_WATCH
/** @} */

/**
 * \block:      System environment
 * @{ */
/** 启用终端文本颜色, 使用Linux的ANSI颜色控制序列 */
#define SDB_SYS_SUPPORT_ANSI_COLOR_SEQUENCES

/** 启用标准输入输出, 如果运行环境不支持printf输出和scanf输入，则注释该选项 */
#define SDB_SYS_SUPPORT_STDIO

/** 启用标准错误信息提示 */
#define SDB_SYS_SUPPORT_STDERR

/** 系统支持size_t */
#define SDB_SYS_SUPPORT_SIZE_T

/** 系统支持long类型, 通常32位平台下长度为4, 64位平台下长度为8 */
#define SDB_SYS_SUPPORT_LONG

/** 系统支持long long类型, 通常长度为8 */
#define SDB_SYS_SUPPORT_LONG_LONG

/** 系统有较大内存空间. 关闭后将减少栈空间使用, 并移除部分提示信息 */
// #define SDB_SYS_SUPPORT_LARGE_MEM

/** @} */

/**
 * \block:      Configuration
 * @{ */
/** 内部输出缓存大小 */
#define SDB_CONF_BUFFER_SIZE    288

/** 内部获取数值的输入缓存 */
#define SDB_CONF_BUFFER_SIZE_GETNUM 32

/// 输出颜色定义
#define SDB_COLOR_RES           "\33[0m"    /* 恢复: normal */
#define SDB_COLOR_WARN          "\33[1;36m" /* 警告高亮: blue, bold */
#define SDB_COLOR_ERR           "\33[1;31m" /* 错误高亮: red, bold */
#define SDB_COLOR_INPUT         "\33[1;32m" /* 输入和反馈标记高亮: green, bold */
#define SDB_COLOR_HL            "\33[1m"    /* 特殊标记高亮: bold */
#define SDB_COLOR_TITLE         "\33[7m"    /* 标题高亮: inverse */
#define SDB_COLOR_LABEL         "\33[1;30m" /* 标签高亮, black, bold */
// 标记定义
#define SDB_MARK_NONE           "    "      /* 无标记 */
#define SDB_MARK_INFO           " .  "      /* 提示标记 */
#define SDB_MARK_WARN           " !  "      /* 警告标记 */
#define SDB_MARK_ERR            " x  "      /* 错误标记 */
#define SDB_MARK_GETNUM         "I.n "      /* 获取数值输入标记 */
#define SDB_MARK_GETSTR         "I.s "      /* 获取字符串输入标记 */
#define SDB_MARK_ECHO           "I.e "      /* 返回数值输出标记 */
#define SDB_MARK_DUMP           "  | "      /* 导出标记 */
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
#define SDB_VERSION_NUMBER      0x00020001

#include "libsdb.h"
#include "sdb_internal.h"

#endif /* __SDB_CONFIG_H__ */

