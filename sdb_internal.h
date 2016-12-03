#ifndef __SDB_INTERNAL_H__
#define __SDB_INTERNAL_H__

#include "sdb_config.h"

typedef struct {                        //!< 内部传递参数结构体
#if defined(SDB_SYS_HAVE_STDERR)
    int errnum;                         //!< 错误码
#endif
    const char *format;                 //!< 格式化字符串
    va_list ap;                         //!< 参数列表结构体指针
    sdb_bio_context_t *ctx;             //!< 调试上下文结构体
} internal_param_t;

typedef struct {
    const sdb_config_t *cfg;            //!< 配置结构体
    int flag;                           //!< 输入类型标记
    char *buf;                          //!< 传入缓存
    size_t bufsize;                     //!< 传入缓存大小
    size_t len;                         //!< 输入长度
    int num;                            //!< 输出数值
    int *pnum;                          //!< 输出数值指针
} input_param_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief       基本输出接口
 * \param       ctx         调试上下文结构体
 * \return      0:Success; <0:SDB_RET_T
 */
int bio_output(sdb_bio_context_t *ctx);

/**
 * \brief       基本输入接口
 * \param       cfg         配置结构体
 * \param       buf         获取输入数据的缓存
 * \param       bufsize     缓存大小, 0:无限制
 * \param       outlen      获取输入数据长度的指针，可能为NULL
 * \return      0:Success; <0:SDB_RET_T
 * \detail      以回车或^D结束，返回结果包含换行符
 */
int bio_input(const sdb_config_t *cfg,
        char *buf, size_t bufsize, size_t *outlen);

/**
 * \brief       带参数传递的格式化输出
 * \param       cfg         配置结构体
 * \param       flag        输出控制, sdb_flag_t
 * \param       file        __FILE__
 * \param       func        __func__
 * \param       line        __LINE__
 * \param       format      格式化输出
 * \param       ap          va_list
 * \return      0:Success; <0:Error
 */
int sdb_output_v(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, int line,
        const char *format, va_list ap);
#ifdef __cplusplus
}
#endif
#endif /* __SDB_INTERNAL_H__ */

