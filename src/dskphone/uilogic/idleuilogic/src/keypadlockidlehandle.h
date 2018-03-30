#ifndef _KEYPAD_LOCK_IDLE_HANDLE_H_
#define _KEYPAD_LOCK_IDLE_HANDLE_H_

#include "baseidlehandle.h"

class CKeypadLockHandle : public CBaseIdleHandle
{
public:
    CKeypadLockHandle(int nType = PS_STATE_LOCK);
    virtual ~CKeypadLockHandle();
    static LRESULT OnKeypadMessage(msgObject & objMessage);

public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
};

#endif //end of _KEYPAD_LOCK_IDLE_HANDLE_H_

