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

#ifndef DS_DEBUG_MAIN
#define DS_DEBUG_MAIN                   //!< 调试模块总开关
#endif /* DS_DEBUG_MAIN */

#ifndef BUFFER_SIZE
#define BUFFER_SIZE         4096        //!< 数据输入输出缓存大小
#endif /* BUFFER_SIZE */

/// 功能配置
#define DBG_USE_LOG                     //!< 使用日志功能
#define DBG_USE_COLOR                   //!< 使用带颜色输出

/// 模块调试开关
#define DS_OUT              1           //!< 输出
#define DS_OUT_ERR          1           //!< 输出错误
#define DS_IN               1           //!< 输入
#define DS_IN_ERR           1           //!< 输入错误
#define DS_LOG              1           //!< 日志
#define DS_LOG_ERR          1           //!< 日志错误
#define DS_TEST             1           //!< 测试
#define DS_TEST_ERR         1           //!< 测试错误

#include "debug_bio.h"
#include "debug_log.h"
#include "debug_out.h"
#include "debug_in.h"
#include "debug_test.h"

/**
 * \block:      STDOUT
 * @{ */
/** \brief      格式化输出 */
#define dbg_out(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout(__VA_ARGS__); \
    } \
}
/** \brief      带标签信息的格式化输出，显示为提示高亮 */
#define dbg_out_I(__debug_switch__, ...) {\
    if(__debug_switch__) {\
        dbg_stdout_label(__func__, __LINE__, DBG_COLOR_INFO,\
                DBG_LABEL_COLOR | DBG_LABEL_INFO | DBG_LABEL_NEWLINE,\
                __VA_ARGS__);\
    }\
}
/** \brief      带标签信息的格式化输出，显示为警告高亮 */
#define dbg_out_W(__debug_switch__, ...) {\
    if(__debug_switch__) {\
        dbg_stdout_label(__func__, __LINE__, DBG_COLOR_WARN,\
                DBG_LABEL_COLOR | DBG_LABEL_WARN | DBG_LABEL_NEWLINE,\
                __VA_ARGS__);\
    }\
}
/** \brief      带标签信息的格式化输出，显示为错误高亮 */
#define dbg_out_E(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__func__, __LINE__, DBG_COLOR_ERR,\
                DBG_LABEL_COLOR | DBG_LABEL_ERR | DBG_LABEL_NEWLINE,\
                __VA_ARGS__);\
    } \
}
/** \brief      带标签信息的格式化输出，显示为高亮 */
#define dbg_out_H(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__func__, __LINE__, DBG_COLOR_HL,\
                DBG_LABEL_COLOR | DBG_LABEL_INFO | DBG_LABEL_NEWLINE,\
                __VA_ARGS__);\
    } \
}
/** \brief      带标签信息的格式化输出, 附带错误信息 */
#define dbg_outerr_I(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__func__, __LINE__, DBG_COLOR_ERR,\
                DBG_LABEL_COLOR | DBG_LABEL_ERR | DBG_LABEL_STDERR\
                | DBG_LABEL_NEWLINE,\
                __VA_ARGS__);\
    } \
}
/** \brief      带标签信息和时间的格式化输出，显示为提示高亮 */
#define dbg_out_IT(__debug_switch__, ...) {\
    if(__debug_switch__) {\
        dbg_stdout_label(__func__, __LINE__, DBG_COLOR_INFO,\
                DBG_LABEL_TIME | DBG_LABEL_COLOR | DBG_LABEL_INFO\
                | DBG_LABEL_NEWLINE,\
                __VA_ARGS__);\
    }\
}
/** \brief      带标签信息和时间的格式化输出，显示为警告高亮 */
#define dbg_out_WT(__debug_switch__, ...) {\
    if(__debug_switch__) {\
        dbg_stdout_label(__func__, __LINE__, DBG_COLOR_WARN,\
                DBG_LABEL_TIME | DBG_LABEL_COLOR | DBG_LABEL_WARN\
                | DBG_LABEL_NEWLINE,\
                __VA_ARGS__);\
    }\
}
/** \brief      带标签信息和时间的格式化输出，显示为错误高亮 */
#define dbg_out_ET(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__func__, __LINE__, DBG_COLOR_ERR,\
                DBG_LABEL_TIME | DBG_LABEL_COLOR | DBG_LABEL_ERR\
                | DBG_LABEL_NEWLINE,\
                __VA_ARGS__);\
    } \
}
/** \brief      带标签信息和时间的格式化输出，显示为高亮 */
#define dbg_out_HT(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__func__, __LINE__, DBG_COLOR_HL,\
                DBG_LABEL_TIME | DBG_LABEL_COLOR | DBG_LABEL_INFO\
                | DBG_LABEL_NEWLINE,\
                __VA_ARGS__);\
    } \
}
/** \brief      带标签信息和时间的格式化输出, 附带错误信息 */
#define dbg_outerr_IT(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout_label(__func__, __LINE__, DBG_COLOR_ERR,\
                DBG_LABEL_TIME | DBG_LABEL_COLOR | DBG_LABEL_ERR\
                | DBG_LABEL_STDERR | DBG_LABEL_NEWLINE,\
                __VA_ARGS__);\
    } \
}
/** @} */
/**
 * \block:      STDIN
 * @{ */
/** \brief      返回输入的数值，如果输入值不可转换为数字则返回-1 */
#define dbg_in() ({ \
        dbg_stdin_label(__func__, __LINE__, DBG_STDIN_RETNUM, 0, 0);\
        })
/** \brief      获取输入的数值，可识别8进制、10进制和16进制 */
#define dbg_in_N(pnum) ({ \
        dbg_stdin_label(__func__, __LINE__, DBG_STDIN_GETNUM, pnum, 0);\
        })
/** \brief      获取输入的字符串，移除换行符 */
#define dbg_in_S(pstr, len) ({ \
        dbg_stdin_label(__func__, __LINE__, DBG_STDIN_GETSTR, pstr, len);\
        })
/** @} */
/**
 * \block:      LOG
 * @{ */

/** @} */
/**
 * \block:      TEST
 * @{ */
#define dbg_test_setlist(...) {\
    DBG_TESTLIST_T list[] = { __VA_ARGS__ }; \
    dbg_testlist(list, (sizeof(list) / sizeof(DBG_TESTLIST_T)));\
}
/** @} */


#endif /* __DEBUG_H__ */

