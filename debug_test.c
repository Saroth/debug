#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 测试列表控制 */
int dbg_testlist(DBG_TESTLIST_T * list, int size)
{
    int i = 0;
    int ret = 0;

    while(1) {
        dbg_out_I(1, "################ (%d)", size);
        for(i = 0; i < size; i++) {
            dbg_out_I(1, "# %5d\t%s", i + 1, (list + i)->info);
        }
        dbg_out_I(1, "#     0\tReturn");
        ret = dbg_in();
        if(ret == 0) {
            break;
        }
        else if(ret > 0 && ret <= size) {
            if((list + ret - 1)->func) {
                (list + ret - 1)->func((list + ret - 1)->param);
            }
        }
    }
    dbg_out(1, "%s", DBG_NL_CHAR);

    return 0;
}

#ifdef __cplusplus
}
#endif

