#ifndef _BLUETOOTH_IDLE_HANDLE_H_
#define _BLUETOOTH_IDLE_HANDLE_H_

#ifdef IF_SUPPORT_BLUETOOTH
#include "baseidlehandle.h"

class CBlueToothHandle : public CBaseIdleHandle
{
public:
    CBlueToothHandle(int nType = PS_STATE_BLUETOOTH);
    virtual ~CBlueToothHandle();
    static LRESULT OnBlueToothMessageMessage(msgObject& msg);
};

#if IF_SUPPORT_BLUETOOTH_ADAPTER
class CBlueToothAdapterHandle : public CBaseIdleHandle
{
public:
    CBlueToothAdapterHandle(int nType = PS_STATE_ADAPTER_ADD);
    virtual ~CBlueToothAdapterHandle();
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData& popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);

};
#endif

#endif

#endif //end of _BLUETOOTH_IDLE_HANDLE_H_

