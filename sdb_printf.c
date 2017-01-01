#include <stdarg.h>
#include "sdb_config.h"

typedef struct printf_param_t printf_param_t;
typedef int (* print_put_t)(printf_param_t *p, const char *buf, size_t len);

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

typedef struct {
    unsigned char flag;                 /* 标记, flag_t */
    unsigned char base;                 /* 进制 */
    unsigned char width;                /* 输出宽度 */
    unsigned char precision;            /* 小数精度 */
} format_t;

struct printf_param_t {
    print_put_t put;                    /* 输出处理 */
#if defined(SDB_CONF_BUFFER_SUPPORT)
    char *buf;                          /* 输出缓存 */
    size_t size;                        /* 缓存大小 */
    size_t len;                         /* 数据长度 */
#endif
    sdb_config_t *cfg;                  /* 配置结构体 */
    size_t flag;                        /* 输出标记, sdb_flag_t */
    format_t *fmt;                      /* 输出格式 */
};

#define PUTS(__buf, __len) do {\
    if ((ret = p->put(p, __buf, __len))) return ret;\
} while (0)
#define PUTC(__c) do {\
    ch = (unsigned char)(__c);\
    if ((ret = p->put(p, &ch, 1))) return ret;\
} while (0)

#define FLAG_SET(__flag) p->fmt->flag |= (__flag)
#define FLAG_IS_SET(__flag) (p->fmt->flag & (__flag))
#define FLAG_UNSET(__flag) (p->fmt->flag &= ~(__flag))

#if defined(SDB_CONF_BUFFER_SUPPORT)
static int put_to_cache(printf_param_t *p, const char *buf, size_t len)
{
    return 0;
}
#endif

int put_direct(printf_param_t *p, const char *buf, size_t len)
{
    if (len == 0)
        while (buf[len++]);
    return p->cfg->put(p->cfg->ptr, p->flag, buf, len);
}

static unsigned int s2u(const char **fmt, int base)
{
    unsigned int n = 0;
    unsigned char c = 0;
    int d;
    while (1) {
        c = *(*fmt)++;
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
    (*fmt)--;
    return n;
}

#define _U2S(__name, __type) static int __name(printf_param_t *p, __type num) {\
    int ret;\
    unsigned int m;\
    unsigned char ch, s = '+', n = 1;\
    __type d = 1;\
    while (num / d >= p->fmt->base) { d *= p->fmt->base; n++; }\
    if (FLAG_IS_SET(ALTERNATE_FORM)) {\
        unsigned char f = 0;\
        if (p->fmt->base == 16) {\
            n += 2;\
            PUTC('0');\
            if (FLAG_IS_SET(CAPITAL_HEX)) PUTC('X');\
            else PUTC('x');\
        }\
        else if (p->fmt->base == 8) { n++; PUTC('0'); }\
    }\
    if (FLAG_IS_SET(SIGN)) {\
        n++;\
        if (FLAG_IS_SET(NEGATIVE_SIGN)) s = '-';\
        if (FLAG_IS_SET(PAD_ZERO)) { PUTC(s); s = 0; }\
    }\
    p->fmt->width = (p->fmt->width > n) ? (p->fmt->width - n) : 0;\
    if (p->fmt->width && !FLAG_IS_SET(ALIGN_LEFT))\
        while (p->fmt->width-- > 0) PUTC(FLAG_IS_SET(PAD_ZERO) ? '0' : ' ');\
    if (FLAG_IS_SET(SIGN)) PUTC(s);\
    while (d > 0) {\
        m = num / d;\
        num %= d;\
        d /= p->fmt->base;\
        PUTC(m + (m < 10 ? '0'\
                    : ((FLAG_IS_SET(CAPITAL_HEX) ? 'A' : 'a') - 10)));\
    }\
    if (p->fmt->width && FLAG_IS_SET(ALIGN_LEFT))\
        while (p->fmt->width-- > 0) PUTC(' ');\
    return 0;\
}
_U2S(ulli2s, unsigned long long int);
_U2S(uli2s, unsigned long int);
_U2S(ui2s, unsigned int);

static int ui2s_print(printf_param_t *p, va_list va)
{
    FLAG_UNSET(SIGN);
#if defined(SDB_SYS_LONG_SUPPORT)
    if (FLAG_IS_SET(LONG_INT))
        return uli2s(p, va_arg(va, unsigned long int));
    else
#endif
#if defined(SDB_SYS_LONG_LONG_SUPPORT)
    if (FLAG_IS_SET(LONG_LONG_INT))
        return ulli2s(p, va_arg(va, unsigned long long int));
    else
#endif
        return ui2s(p, va_arg(va, unsigned int));
}

#define _I2S(__name, __type) static int __name(printf_param_t *p, __type num) {\
    if (num < 0) {\
        FLAG_SET(SIGN | NEGATIVE_SIGN);\
        num = -num;\
    }\
    u##__name(p, num);\
}
_I2S(lli2s, long long int);
_I2S(li2s, long int);
_I2S(i2s, int);

static int i2s_print(printf_param_t *p, va_list va)
{
#if defined(SDB_SYS_LONG_SUPPORT)
    if (FLAG_IS_SET(LONG_INT))
        return li2s(p, va_arg(va, long int));
    else
#endif
#if defined(SDB_SYS_LONG_LONG_SUPPORT)
    if (FLAG_IS_SET(LONG_LONG_INT))
        return lli2s(p, va_arg(va, long long int));
    else
#endif
        return i2s(p, va_arg(va, unsigned int));
}

static int s_print(printf_param_t *p, va_list va)
{
    int ret;
    unsigned char ch;
    unsigned char *s = va_arg(va, unsigned char *);

    if (p->fmt->width) {
        unsigned int l = 0;
        while (s[l++]);
        p->fmt->width -= l - 1;
        if (!FLAG_IS_SET(ALIGN_LEFT))
            while (p->fmt->width--)
                PUTC(' ');
    }
    PUTS(s, 0);
    if (p->fmt->width && FLAG_IS_SET(ALIGN_LEFT))
        while (p->fmt->width--)
            PUTC(' ');

    return 0;
}

static int f_printf(printf_param_t *p, va_list va)
{
    va_arg(va, double);
    return 0;
}

int sdb_vfnprintf(printf_param_t *p, const char *fmt, va_list va)
{
    int ret;
    unsigned char ch;
    unsigned char c;
    format_t f;
    p->fmt = &f;

    while ((c = *fmt++)) {
        if (c != '%') {
            PUTC(c);
            continue;
        }
        f.flag = 0;
        f.width = 0;
        f.precision = 0;
        while ((c = *fmt++)) {          /* Flags */
            switch (c) {
                case '-': FLAG_SET(ALIGN_LEFT); continue;
                case '#': FLAG_SET(ALTERNATE_FORM); continue;
                case '+': FLAG_SET(SIGN); continue;
                case '0': FLAG_SET(PAD_ZERO); continue;
                case ' ': continue;
            }
            break;
        }
        if (c == '*') {                 /* Width */
            f.width = (unsigned char)va_arg(va, unsigned int);
            c = *fmt++;
        }
        else if (c >= '0' && c <= '9') {
            fmt--;
            f.width = (unsigned char)s2u(&fmt, 10);
            c = *fmt++;
        }
        if (c == '.') {                 /* Float */
            FLAG_SET(PAD_ZERO);
            c = *fmt++;
            if (c == '*')
                f.precision = (unsigned char)va_arg(va, unsigned int);
            else {
                fmt--;
                f.precision = (unsigned char)s2u(&fmt, 10);
            }
            c = *fmt++;
        }
#if defined(SDB_SYS_LONG_SUPPORT)
        if (c == 'z') {                 /* Long */
#if defined(SDB_SYS_SIZE_T_SUPPORT)
            if (sizeof(size_t) == sizeof(unsigned long int))
                FLAG_SET(LONG_INT);
#if defined(SDB_SYS_LONG_LONG_SUPPORT)
            else if (sizeof(size_t) == sizeof(unsigned long long int))
                FLAG_SET(LONG_LONG_INT);
#endif
#endif
            c = *fmt++;
        }
        else if (c == 'l') {
            FLAG_SET(LONG_INT);
#if defined(SDB_SYS_LONG_LONG_SUPPORT)
            if ((c = *fmt) == 'l') {
                FLAG_SET(LONG_LONG_INT);
                fmt++;
            }
#endif
            c = *fmt++;
        }
#endif
        switch (c) {                    /* Type */
            case 'o':
                f.base = 8;
                if ((ret = ui2s_print(p, va)))
                    return ret;
                break;
            case 'f': if ((ret = f_printf(p, va))) return ret; break;
            case 'u':
                f.base = 10;
                if ((ret = ui2s_print(p, va)))
                    return ret;
                break;
            case 'i':
            case 'd':
                f.base = 10;
                if ((ret = i2s_print(p, va)))
                    return ret;
                break;
            case 'p': FLAG_SET(ALTERNATE_FORM);
            case 'x':
            case 'X':
                f.base = 16;
                if (c == 'X')
                    FLAG_SET(CAPITAL_HEX);
                if ((ret = ui2s_print(p, va)))
                    return ret;
                break;
            case 's': if ((ret = s_print(p, va))) return ret; break;
            case 'c': PUTC(va_arg(va, unsigned int)); break;
            default: PUTC('%');
            case '%': PUTC(c); break;
        }
    }
    return 0;
}

