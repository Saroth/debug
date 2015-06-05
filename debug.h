/* {
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

#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifndef DS_DEBUG_MAIN
#define DS_DEBUG_MAIN                   //!< 调试模块总开关
#endif /* DS_DEBUG_MAIN */
#ifndef DS_DEBUG_ERR
#define DS_DEBUG_ERR                    //!< 调试模块错误信息
#endif /* DS_DEBUG_MAIN */
#ifndef BUFFER_SIZE
#define BUFFER_SIZE         4096        //!< 数据输入输出缓存大小
#endif /* BUFFER_SIZE */


#define DBG_USE_LOG                     //!< 使用日志功能
#define DBG_USE_COLOR                   //!< 使用带颜色输出

#include "debug_bio.h"
#include "debug_log.h"
#include "debug_out.h"
#include "debug_in.h"

/**
 * \block:      STDOUT
 * @{ */
/**
 * \brief       格式化输出
 * \param       __debug_switch__    调试开关
 * \param       ...                 格式字符串和参数
 */
#define dbg_out(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout(__VA_ARGS__); \
    } \
}
/**
 * \brief       带标签信息的格式化输出，显示为提示高亮
 * \param       __debug_switch__    调试开关
 * \param       ...                 格式字符串和参数
 */
#define dbg_out_I(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout("%s:%5d:\t", __func__, __LINE__); \
        dbg_color_set(DBG_COLOR_INFO);\
        dbg_stdout(__VA_ARGS__); \
        dbg_color_set(DBG_COLOR_RES);\
        dbg_stdout("\n"); \
    } \
}
/**
 * \brief       带标签信息的格式化输出，显示为警告高亮
 * \param       __debug_switch__    调试开关
 * \param       ...                 格式字符串和参数
 */
#define dbg_out_W(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout("%s:%5d:\t", __func__, __LINE__); \
        dbg_color_set(DBG_COLOR_WARN);\
        dbg_stdout(__VA_ARGS__); \
        dbg_color_set(DBG_COLOR_RES);\
        dbg_stdout("\n"); \
    } \
}
/**
 * \brief       带标签信息的格式化输出，显示为错误高亮
 * \param       __debug_switch__    调试开关
 * \param       ...                 格式字符串和参数
 */
#define dbg_out_E(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout("%s:%5d:\t", __func__, __LINE__); \
        dbg_color_set(DBG_COLOR_ERR);\
        dbg_stdout(__VA_ARGS__); \
        dbg_color_set(DBG_COLOR_RES);\
        dbg_stdout("\n"); \
    } \
}
/**
 * \brief       带标签信息的格式化输出，显示为高亮
 * \param       __debug_switch__    调试开关
 * \param       ...                 格式字符串和参数
 */
#define dbg_out_H(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout("%s:%5d:\t", __func__, __LINE__); \
        dbg_color_set(DBG_COLOR_HL);\
        dbg_stdout(__VA_ARGS__); \
        dbg_color_set(DBG_COLOR_RES);\
        dbg_stdout("\n"); \
    } \
}
/**
 * \brief       带标签信息的格式化输出, 附带错误信息
 * \param       __debug_switch__    调试开关
 * \param       ...                 格式字符串和参数
 */
#define dbg_outerr_I(__debug_switch__, ...) { \
    if(__debug_switch__) { \
        dbg_stdout("%s:%5d:\t", __func__, __LINE__); \
        dbg_color_set(DBG_COLOR_ERR);\
        dbg_stdout(__VA_ARGS__); \
        dbg_stdout(": "); \
        dbg_stderr(); \
        dbg_color_set(DBG_COLOR_RES);\
        dbg_stdout("\n"); \
    } \
}
/** @} */

/**
 * \block:      STDIN
 * @{ */

/** @} */



#endif /* __DEBUG_H__ */

