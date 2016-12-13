/**
 * Copyright(C) For free. All right reserved.
 *
 * Slim Debugger, a simple code tracer for C/C++.
 */

/**
 * \block:      Heads
 * @{ */
#ifndef __SDB_H__
#define __SDB_H__

#include <stdio.h>
#include <stdlib.h>

#define __sdb_cfg SDB_SET_CONFIG        //!< 调试配置宏

typedef enum {                          //!< 调试选项定义
    SDB_IO                  = (1 << 0), //!< 调试使能

    SDB_DUMP_NONUM          = (1 << 1), //!< 不输出计数或地址
    SDB_DUMP_NOHEX          = (1 << 2), //!< 不输出16进制
    SDB_DUMP_NOCHAR         = (1 << 3), //!< 不输出字符
    SDB_DUMP_SEGADD16       = (1 << 4), //!< 每行增加16字节
    SDB_DUMP_SEGADD32       = (1 << 5), //!< 每行增加32字节
} sdb_option_t;

typedef enum {                                              //!< 输出标记定义
    SDB_FLG_LV_OFS          = 0,                            //!< 调试等级偏移
    SDB_FLG_LV_ERR          = (0x01 << SDB_FLG_LV_OFS),     //!< 错误
    SDB_FLG_LV_WARN         = (0x02 << SDB_FLG_LV_OFS),     //!< 警告
    SDB_FLG_LV_INFO         = (0x03 << SDB_FLG_LV_OFS),     //!< 消息
    SDB_FLG_LV              = (0x0f << SDB_FLG_LV_OFS),     //!< 调试等级位与

    SDB_FLG_T_OFS           = 4,                            //!< 消息类型标记
    SDB_FLG_T_INPUT_NUM     = (0x01 << (SDB_FLG_T_OFS)),    //!< 输入数值
    SDB_FLG_T_INPUT_STR     = (0x02 << (SDB_FLG_T_OFS)),    //!< 输入字符
    SDB_FLG_T_INPUTECHO     = (0x03 << (SDB_FLG_T_OFS)),    //!< 输入反馈
    SDB_FLG_T_DUMP          = (0x04 << (SDB_FLG_T_OFS)),    //!< 导出数据
    SDB_FLG_T               = (0x0f << (SDB_FLG_T_OFS)),    //!< 消息类型位与

    SDB_FLG_OFS             = 8,                            //!< 信息属性偏移
    SDB_FLG_BARE            = (1 << (SDB_FLG_OFS + 0)),     //!< 无格式
    SDB_FLG_NOWRAP          = (1 << (SDB_FLG_OFS + 1)),     //!< 无换行
    SDB_FLG_STDERR          = (1 << (SDB_FLG_OFS + 2)),     //!< 带有标准错误信息
} sdb_flag_t;

typedef enum {                          //!< 返回值定义
    SDB_RET_OK              = 0,        //!< 成功
    SDB_RET_PARAM_ERR       = -0x0a,    //!< 参数错误
    SDB_RET_NO_INPUT        = -0x0b,    //!< 无输入
    SDB_RET_UNKNOWN_INPUT   = -0x0c,    //!< 未知输入
    SDB_RET_PROCESS_ERR     = -0x0d,    //!< 处理错误
} sdb_ret_t;

typedef struct sdb_config_t sdb_config_t;
typedef struct sdb_bio_context_t sdb_bio_context_t;
typedef struct sdb_item_t sdb_item_t;
/**
 * \brief       输出接口函数类型
 * \param       ctx         调试上下文结构体
 */
typedef void (* sdb_bio_out_t)(const sdb_bio_context_t *ctx);
/**
 * \brief       输入接口函数类型
 * \param       cfg         配置结构体
 * \param       buf         获取输入数据的缓存
 * \param       bufsize     缓存大小, 0:无限制
 * \param       outlen      获取输入数据长度的指针，可能为NULL
 * \return      0:Success; <0:Error
 * \detail      输入过程不退出，直到收到回车或缓存超出才返回，包含换行符
 */
typedef int (* sdb_bio_in_t)(char * buf, size_t bufsize, size_t *outlen);
struct sdb_config_t {                   //!< 配置结构体
    int opt;                            //!< 调试项目选项, sdb_option_t
    sdb_bio_in_t input;                 //!< 输入接口函数
    sdb_bio_out_t output;               //!< 输出接口函数
    void *ptr;                          //!< 输出接口传递参数
};
struct sdb_bio_context_t {              //!< 调试上下文结构体
    const sdb_config_t *cfg;            //!< 配置结构体
    int flag;                           //!< 输出标记定义, sdb_flag_t
    const char *file;                   //!< 文件名
    const char *func;                   //!< 函数名
    size_t line;                        //!< 行号
    char *buf;                          //!< 缓存, size:SDB_CONF_BUFFER_SIZE
    int len;                            //!< 缓存数据长度
};
struct sdb_item_t {                     //!< 调试菜单项目结构体
    char *info;                         //!< 项目显示信息
    void *param;                        //!< 参数
    int (*func)(void *);                //!< 函数
};

#ifdef __cplusplus
extern "C" {
#endif
/**\brief       标准输入输出，SDB_SYS_HAVE_STDIO启用时有效 */
extern const sdb_config_t sdb_cfg_std;

/**
 * \brief       调试输出控制
 * \param       cfg         配置结构体
 * \param       flag        输出标记, sdb_flag_t
 * \param       file        __FILE__
 * \param       func        __func__
 * \param       line        __LINE__
 * \param       fmt         格式化输出
 * \param       ...         不定参数
 * \return      0:Success; <0:Error
 */
int sdb_output(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, size_t line,
        const char *fmt, ...);

/**
 * \brief       调试输入控制
 * \param       cfg         配置结构体
 * \param       flag        输出标记, sdb_flag_t
 * \param       file        __FILE__
 * \param       func        __func__
 * \param       line        __LINE__
 * \param       buf         SDB_FLG_T_INPUT_STR: 输出缓存，可以为NULL
 *                          SDB_FLG_T_INPUT_NUM: 不使用
 * \param       bufsize     SDB_FLG_T_INPUT_STR: 输出缓存大小
 *                          SDB_FLG_T_INPUT_NUM: 不使用
 * \param       pnum        SDB_FLG_T_INPUT_STR: 输出长度
 *                          SDB_FLG_T_INPUT_NUM: 数值
 *                          可以为NULL
 * \param       fmt         格式化输出
 * \param       ...         不定参数
 * \return      SDB_FLG_T_INPUT_STR:    0:Success; <0:Error
 *              SDB_FLG_T_INPUT_NUM:    ~:输入的数值 sdb_ret_t:Error;
 */
int sdb_input(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, size_t line,
        char *buf, size_t bufsize, int *pnum, const char *fmt, ...);

/**
 * \brief       数据导出控制
 * \param       cfg         配置结构体
 * \param       opt         数据导出控制选项, sdb_option_t
 * \param       file        __FILE__
 * \param       func        __func__
 * \param       line        __LINE__
 * \param       buf         数据
 * \param       len         数据长度
 * \param       addr        地址
 * \param       fmt         格式化输出
 * \param       ...         不定参数
 * \return      0:Success; <0:Error
 */
int sdb_dump(const sdb_config_t *cfg, int opt,
        const char *file, const char *func, size_t line,
        void *data, size_t len, void *addr, const char *fmt, ...);

int sdb_menu(const sdb_config_t *cfg,
        const char *file, const char *func, size_t line,
        sdb_item_t *list, size_t num);

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
#undef SDB_OUT              //!< bare
#endif
#ifdef SDB_OUT_I
#undef SDB_OUT_I            //!< send Information
#endif
#ifdef SDB_OUT_W
#undef SDB_OUT_W            //!< send Warning
#endif
#ifdef SDB_OUT_E
#undef SDB_OUT_E            //!< send Error
#endif

#if defined(SDB_ENABLE)
#define SDB_OUT(...)        sdb_output(__sdb_cfg,\
        SDB_FLG_LV_INFO | SDB_FLG_BARE | SDB_FLG_NOWRAP,\
        __FILE__, __func__, __LINE__, __VA_ARGS__)
#define SDB_OUT_I(...)      sdb_output(__sdb_cfg,\
        SDB_FLG_LV_INFO,\
        __FILE__, __func__, __LINE__, __VA_ARGS__)
#define SDB_OUT_W(...)      sdb_output(__sdb_cfg,\
        SDB_FLG_LV_WARN,\
        __FILE__, __func__, __LINE__, __VA_ARGS__)
#define SDB_OUT_E(...)      sdb_output(__sdb_cfg,\
        SDB_FLG_LV_ERR,\
        __FILE__, __func__, __LINE__, __VA_ARGS__)
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
#undef SDB_IN_N             //!< get Number
#endif
#ifdef SDB_IN_S
#undef SDB_IN_S             //!< get String
#endif
#ifdef SDB_IN_NI
#undef SDB_IN_NI            //!< get Number, with Information
#endif
#ifdef SDB_IN_SI
#undef SDB_IN_SI            //!< get String, with Information
#endif

#if defined(SDB_ENABLE)
#define SDB_IN_N(__pnum)                        ({ sdb_input(__sdb_cfg,\
            SDB_FLG_T_INPUT_NUM,\
            __FILE__, __func__, __LINE__,\
            NULL, 0, __pnum, NULL); })
#define SDB_IN_S(__buf, __psize, __pnum)        ({ sdb_input(__sdb_cfg,\
            SDB_FLG_T_INPUT_STR,\
            __FILE__, __func__, __LINE__,\
            __buf, __psize, __pnum, NULL); })
#define SDB_IN_NI(__pnum, ...)                  ({ sdb_input(__sdb_cfg,\
            SDB_FLG_T_INPUT_NUM,\
            __FILE__, __func__, __LINE__,\
            NULL, 0, __pnum, __VA_ARGS__); })
#define SDB_IN_SI(__buf, __psize, __pnum, ...)  ({ sdb_input(__sdb_cfg,\
            SDB_FLG_T_INPUT_STR,\
            __FILE__, __func__, __LINE__,\
            __buf, __psize, __pnum, __VA_ARGS__); })
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
#undef SDB_DMP              //!< dump in Hex
#endif
#ifdef SDB_DMP_C
#undef SDB_DMP_C            //!< dump with SOME Charactors
#endif
#ifdef SDB_DMP_CA
#undef SDB_DMP_CA           //!< dump, with address and some Charactors
#endif
#ifdef SDB_DMP_I
#undef SDB_DMP_I            //!< dump with Information
#endif
#ifdef SDB_DMP_CI
#undef SDB_DMP_CI           //!< dump with Information and some Charactors
#endif
#ifdef SDB_DMP_CAI
#undef SDB_DMP_CAI          //!< dump with Information, address, and some Charactors
#endif

#if defined(SDB_ENABLE)
#define SDB_DMP(__buf, __len)                   sdb_dump(__sdb_cfg,\
        SDB_DUMP_NOCHAR,\
        __FILE__, __func__, __LINE__, __buf, __len, NULL, NULL);
#define SDB_DMP_C(__buf, __len)                 sdb_dump(__sdb_cfg,\
        0,\
        __FILE__, __func__, __LINE__, __buf, __len, NULL, NULL);
#define SDB_DMP_CA(__buf, __len, __addr)        sdb_dump(__sdb_cfg,\
        0,\
        __FILE__, __func__, __LINE__, __buf, __len, __addr, NULL);
#define SDB_DMP_I(__buf, __len, ...)            sdb_dump(__sdb_cfg,\
        SDB_DUMP_NOCHAR,\
        __FILE__, __func__, __LINE__, __buf, __len, NULL, __VA_ARGS__);
#define SDB_DMP_CI(__buf, __len, ...)           sdb_dump(__sdb_cfg,\
        0,\
        __FILE__, __func__, __LINE__, __buf, __len, NULL, __VA_ARGS__);
#define SDB_DMP_CAI(__buf, __len, __addr, ...)  sdb_dump(__sdb_cfg,\
        0,\
        __FILE__, __func__, __LINE__, __buf, __len, __addr, __VA_ARGS__);
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
    sdb_menu(__sdb_cfg, __FILE__, __func__, __LINE__,\
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

