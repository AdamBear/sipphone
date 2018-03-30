#ifndef __MULTI_TEXT_COMMON_FUNC_H__
#define __MULTI_TEXT_COMMON_FUNC_H__

#include "ylstring.h"

typedef yl::wstring::value_type   IMECHAR;


int xAllocMore(int alloc, int extra);

/* \internal
   Allows you to throw an exception without including <new>
   Called internally from Q_CHECK_PTR on certain OS combinations
*/
void xBadAlloc();

#endif

