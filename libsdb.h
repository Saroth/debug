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

#include <stdio.h>
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
} sdb_context;

inline int sdb_nop(void) { return 0; }

size_t sdb_stack_mark(sdb_context *ctx);
size_t sdb_stack_touch(sdb_context *ctx);
size_t sdb_stack_max_usage(sdb_context *ctx);

size_t sdb_config_size(void);
void sdb_config_init(sdb_context *ctx);
void sdb_config_bio(sdb_context *ctx,
        func_sdb_bio_out out, func_sdb_bio_in in, void *p);

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

int sdb_vmcout(sdb_context *ctx, unsigned int mode,
        const char *file, size_t line, const char *fmt, va_list va);
#define sdb_vcout(_c, _f, _va) sdb_vmcout(_c, 0, 0, 0, _f, _v)
inline int sdb_mcout(sdb_context *ctx, unsigned int mode,
        const char *file, size_t line, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    int ret = sdb_vmcout(ctx, mode, file, line, fmt, va);
    va_end(va);
    return ret;
}
#define sdb_cout(_c, _f, ...) sdb_mcout(_c, 0, 0, 0, _f, __VA_ARGS__)

int sdb_cin(sdb_context *ctx, char *buf, size_t size, size_t *len);
inline int sdb_mcin(sdb_context *ctx, unsigned int mode,
        char *buf, size_t size, size_t *len,
        const char *file, size_t line, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    int ret = sdb_vmcout(ctx, mode, file, line, fmt, va);

    va_end(va);
    return ret;
}

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

