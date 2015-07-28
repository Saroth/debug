#ifndef __DEBUG_LOG_H__
#define __DEBUG_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#if(DBG_USE_LOG == 1)
#define DBG_P       ( 1 << 0 )          //!< 1 << 0 print only
#define DBG_W       ( 1 << 1 )          //!< 1 << 1 write log only
#define DBG_PW      ( DBG_P | DBG_W )   //!< both print and write log
#define DBG_S       ( 1 << 2 )          //!< sync mode

/**
 * \brief       打开新日志
 * \param       filename    日志文件名
 * \return      0       Success
 *              -1      Error
 */
int dbg_log_open(char * filename);
/**
 * \brief       关闭日志
 * \return      0       Success
 *              -1      Error
 */
int dbg_log_close(void);
/**
 * \brief       写日志
 * \param       buf     数据
 * \param       len     数据长度
 * \return      >=0     写入数据长度
 *              -1      Error
 * \descript    不输出
 */
int dbg_log_write(char * buf, int len);
/**
 * \brief       设置日志记录模式
 * \param       mode    调试模式 dbg_modes_e
 * \return      0       Success
 *              -1      Error
 */
int dbg_log_setmode(int mode);
/**
 * \brief       获取日志记录模式
 * \return      0       Success
 *              -1      Error
 */
int dbg_log_getmode(void);
#endif /* (DBG_USE_LOG == 1) */

#ifdef __cplusplus
}
#endif
#endif /* __DEBUG_LOG_H__ */

