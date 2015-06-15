#ifndef __DEBUG_TEST_H__
#define __DEBUG_TEST_H__

#include "debug.h"

#ifdef DS_DEBUG_MAIN
#ifdef __cplusplus
extern "C" {
#endif

/** \brief 测试列表结构体 */
typedef struct {
    char * info;                        //!< 列表项信息
    void * param;                       //!< 测试函数参数
    int (*func)(void *);                //!< 测试函数
}DBG_TESTLIST_T;

/**
 * \brief       测试列表控制
 * \param       list            列表数组
 * \return      0               Success
 */
int dbg_testlist(DBG_TESTLIST_T * list, int size);

#ifdef __cplusplus
}
#endif
#endif /* DS_DEBUG_MAIN */
#endif /* __DEBUG_TEST_H__ */

