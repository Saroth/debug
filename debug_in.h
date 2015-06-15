#ifndef __DEBUG_IN_H__
#define __DEBUG_IN_H__

#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DBG_STDIN_RETNUM    1                   //!< 返回输入的数值
#define DBG_STDIN_GETNUM    2                   //!< 获取输入的数值
#define DBG_STDIN_GETSTR    3                   //!< 获取输入的字符串

/**
 * \brief       获取输入的字符串
 * \param       str         字符串缓存
 * \param       len         缓存大小
 * \return      >=0         获取的数据长度
 *              -1          Error
 * \descript    自动移除末尾换行符
 */
int dbg_stdin(char * str, int len);
/**
 * \brief       获取输入的数值
 * \param       num         变量指针，输出获取到的数值
 * \return      0           Success
 *              -1          Error
 * \descript    0x开头识别为16进制，0开头识别为8进制
 */
int dbg_stdin_num(int * num);
/**
 * \brief       带调试标签的输入
 * \param       func            __func__
 * \param       line            __LINE__
 * \param       mode            模式，参考DBG_STDIN_*
 * \param       output          DBG_STDIN_GETSTR：输出数据的缓存
 *                              DBG_STDIN_GETNUM：整形指针，用于获取输入数值
 *                              DBG_STDIN_RETNUM：不使用，NULL
 * \param       len             DBG_STDIN_GETSTR：获取的数据长度
 *                              DBG_STDIN_GETNUM：不使用
 *                              DBG_STDIN_RETNUM：不使用
 * \return      
 */
int dbg_stdin_label(const char * func, int line, int mode,
        void * output, int len);

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_IN_H__ */

