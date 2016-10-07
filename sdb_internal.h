#ifndef __SDB_INTERNAL_H__
#define __SDB_INTERNAL_H__

#include "sdb_config.h"

typedef struct {            //!< 输出参数结构体
    int options;            //!< 调试项目选项, SDB_OPTION_T
    int mode;               //!< 调试输出模式

    const char *file;       //!< 文件名
    const char *func;       //!< 函数名
    int line;               //!< 行号
    int error;              //!< 错误码

    const char *format;     //!< 格式化字符串
    va_list ap;             //!< 参数列表结构体指针

    void *addr;             //!< 数据地址
    size_t length;          //!< 缓存数据长度
    char cache[SDB_CONF_IO_CACHE_SIZE]; //!< 缓存
} SDB_IO_PARAM_T;

typedef struct {            //!< 标签信息结构体, SDB_MODE_SBL_*
    int hl_type;            //!< 高亮类型, SDB_HL_TYPES_T
    const char *mark;       //!< 高亮定义, SDB_COLOR_*
} SDB_MARK_T;

#ifdef __cplusplus
extern "C" {
#endif

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
 * \param       bufsize     缓存大小, 0:无限制
 * \param       outlen      获取输入数据长度的指针
 * \return      0: Success      <0: SDB_RET_T
 * \detail      以回车或^D结束，返回结果包含换行符
 */
int sdb_bio_in(char *buf, size_t bufsize, size_t *outlen);

/**
 * \brief       样式&格式化输出控制
 * \param       p           参数结构体制作
 * \return      0:Success <0:Error
 */
int sdb_out_style(SDB_IO_PARAM_T *p);
#ifdef __cplusplus
}
#endif
#endif /* __SDB_INTERNAL_H__ */

