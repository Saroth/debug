#ifndef __SDB_CONFIG_H__
#define __SDB_CONFIG_H__

/**
 * 版本号格式: 0x00AAIIPP.
 *      AA: Major
 *      II: Minor
 *      PP: Patch
 */
#define SDB_VERSION_NUMBER      0x00020001

#define SDB_SYSTEM_HAS_ANSI_COLOR_SEQUENCES
#define SDB_SYSTEM_HAS_STDIO
#define SDB_SYSTEM_HAS_STDERR
#define SDB_SYSTEM_HAS_LARGE_MEMORY
#define SDB_SYSTEM_HAS_SIZE_T
#define SDB_SYSTEM_HAS_LONG
#define SDB_SYSTEM_HAS_LONG_LONG

#define SDB_CONFIG_COLUMN_LIMIT_DEF             88
#define SDB_CONFIG_COLUMN_LIMIT_MINIMUM         36
#define SDB_CONFIG_OUTPUT_BUFFER_RESERVE        36
#define SDB_CONFIG_INPUT_BUFFER_SIZE            32
#define SDB_CONFIG_DUMP_BYTE_PERLINE_DEF        16
#define SDB_CONFIG_DUMP_BYTE_PERLINE_MINIMUM    4

#define SDB_NULL_MARK           "(null)"

#define SDB_COLOR_RECOVERY      "\33[0m"    /* 恢复: normal */
#define SDB_COLOR_WARNING       "\33[1;36m" /* 警告高亮: blue, bold */
#define SDB_COLOR_ERROR         "\33[1;31m" /* 错误高亮: red, bold */
#define SDB_COLOR_INPUT         "\33[1;32m" /* 输入和反馈标记高亮: green, bold */
#define SDB_COLOR_HIGHLIGHT     "\33[1m"    /* 特殊标记高亮: bold */
#define SDB_COLOR_TITLE         "\33[7m"    /* 标题高亮: inverse */
#define SDB_COLOR_LABEL         "\33[1;30m" /* 标签高亮, black, bold */

#define SDB_MARK_NONE           "    "      /* 无标记 */
#define SDB_MARK_INFO           " .  "      /* 提示标记 */
#define SDB_MARK_WARNING        " !  "      /* 警告标记 */
#define SDB_MARK_ERROR          " x  "      /* 错误标记 */
#define SDB_MARK_GETNUM         "i.n "      /* 获取数值输入标记 */
#define SDB_MARK_GETSTRING      "i.s "      /* 获取字符串输入标记 */
#define SDB_MARK_ECHO           "i.e "      /* 返回数值输出标记 */
#define SDB_MARK_DUMP           "  | "      /* 导出标记 */
#define SDB_MARK_MENU           "   #"      /* 菜单标记 */

#endif /* __SDB_CONFIG_H__ */

