#ifndef __SDB_INTERNAL_H__
#define __SDB_INTERNAL_H__

#include "config.h"

typedef struct {            //!< 输出参数结构体
    int options;            //!< 调试项目选项, SDB_IO_OPTION_T
    int mode;               //!< 调试输出模式
    int error;              //!< 错误码
    int outlen;             //!< 输出长度
// #if defined(SDB_SYS_HAVE_MEM_ALLOC)
//     char *outbuf;           //!< 输出内容缓存
// #else
    char outbuf[SDB_CONF_IO_CACHE_SIZE + 4];
// #endif
} SDB_OUT_PARAM_T;

/**
 * \brief       基本输出接口
 * \param       buf         输出数据的缓存
 * \param       len         输出长度
 * \return      0: Success      <0: SDB_RET_T
 */
int sdb_bio_out(char *buf, size_t len);

/**
 * \brief       基本输入接口
 * \param       buf         获取输入数据的缓存
 * \param       buflen      缓存大小
 * \param       outlen      输出数据长度
 * \return      0: Success      <0: SDB_RET_T
 * \detail      以回车或^D结束，返回结果包含换行符
 */
int sdb_bio_in(char *buf, size_t buflen, size_t *outlen);


#endif /* __SDB_INTERNAL_H__ */

