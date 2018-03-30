#ifndef _DND_IDLE_HANDLE_H_
#define _DND_IDLE_HANDLE_H_

#include "baseidlehandle.h"

class CDNDHandle : public CBaseIdleHandle
{
public:
    CDNDHandle(int nType = PS_STATE_DND);
    virtual ~CDNDHandle();
    static LRESULT OnDNDMessage(msgObject& msg);
};

#endif //end of _DND_IDLE_HANDLE_H_

