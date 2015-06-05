#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug_bio.h"
#include "debug_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DBG_USE_LOG

static char * gdbg_log_filename = 0;    //!< 日志文件名字符串
static int gdbg_mode = DBG_P;           //!< 调试模式，默认仅输出

/** 打开新日志 */
int dbg_log_open(char * filename)
{
    /// run
    if(NULL == filename) {
        /// err
        return -1;
    }
    if(dbg_bio_open(filename)) {
        /// err
        return -1;
    }
    gdbg_log_filename = (char *)malloc(strlen(filename) + 1);
    if(gdbg_log_filename == NULL) {
        /// err
        return -1;
    }
    memset(gdbg_log_filename, 0x00, (strlen(filename) + 1));
    strcpy(gdbg_log_filename, filename);

    return 0;
}

/** 关闭日志 */
int dbg_log_close(void)
{
    /// run
    if(gdbg_log_filename) {
        free(gdbg_log_filename);
        gdbg_log_filename = NULL;
    }
    return dbg_bio_close();
}

/** 写日志 */
int dbg_log_write(char * buf, int len)
{
    /// run
    return dbg_bio_write(buf, len);
}

/** 设置日志记录模式 */
int dbg_log_setmode(int mode)
{
    /// run
    if(mode < DBG_P || mode > DBG_PW) {
        /// err
        return -1;
    }
    gdbg_mode = mode;
    return 0;
}

/** 获取日志记录模式 */
int dbg_log_getmode(void)
{
    /// run
    return gdbg_mode;
}

#endif /* DBG_USE_LOG */

#ifdef __cplusplus
}
#endif

