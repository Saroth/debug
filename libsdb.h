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

typedef enum {                          //!< 调试输入输出选项
    SDB_IO              = (1 << 0),     //!< 输入输出使能
    //!< 标签输出控制
    SDB_NO_LINE         = (1 << 1),     //!< 标签 - 不输出行号
    SDB_NO_FILE         = (1 << 2),     //!< 标签 - 不输出文件名
    SDB_FUNC            = (1 << 3),     //!< 标签 - 输出函数名，默认关闭
    SDB_TIME            = (1 << 4),     //!< 标签 - 输出时间戳，默认关闭
    //!< 信息类型筛选
    SDB_NO_INFO         = (1 << 8),     //!< 不输出所有信息
    SDB_NO_WARNING      = (1 << 9),     //!< 不输出所有警告
    SDB_NO_ERROR        = (1 << 10),    //!< 不输出所有错误
    SDB_NO_DUMP         = (1 << 11),    //!< 不输出所有dump信息
    //!< 输出样式控制
    SDB_NO_STDERR       = (1 << 12),    //!< 不输出标准错误信息
    SDB_NO_COLOR        = (1 << 13),    //!< 不输出高亮色
    SDB_NO_MARK         = (1 << 14),    //!< 不输出指示标记
    SDB_NO_LABLE        = (1 << 15),    //!< 不输出标签
    SDB_NO_WRAP         = (1 << 16),    //!< 不输出换行
    //!< 导出样式控制 - 字节分割，默认每行16
    SDB_DUMP_SEC_ADD16  = (1 << 17),    //!< 每行输出增加16字节
    SDB_DUMP_SEC_ADD32  = (1 << 18),    //!< 每行输出增加32字节
} SDB_OPTION_T;

typedef enum {                          //!< 显示模式 - 控制输出样式
    SDB_MODE_MARK_OFS       = 0,                            //!< 标记偏移
    SDB_MODE_MARK_NONE      = (0x00 << SDB_MODE_MARK_OFS),  //!< 无标记
    SDB_MODE_MARK_INFO      = (0x01 << SDB_MODE_MARK_OFS),  //!< 高亮提示标记
    SDB_MODE_MARK_WARNING   = (0x02 << SDB_MODE_MARK_OFS),  //!< 高亮警告标记
    SDB_MODE_MARK_ERROR     = (0x03 << SDB_MODE_MARK_OFS),  //!< 高亮错误标记
    SDB_MODE_MARK_GETNUM    = (0x04 << SDB_MODE_MARK_OFS),  //!< 高亮获取数值标记
    SDB_MODE_MARK_GETSTR    = (0x05 << SDB_MODE_MARK_OFS),  //!< 高亮获取字符串标记
    SDB_MODE_MARK_ECHO      = (0x06 << SDB_MODE_MARK_OFS),  //!< 高亮输入反馈标记
    SDB_MODE_MARK_ENTRY     = (0x07 << SDB_MODE_MARK_OFS),  //!< 入口标记
    SDB_MODE_MARK_EXIT      = (0x08 << SDB_MODE_MARK_OFS),  //!< 出口标记
    SDB_MODE_MARK_DUMP      = (0x09 << SDB_MODE_MARK_OFS),  //!< 导出标记
    SDB_MODE_MARK_MAX       = (0x0a << SDB_MODE_MARK_OFS),  //!< 标记类型数量
    SDB_MODE_MARK           = (0x0f << SDB_MODE_MARK_OFS),  //!< 标记类型获取

    SDB_MODE_HL_OFS         = 4,                            //!< 高亮类型偏移
    SDB_MODE_HL_NONE        = (0x00 << SDB_MODE_HL_OFS),    //!< 无高亮
    SDB_MODE_HL_SIGN        = (0x01 << SDB_MODE_HL_OFS),    //!< 特殊标记高亮
    SDB_MODE_HL_WARN        = (0x02 << SDB_MODE_HL_OFS),    //!< 警告高亮
    SDB_MODE_HL_ERR         = (0x03 << SDB_MODE_HL_OFS),    //!< 错误高亮
    SDB_MODE_HL_TITLE       = (0x04 << SDB_MODE_HL_OFS),    //!< 标题高亮
    SDB_MODE_HL_INPUT       = (0x05 << SDB_MODE_HL_OFS),    //!< 输入和反馈标记高亮
    SDB_MODE_HL_LABEL       = (0x06 << SDB_MODE_HL_OFS),    //!< 标签高亮
    SDB_MODE_HL_RES         = (0x07 << SDB_MODE_HL_OFS),    //!< 默认高亮
    SDB_MODE_HL_MAX         = (0x08 << SDB_MODE_HL_OFS),    //!< 高亮类型数量
    SDB_MODE_HL             = (0x0f << SDB_MODE_HL_OFS),    //!< 高亮类型获取

    SDB_MODE_MSG_OFS        = 8,                            //!< 信息类型偏移
    SDB_MODE_MSG_NONE       = (0x00 << SDB_MODE_MSG_OFS),   //!< 无特殊信息
    SDB_MODE_MSG_ENTRY      = (0x01 << SDB_MODE_MSG_OFS),   //!< 入口标记
    SDB_MODE_MSG_EXIT       = (0x02 << SDB_MODE_MSG_OFS),   //!< 出口标记
    SDB_MODE_MSG_STDERR     = (0x03 << SDB_MODE_MSG_OFS),   //!< 系统错误信息
    SDB_MODE_MSG_INPUTFLAG  = (0x04 << SDB_MODE_MSG_OFS),   //!< 输入前标记
    SDB_MODE_MSG_DUMPSIZE   = (0x05 << SDB_MODE_MSG_OFS),   //!< 导出数据大小
    SDB_MODE_MSG_MAX        = (0x06 << SDB_MODE_MSG_OFS),   //!< 信息类型数量
    SDB_MODE_MSG            = (0x0f << SDB_MODE_MSG_OFS),   //!< 信息类型获取

    SDB_MODE_DMP_OFS        = 12,                           //!< 数据导出类型偏移
    SDB_MODE_DMP_STR        = (0x00 << SDB_MODE_DMP_OFS),   //!< 字符输出,无换行,独立使用
    SDB_MODE_DMP_HEX        = (0x01 << SDB_MODE_DMP_OFS),   //!< 16进制输出
    SDB_MODE_DMP_CHAR       = (0x02 << SDB_MODE_DMP_OFS),   //!< 字符输出
    SDB_MODE_DMP_MAX        = (0x03 << SDB_MODE_DMP_OFS),   //!< 数据导出类型数量
    SDB_MODE_DMP            = (0x0f << SDB_MODE_DMP_OFS),   //!< 数据导出类型获取

} SDB_MODE_T;

typedef enum {                          //!< 返回值
    SDB_RET_OK              = 0,        //!< 成功
    SDB_RET_PARAM_ERR       = -0x0a,    //!< 参数错误
    SDB_RET_NO_INPUT        = -0x0b,    //!< 无输入
    SDB_RET_UNKNOWN_INPUT   = -0x0c,    //!< 未知输入
    SDB_RET_PROCESS_ERR     = -0x0d,    //!< 处理错误
} SDB_RET_T;

/**
 * \brief       输出接口函数类型
 * \param       buf         输出数据的缓存
 * \param       len         输出长度
 * \return      0:Success; <0:Error
 */
typedef int (* SDB_FUNC_OUTPUT_T)(char * buf, size_t len);
/**
 * \brief       输入接口函数类型
 * \param       buf         获取输入数据的缓存
 * \param       bufsize     缓存大小, 0:无限制
 * \param       outlen      获取输入数据长度的指针
 * \return      0:Success; <0:Error
 * \detail      输入过程不退出，直到收到回车或缓存超出才返回，包含换行符
 */
typedef int (* SDB_FUNC_INPUT_T)(char * buf, size_t bufsize, size_t *outlen);

typedef struct {                        //!< 基本IO配置结构体
    SDB_FUNC_OUTPUT_T f_output;         //!< 输出接口函数
    SDB_FUNC_INPUT_T f_input;           //!< 输入接口函数
} SDB_BIO_T;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief       设置基本IO
 * \param       bio         基本IO配置结构体指针
 * \return      0:Success; <0:Error
 */
int sdb_bio_conf(SDB_BIO_T *bio);

/**
 * \brief       样式&格式化输出控制
 * \param       opt         输出控制, SDB_OPTION_T
 * \param       mode        显示模式, SDB_MODE_T
 * \param       file        __FILE__
 * \param       func        __func__
 * \param       line        __LINE__
 * \param       format      格式化输出
 * \param       ...         不定参数
 * \return      0:Success; <0:Error
 */
int sdb_output(int opt, int mode, const char *file, const char *func, int line,
        const char *format, ...);

/**
 * \brief       样式输入控制
 * \param       opt         输出控制, SDB_OPTION_T
 * \param       mode        显示模式, SDB_MODE_T
 * \param       file        __FILE__
 * \param       func        __func__
 * \param       line        __LINE__
 * \param       buf         SDB_MODE_MARK_GETSTR: 输出缓存
 *                          SDB_MODE_MARK_GETNUM: 不使用
 * \param       num         SDB_MODE_MARK_GETSTR: 输出长度
 *                          SDB_MODE_MARK_GETNUM: 数值
 * \param       format      格式化输出
 * \param       ...         不定参数
 * \return      num != NULL:  0:Success <0:Error;
 *              num == NULL:  >=0:输入的数值 <0:Error;
 */
int sdb_input(int opt, int mode, const char *file, const char *func, int line,
        char *buf, int *num, const char *format, ...);

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
 * \return      0
 */
int sdb_nop(void);
#ifdef __cplusplus
}
#endif

#endif /* __SDB_H__ */
/** @} */

/**
 * \block:      Output
 * @{ */
#ifdef sdb_out
#undef sdb_out
#endif
#ifdef sdb_out_i
#undef sdb_out_i
#endif
#ifdef sdb_out_w
#undef sdb_out_w
#endif
#ifdef sdb_out_e
#undef sdb_out_e
#endif
#ifdef sdb_out_t
#undef sdb_out_t
#endif
#ifdef sdb_out_entry
#undef sdb_out_entry
#endif
#ifdef sdb_out_exit
#undef sdb_out_exit
#endif

#if defined(SDB_ENABLE)

/** \brief      格式化输出 */
#define sdb_out(__sdb_opt, ...) ({\
        sdb_output(__sdb_opt | SDB_NO_MARK | SDB_NO_LABLE | SDB_NO_WRAP, 0,\
                __FILE__, __func__, __LINE__, __VA_ARGS__);\
        })
/** \brief      带标签的格式化输出 */
#define sdb_out_i(__sdb_opt, ...) ({\
        sdb_output(__sdb_opt, SDB_MODE_MARK_INFO,\
                __FILE__, __func__, __LINE__, __VA_ARGS__);\
        })
/** \brief      带标签的格式化输出，带警告标记 */
#define sdb_out_w(__sdb_opt, ...) ({\
        sdb_output(__sdb_opt, SDB_MODE_MARK_WARNING,\
                __FILE__, __func__, __LINE__, __VA_ARGS__);\
        })
/** \brief      带标签的格式化输出，带错误标记 */
#define sdb_out_e(__sdb_opt, ...) ({\
        sdb_output(__sdb_opt, SDB_MODE_MARK_ERROR,\
                __FILE__, __func__, __LINE__, __VA_ARGS__);\
        })
/** \brief      带标签的格式化输出，标题高亮 */
#define sdb_out_t(__sdb_opt, ...) ({\
        sdb_output(__sdb_opt, SDB_MODE_MARK_INFO | SDB_MODE_HL_TITLE,\
                __FILE__, __func__, __LINE__, __VA_ARGS__);\
        })
/** \brief      带标签的入口标记输出 */
#define sdb_out_entry(__sdb_opt) ({\
        sdb_output(__sdb_opt,\
                SDB_MODE_MARK_ENTRY | SDB_MODE_HL_SIGN | SDB_MODE_MSG_ENTRY,\
                __FILE__, __func__, __LINE__, NULL);\
        })
/** \brief      带标签的出口标记输出 */
#define sdb_out_exit(__sdb_opt) ({\
        sdb_output(__sdb_opt,\
                SDB_MODE_MARK_EXIT | SDB_MODE_HL_SIGN | SDB_MODE_MSG_EXIT,\
                __FILE__, __func__, __LINE__, NULL);\
        })

#else /* defined(SDB_ENABLE) */

#define sdb_out(...)        sdb_nop()
#define sdb_out_i(...)      sdb_nop()
#define sdb_out_w(...)      sdb_nop()
#define sdb_out_e(...)      sdb_nop()
#define sdb_out_t(...)      sdb_nop()
#define sdb_out_entry(...)  sdb_nop()
#define sdb_out_exit(...)   sdb_nop()

#endif /* defined(SDB_ENABLE) */
/** @} */

/**
 * \block:      Output stderr
 * @{ */
#ifdef sdb_err
#undef sdb_err
#endif
#ifdef sdb_err_i
#undef sdb_err_i
#endif
#ifdef sdb_err_w
#undef sdb_err_w
#endif
#ifdef sdb_err_e
#undef sdb_err_e
#endif
#ifdef sdb_err_t
#undef sdb_err_t
#endif

#if defined(SDB_ENABLE)

/** \brief      格式化输出，带错误信息 */
#define sdb_err(__sdb_opt, ...) ({\
        sdb_output(__sdb_opt | SDB_NO_MARK | SDB_NO_LABLE | SDB_NO_WRAP,\
                SDB_MODE_MSG_STDERR,\
                __FILE__, __func__, __LINE__, __VA_ARGS__);\
        })
/** \brief      带标签的格式化输出，带错误信息 */
#define sdb_err_i(__sdb_opt, ...) ({\
        sdb_output(__sdb_opt, SDB_MODE_MARK_INFO | SDB_MODE_MSG_STDERR,\
                __FILE__, __func__, __LINE__, __VA_ARGS__);\
        })
/** \brief      带标签的格式化输出，带警告标记，带错误信息 */
#define sdb_err_w(__sdb_opt, ...) ({\
        sdb_output(__sdb_opt, SDB_MODE_MARK_WARNING | SDB_MODE_MSG_STDERR,\
                __FILE__, __func__, __LINE__, __VA_ARGS__);\
        })
/** \brief      带标签的格式化输出，带错误标记，带错误信息 */
#define sdb_err_e(__sdb_opt, ...) ({\
        sdb_output(__sdb_opt, SDB_MODE_MARK_ERROR | SDB_MODE_MSG_STDERR,\
                __FILE__, __func__, __LINE__, __VA_ARGS__);\
        })
/** \brief      带标签的格式化输出，标题高亮，带错误信息 */
#define sdb_err_t(__sdb_opt, ...) ({\
        sdb_output(__sdb_opt,\
                SDB_MODE_MARK_INFO | SDB_MODE_HL_TITLE | SDB_MODE_MSG_STDERR,\
                __FILE__, __func__, __LINE__, __VA_ARGS__);\
        })

#else

#define sdb_err(...)    sdb_nop()
#define sdb_err_i(...)  sdb_nop()
#define sdb_err_w(...)  sdb_nop()
#define sdb_err_e(...)  sdb_nop()
#define sdb_err_t(...)  sdb_nop()

#endif /* defined(SDB_ENABLE) */
/** @} */

/**
 * \block:      Input
 * @{ */
#ifdef sdb_in
#undef sdb_in
#endif
#ifdef sdb_in_n
#undef sdb_in_n
#endif
#ifdef sdb_in_s
#undef sdb_in_s
#endif
#ifdef sdb_in_nt
#undef sdb_in_nt
#endif
#ifdef sdb_in_st
#undef sdb_in_st
#endif

#if defined(SDB_ENABLE)

/** \brief      返回输入的数值，无效输入返回对应错误码 */
#define sdb_in(__sdb_opt) ({\
        sdb_input(__sdb_opt, SDB_MODE_MARK_GETNUM,\
                __FILE__, __func__, __LINE__, NULL, NULL, NULL);\
        })
/** \brief      返回输入的数值，无效输入返回对应错误码，可识别8进制、10进制和16进制 */
#define sdb_in_n(__sdb_opt, __pnum) ({\
        sdb_input(__sdb_opt, SDB_MODE_MARK_GETNUM,\
                __FILE__, __func__, __LINE__, NULL, __pnum, NULL);\
        })
/** \brief      获取输入的字符串，不包含换行符 */
#define sdb_in_s(__sdb_opt, __buf, __pnum) ({\
        sdb_input(__sdb_opt, SDB_MODE_MARK_GETSTR,\
                __FILE__, __func__, __LINE__, __buf, __pnum, NULL);\
        })
/** \brief      返回输入的数值，无效输入返回对应错误码，可识别8进制、10进制和16进制，同时输出标题 */
#define sdb_in_nt(__sdb_opt, __pnum, ...) ({\
        sdb_input(__sdb_opt, SDB_MODE_MARK_GETNUM,\
                __FILE__, __func__, __LINE__, NULL, __pnum, __VA_ARGS__);\
        })
/** \brief      获取输入的字符串，不包含换行符，同时输出标题 */
#define sdb_in_st(__sdb_opt, __buf, __pnum, ...) ({\
        sdb_input(__sdb_opt, SDB_MODE_MARK_GETSTR,\
                __FILE__, __func__, __LINE__, __buf, __pnum, __VA_ARGS__);\
        })

#else

#define sdb_in(...)     sdb_nop()
#define sdb_in_n(...)   sdb_nop()
#define sdb_in_s(...)   sdb_nop()
#define sdb_in_nt(...)  sdb_nop()
#define sdb_in_st(...)  sdb_nop()

#endif /* defined(SDB_ENABLE) */
/** @} */

/**
 * \block:      Dump
 * @{ */
#ifdef sdb_dmp
#undef sdb_dmp
#endif
#ifdef sdb_dmp_h
#undef sdb_dmp_h
#endif
#ifdef sdb_dmp_hc
#undef sdb_dmp_hc
#endif
#ifdef sdb_dmp_hca
#undef sdb_dmp_hca
#endif
#ifdef sdb_dmp_ht
#undef sdb_dmp_ht
#endif
#ifdef sdb_dmp_hct
#undef sdb_dmp_hct
#endif
#ifdef sdb_dmp_hcat
#undef sdb_dmp_hcat
#endif

#if defined(SDB_ENABLE)

/** \brief      纯字符导出, 无任何格式(可用于将内存或Flash数据导出到文件) */
#define sdb_dmp(__sdb_opt, __buf, __len) ({\
        sdb_dump(__sdb_opt | SDB_NO_MARK | SDB_NO_LABLE | SDB_NO_WRAP,\
                SDB_MODE_DMP_STR, __FILE__, __func__, __LINE__,\
                __buf, __len, NULL, NULL);\
        })
/** \brief      16进制导出, 带行号, 每行16字节 */
#define sdb_dmp_h(__sdb_opt, __buf, __len) ({\
        sdb_dump(__sdb_opt,\
                SDB_MODE_MARK_DUMP | SDB_MODE_DMP_HEX,\
                __FILE__, __func__, __LINE__, __buf, __len, NULL, NULL);\
        })
/** \brief      16进制导出, 同时输出字符, 带行号, 每行16字节 */
#define sdb_dmp_hc(__sdb_opt, __buf, __len) ({\
        sdb_dump(__sdb_opt,\
                SDB_MODE_MARK_DUMP | SDB_MODE_DMP_HEX | SDB_MODE_DMP_CHAR,\
                __FILE__, __func__, __LINE__, __buf, __len, NULL, NULL);\
        })
/** \brief      16进制导出, 同时输出字符, 带行号, 带地址, 每行16字节 */
#define sdb_dmp_hca(__sdb_opt, __buf, __len, __addr) ({\
        sdb_dump(__sdb_opt,\
                SDB_MODE_MARK_DUMP | SDB_MODE_DMP_HEX | SDB_MODE_DMP_CHAR,\
                __FILE__, __func__, __LINE__, __buf, __len, __addr, NULL);\
        })
/** \brief      16进制导出, 带标题和行号, 每行16字节 */
#define sdb_dmp_ht(__sdb_opt, __buf, __len, ...) ({\
        sdb_dump(__sdb_opt,\
                SDB_MODE_MARK_DUMP | SDB_MODE_DMP_HEX,\
                __FILE__, __func__, __LINE__, __buf, __len, NULL, __VA_ARGS__);\
        })
/** \brief      16进制导出, 同时输出字符, 带标题和行号, 每行16字节 */
#define sdb_dmp_hct(__sdb_opt, __buf, __len, ...) ({\
        sdb_dump(__sdb_opt,\
                SDB_MODE_MARK_DUMP | SDB_MODE_DMP_HEX | SDB_MODE_DMP_CHAR,\
                __FILE__, __func__, __LINE__, __buf, __len, NULL, __VA_ARGS__);\
        })
/** \brief      16进制导出, 同时输出字符, 带标题和地址, 每行16字节 */
#define sdb_dmp_hcat(__sdb_opt, __buf, __len, __addr, ...) ({\
        sdb_dump(__sdb_opt,\
                SDB_MODE_MARK_DUMP | SDB_MODE_DMP_HEX | SDB_MODE_DMP_CHAR,\
                __FILE__, __func__, __LINE__, __buf, __len, __addr,\
                __VA_ARGS__);\
        })

#else

#define sdb_dmp(...)        sdb_nop()
#define sdb_dmp_h(...)      sdb_nop()
#define sdb_dmp_hc(...)     sdb_nop()
#define sdb_dmp_hca(...)    sdb_nop()
#define sdb_dmp_ht(...)     sdb_nop()
#define sdb_dmp_hct(...)    sdb_nop()
#define sdb_dmp_hcat(...)   sdb_nop()

#endif /* defined(SDB_ENABLE) */
/** @} */

#define sdb_menu(...)


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

