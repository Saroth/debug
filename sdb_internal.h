#ifndef __SDB_INTERNAL_H__
#define __SDB_INTERNAL_H__

#include "sdb_config.h"

typedef struct {            //!< 输出参数结构体
    int options;            //!< 调试项目选项, SDB_OPTION_T
    int mode;               //!< 调试输出模式
    unsigned int mark_type; //!< 标记类型
    unsigned int msg_type;  //!< 信息类型
    unsigned int hl_type;   //!< 高亮类型

    char *file;             //!< 文件名
    const char *func;       //!< 函数名
    int line;               //!< 行号
    int error;              //!< 错误码

    const char *format;     //!< 格式化字符串
    va_list ap;             //!< 参数列表结构体指针

    size_t length;          //!< 缓存数据长度
    char cache[SDB_CONF_IO_CACHE_SIZE]; //!< 缓存
} SDB_IO_PARAM_T;

typedef struct {            //!< 标签信息结构体, SDB_MODE_SBL_*
    int hl_type;            //!< 高亮类型, SDB_HL_TYPES_T
    char *mark;             //!< 高亮定义, SDB_COLOR_*
} SDB_MARK_T;

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
 * \param       opt         输出控制, SDB_OPTION_T
 * \param       mode        显示模式, SDB_MODE_T
 * \param       file        __FILE__
 * \param       func        __func__
 * \param       line        __LINE__
 * \param       format      格式化输出
 * \param       ap          不定参数结构体
 * \return      0:Success <0:Error
 */
int sdb_voutput(int opt, int mode, char *file, const char *func, int line,
        const char *format, va_list ap);

#endif /* __SDB_INTERNAL_H__ */

