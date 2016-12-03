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

typedef enum {                                              //!< 调试选项定义
    SDB_IO                  = (1 << 0),                     //!< 调试使能

    SDB_OPT_DUMP_OFS        = 4,                            //!< 数据导出控制偏移
    SDB_OPT_DUMP_NONUM      = (0x01 << SDB_OPT_DUMP_OFS),   //!< 不输出计数或地址
    SDB_OPT_DUMP_NOHEX      = (0x02 << SDB_OPT_DUMP_OFS),   //!< 不输出16进制
    SDB_OPT_DUMP_NOCHAR     = (0x03 << SDB_OPT_DUMP_OFS),   //!< 不输出字符
    SDB_OPT_DUMP_SEGADD16   = (0x04 << SDB_OPT_DUMP_OFS),   //!< 每行增加16字节
    SDB_OPT_DUMP_SEGADD32   = (0x05 << SDB_OPT_DUMP_OFS),   //!< 每行增加32字节
    SDB_OPT_DUMP            = (0x0f << SDB_OPT_DUMP_OFS),   //!< 数据导出控制位与
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
    int line;                           //!< 行号
    int length;                         //!< 缓存数据长度
    char *buffer;                       //!< 缓存, size:SDB_CONF_BUFFER_SIZE
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
 * \param       format      格式化输出
 * \param       ...         不定参数
 * \return      0:Success; <0:Error
 */
int sdb_output(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, int line, const char *format, ...);

/**
 * \brief       调试输入控制
 * \param       cfg         配置结构体
 * \param       flag        输出标记, sdb_flag_t
 * \param       file        __FILE__
 * \param       func        __func__
 * \param       line        __LINE__
 * \param       buf         SDB_FLG_T_INPUT_STR: 输出缓存
 *                          SDB_FLG_T_INPUT_NUM: 不使用
 * \param       bufsize     SDB_FLG_T_INPUT_STR: 输出缓存大小
 *                          SDB_FLG_T_INPUT_NUM: 不使用
 * \param       pnum        SDB_FLG_T_INPUT_STR: 输出长度
 *                          SDB_FLG_T_INPUT_NUM: 数值
 * \param       format      格式化输出
 * \param       ...         不定参数
 * \return      num != NULL:  0:Success <0:Error;
 *              num == NULL:  >=0:输入的数值 <0:Error;
 */
int sdb_input(const sdb_config_t *cfg, int flag,
        const char *file, const char *func, int line,
        char *buf, size_t bufsize, int *pnum, const char *format, ...);

/**
 * \brief       数据导出控制
 * \param       opt         输出控制, SDB_OPTION_T
 * \param       mode        显示模式, SDB_MODE_T
 * \param       file        __FILE__
 * \param       func        __func__
 * \param       line        __LINE__
 * \param       buf         数据
 * \param       len         数据长度
 * \param       addr        地址
 * \param       format      格式化输出
 * \param       ...         不定参数
 * \return      0:Success; <0:Error
 */
int sdb_dump(size_t opt, size_t mode,
        const char *file, const char *func, size_t line,
        void *buf, size_t len, void *addr, const char *format, ...);

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
#ifdef SDB_CONFIG_LOAD
#undef SDB_CONFIG_LOAD
#endif

#if defined(SDB_ENABLE)
#define SDB_CONFIG_LOAD(p) static const sdb_config_t *__sdb_cfg = p;
#else
#define SDB_CONFIG_LOAD(p)
#endif
/** @} */
/**
 * \block:      Output
 * @{ */
#ifdef SDB_OUT
#undef SDB_OUT
#endif
#ifdef SDB_OUT_I
#undef SDB_OUT_I
#endif
#ifdef SDB_OUT_W
#undef SDB_OUT_W
#endif
#ifdef SDB_OUT_E
#undef SDB_OUT_E
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
#undef SDB_IN_N
#endif
#ifdef SDB_IN_S
#undef SDB_IN_S
#endif
#ifdef SDB_IN_NT
#undef SDB_IN_NT
#endif
#ifdef SDB_IN_ST
#undef SDB_IN_ST
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
#define SDB_IN_NT(__pnum, ...)                  ({ sdb_input(__sdb_cfg,\
            SDB_FLG_T_INPUT_NUM,\
            __FILE__, __func__, __LINE__,\
            NULL, 0, __pnum, __VA_ARGS__); })
#define SDB_IN_ST(__buf, __psize, __pnum, ...)  ({ sdb_input(__sdb_cfg,\
            SDB_FLG_T_INPUT_STR,\
            __FILE__, __func__, __LINE__,\
            __buf, __psize, __pnum, __VA_ARGS__); })
#else
#define SDB_IN_N(...)   sdb_nop()
#define SDB_IN_S(...)   sdb_nop()
#define SDB_IN_NT(...)  sdb_nop()
#define SDB_IN_ST(...)  sdb_nop()
#endif /* defined(SDB_ENABLE) */
/** @} */
/**
 * \block:      Dump
 * @{ */
#ifdef SDB_DMP_H
#undef SDB_DMP_H
#endif
#ifdef SDB_DMP_HC
#undef SDB_DMP_HC
#endif
#ifdef SDB_DMP_HCA
#undef SDB_DMP_HCA
#endif
#ifdef SDB_DMP_HT
#undef SDB_DMP_HT
#endif
#ifdef SDB_DMP_HCT
#undef SDB_DMP_HCT
#endif
#ifdef SDB_DMP_HCAT
#undef SDB_DMP_HCAT
#endif

#if defined(SDB_ENABLE)
#define sdb_dmp_h(__sdb_opt, __buf, __len) ({\
        sdb_dump(__sdb_opt,\
                SDB_MODE_MARK_DUMP | SDB_MODE_DMP_HEX,\
                __FILE__, __func__, __LINE__, __buf, __len, NULL, NULL);\
        })
#define sdb_dmp_hc(__sdb_opt, __buf, __len) ({\
        sdb_dump(__sdb_opt,\
                SDB_MODE_MARK_DUMP | SDB_MODE_DMP_HEX | SDB_MODE_DMP_CHAR,\
                __FILE__, __func__, __LINE__, __buf, __len, NULL, NULL);\
        })
#define sdb_dmp_hca(__sdb_opt, __buf, __len, __addr) ({\
        sdb_dump(__sdb_opt,\
                SDB_MODE_MARK_DUMP | SDB_MODE_DMP_HEX | SDB_MODE_DMP_CHAR,\
                __FILE__, __func__, __LINE__, __buf, __len, __addr, NULL);\
        })
#define sdb_dmp_ht(__sdb_opt, __buf, __len, ...) ({\
        sdb_dump(__sdb_opt,\
                SDB_MODE_MARK_DUMP | SDB_MODE_DMP_HEX,\
                __FILE__, __func__, __LINE__, __buf, __len, NULL, __VA_ARGS__);\
        })
#define sdb_dmp_hct(__sdb_opt, __buf, __len, ...) ({\
        sdb_dump(__sdb_opt,\
                SDB_MODE_MARK_DUMP | SDB_MODE_DMP_HEX | SDB_MODE_DMP_CHAR,\
                __FILE__, __func__, __LINE__, __buf, __len, NULL, __VA_ARGS__);\
        })
#define sdb_dmp_hcat(__sdb_opt, __buf, __len, __addr, ...) ({\
        sdb_dump(__sdb_opt,\
                SDB_MODE_MARK_DUMP | SDB_MODE_DMP_HEX | SDB_MODE_DMP_CHAR,\
                __FILE__, __func__, __LINE__, __buf, __len, __addr,\
                __VA_ARGS__);\
        })
#else
#define SDB_DMP_H(...)      sdb_nop()
#define SDB_DMP_HC(...)     sdb_nop()
#define SDB_DMP_HCA(...)    sdb_nop()
#define SDB_DMP_HT(...)     sdb_nop()
#define SDB_DMP_HCT(...)    sdb_nop()
#define SDB_DMP_HCAT(...)   sdb_nop()
#endif /* defined(SDB_ENABLE) */
/** @} */
/**
 * \block:      Menu
 * @{ */
#ifdef SDB_MENU
#undef SDB_MENU
#endif

#if defined(SDB_ENABLE)
#define SDB_MENU(...)
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

