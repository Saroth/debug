#ifndef __DEBUG_OUT_H__
#define __DEBUG_OUT_H__

#ifdef __cplusplus
extern "C" {
#endif

/// 输出颜色定义
#define DBG_COLOR_RES       "\33[0m"            //!< 恢复
#define DBG_COLOR_INFO      "\33[0m"            //!< 提示高亮
#define DBG_COLOR_WARN      "\33[1;31m"         //!< 警告高亮
#define DBG_COLOR_ERR       "\33[1;91m"         //!< 错误高亮
#define DBG_COLOR_HL        "\33[1;7m"          //!< 反白高亮
#define DBG_COLOR_INPUT     "\33[1;96m"         //!< 输入标志高亮

// 标记定义
#define DBG_SIGN_INFO       ""                  //!< 提示标记
#define DBG_SIGN_WARN       "[Warning]"         //!< 警告标记
#define DBG_SIGN_ERR        "[Error]"           //!< 错误标记
#define DBG_SIGN_IN_RETNUM  "[I_n]"             //!< 返回数值输入标记
#define DBG_SIGN_IN_GETNUM  "[I_n]"             //!< 获取数组输入标记
#define DBG_SIGN_IN_GETSTR  "[I_s]"             //!< 获取字符串输入标记
#define DBG_SIGN_OUT_RETNUM "[O_n]"             //!< 返回数值输出标记
#define DBG_SIGN_OUT_GETNUM "[O_n]"             //!< 获取数组输出标记
#define DBG_SIGN_OUT_GETSTR "[O_s]"             //!< 获取字符串输出标记

// 输入选项定义
#define DBG_LABEL_TIME      ( 1 << 0 )          //!< 显示时间
#define DBG_LABEL_COLOR     ( 1 << 1 )          //!< 颜色显示标签，需要DBG_USE_COLOR
#define DBG_LABEL_TEXTCOLOR ( 1 << 2 )          //!< 颜色显示信息，需要DBG_USE_COLOR
#define DBG_LABEL_STDERR    ( 1 << 3 )          //!< 显示错误信息
#define DBG_LABEL_NEWLINE   ( 1 << 4 )          //!< 添加换行符
#define DBG_LABEL_COL_INFO  ( 1 << 5 )          //!< 使用提示高亮
#define DBG_LABEL_COL_WARN  ( 1 << 6 )          //!< 使用警告高亮
#define DBG_LABEL_COL_ERR   ( 1 << 7 )          //!< 使用错误高亮
#define DBG_LABEL_COL_HL    ( 1 << 8 )          //!< 使用反白高亮
#define DBG_LABEL_COL_INPUT ( 1 << 9 )          //!< 使用输入标志高亮
#define DBG_LABEL_IN_RETN   ( 1 << 15 )         //!< 显示返回数值输入标记
#define DBG_LABEL_IN_GETN   ( 1 << 16 )         //!< 显示获取数组输入标记
#define DBG_LABEL_IN_GETS   ( 1 << 17 )         //!< 显示获取字符串输入标记
#define DBG_LABEL_OUT_RETN  ( 1 << 18 )         //!< 显示返回数值输出标记
#define DBG_LABEL_OUT_GETN  ( 1 << 19 )         //!< 显示获取数组输出标记
#define DBG_LABEL_OUT_GETS  ( 1 << 20 )         //!< 显示获取字符串输出标记
#define DBG_LABEL_INFO      ( 1 << 21 )         //!< 添加提示标记
#define DBG_LABEL_WARN      ( 1 << 22 )         //!< 添加警告标记
#define DBG_LABEL_ERR       ( 1 << 23 )         //!< 添加错误标记

enum {
    DBG_STDOUT_TIME,                            //!< 显示时间
    DBG_STDOUT_POS,                             //!< 显示位置
    DBG_STDOUT_SIGN,                            //!< 显示标记
    DBG_STDOUT_STDERR,                          //!< 显示错误
    DBG_STDOUT_NL,                              //!< 显示新行
    DBG_STDOUT_COLOR_SET,                       //!< 设置颜色
    DBG_STDOUT_COLOR_RES,                       //!< 还原颜色
};

/**
 * \brief       格式化输出
 * \param       fmt     格式字符串，内部缓存大小为BUFFER_SIZE
 * \param       ...     格式化参数
 * \return      >=0     输出长度
 *              <0      Error
 */
int dbg_stdout(const char * fmt, ...);
/**
 * \brief       标准错误信息输出
 * \return      >=0     实际输出字符长度
 *              <0      Error
 */
int dbg_stderr(void);
#ifdef DBG_USE_COLOR
/**
 * \brief       设置终端输出颜色
 * \param       color   颜色标示
 * \return      0       Success
 */
int dbg_color_set(char * color);
#endif /* DBG_USE_COLOR */
/**
 * \brief       带调试标签的格式化输出
 * \param       func        __func__
 * \param       line        __LINE__
 * \param       opt         参考宏定义DBG_LABEL_*
 * \param       fmt         格式化输出字符串
 * \param       ...         格式化参数
 * \return      0           Success
 *              -1          Error
 */
int dbg_stdout_label(const char * func, int line, int opt, char * fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_OUT_H__ */

