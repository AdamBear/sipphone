#ifndef _MUTE_IDLE_HANDLE_H_
#define _MUTE_IDLE_HANDLE_H_

#include "baseidlehandle.h"

class CMuteHandle : public CBaseIdleHandle
{
public:
    CMuteHandle(int nType = PS_STATE_RING_MUTE);
    virtual ~CMuteHandle();
public:
    static LRESULT OnReceiveMsg(msgObject & objMessage);
};

#endif //end of _MUTE_IDLE_HANDLE_H_

