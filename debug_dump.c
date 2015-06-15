#include <unistd.h>

#include "debug_dump.h"

#if defined(DS_DEBUG_MAIN) && defined(DBG_USE_DUMP)
#ifdef __cplusplus
extern "C" {
#endif

/** 每行数据输出控制 */
int dbg_dump_line(char * buf, unsigned int len, long addr, int mode)
{
    int i = 0;
    int chpline = 0;                    //!< 每行输出的字符数
    int outputchar = 0;                 //!< 各模式下的输出计数
    int headspace = 0;                  //!< 对齐后数据首行的行首留空
    unsigned int addr_int = addr;       //!< 对齐地址
    unsigned int addr_cur = addr_int;   //!< 偏移地址

    // 判断分割显示格式
    if(mode & 0xf) {
        chpline = 8 * (mode & 0xf);
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

#ifdef DBG_USE_COLOR
    dbg_color_set(DBG_COLOR_WARN);
#endif /* DBG_USE_COLOR */
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
        dbg_out(1, "%#010x", addr_int);
    }
#ifdef DBG_USE_COLOR
    dbg_color_set(DBG_COLOR_RES);
#endif /* DBG_USE_COLOR */

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
                    dbg_out(1, ".");
                }
                else if(buf[addr_cur - addr] == 10
                        || buf[addr_cur - addr] == 13) {
                    dbg_out(1, "~");
                }
                /*
                else if(buf[addr_cur - addr] < 32
                        || buf[addr_cur - addr] >= 128) {
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
    dbg_out(1, "\r\n");

    return outputchar;
}

/** 数据格式化导出 */
int dbg_dump(char * buf, unsigned int len, unsigned long addr, int mode)
{
    int ret = 0;

    if(buf == NULL) {
        dbg_out_E(DS_DUMP_ERR, "param error.");
        return -1;
    }
    while(len > 0) {
        ret = dbg_dump_line(buf, len, addr, mode);
        len -= ret;
        addr += ret;
        buf += ret;
    }

    return 0;
}

/** 带标签的数据格式化导出 */
int dbg_dump_label(const char * func, int line, char * buf, unsigned int len,
        unsigned long addr, int mode)
{
    int ret = 0;
    dbg_stdout_label(func, line,
            DBG_LABEL_TEXTCOLOR | DBG_LABEL_COL_HL | DBG_LABEL_INFO
            | DBG_LABEL_NEWLINE, "________ DATA DUMP ________");
    ret = dbg_dump(buf, len, addr, mode);
    dbg_stdout_label(func, line,
            DBG_LABEL_COLOR | DBG_LABEL_COL_HL | DBG_LABEL_INFO
            | DBG_LABEL_NEWLINE, "#### buffer size: %d", len);

    return ret;
}

#ifdef __cplusplus
}
#endif
#endif /* defined(DS_DEBUG_MAIN) && defined(DBG_USE_DUMP) */

