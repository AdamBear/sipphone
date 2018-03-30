#ifndef __BLACKLIGHT_LISTENER_H__
#define __BLACKLIGHT_LISTENER_H__

#include "basetalklistener.h"

/* 背光灯业务 */

class CBackLightListener : public CBaseTalkListener
{
public:
    CBackLightListener();

    DECL_SIGNAL_HANDLER(FlushLight);

    DECL_SIGNAL_HANDLER(RegisterHandle);

    DECL_SIGNAL_HANDLER(UnRegisterHandle);
};

#endif
