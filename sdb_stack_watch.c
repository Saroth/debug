#include "sdb_internal.h"

/* ! 不支持多线程 */
#if defined(SDB_MODULE_STACK_WATCH)
static unsigned long *stack_mark = 0;
static unsigned long stack_max_usage = 0;

unsigned long sdb_stack_mark(void)
{
    unsigned long i;
    stack_mark = &i;
    stack_max_usage = 0;
    return (unsigned long)stack_mark;
}

unsigned long sdb_stack_touch(void)
{
    unsigned long p;
    p = (unsigned long)(stack_mark - &p);
    if (p > stack_max_usage) {
        stack_max_usage = p;
    }
    return p;
}

unsigned long sdb_stack_max_usage(void)
{
    return stack_max_usage;
}

#endif /* defined(SDB_MODULE_STACK_WATCH) */

