#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "debug.h"
#include "debug_bio.h"
#include "debug_out.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DBG_USE_LOG
#include "debug_log.h"

/** 输出控制(如果输出成功写文件失败，将返回成功) */
static int dbg_ioctl(char * buf, int len)
{
    int mode = dbg_log_getmode();
    int ret = 0;
    if(mode & DBG_W) {
        ret = dbg_log_write(buf, len);        //!< 写日志，不检查结果
    }
    if(mode & DBG_P) {
        ret = dbg_bio_out(buf, len);
    }
    return ret;
}
#else
/** 输出控制 */
static int dbg_ioctl(char * buf, int len)
{
    return dbg_bio_out(buf, len);
}
#endif /* DBG_USE_LOG */

/** 格式化输出 */
int dbg_stdout(const char * fmt, ...)
{
    int count = 0;
    va_list marker;
    char buf[BUFFER_SIZE + 4] = { 0 };

    if(NULL == fmt) {
        return -1;
    }
    va_start(marker, fmt);
    count = vsnprintf(buf, BUFFER_SIZE, fmt, marker);
    va_end(marker);
    if(count > BUFFER_SIZE) {
        count = BUFFER_SIZE;
    }
    else if(count < 0) {
        return count;
    }
    return dbg_ioctl(buf, count);
}

/** 标准错误信息输出 */
int dbg_stderr(void)
{
    char buf[BUFFER_SIZE + 4] = { 0 };

    int ret = sprintf(buf, "%s(%d)", strerror(errno), errno);
    if(ret <= 0) {
        return ret;
    }
    return dbg_ioctl(buf, ret);
}

/** 设置终端输出颜色 */
int dbg_color_set(char * color)
{
#ifdef DBG_USE_COLOR
    if(color == NULL) {
        /// Err
        return -1;
    }
    dbg_bio_out(color, strlen(color));
#endif /* DBG_USE_COLOR */
    return 0;
}

#ifdef __cplusplus
}
#endif

