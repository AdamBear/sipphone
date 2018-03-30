#ifndef _PCIKUPMENU_IDLE_HANDLE_H_
#define _PCIKUPMENU_IDLE_HANDLE_H_

#include "baseidlehandle.h"

class CPickupMenuHandle : public CBaseIdleHandle
{
public:
    CPickupMenuHandle(int nType = PS_STATE_PICKUP_MENU);
    virtual ~CPickupMenuHandle();

    static void OnPickupMenuChange(STATUS_ACTION eAction);
public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
};

#endif //end of _PCIKUPMENU_IDLE_HANDLE_H_

