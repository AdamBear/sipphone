#ifndef _IDLE_IDLE_HANDLE_H_
#define _IDLE_IDLE_HANDLE_H_

#include "baseidlehandle.h"

class CIdleHandle : public CBaseIdleHandle
{
public:
    CIdleHandle(int nType = PS_STATE_IDLE);
    virtual ~CIdleHandle();

public:
    // 获取按键信息
    virtual bool GetSoftkey(VEC_SOFTKET_TYPE& m_vecSoftkey);
};

#endif //end of _IDLE_IDLE_HANDLE_H_

