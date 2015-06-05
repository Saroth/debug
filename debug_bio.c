#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "debug_bio.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 输出接口 */
int dbg_bio_out(char * buf, int len)
{
    printf(buf);
    fflush(stdout);
    return len;
}

/** 输入接口 */
int dbg_bio_in(char * buf, int len)
{
    return 0;
}

static int gdbg_log_fd = -1;            //!< 文件描述符存储

/** 打开日志文件 */
int dbg_bio_open(char * file)
{
    /// run
    if(NULL == file) {
        /// err
        return -1;
    }
    if(gdbg_log_fd != -1) {
        if(close(gdbg_log_fd) && errno != EBADF) {
            /// err
            return -1;
        }
        gdbg_log_fd = -1;
    }
    if((gdbg_log_fd = open(file, O_RDWR | O_APPEND | O_CREAT | O_SYNC,
            S_IRWXU | S_IRWXG | S_IRWXO)) < 0) {
        gdbg_log_fd = -1;
        /// err
        return -1;
    }

    return 0;
}

/** 关闭日志文件 */
int dbg_bio_close(void)
{
    /// run
    if(gdbg_log_fd != -1) {
        if(close(gdbg_log_fd) && errno != EBADF) {
            /// err
            return -1;
        }
        gdbg_log_fd = -1;
    }

    return 0;
}

/** 写日志 */
int dbg_bio_write(char * buf, int len)
{
    /// run
    if(buf == NULL || len < 0) {
        /// err
        return -1;
    }
    if(gdbg_log_fd != -1) {
        if(write(gdbg_log_fd, buf, len) != len) {
            /// err
            return -1;
        }
    }

    return len;
}

#ifdef __cplusplus
}
#endif

