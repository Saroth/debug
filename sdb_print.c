#include "sdb_config.h"

#if defined(SDB_SYS_SUPPORT_SIZE_T)
#include <stddef.h>
#endif

#if defined(SDB_ENABLE)

#define PUTC(_c) do {\
    ctx->c1 = (unsigned char)(_c);\
    if ((ctx->ret = ctx->put(ctx->ptr, (const char *)&ctx->c1, 1)))\
    return ctx->ret;\
} while (0)
#define PUTS(_s, _l) do {\
    if ((ctx->ret = ctx->put(ctx->ptr, _s, _l)))\
    return ctx->ret;\
} while (0)
#define FLAG_SET(__flag) ctx.flag |= (__flag)
#define FLAG_IS_SET(__flag) (ctx->flag & (__flag))
#define FLAG_UNSET(__flag) (ctx->flag &= ~(__flag))

static unsigned int s2u(const char **str, int base)
{
    unsigned int n = 0;
    unsigned char c = 0;
    int d;
    while (1) {
        c = (unsigned char)*(*str)++;
        if (c >= '0' && c <= '9')
            d = c - '0';
        else if (c >= 'a' && c <= 'f')
            d = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F')
            d = c - 'A' + 10;
        else
            break;
        if (d > base)
            break;
        n = n * base + d;
    }
    (*str)--;
    return n;
}

#define _U2S(__name, __type) \
    int __name(print_context_t *ctx, __type num) {\
        ctx->c2 = 0;\
        ctx->c4 = 1;\
        __type d = 1;\
        while (num / d >= ctx->base) { d *= ctx->base; ctx->c4++; }\
        if (FLAG_IS_SET(SIGN)) { ctx->c4++; }\
        if (FLAG_IS_SET(ALTERNATE_FORM)) {\
            if (ctx->base == 16) { ctx->c4 += 2; }\
            else if (ctx->base == 8) { ctx->c4++; }\
        }\
        ctx->width = (ctx->width > ctx->c4) ? (ctx->width - ctx->c4) : 0;\
        \
        if (ctx->width && !FLAG_IS_SET(ALIGN_LEFT) && !FLAG_IS_SET(PAD_ZERO)) {\
            while (ctx->width-- > 0) PUTS(" ", 1); }\
        if (FLAG_IS_SET(SIGN) && FLAG_IS_SET(PAD_ZERO)) {\
                PUTS(FLAG_IS_SET(NEGATIVE_SIGN) ? "-" : "+", 1); }\
        if (FLAG_IS_SET(ALTERNATE_FORM)) {\
            if (ctx->base == 16) {\
                if (FLAG_IS_SET(CAPITAL_HEX)) PUTS("0X", 2);\
                else PUTS("0x", 2);\
            }\
            else if (ctx->base == 8) { PUTS("0", 1); }\
        }\
        if (ctx->width && !FLAG_IS_SET(ALIGN_LEFT) && FLAG_IS_SET(PAD_ZERO)) {\
            while (ctx->width-- > 0) PUTS("0", 1); }\
        if (FLAG_IS_SET(SIGN) && !FLAG_IS_SET(PAD_ZERO)) {\
            PUTS(FLAG_IS_SET(NEGATIVE_SIGN) ? "-" : "+", 1); }\
        while (d > 0) {\
            ctx->ret = num / d;\
            num %= d;\
            d /= ctx->base;\
            PUTC(ctx->ret + (ctx->ret < 10 ? '0'\
                        : ((FLAG_IS_SET(CAPITAL_HEX) ? 'A' : 'a') - 10)));\
        }\
        if (ctx->width && FLAG_IS_SET(ALIGN_LEFT))\
        while (ctx->width-- > 0) PUTS(" ", 1);\
        return 0;\
    }
#if defined(SDB_SYS_SUPPORT_LONG_LONG)
_U2S(ulli2s, unsigned long long int);
#endif
#if defined(SDB_SYS_SUPPORT_LONG)
_U2S(uli2s, unsigned long int);
#endif
_U2S(ui2s, unsigned int);

static int print_u2s(print_context_t *ctx, va_list va)
{
    FLAG_UNSET(SIGN);
#if defined(SDB_SYS_SUPPORT_LONG)
    if (FLAG_IS_SET(LONG_INT))
        return uli2s(ctx, va_arg(va, unsigned long int));
    else
#endif
#if defined(SDB_SYS_SUPPORT_LONG_LONG)
    if (FLAG_IS_SET(LONG_LONG_INT))
        return ulli2s(ctx, va_arg(va, unsigned long long int));
    else
#endif
        return ui2s(ctx, va_arg(va, unsigned int));
}

#define _I2S(__name, __type) \
    int __name(print_context_t *ctx, __type num) {\
        if (num < 0) { ctx->flag |= (SIGN | NEGATIVE_SIGN); num = -num; }\
        u##__name(ctx, num);\
    }
#if defined(SDB_SYS_SUPPORT_LONG_LONG)
_I2S(lli2s, long long int);
#endif
#if defined(SDB_SYS_SUPPORT_LONG)
_I2S(li2s, long int);
#endif
_I2S(i2s, int);

static int print_i2s(print_context_t *ctx, va_list va)
{
#if defined(SDB_SYS_SUPPORT_LONG)
    if (FLAG_IS_SET(LONG_INT))
        return li2s(ctx, va_arg(va, long int));
    else
#endif
#if defined(SDB_SYS_SUPPORT_LONG_LONG)
    if (FLAG_IS_SET(LONG_LONG_INT))
        return lli2s(ctx, va_arg(va, long long int));
    else
#endif
        return i2s(ctx, va_arg(va, unsigned int));
}

static int print_str(print_context_t *ctx, va_list va)
{
    unsigned char *s = va_arg(va, unsigned char *);

    if (ctx->width) {
        unsigned int l = 0;
        while (s[l++]);
        ctx->width -= l - 1;
        if (!FLAG_IS_SET(ALIGN_LEFT))
            while (ctx->width--)
                PUTS(" ", 1);
    }
    if ((ctx->ret = ctx->put(ctx->ptr, s, 0)))
        return ctx->ret;
    if (ctx->width && FLAG_IS_SET(ALIGN_LEFT))
        while (ctx->width--)
            PUTS(" ", 1);

    return 0;
}

static int print_float(print_context_t *ctx, va_list va)
{
    /* Unsupported */
    ((void) ctx);
    va_arg(va, double);
    return 0;
}

int vxprint(void *ptr, put_t put, const char *fmt, va_list va)
{
    print_context_t ctx;

    ctx.c1 = 0;
    ctx.c2 = 0;
    ctx.c3 = 0;
    ctx.c4 = 0;
    ctx.ret = 0;
    ctx.ptr = ptr;
    ctx.put = put;

    while (ctx.ret == 0 && (ctx.c1 = (unsigned char)*fmt++)) {
        if (ctx.c1 != '%') {
            ctx.ret = put(ptr, (const char *)&ctx.c1, 1);
            continue;
        }
        ctx.flag = 0;
        ctx.width = 0;
        ctx.precision = 0;
        while ((ctx.c1 = *fmt++)) {          /* Flags */
            switch (ctx.c1) {
                case '-': FLAG_SET(ALIGN_LEFT);     continue;
                case '#': FLAG_SET(ALTERNATE_FORM); continue;
                case '+': FLAG_SET(SIGN);           continue;
                case '0': FLAG_SET(PAD_ZERO);       continue;
                case ' ':                           continue;
            }
            break;
        }
        if (ctx.c1 == '*') {                 /* Width */
            ctx.width = (unsigned char)va_arg(va, unsigned int);
            ctx.c1 = *fmt++;
        }
        else if (ctx.c1 >= '0' && ctx.c1 <= '9') {
            fmt--;
            ctx.width = (unsigned char)s2u(&fmt, 10);
            ctx.c1 = *fmt++;
        }
        if (ctx.c1 == '.') {                 /* Float */
            FLAG_SET(PAD_ZERO);
            ctx.c1 = *fmt++;
            if (ctx.c1 == '*')
                ctx.precision = (unsigned char)va_arg(va, unsigned int);
            else {
                fmt--;
                ctx.precision = (unsigned char)s2u(&fmt, 10);
            }
            ctx.c1 = *fmt++;
        }
#if defined(SDB_SYS_SUPPORT_LONG)
        if (ctx.c1 == 'z') {                 /* Long */
#if defined(SDB_SYS_SUPPORT_SIZE_T)
            if (sizeof(size_t) == sizeof(unsigned long int))
                FLAG_SET(LONG_INT);
#if defined(SDB_SYS_SUPPORT_LONG_LONG)
            else if (sizeof(size_t) == sizeof(unsigned long long int))
                FLAG_SET(LONG_LONG_INT);
#endif
#endif
            ctx.c1 = *fmt++;
        }
        else if (ctx.c1 == 'l') {
            FLAG_SET(LONG_INT);
#if defined(SDB_SYS_SUPPORT_LONG_LONG)
            if ((ctx.c1 = *fmt) == 'l') {
                FLAG_SET(LONG_LONG_INT);
                fmt++;
            }
#endif
            ctx.c1 = *fmt++;
        }
#endif
        switch (ctx.c1) {                    /* Type */
            case 'o': ctx.base = 8;
                      print_u2s(&ctx, va);
                      break;
            case 'f': print_float(&ctx, va);
                      break;
            case 'u': ctx.base = 10;
                      print_u2s(&ctx, va);
                      break;
            case 'i':
            case 'd': ctx.base = 10;
                      print_i2s(&ctx, va);
                      break;
            case 'p': FLAG_SET(ALTERNATE_FORM);
            case 'x':
            case 'X': ctx.base = 16;
                      if (ctx.c1 == 'X')
                          FLAG_SET(CAPITAL_HEX);
                      print_u2s(&ctx, va);
                      break;
            case 's': print_str(&ctx, va);
                      break;
            case 'c': ctx.c1 = (unsigned char)va_arg(va, unsigned int);
                      ctx.ret = put(ptr, (const char *)&ctx.c1, 1);
                      break;
            default: if ((ctx.ret = put(ptr, "%", 1)))
                         break;
            case '%': ctx.ret = put(ptr, (const char *)&ctx.c1, 1);
                      break;
        }
    }
    return ctx.ret;
}

int xprint(void *ptr, put_t put, const char *fmt, ...)
{
    int ret;
    va_list va;

    va_start(va, fmt);
    ret = vxprint(ptr, put, fmt, va);
    va_end(va);

    return ret;
}

#endif /* defined(SDB_ENABLE) */

