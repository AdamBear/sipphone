#ifndef __XKERNEL_COMMON_H__
#define __XKERNEL_COMMON_H__

#include "xkerneldefine.h"
#include <ETLLib/ETLLib.hpp>


#define MULTI_KERNEL_INSTANCE(className) \
    className*& pInstance = getTlsValue<className*>(NULL); \
    if (NULL == pInstance) \
    { \
        pInstance = new className; \
    } \
    return *pInstance;

#define SINGLE_KERNEL_INSTANCE(className) \
    static className* s_pInstance = NULL; \
    if (NULL == s_pInstance) \
    { \
        s_pInstance = new className; \
    } \
    return *s_pInstance;

#define DEAD_LOOP_BREAK(nLoop) ++nLoop;\
    if (nLoop > 1000000) {XWINDOW_ERR("Dead loop break at %s %d", __FILE__, __LINE__); break;}

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) if (NULL != p) { delete p; p = NULL; }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) if (NULL != p) { delete [] p; p = NULL; }
#endif

#endif
