#ifndef __SDB_INTERNAL_H__
#define __SDB_INTERNAL_H__

#include <stdarg.h>

#include "sdb_config.h"

typedef enum {
    PAD_ZERO                = 0x01,     /* 填充0 */
    ALIGN_LEFT              = 0x02,     /* 左对齐 */
    CAPITAL_HEX             = 0x04,     /* 字母大写 */
    ALTERNATE_FORM          = 0x08,     /* 数值格式, %#x => 0xff */
    SIGN                    = 0x10,     /* 显示正负号 */
    NEGATIVE_SIGN           = 0x20,     /* 显示为负号 */
    LONG_INT                = 0x40,     /* unsigned long int */
    LONG_LONG_INT           = 0x80,     /* unsigned long long int */
} flag_t;
typedef int (* put_t)(void *p, const char *buf, unsigned int len);
typedef struct {                        /* 格式化输出解析参数结构体 */
    unsigned char flag;                 /* 标记, flag_t */
    unsigned char base;                 /* 进制 */
    unsigned char width;                /* 输出宽度 */
    unsigned char precision;            /* 小数精度 */
} fmt_t;
typedef struct {                        /* print 内部参数结构体 */
    unsigned char c1;                   /* 临时字符存放 */
    unsigned char c2;                   /* 结束符 / 备用临时字符存放 */
    unsigned char c3;                   /* 结束符 */
    unsigned char c4;                   /* 临时计数 */
    int ret;                            /* 返回值  (_U2S中临时复用) */

    void *ptr;                          /* 外部指针传递 */
    put_t put;                          /* 输出处理函数指针 */
    fmt_t fmt;                          /* 格式化输出解析参数结构体 */
} print_context_t;

typedef struct {                        /* 基本输出参数传递结构体 */
    const sdb_config_t *cfg;            /* 配置结构体 */
    unsigned int flag;                  /* 输出标记定义, sdb_flag_t */
} bio_put_param_t;
typedef struct {                        /* 内部输出参数传递结构体 */
    bio_put_param_t bio;                /* 基本输出参数传递 */
    const char *file;                   /* 文件名 */
    const char *func;                   /* 函数名 */
    unsigned int line;                  /* 行号 */
#if defined(SDB_SYS_SUPPORT_STDERR)
    int err;                            /* 错误码 */
#endif
    const char *fmt;                    /* 格式化字符串 */
    va_list va;                         /* 参数列表 */
} put_param_t;
typedef struct {                        /* 内部数据导出参数传递结构体 */
    int opt;                            /* 数据导出控制选项, sdb_option_t */
    char *data;                         /* 数据 */
    unsigned int len;                   /* 数据长度 */
    void *addr;                         /* 数据地址 */
} dump_param_t;

#define PUT_PEND(__p) do {\
    if ((ret = bio_put((__p)->cfg, (__p)->flag | SDB_DATA_PEND, "^", 1)))\
    return ret;\
} while (0)
#define PUT_POST(__p) do {\
    if ((ret = bio_put((__p)->cfg, (__p)->flag | SDB_DATA_POST, "$", 1)))\
    return ret;\
} while (0)
#define PUT_WRAP(__p) do {\
    if ((ret = bio_put((__p)->cfg, (__p)->flag | SDB_DATA_WRAP, "\n", 1)))\
    return ret;\
} while (0)
#define PUT_BLANK(__p) do {\
    if ((ret = bio_put((__p)->cfg, (__p)->flag | SDB_DATA_BLANK, " ", 1)))\
    return ret;\
} while (0)
#define PUT_STR_BLK(__p, __data_flag, __buf, __len) do {\
    if ((ret = bio_put((__p)->cfg, (__p)->flag | __data_flag, __buf, __len)))\
    return ret;\
    PUT_BLANK(__p);\
} while (0)
#define PUT_STR(__p, __data_flag, __buf, __len) do {\
    if ((ret = bio_put((__p)->cfg, (__p)->flag | __data_flag, __buf, __len)))\
    return ret;\
} while (0)
#define PUT_NUM_BLK(__p, __data_flag, __num) do {\
    (__p)->flag |= __data_flag;\
    if ((ret = put_i2s(__p, __num)))\
    return ret;\
    (__p)->flag &= ~__data_flag;\
    PUT_BLANK(__p);\
} while (0)
#define PUT_NUM(__p, __data_flag, __num) do {\
    (__p)->flag |= __data_flag;\
    if ((ret = put_i2s(__p, __num)))\
    return ret;\
    (__p)->flag &= ~__data_flag;\
} while (0)

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

int put_i2s(bio_put_param_t *p, int num);
int cb_putx(void *p, const char *buf, unsigned int len);
void set_color(unsigned int flag, const char **head, const char **end);

int vxprint(void *ptr, put_t put, const char *fmt, va_list va);
#if defined(SDB_SYS_SUPPORT_LONG_LONG)
int ulli2s(print_context_t *ctx, unsigned long long int num);
int lli2s(print_context_t *ctx, long long int num);
#endif
#if defined(SDB_SYS_SUPPORT_LONG)
int uli2s(print_context_t *ctx, unsigned long int num);
int li2s(print_context_t *ctx, long int num);
#endif
int ui2s(print_context_t *ctx, unsigned int num);
int i2s(print_context_t *ctx, int num);


#ifdef __cplusplus
}
#endif
#endif /* __SDB_INTERNAL_H__ */

