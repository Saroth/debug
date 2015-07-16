#ifndef __DEBUG_DUMP_H__
#define __DEBUG_DUMP_H__

#ifdef __cplusplus
extern "C" {
#endif

#define DBG_DMP_SEG_NO      ( 0 )       //!< 无格式纯数据导出
#define DBG_DMP_SEG_8       ( 1 << 0 )  //!< 8字节分割
#define DBG_DMP_SEG_16      ( 1 << 1 )  //!< 16字节分割
#define DBG_DMP_SEG_32      ( 1 << 2 )  //!< 32字节分割
#define DBG_DMP_SEG_64      ( 1 << 3 )  //!< 64字节分割
#define DBG_DMP_TAG_LINE    ( 1 << 4 )  //!< 显示行号
#define DBG_DMP_TAG_ADDR    ( 1 << 5 )  //!< 显示地址
#define DBG_DMP_DAT_NO      ( 0 << 6 )  //!< 不显示数据
#define DBG_DMP_DAT_HEX     ( 1 << 6 )  //!< 16进制显示
#define DBG_DMP_DAT_DEC     ( 1 << 7 )  //!< 10进制显示
#define DBG_DMP_DAT_CHAR    ( 1 << 8 )  //!< 字符显示

/**
 * \brief       数据格式化导出
 * \param       buf         数据缓存
 * \param       len         数据长度
 * \param       addr        mode == DBG_DMP_ADDR:   数据地址
 *                          mode == DBG_DMP_LINE:   行号
 * \param       mode        参考DBG_DMP_*定义
 * \return      0           Success
 *              <0          Error
 */
int dbg_dump(char * buf, unsigned int len, void * addr, int mode);

/**
 * \brief       带标签的数据格式化导出
 * \param       func        __func__
 * \param       line        __LINE__
 * \param       buf         数据缓存
 * \param       len         数据长度
 * \param       addr        mode == DBG_DMP_ADDR:   数据地址
 *                          mode == DBG_DMP_LINE:   行号
 * \param       mode        参考DBG_DMP_*定义
 * \return      0           Success
 *              <0          Error
 */
int dbg_dump_label(const char * func, int line, char * buf, unsigned int len,
        void * addr, int mode);

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_DUMP_H__ */

