#ifndef __DEBUG_BIO_H__
#define __DEBUG_BIO_H__

#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief       输出接口
 * \param       buf     输出数据的缓存
 * \param       len     输出长度
 * \return      0       Success
 *              -1      Error
 */
int dbg_bio_out(char * buf, int len);
/**
 * \brief       输入接口
 * \param       buf     获取输入数据的缓存
 * \param       len     希望获取的长度
 * \return      0       Success
 *              -1      Error
 */
int dbg_bio_in(char * buf, int len);
/**
 * \brief       打开日志文件
 * \param       file    文件名
 * \return      0       Success
 *              -1      Error
 * \descript    打开新日志时自动关闭旧的日志文件
 *              使用追加模式打开日志，不存在则创建
 */
int dbg_bio_open(char * file);
/**
 * \brief       关闭日志文件
 * \return      0       Success
 *              -1      Error
 */
int dbg_bio_close(void);
/**
 * \brief       写日志
 * \param       buf     写入数据缓存
 * \param       len     写入数据长度
 * \return      0       Success
 *              -1      Error
 */
int dbg_bio_write(char * buf, int len);

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_BIO_H__ */

