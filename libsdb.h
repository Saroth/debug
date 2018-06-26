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
} sdb_err_type;

typedef int (*func_sdb_bio_out)(void *p, const char *file, size_t line,
        const char *str);
typedef int (*func_sdb_bio_in)(void *p, char *buf, size_t size, size_t *len);
typedef struct {
    size_t *stack_mark;
    size_t stack_top;
    size_t stack_max_usage;

    func_sdb_bio_out bio_out;
    func_sdb_bio_in bio_in;
    void *bio_param;

    size_t out_column_limit;
    size_t dump_bytes_perline;
    unsigned char out_has_color;
    unsigned char dump_has_addr;
    unsigned char dump_has_hex;
    unsigned char dump_has_ascii;
} sdb_context;

inline int sdb_nop(void) { return 0; }
#define sdb_assert(_f) do { if ((ret = _f) < 0) { return ret; } } while (0)

size_t sdb_stack_mark(sdb_context *ctx);
size_t sdb_stack_touch(sdb_context *ctx);
size_t sdb_stack_max_usage(sdb_context *ctx);

void sdb_config_init(sdb_context *ctx);
void sdb_config_bio(sdb_context *ctx,
        func_sdb_bio_out out, func_sdb_bio_in in, void *p);
void sdb_config_out_color(sdb_context *ctx, unsigned int has_color);
void sdb_config_out_column_limit(sdb_context *ctx, size_t limit);
void sdb_config_dump_bytes_perline(sdb_context *ctx, size_t size);
void sdb_config_dump_format(sdb_context *ctx, unsigned int has_addr,
        unsigned int has_hex, unsigned int has_ascii);

typedef enum {          /* 输出模式定义 */
    SDB_MSG_OFS         = 0,
    SDB_MSG_LEVEL_16    = (0   << SDB_MSG_OFS),
    SDB_MSG_LEVEL_15    = (1   << SDB_MSG_OFS),
    SDB_MSG_LEVEL_14    = (2   << SDB_MSG_OFS),
    SDB_MSG_LEVEL_13    = (3   << SDB_MSG_OFS),
    SDB_MSG_LEVEL_12    = (4   << SDB_MSG_OFS),
    SDB_MSG_LEVEL_11    = (5   << SDB_MSG_OFS),
    SDB_MSG_LEVEL_10    = (6   << SDB_MSG_OFS),
    SDB_MSG_LEVEL_9     = (7   << SDB_MSG_OFS),
    SDB_MSG_LEVEL_8     = (8   << SDB_MSG_OFS),
    SDB_MSG_LEVEL_7     = (9   << SDB_MSG_OFS),
    SDB_MSG_LEVEL_6     = (10  << SDB_MSG_OFS),
    SDB_MSG_LEVEL_5     = (11  << SDB_MSG_OFS),
    SDB_MSG_LEVEL_4     = (12  << SDB_MSG_OFS),
    SDB_MSG_LEVEL_3     = (13  << SDB_MSG_OFS),
    SDB_MSG_LEVEL_2     = (14  << SDB_MSG_OFS),
    SDB_MSG_LEVEL_1     = (15  << SDB_MSG_OFS),
    SDB_MSG_MASK        = (0xF << SDB_MSG_OFS),

    SDB_TYPE_OFS        = 4,
    SDB_TYPE_INFO       = (0   << SDB_TYPE_OFS),
    SDB_TYPE_WARNING    = (1   << SDB_TYPE_OFS),
    SDB_TYPE_ERROR      = (2   << SDB_TYPE_OFS),
    SDB_TYPE_DUMP       = (3   << SDB_TYPE_OFS),
    SDB_TYPE_INPUT_STR  = (4   << SDB_TYPE_OFS),
    SDB_TYPE_INPUT_NUM  = (5   << SDB_TYPE_OFS),
    SDB_TYPE_INPUT_ECHO = (6   << SDB_TYPE_OFS),
    SDB_TYPE_MENU_LIST  = (7   << SDB_TYPE_OFS),
    SDB_TYPE_MENU_FORM  = (8   << SDB_TYPE_OFS),
    SDB_TYPE_MENU_CHAOS = (9   << SDB_TYPE_OFS),
    SDB_TYPE_MASK       = (0xF << SDB_TYPE_OFS),

    SDB_MSG_NONE        = (SDB_MSG_LEVEL_12 | SDB_TYPE_INFO),
    SDB_MSG_INFO        = (SDB_MSG_LEVEL_8  | SDB_TYPE_INFO),
    SDB_MSG_WARNING     = (SDB_MSG_LEVEL_6  | SDB_TYPE_WARNING),
    SDB_MSG_ERROR       = (SDB_MSG_LEVEL_2  | SDB_TYPE_ERROR),
    SDB_MSG_INPUT_STR   = (SDB_MSG_LEVEL_4  | SDB_TYPE_INPUT_STR),
    SDB_MSG_INPUT_NUM   = (SDB_MSG_LEVEL_4  | SDB_TYPE_INPUT_NUM),
    SDB_MSG_INPUT_ECHO  = (SDB_MSG_LEVEL_4  | SDB_TYPE_INPUT_ECHO),
    SDB_MSG_MENU_LIST   = (SDB_MSG_LEVEL_2  | SDB_TYPE_MENU_LIST),
    SDB_MSG_MENU_FORM   = (SDB_MSG_LEVEL_2  | SDB_TYPE_MENU_FORM),
    SDB_MSG_MENU_CHAOS  = (SDB_MSG_LEVEL_2  | SDB_TYPE_MENU_CHAOS),
    SDB_MSG_DUMP        = (SDB_MSG_LEVEL_10 | SDB_TYPE_DUMP),
} sdb_mode_type;
int __sdb_vmcout(sdb_context *ctx, unsigned int mode,
        const char *file, size_t line, const char *fmt, va_list va);
#define __sdb_vcout(_c, _f, _v) __sdb_vmcout(_c, 0, 0, 0, _f, _v)
inline int __sdb_mcout(sdb_context *ctx, unsigned int mode,
        const char *file, size_t line, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    int ret = __sdb_vmcout(ctx, mode, file, line, fmt, va);
    va_end(va);
    return ret;
}
#define __sdb_cout(_c, _f, ...) __sdb_mcout(_c, 0, 0, 0, _f, __VA_ARGS__)
typedef struct {
    sdb_context *ctx;
    unsigned int mode;
    const char *file;
    size_t line;
    int err;
    char *line_buf;
    size_t line_buf_size;
    size_t line_buf_len;
    size_t counter;
} sdb_cout_context;
void __sdb_mcout_init(sdb_cout_context *ctx,
        sdb_context *sdb_ctx, unsigned int mode, char *buf, size_t size,
        const char *file, size_t line);
int __sdb_mcout_append_string(sdb_cout_context *ctx, const char *str);
int __sdb_mcout_append(sdb_cout_context *ctx, const char *fmt, ...);
int __sdb_mcout_append_va(sdb_cout_context *ctx, const char *fmt, va_list va);
int __sdb_mcout_append_endline(sdb_cout_context *ctx);
int __sdb_mcout_final(sdb_cout_context *ctx);

int __sdb_vmcin(sdb_context *ctx, unsigned int mode,
        char *buf, size_t size, size_t *len,
        const char *file, size_t line, const char *fmt, va_list va);
inline int __sdb_mcin(sdb_context *ctx, unsigned int mode,
        char *buf, size_t size, size_t *len,
        const char *file, size_t line, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    int ret = __sdb_vmcin(ctx, mode, buf, size, len, file, line, fmt, va);
    va_end(va);
    return ret;
}
int __sdb_vnmcin(sdb_context *ctx, int *num,
        const char *file, size_t line, const char *fmt, va_list va);
inline int __sdb_nmcin(sdb_context *ctx, int *num,
        const char *file, size_t line, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    int ret = __sdb_vnmcin(ctx, num, file, line, fmt, va);
    va_end(va);
    return ret;
}
int __sdb_cin(sdb_context *ctx, char *buf, size_t size, size_t *len);

int __sdb_vmdump(sdb_context *ctx, const void *buf, size_t size, void *addr,
        const char *file, size_t line, const char *fmt, va_list va);
inline int __sdb_mdump(sdb_context *ctx,
        const void *buf, size_t size, void *addr,
        const char *file, size_t line, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    int ret = __sdb_vmdump(ctx, buf, size, addr, file, line, fmt, va);
    va_end(va);
    return ret;
}
#define __sdb_dump(_c, _b, _s) __sdb_mdump(_c, _b, _s, 0, 0, 0, 0)

typedef struct {
    const char *info;
    int (*func)(void *);
    void *param;
} sdb_menu_list;
int __sdb_menu(sdb_context *ctx, unsigned int mode,
        const sdb_menu_list *list, size_t num, const char *file, size_t line);



int sdb_vsnprintf(char *buf, size_t size, const char *fmt, va_list va);
#define sdb_vsprintf(_b, _f, _v) sdb_vsnprintf(_b, 0xFFFFFFFF, _f, _v);
inline int sdb_snprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    int ret = sdb_vsnprintf(buf, size, fmt, va);
    va_end(va);
    return ret;
}
#define sdb_sprintf(_b, _f, ...) sdb_snprintf(_b, 0xFFFFFFFF, _f, __VA_ARGS__)

#ifdef __cplusplus
}
#endif
#endif /* __SDB_H__ */

#define sdb_out()
#define sdb_out_info()
#define sdb_out_warn()
#define sdb_out_err()

#define sdb_in()
#define sdb_in_num()
#define sdb_in_str()
#define sdb_in_num_info()
#define sdb_in_str_info()

#define sdb_dump()
#define sdb_dump_info()
#define sdb_dump_addr()
#define sdb_dump_addr_info()


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

