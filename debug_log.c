#include <string.h>

#include "debug.h"

#if ((DBG_USE_LOG == 1) && (DS_DEBUG_MAIN == 1))

static int gdbg_mode = DBG_MODE_P;      //!< 默认调试模式

/** 打开新日志 */
int dbg_log_open(char * filename)
{
    if(filename == NULL) {
        dbg_out_E(DS_LOG_ERR, "param error");
        return -1;
    }
    dbg_log_close();
    if(dbg_bio_open(filename)) {
        dbg_out_E(DS_LOG_ERR, "Open file failed.");
        return -1;
    }

    return 0;
}

/** 关闭日志 */
int dbg_log_close(void)
{
    return dbg_bio_close();
}

/** 写日志 */
int dbg_log_write(char * buf, int len)
{
    return dbg_bio_write(buf, len);
}

/** 设置日志记录模式 */
int dbg_log_setmode(int mode)
{
    if(mode < 0) {
        dbg_out_E(DS_LOG_ERR, "param error.");
        return -1;
    }
    gdbg_mode = mode;
    return 0;
}

/** 获取日志记录模式 */
int dbg_log_getmode(void)
{
    return gdbg_mode;
}

#endif /* ((DBG_USE_LOG == 1) && (DS_DEBUG_MAIN == 1)) */

