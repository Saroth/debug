/**
 * Copyright(C) For free. All right reserved.
 *
 * Slim Debugger, a simple code tracer for C/C++.
 */

#include <stdarg.h>

/**
 * \block:      Heads
 * @{ */
#ifndef __SDB_H__
#define __SDB_H__

#define __sdb_cfg SDB_SET_CONFIG                            /* 调试配置宏 */

typedef enum {                                              /* 调试配置选项定义 */
    SDB_DUMP_NONUM          = (1 << 1),                     /* 不输出计数或地址 */
    SDB_DUMP_NOHEX          = (1 << 2),                     /* 不输出16进制 */
    SDB_DUMP_NOCHAR         = (1 << 3),                     /* 不输出字符 */
    SDB_DUMP_SEGADD16       = (1 << 4),                     /* 每行增加16字节 */
    SDB_DUMP_SEGADD32       = (1 << 5)                      /* 每行增加32字节 */
} sdb_option_t;

typedef enum {                                              /* 输出标记定义 */
    /* 消息类型 */
    SDB_TYPE_OFS            = 0,                            /* 偏移 */
    SDB_TYPE_ERR            = (0x01 << SDB_TYPE_OFS),       /* 错误 */
    SDB_TYPE_WARN           = (0x02 << SDB_TYPE_OFS),       /* 警告 */
    SDB_TYPE_INFO           = (0x03 << SDB_TYPE_OFS),       /* 普通消息 */
    SDB_TYPE_INPUT_NUM      = (0x04 << SDB_TYPE_OFS),       /* 输入数值 */
    SDB_TYPE_INPUT_STR      = (0x05 << SDB_TYPE_OFS),       /* 输入字符 */
    SDB_TYPE_INPUT_ECHO     = (0x06 << SDB_TYPE_OFS),       /* 输入反馈 */
    SDB_TYPE_DUMP           = (0x07 << SDB_TYPE_OFS),       /* 导出数据 */
    SDB_TYPE_MASK           = (0x0f << SDB_TYPE_OFS),       /* 掩码 */
    /* 数据类型 */
    SDB_DATA_OFS            = 4,                            /* 偏移 */
    SDB_DATA_PEND           = (0x01 << SDB_DATA_OFS),       /* 挂起(输出开始) */
    SDB_DATA_POST           = (0x02 << SDB_DATA_OFS),       /* 释放(输出结束) */
    SDB_DATA_FILE           = (0x03 << SDB_DATA_OFS),       /* 文件名 */
    SDB_DATA_FUNC           = (0x04 << SDB_DATA_OFS),       /* 函数名 */
    SDB_DATA_LINE           = (0x05 << SDB_DATA_OFS),       /* 行号 */
    SDB_DATA_INFO           = (0x06 << SDB_DATA_OFS),       /* 消息数据 */
    SDB_DATA_STDERR         = (0x07 << SDB_DATA_OFS),       /* 标准错误信息 */
    SDB_DATA_WRAP           = (0x09 << SDB_DATA_OFS),       /* 换行 */
    SDB_DATA_COLOR          = (0x0a << SDB_DATA_OFS),       /* 颜色控制序列 */
    SDB_DATA_BLANK          = (0x0b << SDB_DATA_OFS),       /* 空白 */
    SDB_DATA_MASK           = (0x0f << SDB_DATA_OFS)        /* 掩码 */
} sdb_flag_t;

typedef enum {                                              /* 返回值定义 */
    SDB_RET_OK              = 0,                            /* 成功 */
    SDB_RET_PARAM_ERR       = -0x0a,                        /* 参数错误 */
    SDB_RET_NO_INPUT        = -0x0b,                        /* 无输入 */
    SDB_RET_UNKNOWN_INPUT   = -0x0c,                        /* 未知输入 */
    SDB_RET_PROCESS_ERR     = -0x0d                         /* 处理错误 */
} sdb_ret_t;

typedef struct sdb_config_t sdb_config_t;
typedef struct sdb_item_t sdb_item_t;
/**
 * \brief       输出接口函数类型
 * \param       ptr         输出接口传递参数
 * \param       flag        输出标记, sdb_flag_t
 * \param       buf         数据缓存
 * \param       len         数据长度
 * \return      0:Success; <0:Error
 */
typedef int (* sdb_bio_put_t)(void *ptr, sdb_flag_t flag,
        const char *buf, unsigned int len);
/**
 * \brief       输入接口函数类型
 * \param       cfg         配置结构体
 * \param       buf         获取输入数据的缓存
 * \param       size        缓存大小, 0:无限制
 * \param       len         获取输入数据长度的指针
 * \return      0:Success; <0:Error
 * \detail      输入过程不退出，直到收到回车或缓存超出才返回
 *              可包含或不包含换行符
 *              数据末尾会补充结束符，因此当len等于size时，最后一字节将丢弃
 */
typedef int (* sdb_bio_get_t)(void *ptr,
        char *buf, unsigned int size, unsigned int *len);
struct sdb_config_t {                   /* 配置结构体 */
    unsigned int opt;                   /* 调试项目选项, sdb_option_t */
    sdb_bio_put_t put;                  /* 输出接口函数 */
    sdb_bio_get_t get;                  /* 输入接口函数 */
    void *ptr;                          /* 输出接口传递参数 */
};
struct sdb_item_t {                     /* 调试菜单项目结构体 */
    char *info;                         /* 项目显示信息 */
    void *param;                        /* 参数 */
    int (*func)(void *);                /* 函数 */
};

#ifdef __cplusplus
extern "C" {
#endif
/**\brief       标准输入输出，SDB_SYS_HAVE_STDIO启用时有效 */
extern const sdb_config_t sdb_cfg_std;

void sdb_set_stack(void);
int sdb_get_stack(void);
int sdb_get_stack_max(void);

/**
 * \brief       调试输出控制
 * \param       cfg         配置结构体
 * \param       flag        输出标记, sdb_flag_t
 * \param       file        __FILE__
 * \param       line        __LINE__
 * \param       fmt         格式化输出
 * \param       ...         不定参数
 * \return      0:Success; <0:Error
 */
int sdb_putx(const sdb_config_t *cfg, int flag,
        const char *file, unsigned int line, const char *fmt, ...);
/**
 * \brief       调试输出控制
 * \param       cfg         配置结构体
 * \param       fmt         格式化输出
 * \param       ...         不定参数
 * \return      0:Success; <0:Error
 */
int sdb_put_bare(const sdb_config_t *cfg, const char *fmt, ...);

int sdb_get(char *buf, unsigned int size, int *len,
        const sdb_config_t *cfg, unsigned flag,
        const char *file, unsigned int line, const char *fmt, ...);

/**
 * \brief       数据导出控制
 * \param       cfg         配置结构体
 * \param       opt         数据导出控制选项, sdb_option_t
 * \param       file        __FILE__
 * \param       line        __LINE__
 * \param       buf         数据
 * \param       len         数据长度
 * \param       addr        地址
 * \param       fmt         格式化输出
 * \param       ...         不定参数
 * \return      0:Success; <0:Error
 */
int sdb_dump(void *data, unsigned int len, unsigned long addr,
        const sdb_config_t *cfg, int opt,
        const char *file, unsigned int line, const char *fmt, ...);

int sdb_menu(sdb_item_t *list, unsigned int num,
        const sdb_config_t *cfg, const char *file, unsigned int line);

/**
 * \brief       内联空实现
 */
int sdb_nop(void);
#ifdef __cplusplus
}
#endif

#endif /* __SDB_H__ */
/** @} */
/**
 * \block:      Config
 * @{ */
#ifdef SDB_SET_CONFIG
#undef SDB_SET_CONFIG
#endif
/*
 * \brief:  设置调试配置, 配置结构体为sdb_config_t
 * \usage:  #define SDB_SET_CONFIG (&<my_config>)
 */
/** @} */
/**
 * \block:      Output
 * @{ */
#ifdef SDB_OUT
#undef SDB_OUT              /* bare */
#endif
#ifdef SDB_OUT_I
#undef SDB_OUT_I            /* send Information */
#endif
#ifdef SDB_OUT_W
#undef SDB_OUT_W            /* send Warning */
#endif
#ifdef SDB_OUT_E
#undef SDB_OUT_E            /* send Error */
#endif

#if defined(SDB_ENABLE)
#define SDB_OUT(...)        sdb_put_bare(__sdb_cfg, __VA_ARGS__)
#define SDB_OUT_I(...) \
    sdb_putx(__sdb_cfg, SDB_TYPE_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define SDB_OUT_W(...) \
    sdb_putx(__sdb_cfg, SDB_TYPE_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define SDB_OUT_E(...) \
    sdb_putx(__sdb_cfg, SDB_TYPE_ERR, __FILE__, __LINE__, __VA_ARGS__)
#else
#define SDB_OUT(...)        sdb_nop()
#define SDB_OUT_I(...)      sdb_nop()
#define SDB_OUT_W(...)      sdb_nop()
#define SDB_OUT_E(...)      sdb_nop()
#endif /* defined(SDB_ENABLE) */
/** @} */
/**
 * \block:      Input
 * @{ */
#ifdef SDB_IN_N
#undef SDB_IN_N             /* get Number */
#endif
#ifdef SDB_IN_S
#undef SDB_IN_S             /* get String */
#endif
#ifdef SDB_IN_NI
#undef SDB_IN_NI            /* get Number, with Information */
#endif
#ifdef SDB_IN_SI
#undef SDB_IN_SI            /* get String, with Information */
#endif

#if defined(SDB_ENABLE)
#define SDB_IN_N(__pnum) \
    sdb_get(0, 0, __pnum,\
            __sdb_cfg, SDB_TYPE_INPUT_NUM, __FILE__, __LINE__, 0)
#define SDB_IN_S(__buf, __size, __plen) \
    sdb_get(__buf, __size, (int *)(__plen),\
            __sdb_cfg, SDB_TYPE_INPUT_STR, __FILE__, __LINE__, 0)
#define SDB_IN_NI(__pnum, ...) \
    sdb_get(0, 0, __pnum,\
            __sdb_cfg, SDB_TYPE_INPUT_NUM, __FILE__, __LINE__, __VA_ARGS__)
#define SDB_IN_SI(__buf, __size, __plen, ...) \
    sdb_get(__buf, __size, (int *)(__plen),\
            __sdb_cfg, SDB_TYPE_INPUT_STR, __FILE__, __LINE__, __VA_ARGS__)
#else
#define SDB_IN_N(...)       sdb_nop()
#define SDB_IN_S(...)       sdb_nop()
#define SDB_IN_NI(...)      sdb_nop()
#define SDB_IN_SI(...)      sdb_nop()
#endif /* defined(SDB_ENABLE) */
/** @} */
/**
 * \block:      Dump
 * @{ */
#ifdef SDB_DMP
#undef SDB_DMP              /* dump in Hex */
#endif
#ifdef SDB_DMP_C
#undef SDB_DMP_C            /* dump with SOME Charactors */
#endif
#ifdef SDB_DMP_CA
#undef SDB_DMP_CA           /* dump, with address and some Charactors */
#endif
#ifdef SDB_DMP_I
#undef SDB_DMP_I            /* dump with Information */
#endif
#ifdef SDB_DMP_CI
#undef SDB_DMP_CI           /* dump with Information and some Charactors */
#endif
#ifdef SDB_DMP_CAI
#undef SDB_DMP_CAI          /* dump with Information, address, and some Charactors */
#endif

#if defined(SDB_ENABLE)
#define SDB_DMP(__buf, __len) \
    sdb_dump(__buf, __len, 0,\
            __sdb_cfg, SDB_DUMP_NOCHAR, __FILE__, __LINE__, 0);
#define SDB_DMP_C(__buf, __len) \
    sdb_dump(__buf, __len, 0,\
            __sdb_cfg, 0, __FILE__, __LINE__, 0);
#define SDB_DMP_CA(__buf, __len, __addr) \
    sdb_dump(__buf, __len, __addr,\
            __sdb_cfg, 0, __FILE__, __LINE__, 0);
#define SDB_DMP_I(__buf, __len, ...) \
    sdb_dump(__buf, __len, 0,\
            __sdb_cfg, SDB_DUMP_NOCHAR, __FILE__, __LINE__, __VA_ARGS__);
#define SDB_DMP_CI(__buf, __len, ...) \
    sdb_dump(__buf, __len, 0,\
            __sdb_cfg, 0, __FILE__, __LINE__, __VA_ARGS__);
#define SDB_DMP_CAI(__buf, __len, __addr, ...) \
    sdb_dump(__buf, __len, __addr,\
            __sdb_cfg, 0, __FILE__, __LINE__, __VA_ARGS__);
#else
#define SDB_DMP(...)        sdb_nop()
#define SDB_DMP_C(...)      sdb_nop()
#define SDB_DMP_CA(...)     sdb_nop()
#define SDB_DMP_I(...)      sdb_nop()
#define SDB_DMP_CI(...)     sdb_nop()
#define SDB_DMP_CAI(...)    sdb_nop()
#endif /* defined(SDB_ENABLE) */
/** @} */
/**
 * \block:      Menu
 * @{ */
#ifdef SDB_MENU
#undef SDB_MENU
#endif

#if defined(SDB_ENABLE)
#define SDB_MENU(...) {\
    sdb_item_t __sdb_item_list[] = { __VA_ARGS__ }; \
    sdb_menu(__sdb_cfg, __FILE__, __LINE__,\
            __sdb_item_list, sizeof(__sdb_item_list) / sizeof(sdb_item_t));\
}
#else
#define SDB_MENU(...)
#endif
/** @} */
/**
 * \block:      Code is far away from bug with the animal protecting
 * @{ */
/** normal {
 *     ┏━┓       ┏━┓
 *   ┏━┛ ┻━━━━━━━┛ ┻━┓
 *   ┃               ┃
 *   ┃      ━        ┃
 *   ┃ ┳━┛     ┗━┳   ┃
 *   ┃               ┃
 *   ┃     ┻         ┃
 *   ┃               ┃
 *   ┗━━━┳━      ┏━━━┛
 *       ┃       ┃
 *       ┃       ┃    神兽护体, 代码永无bug
 *       ┃       ┗━━━━━━━┓
 *       ┃               ┣━┓
 *       ┃               ┏━┛
 *       ┗━┓ ┓ ┏━━━┳━┓ ┏━┛
 *         ┃ ┫ ┫   ┃ ┫ ┫
 *         ┗━┻━┛   ┗━┻━┛
}*/
/** happy {
 *     ┏━┓       ┏━┓
 *   ┏━┛ ┻━━━━━━━┛ ┻━┓
 *   ┃               ┃
 *   ┃      ━        ┃
 *   ┃  >       <    ┃
 *   ┃               ┃
 *   ///   v   ///   ┃
 *   ┃               ┃
 *   ┗━━━┳━      ┏━━━┛
 *       ┃       ┃
 *       ┃       ┃    神兽护体, 代码永无bug
 *       ┃       ┗━━━━━━━┓
 *       ┃               ┣━┓
 *       ┃               ┏━┛
 *       ┗━┓ ┓ ┏━━━┳━┓ ┏━┛
 *         ┃ ┫ ┫   ┃ ┫ ┫
 *         ┗━┻━┛   ┗━┻━┛
}*/
/** cool {
 *     ┏━┓       ┏━┓  + +
 *   ┏━┛ ┻━━━━━━━┛ ┻━┓  + +
 *   ┃               ┃
 *   ┃      ━        ┃  + + + + +
 *   #####--#####    ┃  +
 *   ┃               ┃  +
 *   ┃     ┻         ┃
 *   ┃               ┃  + +
 *   ┗━━━┳━      ┏━━━┛
 *       ┃       ┃  + + + +
 *       ┃       ┃  + 神兽护体, 代码永无bug
 *       ┃       ┗━━━━━━━┓  + +
 *       ┃               ┣━┓
 *       ┃               ┏━┛
 *       ┗━┓ ┓ ┏━━━┳━┓ ┏━┛  + + + +
 *         ┃ ┫ ┫   ┃ ┫ ┫
 *         ┗━┻━┛   ┗━┻━┛  + + + +
}*/
/** @} */

