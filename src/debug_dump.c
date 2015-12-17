#include <stdio.h>

#include "debug.h"

#if ((DBG_DUMP_EN == 1) && (DS_DEBUG_MAIN == 1))

/** 每行数据输出控制 */
int dbg_dump_line(char * buf, unsigned int len, long addr, int mode)
{
    int i = 0;
    int chpline = 0;                    //!< 每行输出的字符数
    int outputchar = 0;                 //!< 各模式下的输出计数
    int headspace = 0;                  //!< 对齐后数据首行的行首留空
    long addr_int = addr;               //!< 对齐地址
    long addr_cur = addr_int;           //!< 偏移地址

    // 判断分割显示格式
    if(mode >> DBG_DMP_SEG_OFS) {
        chpline = 8 * ((mode >> DBG_DMP_SEG_OFS) & 0xf);
    }
    else {
        // 纯数据导出
        outputchar = 0;
        for(i = 0; i < len; i++) {
            dbg_out(1, "%c", buf[i]);
            outputchar++;
        }

        return outputchar;
    }

#if(DBG_COLOR_EN == 1)
    dbg_color_set(DBG_COLOR_WARN);
#endif /* (DBG_COLOR_EN == 1) */
    // 显示行号
    if((mode & DBG_DMP_TAG_LINE) && chpline) {
        dbg_out(1, "|");
        dbg_out(1, "%03d", addr_int / chpline);
    }
    // 显示地址，16进制下首行自动对齐到行字符个数
    if((mode & DBG_DMP_TAG_ADDR) && chpline) {
        dbg_out(1, "|");
        if(addr % chpline) {
            headspace = addr % chpline;
            addr_int = addr - headspace;
        }
        if(sizeof(void *) == 8) {
            dbg_out(1, "%018llp", addr_int);
        }
        else if(sizeof(void *) == 4) {
            dbg_out(1, "%010llp", addr_int);
        }
        else if(sizeof(void *) == 2) {
            dbg_out(1, "%06llp", addr_int);
        }
    }
#if(DBG_COLOR_EN == 1)
    dbg_color_set(DBG_COLOR_RES);
#endif /* (DBG_COLOR_EN == 1) */

    // 数据数值16进制格式显示
    if((mode & DBG_DMP_DAT_HEX) && chpline) {
        outputchar = 0;
        addr_cur = addr_int;
        dbg_out(1, "|");
        for(i = 0; i < chpline; i++, addr_cur++) {
            if(addr_cur < addr || i >= len + headspace) {
                dbg_out(1, "   ");
            }
            else {
                dbg_out(1, " %02x", buf[addr_cur - addr] & 0xff);
                outputchar++;
            }
        }
    }
    // 数据数值10进制格式显示
    if((mode & DBG_DMP_DAT_DEC) && chpline) {
        outputchar = 0;
        addr_cur = addr_int;
        dbg_out(1, "|");
        for(i = 0; i < chpline; i++, addr_cur++) {
            if(addr_cur < addr || i >= len + headspace) {
                dbg_out(1, "   ");
            }
            else {
                dbg_out(1, " %03d", buf[addr_cur - addr]);
                outputchar++;
            }
        }
    }
    // 数据字符格式输出
    if((mode & DBG_DMP_DAT_CHAR) && chpline) {
        outputchar = 0;
        addr_cur = addr_int;
        dbg_out(1, "| ");
        for(i = 0; i < chpline; i++, addr_cur++) {
            if(addr_cur < addr || i >= len + headspace) {
                dbg_out(1, " ");
            }
            else {
                if(buf[addr_cur - addr] == 0) {
                    /// 空字符显示为点
                    dbg_out(1, ".");
                }
                else if((buf[addr_cur - addr] >= 10
                        && buf[addr_cur - addr] <= 14)
                        ) {
                    /// 不显示换行符(10, 13)、换页符(12)或垂直制表符(11)
                    /// 不显示'\xe'(14)，在SSH远程终端会引起乱码
                    dbg_out(1, "~");
                }
                /*
                 * 禁用以显示双字节字符
                else if(buf[addr_cur - addr] < 32
                        || buf[addr_cur - addr] >= 128) {
                    /// 不显示非ASCII字符
                    dbg_out(1, "?");
                }
                */
                else {
                    dbg_out(1, "%c", buf[addr_cur - addr]);
                }
                outputchar++;
            }
        }
    }

    return outputchar;
}

/** 数据格式化导出 */
int dbg_dump(char * buf, unsigned int len, void * addr, int mode)
{
    int ret = 0;

    if(buf == NULL) {
        dbg_out_E(DS_DUMP_ERR, "param error.");
        return -1;
    }
    while(len > 0) {
#if (DBG_NL_HEAD == 1)
        dbg_out(1, "%s", DBG_NL_CHAR);
#endif /* (DBG_NL_HEAD == 1) */
        ret = dbg_dump_line(buf, len, (long)addr, mode);
#if !(DBG_NL_HEAD == 1)
        dbg_out(1, "%s", DBG_NL_CHAR);
#endif /* !(DBG_NL_HEAD == 1) */
        len -= ret;
        addr += ret;
        buf += ret;
    }

    return 0;
}

/** 带标签的数据格式化导出 */
int dbg_dump_label(const char * file, const char * func, int line,
        char * buf, unsigned int len, void * addr, int mode, char * label)
{
    int ret = 0;
    if(mode & DBG_NO_DUMP) {
        return 0;
    }
    if(mode & DBG_DMP_TAG_LABEL) {
#if (DBG_NL_HEAD == 1)
        dbg_out(1, "%s", DBG_NL_CHAR);
#endif /* (DBG_NL_HEAD == 1) */
        char label_str[256] = { "________ DATA DUMP ________" };

        if(label) {
            sprintf(label_str, "________ %s ________", label);
        }
        dbg_stdout_label(file, func, line, (mode & 0xff) // 只取通用配置位
                 | DBG_LABEL_COL_HL | DBG_LABEL_INFO, "%s", label_str);
        dbg_stdout_label(file, func, line, DBG_LABEL_COL_HL | DBG_LABEL_INFO,
                "(%d byte)", len);
#if !(DBG_NL_HEAD == 1)
        dbg_out(1, "%s", DBG_NL_CHAR);
#endif /* !(DBG_NL_HEAD == 1) */
    }
    ret = dbg_dump(buf, len, addr, mode);

    return ret;
}

#endif /* ((DBG_DUMP_EN == 1) && (DS_DEBUG_MAIN == 1)) */

