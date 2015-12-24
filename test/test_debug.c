#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "libdebug.h"

#if (DBG_MODULE_TEST == 1)

#define DS_DEBUG_MODULE (DBG_INFO | DBG_LABEL_FUNC | DBG_LABEL_LINE)

int test_output(void *p)
{
    /// 输出测试
    int i = 0;

    for(i = 0; i < 32; i++) {
        dbg_out_entry(i);
        dbg_out_I(i, "flag = %p", i);
        dbg_out(i, "\n格式化输出测试: %d = %#x = %c\t%s\n",
                65, 65, 65, "This is a TEST.");
        dbg_out_I(i, "带提示标签的格式化输出，标签为提示高亮");
        dbg_out_W(i, "带警告标签的格式化输出，标签为警告高亮");
        dbg_out_E(i, "带错误标签的格式化输出，标签为错误高亮");
        dbg_out_H(i, "反白高亮测试");
        dbg_outerr_I(i, "带错误标签的格式化输出，标签为错误高亮，附带错误信息");
        dbg_out_exit(i);
    }

    return 0;
}

int test_dump(void *p)
{
    char buf[1024];

    strcpy(buf, "#### This is a TEST.\r\n ####");
    dbg_out_I(DS_DEBUG_MODULE, "buf address: %p", buf);
    dbg_dmp_H(DS_DEBUG_MODULE, buf, strlen(buf));
    dbg_dmp_HC(DS_DEBUG_MODULE, buf, strlen(buf));
    dbg_dmp_HCA(DS_DEBUG_MODULE, buf, strlen(buf), buf);
    dbg_dmp_HL(DS_DEBUG_MODULE, buf, strlen(buf), "test1");
    dbg_dmp_HCL(DS_DEBUG_MODULE, buf, strlen(buf), "test2");
    dbg_dmp_HCAL(DS_DEBUG_MODULE, buf, strlen(buf), buf, "test3");
    dbg_dmp_HL(DS_DEBUG_MODULE, buf, strlen(buf), NULL);
    dbg_dmp_HCL(DS_DEBUG_MODULE, buf, strlen(buf), NULL);
    dbg_dmp_HCAL(DS_DEBUG_MODULE, buf, strlen(buf), buf, NULL);
    dbg_dmp_C(DS_DEBUG_MODULE, buf, strlen(buf));

    return 0;
}

int test_getchar(void * p)
{
    char ch = 0;
    int breakflag = 0;

    while(1) {
        ch = getchar();
        printf("Get character: %d - %x - %c\n", ch, ch, ch);
        if(ch == 'q') {
            if(breakflag) {
                getchar();
                break;
            }
            breakflag = 1;
        }
    }

    return 0;
}

int test_dbg_in_S(void *p)
{
    char buf[256];
    memset(buf, 0x00, sizeof(buf));
    dbg_in_S(buf, sizeof(buf));
    dbg_out_I(DS_DEBUG_MODULE, "Get string: %s", buf);
    return 0;
}

int test_dbg_in_N(void *p)
{
    int num = 0;
    dbg_in_N(&num);
    dbg_out_I(DS_DEBUG_MODULE, "Get number: %d", num);
    return num;
}

int test_setlogname(void *p)
{
    dbg_log_setname("log_test.txt");
    return 0;
}

int test_setlog_off(void *p)
{
    dbg_log_off();
    return 0;
}

int test_setlog_on(void *p)
{
    dbg_log_on();
    return 0;
}

int test_setlog_on_s(void *p)
{
    dbg_log_on_s();
    return 0;
}

int test_setlog_only(void *p)
{
    dbg_log_only();
    return 0;
}

int test_setlog_only_s(void *p)
{
    dbg_log_only_s();
    return 0;
}

#include <errno.h>
static int test_std_out(char * buf, int len)
{
    printf("<so>");
    printf(buf);
    fflush(stdout);
    return 0;
}
static int test_std_in(char * buf, int len)
{
    printf("<si>");
    int ch = 0;
    char * b = buf;
    while(1) {
        ch = getchar();
        if(ch == EOF) {
            break;
        }
        else {
            *b = ch;
            if(*b == '\n' || (++b - buf) >= len) {
                break;
            }
        }
    }
    return 0;
}
static void * s_log_fp = NULL;                 //!< 日志文件指针
static int test_f_open(char * filename)
{
    printf("<fo>");
    s_log_fp = fopen(filename, "ab+");
    if(NULL == s_log_fp) {
        return -1;
    }
    return 0;
}
static int test_f_close(void)
{
    printf("<fc>");
    if(NULL == s_log_fp) {
        return 0;
    }
    if(fclose(s_log_fp) && errno != EBADF) {
        return -1;
    }
    s_log_fp = NULL;
    return 0;
}
static int test_f_write(char * buf, int len)
{
    printf("<fw>");
    if(NULL == s_log_fp) {
        return 0;
    }
    if(fwrite(buf, 1, len, s_log_fp) != len) {
        return -1;
    }
    return 0;
}
static int test_f_sync(void)
{
    if(NULL == s_log_fp) {
        return 0;
    }
    printf("<fs>");
    fflush(s_log_fp);                   //!< 将数据从C库缓存写到内核缓存
#if defined(__unix__) || defined(__linux__)
#include <unistd.h>
    fsync(fileno(s_log_fp));            //!< 将数据从内核缓存写到磁盘
#endif
    return 0;
}
static DBG_BIO_T g_dbg_bio_test = {
    .f_output   = test_std_out,
    .f_input    = test_std_in,
    .f_open     = test_f_open,
    .f_close    = test_f_close,
    .f_write    = test_f_write,
    .f_sync     = test_f_sync,
};
int test_set_custom_func(void *p)
{
    dbg_bio_conf(&g_dbg_bio_test);
    return 0;
}

int test_set_default_func(void *p)
{
    dbg_bio_conf(NULL);
    return 0;
}

// 模块测试
int main(int argc, char * argv[])
{
    dbg_test_setlist(
        { "dbg_out_*",          NULL,   test_output,            },
        { "dbg_dmp_*",          NULL,   test_dump,              },
        { "test_getchar",       NULL,   test_getchar,           },
#if (DBG_INPUT_EN == 1)
        { "dbg_in_S",           NULL,   test_dbg_in_S,          },
        { "dbg_in_N",           NULL,   test_dbg_in_N,          },
#else
        { "---- DBG_INPUT_EN disabled", NULL,   NULL,           },
#endif /* (DBG_INPUT_EN == 1) */
#if (DBG_LOG_EN == 1)
        { "dbg_log_setname",    NULL,   test_setlogname,        },
        { "dbg_log_off",        NULL,   test_setlog_off,        },
        { "dbg_log_on",         NULL,   test_setlog_on,         },
        { "dbg_log_on_s",       NULL,   test_setlog_on_s,       },
        { "dbg_log_only",       NULL,   test_setlog_only,       },
        { "dbg_log_only_s",     NULL,   test_setlog_only_s,     },
#else
        { "---- DBG_LOG_EN disabled",   NULL,   NULL,           },
#endif /* (DBG_LOG_EN == 1) */
        { "set custom func",    NULL,   test_set_custom_func,   },
        { "set default func",   NULL,   test_set_default_func,  },
    );

    return 0;
}

#else
int main(int argc, char * argv[]) { return 0; }
#endif /* (DBG_MODULE_TEST == 1) */

