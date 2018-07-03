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

#define SDB_CONFIG_COLUMN_LIMIT_DEF         88
#define SDB_CONFIG_COLUMN_LIMIT_MINIMUM     36
#define SDB_CONFIG_OUTPUT_BUFFER_RESERVE    64  /* for invisible characters */
#define SDB_CONFIG_INPUT_BUFFER_SIZE        32
#define SDB_CONFIG_DUMP_BYTE_PERLINE_DEF    16
#define SDB_CONFIG_DUMP_BYTE_PERLINE_MINIMUM    4

#define SDB_NULL_MARK           "(null)"

#endif /* __SDB_CONFIG_H__ */

