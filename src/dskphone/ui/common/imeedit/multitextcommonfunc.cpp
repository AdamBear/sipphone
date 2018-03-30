#include "multitextcommonfunc.h"
#include <new>

namespace
{
#define INT_MAX       2147483647    /* maximum (signed) int value */
#define X_THROW(A) throw A
}

int xAllocMore(int alloc, int extra)
{
    if (alloc == 0 && extra == 0)
    {
        return 0;
    }
    const int page = 1 << 12;
    int nalloc;
    alloc += extra;
    if (alloc < 1 << 6)
    {
        nalloc = (1 << 3) + ((alloc >> 3) << 3);
    }
    else
    {
        // don't do anything if the loop will overflow signed int.
        if (alloc >= INT_MAX / 2)
        {
            return INT_MAX;
        }
        nalloc = (alloc < page) ? 1 << 3 : page;
        while (nalloc < alloc)
        {
            if (nalloc <= 0)
            {
                return INT_MAX;
            }
            nalloc *= 2;
        }
    }
    return nalloc - extra;
}

/* \internal
   Allows you to throw an exception without including <new>
   Called internally from Q_CHECK_PTR on certain OS combinations
*/
void xBadAlloc()
{
    //X_THROW(std::bad_alloc());
}

