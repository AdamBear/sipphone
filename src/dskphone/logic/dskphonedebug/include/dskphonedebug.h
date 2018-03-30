/**********************************************************************************************//**
 * @file    \src\dskphone\logic\dskphonedebug\include\dskphonedebug.h
 *
 * @brief   基于mkit机制实现，此模块只封装c接口
 **************************************************************************************************/

#ifndef __DSKPHONE_DEBUG_H__
#define __DSKPHONE_DEBUG_H__
#include "msgkit/mkmessage.h"

#ifdef IF_FEATURE_TOOLS

#ifdef __cplusplus
extern "C"
{
#endif
/************************************************************************/
typedef struct
{
    mkit_handle_t  handle;

} dskphone_debug_t;


/************************************************************************/


/************************************************************************/
/*                          public function                             */
/************************************************************************/
int dskphone_debug_run(void);

#ifdef __cplusplus
}
#endif

#endif // #ifdef IF_FEATURE_TOOLS
#endif  // __DSKPHONE_DEBUG_H__