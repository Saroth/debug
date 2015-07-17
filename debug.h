/** {
 * Code is far away from bug with the animal protecting.
 *     ┏┓     ┏┓
 *    ┏┛┻━━━━━┛┻┓
 *    ┃         ┃
 *    ┃    ━    ┃
 *    ┃ ┳┛   ┗┳ ┃
 *    ┃         ┃
 *    ┃    ┻    ┃
 *    ┗━┓     ┏━┛
 *      ┃     ┃神兽保佑, 远离BUG!
 *      ┃     ┗━━━┓
 *      ┃         ┣┓
 *      ┃         ┏┛
 *      ┗┓┓┏━━━┳┓┏┛
 *       ┃┫┫   ┃┫┫
 *       ┗┻┛   ┗┻┛
}*/
/**Copyright(C) For free. All right reserved.
 */
/**
 * \file        debug.h
 * \brief       调试模块
 * \author      huanglf
 * \version     1.1
 * \date        2015-06-06
 */


#ifndef __DEBUG_H__
#define __DEBUG_H__

// #include "config.h"

#define DS_DEBUG_MAIN                   //!< 调试模块总开关

/// 功能配置
#define DBG_USE_COLOR                   //!< 使用带颜色输出
#define DBG_USE_LOG                     //!< 使用日志功能
#define DBG_USE_DUMP                    //!< 使用数据导出
#define DBG_NL_HEAD                     //!< 换行符放在开头

#ifndef BUFFER_SIZE
#define BUFFER_SIZE         4096        //!< 数据输入输出缓存大小
#endif /* BUFFER_SIZE */

/// 模块调试开关
#define DS_OUT              0           //!< 输出
#define DS_OUT_ERR          1           //!< 输出错误
#define DS_IN               0           //!< 输入
#define DS_IN_ERR           1           //!< 输入错误
#define DS_LOG              0           //!< 日志
#define DS_LOG_ERR          1           //!< 日志错误
#define DS_DUMP             0           //!< 数据导出
#define DS_DUMP_ERR         1           //!< 数据导出错误
#define DS_TEST             0           //!< 测试
#define DS_TEST_ERR         1           //!< 测试错误

#include "debug_bio.h"
#include "debug_out.h"
#include "debug_in.h"
#include "debug_log.h"
#include "debug_dump.h"
#include "debug_test.h"

/**
 * \block:      STDOUT
 * @{ */
#ifdef DS_DEBUG_MAIN
/** \brief      格式化输出 */
#define dbg_out(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout(__VA_ARGS__); \
    } \
}
/** \brief      带标签信息的格式化输出，显示为提示高亮 */
#define dbg_out_I(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__func__, __LINE__, \
                DBG_LABEL_COLOR | DBG_LABEL_COL_INFO | DBG_LABEL_INFO \
                | DBG_LABEL_NEWLINE, __VA_ARGS__); \
    } \
}
/** \brief      带标签信息的格式化输出，显示为警告高亮 */
#define dbg_out_W(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__func__, __LINE__, \
                DBG_LABEL_COLOR | DBG_LABEL_COL_WARN | DBG_LABEL_WARN \
                | DBG_LABEL_NEWLINE, __VA_ARGS__); \
    } \
}
/** \brief      带标签信息的格式化输出，显示为错误高亮 */
#define dbg_out_E(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__func__, __LINE__, \
                DBG_LABEL_COLOR | DBG_LABEL_COL_ERR | DBG_LABEL_ERR \
                | DBG_LABEL_NEWLINE, __VA_ARGS__); \
    } \
}
/** \brief      带标签信息的格式化输出，显示为高亮 */
#define dbg_out_H(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__func__, __LINE__, \
                DBG_LABEL_COLOR | DBG_LABEL_COL_HL | DBG_LABEL_INFO \
                | DBG_LABEL_NEWLINE, __VA_ARGS__); \
    } \
}
/** \brief      带标签信息的格式化输出, 附带错误信息 */
#define dbg_outerr_I(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__func__, __LINE__, \
                DBG_LABEL_COLOR | DBG_LABEL_COL_ERR | DBG_LABEL_ERR \
                | DBG_LABEL_STDERR | DBG_LABEL_NEWLINE, __VA_ARGS__); \
    } \
}
/** \brief      带标签信息和时间的格式化输出，显示为提示高亮 */
#define dbg_out_IT(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__func__, __LINE__, \
                DBG_LABEL_TIME | DBG_LABEL_COL_INFO | DBG_LABEL_COLOR \
                | DBG_LABEL_INFO | DBG_LABEL_NEWLINE, __VA_ARGS__); \
    }\
}
/** \brief      带标签信息和时间的格式化输出，显示为警告高亮 */
#define dbg_out_WT(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__func__, __LINE__, \
                DBG_LABEL_TIME | DBG_LABEL_COL_WARN | DBG_LABEL_COLOR \
                | DBG_LABEL_WARN | DBG_LABEL_NEWLINE, __VA_ARGS__); \
    }\
}
/** \brief      带标签信息和时间的格式化输出，显示为错误高亮 */
#define dbg_out_ET(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__func__, __LINE__, \
                DBG_LABEL_TIME | DBG_LABEL_COL_ERR | DBG_LABEL_COLOR \
                | DBG_LABEL_ERR | DBG_LABEL_NEWLINE, __VA_ARGS__); \
    } \
}
/** \brief      带标签信息和时间的格式化输出，显示为高亮 */
#define dbg_out_HT(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__func__, __LINE__, \
                DBG_LABEL_TIME | DBG_LABEL_COL_HL | DBG_LABEL_COLOR \
                | DBG_LABEL_INFO | DBG_LABEL_NEWLINE, __VA_ARGS__); \
    } \
}
/** \brief      带标签信息和时间的格式化输出, 附带错误信息 */
#define dbg_outerr_IT(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__func__, __LINE__, \
                DBG_LABEL_TIME | DBG_LABEL_COL_ERR | DBG_LABEL_COLOR \
                | DBG_LABEL_ERR | DBG_LABEL_STDERR | DBG_LABEL_NEWLINE, \
                __VA_ARGS__); \
    } \
}
#else
#define dbg_out(...)
#define dbg_out_I(...)
#define dbg_out_W(...)
#define dbg_out_E(...)
#define dbg_out_H(...)
#define dbg_outerr_I(...)
#define dbg_out_IT(...)
#define dbg_out_WT(...)
#define dbg_out_ET(...)
#define dbg_out_HT(...)
#define dbg_outerr_IT(...)
#endif /* DS_DEBUG_MAIN */
/** @} */
/**
 * \block:      STDIN
 * @{ */
#ifdef DS_DEBUG_MAIN
/** \brief      返回输入的数值，如果输入值不可转换为数字则返回-1 */
#define dbg_in() ({ \
        dbg_stdin_label(__func__, __LINE__, DBG_STDIN_RETNUM, 0, 0); \
        })
/** \brief      获取输入的数值，可识别8进制、10进制和16进制 */
#define dbg_in_N(pnum) ({ \
        dbg_stdin_label(__func__, __LINE__, DBG_STDIN_GETNUM, pnum, 0); \
        })
/** \brief      获取输入的字符串，移除换行符 */
#define dbg_in_S(pstr, len) ({ \
        dbg_stdin_label(__func__, __LINE__, DBG_STDIN_GETSTR, pstr, len); \
        })
#else
#define dbg_in() 0
#define dbg_in_N(...)
#define dbg_in_S(...)
#endif /* DS_DEBUG_MAIN */
/** @} */
/**
 * \block:      LOG
 * @{ */
#if defined(DBG_USE_LOG) && defined(DS_DEBUG_MAIN)
/** \brief      设置日志文件名 */
#define dbg_log_setname(filename) { \
    dbg_log_open(filename); \
}
/** \brief      关闭日志调试 */
#define dbg_log_off() { \
    dbg_log_setmode(DBG_P); \
}
/** \brief      打开日志调试 */
#define dbg_log_on() { \
    dbg_log_setmode(DBG_PW); \
}
/** \brief      关闭输出，打开日志调试 */
#define dbg_log_only() { \
    dbg_log_setmode(DBG_W); \
}
/** \brief      开启日志调试，同步写入 */
#define dbg_log_on_s() { \
    dbg_log_setmode(DBG_PW | DBG_S); \
}
/** \brief      关闭输出，打开日志调试，同步写入 */
#define dbg_log_only_s() { \
    dbg_log_setmode(DBG_W | DBG_S); \
}
#else
#define dbg_log_setname(...) { \
    dbg_out_E(1, "DBG_USE_LOG undefine!"); \
}
#define dbg_log_off()
#define dbg_log_on()
#define dbg_log_only()
#define dbg_log_on_s()
#define dbg_log_only_s()
#endif /* defined(DBG_USE_LOG) && defined(DS_DEBUG_MAIN) */
/** @} */
/**
 * \block:      DUMP
 * @{ */
#if defined(DS_DEBUG_MAIN) && defined(DBG_USE_DUMP)
/** \brief      16进制导出, 带行号, 每行16字节 */
#define dbg_dmp_H(__debug_switch__, buf, len) { \
    if(__debug_switch__) { \
        dbg_dump_label(__func__, __LINE__, buf, len, 0, \
                DBG_DMP_SEG_16 | DBG_DMP_TAG_LINE | DBG_DMP_DAT_HEX, NULL); \
    } \
}
/** \brief      16进制导出, 同时输出字符, 带行号, 每行16字节 */
#define dbg_dmp_HC(__debug_switch__, buf, len) { \
    if(__debug_switch__) { \
        dbg_dump_label(__func__, __LINE__, buf, len, 0, \
                DBG_DMP_SEG_16 | DBG_DMP_TAG_LINE \
                | DBG_DMP_DAT_HEX | DBG_DMP_DAT_CHAR, NULL); \
    } \
}
/** \brief      16进制导出, 同时输出字符, 带地址, 每行16字节 */
#define dbg_dmp_HCA(__debug_switch__, buf, len, addr) { \
    if(__debug_switch__) { \
        dbg_dump_label(__func__, __LINE__, buf, len, addr, \
                DBG_DMP_SEG_16 | DBG_DMP_TAG_ADDR \
                | DBG_DMP_DAT_HEX | DBG_DMP_DAT_CHAR, NULL); \
    } \
}
/** \brief      16进制导出, 带标签和行号, 每行16字节 */
#define dbg_dmp_HL(__debug_switch__, buf, len, label) { \
    if(__debug_switch__) { \
        dbg_dump_label(__func__, __LINE__, buf, len, 0, \
                DBG_DMP_SEG_16 | DBG_DMP_TAG_LINE | DBG_DMP_DAT_HEX\
                | DBG_DMP_TAG_LABEL, label); \
    } \
}
/** \brief      16进制导出, 同时输出字符, 带标签和行号, 每行16字节 */
#define dbg_dmp_HCL(__debug_switch__, buf, len, label) { \
    if(__debug_switch__) { \
        dbg_dump_label(__func__, __LINE__, buf, len, 0, \
                DBG_DMP_SEG_16 | DBG_DMP_TAG_LINE | DBG_DMP_DAT_HEX\
                | DBG_DMP_DAT_CHAR | DBG_DMP_TAG_LABEL, label); \
    } \
}
/** \brief      16进制导出, 同时输出字符, 带标签和地址, 每行16字节 */
#define dbg_dmp_HCAL(__debug_switch__, buf, len, addr, label) { \
    if(__debug_switch__) { \
        dbg_dump_label(__func__, __LINE__, buf, len, addr, \
                DBG_DMP_SEG_16 | DBG_DMP_TAG_ADDR | DBG_DMP_DAT_HEX\
                | DBG_DMP_DAT_CHAR | DBG_DMP_TAG_LABEL, label); \
    } \
}
/** \brief      纯字符导出, 无任何格式(可用于将内存或Flash数据导出到文件) */
#define dbg_dmp_C(__debug_switch__, buf, len) { \
    if(__debug_switch__) { \
        dbg_dump_label(__func__, __LINE__, buf, len, 0, 0, NULL); \
    } \
}
#else
#define dbg_dmp_H(...)
#define dbg_dmp_HC(...)
#define dbg_dmp_HCA(...)
#define dbg_dmp_HL(...)
#define dbg_dmp_HCL(...)
#define dbg_dmp_HCAL(...)
#define dbg_dmp_C(...)
#endif /* defined(DS_DEBUG_MAIN) && defined(DBG_USE_DUMP) */
/** @} */
/**
 * \block:      TEST
 * @{ */
#ifdef DS_DEBUG_MAIN
/** \brief      设置测试函数列表并加载 */
#define dbg_test_setlist(...) { \
    DBG_TESTLIST_T list[] = { __VA_ARGS__ }; \
    dbg_testlist(list, (sizeof(list) / sizeof(DBG_TESTLIST_T))); \
}
#else
#define dbg_test_setlist(...)
#endif /* DS_DEBUG_MAIN */
/** @} */


#endif /* __DEBUG_H__ */

