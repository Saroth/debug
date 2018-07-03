/**
 * Copyright(C) For free. All right reserved.
 *
 * Slim Debugger, a simple code tracer for C/C++.
 */
/**
 * \block:      heads
 * @{ */
#ifndef __SDB_H__
#define __SDB_H__
#ifdef __cplusplus
extern "C" {
#endif

/* For size_t */
#include <stddef.h>
/* For va_list */
#include <stdarg.h>

typedef enum {
    SDB_ERR_BAD_PARAM       = -0x100,
    SDB_ERR_NO_INPUT        = -0x101,
    SDB_ERR_UNKNOWN_INPUT   = -0x102,

    SDB_ERR_MAX             = -0x103
} sdb_err_type;

typedef int (*func_sdb_bio_out)(void *p, const char *file, size_t line,
        const char *str);
typedef int (*func_sdb_bio_in)(void *p, char *buf, size_t size, size_t *len);
typedef struct {
    const char *normal;     /* 默认: normal */
    const char *info;       /* 提示高亮: bold */
    const char *warning;    /* 警告高亮: blue, bold */
    const char *error;      /* 错误高亮: red, bold */
    const char *input;      /* 输入和反馈标记高亮: green, bold */
    const char *title;      /* 标题高亮: inverse */
    const char *tail;       /* 末尾/恢复默认 */
} sdb_color_codes;
typedef struct {
    const char *none;
    const char *info;
    const char *warning;
    const char *error;
    const char *input_number;
    const char *input_string;
    const char *input_echo;
    const char *dump;
    const char *menu;
} sdb_mark_codes;
typedef struct {
    size_t *stack_mark;
    size_t stack_top;
    size_t stack_max_usage;

    func_sdb_bio_out bio_out;
    func_sdb_bio_in bio_in;
    void *bio_param;

    size_t out_column_limit;            /* range:[36, ~) */
    size_t dump_bytes_perline;          /* range:[4, 0xffff] */
    unsigned char dump_has_addr;
    unsigned char dump_has_hex;
    unsigned char dump_has_ascii;

    const sdb_color_codes *colors;
    const sdb_mark_codes *marks;
} sdb_context;

inline int sdb_nop(void) { return 0; }
#define sdb_assert(_f) do { if ((ret = _f) < 0) { return ret; } } while (0)

extern const sdb_color_codes sdb_color_terminal;
extern const sdb_mark_codes sdb_mark_default;
extern const sdb_context sdb_ctx_default;

size_t sdb_stack_mark(sdb_context *ctx);
size_t sdb_stack_touch(sdb_context *ctx);
size_t sdb_stack_max_usage(const sdb_context *ctx);

void sdb_config_init(sdb_context *ctx);
void sdb_config_bio(sdb_context *ctx,
        func_sdb_bio_out out, func_sdb_bio_in in, void *p);
void sdb_config_color(sdb_context *ctx, const sdb_color_codes *colors);
void sdb_config_mark(sdb_context *ctx, const sdb_mark_codes *marks);
void sdb_config_out_column_limit(sdb_context *ctx, size_t limit);
void sdb_config_dump_bytes_perline(sdb_context *ctx, size_t size);
void sdb_config_dump_format(sdb_context *ctx, unsigned int has_addr,
        unsigned int has_hex, unsigned int has_ascii);

typedef enum {              /* 输出模式定义 */
    SDB_LEVEL_OFS           = 0,
    SDB_LEVEL_16            = (0    << SDB_LEVEL_OFS),
    SDB_LEVEL_15            = (1    << SDB_LEVEL_OFS),
    SDB_LEVEL_14            = (2    << SDB_LEVEL_OFS),
    SDB_LEVEL_13            = (3    << SDB_LEVEL_OFS),
    SDB_LEVEL_12            = (4    << SDB_LEVEL_OFS),
    SDB_LEVEL_11            = (5    << SDB_LEVEL_OFS),
    SDB_LEVEL_10            = (6    << SDB_LEVEL_OFS),
    SDB_LEVEL_9             = (7    << SDB_LEVEL_OFS),
    SDB_LEVEL_8             = (8    << SDB_LEVEL_OFS),
    SDB_LEVEL_7             = (9    << SDB_LEVEL_OFS),
    SDB_LEVEL_6             = (10   << SDB_LEVEL_OFS),
    SDB_LEVEL_5             = (11   << SDB_LEVEL_OFS),
    SDB_LEVEL_4             = (12   << SDB_LEVEL_OFS),
    SDB_LEVEL_3             = (13   << SDB_LEVEL_OFS),
    SDB_LEVEL_2             = (14   << SDB_LEVEL_OFS),
    SDB_LEVEL_1             = (15   << SDB_LEVEL_OFS),
    SDB_LEVEL_MASK          = (0xF  << SDB_LEVEL_OFS),

    SDB_TYPE_OFS            = 4,
    SDB_TYPE_NONE           = (0    << SDB_TYPE_OFS),
    SDB_TYPE_INFO           = (1    << SDB_TYPE_OFS),
    SDB_TYPE_WARNING        = (2    << SDB_TYPE_OFS),
    SDB_TYPE_ERROR          = (3    << SDB_TYPE_OFS),
    SDB_TYPE_DUMP           = (4    << SDB_TYPE_OFS),
    SDB_TYPE_INPUT_STR      = (5    << SDB_TYPE_OFS),
    SDB_TYPE_INPUT_NUM      = (6    << SDB_TYPE_OFS),
    SDB_TYPE_INPUT_ECHO     = (7    << SDB_TYPE_OFS),
    SDB_TYPE_MENU           = (8    << SDB_TYPE_OFS),
    SDB_TYPE_MASK           = (0xF  << SDB_TYPE_OFS),

    SDB_MENU_OFS            = 8,
    SDB_MENU_LIST           = (0    << SDB_MENU_OFS),
    SDB_MENU_FORM           = (1    << SDB_MENU_OFS),
    SDB_MENU_PILE           = (2    << SDB_MENU_OFS),
    SDB_MENU_MAX            = (3    << SDB_MENU_OFS),
    SDB_MENU_MASK           = (0xF  << SDB_MENU_OFS),

    SDB_FLAG_OFS            = 12,
    SDB_FLAG_NO_DECORATE    = (1    << SDB_FLAG_OFS),
    SDB_FLAG_MASK           = (0xF  << SDB_FLAG_OFS),


    SDB_MSG_NONE            = (SDB_LEVEL_12 | SDB_TYPE_NONE),
    SDB_MSG_INFO            = (SDB_LEVEL_8  | SDB_TYPE_INFO),
    SDB_MSG_WARNING         = (SDB_LEVEL_6  | SDB_TYPE_WARNING),
    SDB_MSG_ERROR           = (SDB_LEVEL_2  | SDB_TYPE_ERROR),
    SDB_MSG_INPUT_STR       = (SDB_LEVEL_4  | SDB_TYPE_INPUT_STR),
    SDB_MSG_INPUT_NUM       = (SDB_LEVEL_4  | SDB_TYPE_INPUT_NUM),
    SDB_MSG_INPUT_ECHO      = (SDB_LEVEL_4  | SDB_TYPE_INPUT_ECHO),
    SDB_MSG_MENU_LIST       = (SDB_LEVEL_2  | SDB_TYPE_MENU | SDB_MENU_LIST),
    SDB_MSG_MENU_FORM       = (SDB_LEVEL_2  | SDB_TYPE_MENU | SDB_MENU_FORM),
    SDB_MSG_MENU_PILE       = (SDB_LEVEL_2  | SDB_TYPE_MENU | SDB_MENU_PILE),
    SDB_MSG_DUMP            = (SDB_LEVEL_10 | SDB_TYPE_DUMP),
} sdb_mode_type;
int __sdb_vmcout(const sdb_context *ctx, unsigned int mode,
        const char *file, size_t line, const char *fmt, va_list va);
#define __sdb_vcout(_c, _f, _v) __sdb_vmcout(_c, 0, 0, 0, _f, _v)
int __sdb_mcout(const sdb_context *ctx, unsigned int mode,
        const char *file, size_t line, const char *fmt, ...);
#define __sdb_cout(_c, ...) __sdb_mcout(_c, 0, 0, 0, __VA_ARGS__)

typedef struct {
    const sdb_context *ctx;
    unsigned int mode;
    const char *file;
    size_t line;
    int err;
    char *line_buf;
    size_t line_buf_size;
    size_t line_buf_len;
    size_t line_buf_offset;
    size_t counter;
} sdb_cout_context;
void __sdb_mcout_init(sdb_cout_context *ctx, const sdb_context *sdb_ctx,
        unsigned int mode, char *buf, size_t size,
        const char *file, size_t line);
int __sdb_mcout_append_string(sdb_cout_context *ctx, const char *str);
int __sdb_mcout_append(sdb_cout_context *ctx, const char *fmt, ...);
int __sdb_mcout_append_va(sdb_cout_context *ctx, const char *fmt, va_list va);
int __sdb_mcout_append_endline(sdb_cout_context *ctx);
int __sdb_mcout_final(sdb_cout_context *ctx);

int __sdb_vmcin(const sdb_context *ctx, unsigned int mode,
        char *buf, size_t size, size_t *len,
        const char *file, size_t line, const char *fmt, va_list va);
int __sdb_mcin(const sdb_context *ctx, unsigned int mode,
        char *buf, size_t size, size_t *len,
        const char *file, size_t line, const char *fmt, ...);
int __sdb_vnmcin(const sdb_context *ctx, unsigned int mode, int *num,
        const char *file, size_t line, const char *fmt, va_list va);
int __sdb_nmcin(const sdb_context *ctx, unsigned int mode, int *num,
        const char *file, size_t line, const char *fmt, ...);
int __sdb_rnmcin(const sdb_context *ctx, unsigned int mode,
        const char *file, size_t line);
int __sdb_cin(const sdb_context *ctx, char *buf, size_t size, size_t *len);

int __sdb_vmdump(const sdb_context *ctx,
        const void *data, size_t size, void *addr,
        const char *file, size_t line, const char *fmt, va_list va);
int __sdb_mdump(const sdb_context *ctx,
        const void *data, size_t size, void *addr,
        const char *file, size_t line, const char *fmt, ...);
#define __sdb_dump(_c, _b, _s) __sdb_mdump(_c, _b, _s, 0, 0, 0, 0)

typedef struct {
    const char *info;
    int (*func)(void *);
    void *param;
} sdb_menu_item;
int __sdb_menu(const sdb_context *ctx, unsigned int mode,
        const sdb_menu_item *list, size_t num, const char *file, size_t line);


int sdb_vsnprintf(char *buf, size_t size, const char *fmt, va_list va);
#define sdb_vsprintf(_b, _f, _v) sdb_vsnprintf(_b, 0xFFFFFFFF, _f, _v);
int sdb_snprintf(char *buf, size_t size, const char *fmt, ...);
#define sdb_sprintf(_b, ...) sdb_snprintf(_b, 0xFFFFFFFF, __VA_ARGS__)

#ifdef __cplusplus
}
#endif
#endif /* __SDB_H__ */
/** @} */

/**
 * \block:      defines
 * @{ */
#if defined(SDB_ENABLE)

#ifdef sdb_out_bare
#undef sdb_out_bare
#endif
#ifdef sdb_out
#undef sdb_out
#endif
#ifdef sdb_out_info
#undef sdb_out_info
#endif
#ifdef sdb_out_warn
#undef sdb_out_warn
#endif
#ifdef sdb_out_err
#undef sdb_out_err
#endif

#ifdef sdb_in_bare
#undef sdb_in_bare
#endif
#ifdef sdb_in
#undef sdb_in
#endif
#ifdef sdb_in_num
#undef sdb_in_num
#endif
#ifdef sdb_in_str
#undef sdb_in_str
#endif
#ifdef sdb_in_num_info
#undef sdb_in_num_info
#endif
#ifdef sdb_in_str_info
#undef sdb_in_str_info
#endif

#ifdef sdb_dump_bare
#undef sdb_dump_bare
#endif
#ifdef sdb_dump
#undef sdb_dump
#endif
#ifdef sdb_dump_info
#undef sdb_dump_info
#endif
#ifdef sdb_dump_addr
#undef sdb_dump_addr
#endif
#ifdef sdb_dump_addr_info
#undef sdb_dump_addr_info
#endif

#ifdef sdb_menu
#undef sdb_menu
#endif
#ifdef sdb_menu_list
#undef sdb_menu_list
#endif
#ifdef sdb_menu_form
#undef sdb_menu_form
#endif
#ifdef sdb_menu_pile
#undef sdb_menu_pile
#endif


#ifndef SDB_CTX_GLOBAL
#define SDB_CTX_GLOBAL (&sdb_ctx_default)
#endif

#define sdb_out_bare(...) \
    __sdb_cout(SDB_CTX_GLOBAL, __VA_ARGS__)
#define sdb_out(...) \
    __sdb_mcout(SDB_CTX_GLOBAL, SDB_MSG_NONE,\
            __FILE__, __LINE__, __VA_ARGS__)
#define sdb_out_info(...) \
    __sdb_mcout(SDB_CTX_GLOBAL, SDB_MSG_INFO,\
            __FILE__, __LINE__, __VA_ARGS__)
#define sdb_out_warn(...) \
    __sdb_mcout(SDB_CTX_GLOBAL, SDB_MSG_WARNING,\
            __FILE__, __LINE__, __VA_ARGS__)
#define sdb_out_err(...) \
    __sdb_mcout(SDB_CTX_GLOBAL, SDB_MSG_ERROR,\
            __FILE__, __LINE__, __VA_ARGS__)

#define sdb_in_bare(__buf, __size, __plen) \
    __sdb_cin(SDB_CTX_GLOBAL, __buf, __size, __plen)
#define sdb_in() \
    __sdb_rnmcin(SDB_CTX_GLOBAL, SDB_MSG_INPUT_NUM,\
            __FILE__, __LINE__)
#define sdb_in_num(__pnum) \
    __sdb_nmcin(SDB_CTX_GLOBAL, SDB_MSG_INPUT_NUM, __pnum,\
            __FILE__, __LINE__, 0)
#define sdb_in_str(__buf, __size, __plen) \
    __sdb_mcin(SDB_CTX_GLOBAL, SDB_MSG_INPUT_STR, __buf, __size, __plen,\
            __FILE__, __LINE__, 0)
#define sdb_in_num_info(__pnum, ...) \
    __sdb_nmcin(SDB_CTX_GLOBAL, SDB_MSG_INPUT_NUM, __pnum,\
            __FILE__, __LINE__, __VA_ARGS__)
#define sdb_in_str_info(__buf, __size, __plen, ...) \
    __sdb_mcin(SDB_CTX_GLOBAL, SDB_MSG_INPUT_STR, __buf, __size, __plen,\
            __FILE__, __LINE__, __VA_ARGS__)

#define sdb_dump_bare(__data, __size) \
    __sdb_dump(SDB_CTX_GLOBAL, __data, __size)
#define sdb_dump(__data, __size) \
    __sdb_mdump(SDB_CTX_GLOBAL, __data, __size, 0,\
            __FILE__, __LINE__, 0)
#define sdb_dump_info(__data, __size, ...) \
    __sdb_mdump(SDB_CTX_GLOBAL, __data, __size, 0,\
            __FILE__, __LINE__, __VA_ARGS__)
#define sdb_dump_addr(__data, __size, __addr) \
    __sdb_mdump(SDB_CTX_GLOBAL, __data, __size, __addr,\
            __FILE__, __LINE__, 0)
#define sdb_dump_addr_info(__data, __size, __addr, ...) \
    __sdb_mdump(SDB_CTX_GLOBAL, __data, __size, __addr,\
            __FILE__, __LINE__, __VA_ARGS__)

#define sdb_menu(__type, ...) ({\
        const sdb_menu_item __list[] = { __VA_ARGS__ };\
        __sdb_menu(SDB_CTX_GLOBAL, __type, __list,\
                sizeof(__list) / sizeof(sdb_menu_item),\
                __FILE__, __LINE__);\
                })
#define sdb_menu_list(__list, __num) \
    __sdb_menu(SDB_CTX_GLOBAL, SDB_MSG_MENU_LIST, __list, __num,\
            __FILE__, __LINE__)
#define sdb_menu_form(__list, __num) \
    __sdb_menu(SDB_CTX_GLOBAL, SDB_MSG_MENU_FORM, __list, __num,\
            __FILE__, __LINE__)
#define sdb_menu_pile(__list, __num) \
    __sdb_menu(SDB_CTX_GLOBAL, SDB_MSG_MENU_PILE, __list, __num,\
            __FILE__, __LINE__)

#else /* defined(SDB_ENABLE) */

#define sdb_out_bare(...)                                   sdb_nop()
#define sdb_out(...)                                        sdb_nop()
#define sdb_out_info(...)                                   sdb_nop()
#define sdb_out_warn(...)                                   sdb_nop()
#define sdb_out_err(...)                                    sdb_nop()

#define sdb_in_bare(__buf, __size, __plen)                  sdb_nop()
#define sdb_in()                                            sdb_nop()
#define sdb_in_num(__pnum)                                  sdb_nop()
#define sdb_in_str(__buf, __size, __plen)                   sdb_nop()
#define sdb_in_num_info(__pnum, ...)                        sdb_nop()
#define sdb_in_str_info(__buf, __size, __plen, ...)         sdb_nop()

#define sdb_dump_bare(__data, __size)                       sdb_nop()
#define sdb_dump(__data, __size)                            sdb_nop()
#define sdb_dump_info(__data, __size, ...)                  sdb_nop()
#define sdb_dump_addr(__data, __size, __addr)               sdb_nop()
#define sdb_dump_addr_info(__data, __size, __addr, ...)     sdb_nop()

#define sdb_menu(__type, ...)                               sdb_nop()
#define sdb_menu_list(__list, __num)                        sdb_nop()
#define sdb_menu_form(__list, __num)                        sdb_nop()
#define sdb_menu_pile(__list, __num)                        sdb_nop()

#endif /* defined(SDB_ENABLE) */

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

