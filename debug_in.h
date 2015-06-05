#ifndef __DEBUG_IN_H__
#define __DEBUG_IN_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief       获取输入的字符串
 * \param       str         字符串缓存
 * \param       len         缓存大小
 * \return      >=0         获取的数据长度
 *              -1          Error
 */
int dbg_in_str(char * str, int len);
/**
 * \brief       获取输入的数值
 * \param       num         整形指针，输出获取到的数值
 * \return      0       Success
 *              -1      Error
 */
int dbg_in_num(int * num);

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_IN_H__ */

