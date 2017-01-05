#ifndef __SDB_INTERNAL_H__
#define __SDB_INTERNAL_H__

#include <stdarg.h>

#include "sdb_config.h"

typedef int (* put_t)(void *p, const char *buf, unsigned int len);
typedef struct {                        /* 内部输入参数传递结构体 */
    const sdb_config_t *cfg;            /* 配置结构体 */
    unsigned int flag;                  /* 输入类型标记 */
    char *buf;                          /* 传入缓存 */
    unsigned int bufsize;               /* 传入缓存大小 */
    unsigned int len;                   /* 输入长度 */
    int num;                            /* 输出数值 */
    int *pnum;                          /* 输出数值指针 */
} get_param_t;
typedef struct {                        /* 内部数据导出参数传递结构体 */
    int opt;                            /* 数据导出控制选项, sdb_option_t */
    char *data;                         /* 数据 */
    unsigned int len;                   /* 数据长度 */
    void *addr;                         /* 数据地址 */
} dump_param_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief       基本输出接口
 * \param       cfg         配置结构体
 * \param       flag        输出控制, sdb_flag_t
 * \param       buf         输出缓存
 * \param       len         输出长度
 * \return      0:Success; <0:SDB_RET_T
 */
int bio_put(const sdb_config_t *cfg,
        unsigned int flag, const char *buf, unsigned int len);

/**
 * \brief       基本输入接口
 * \param       cfg         配置结构体
 * \param       buf         获取输入数据的缓存
 * \param       size        缓存大小, 0:无限制
 * \param       len         获取输入数据长度的指针，可能为NULL
 * \return      0:Success; <0:SDB_RET_T
 * \detail      以回车或^D结束，返回结果包含换行符
 */
int bio_get(const sdb_config_t *cfg,
        char *buf, unsigned int size, unsigned int *len);

int vxprint(void *ptr, put_t put, const char *fmt, va_list va);

#ifdef __cplusplus
}
#endif
#endif /* __SDB_INTERNAL_H__ */

