#ifndef __DEBUG_OUT_H__
#define __DEBUG_OUT_H__

#ifdef __cplusplus
extern "C" {
#endif

/// 输出颜色定义
#define DBG_COLOR_RES       "\33[0m"            //!< 恢复
#define DBG_COLOR_INFO      "\33[1m"            //!< 提示高亮
#define DBG_COLOR_WARN      "\33[1;31m"         //!< 警告高亮
#define DBG_COLOR_ERR       "\33[1;91m"         //!< 错误高亮
#define DBG_COLOR_HL        "\33[1;7m"          //!< 其他高亮

/**
 * \brief       格式化输出
 * \param       fmt     格式字符串，内部缓存大小为BUFFER_SIZE
 * \param       ...     参数
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
/**
 * \brief       设置终端输出颜色
 * \param       color   颜色标示
 * \return      0       Success
 */
int dbg_color_set(char * color);

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_OUT_H__ */

