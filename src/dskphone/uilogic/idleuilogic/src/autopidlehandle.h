#ifndef _AUTOP_IDLE_HANDLE_H_
#define _AUTOP_IDLE_HANDLE_H_

#include "baseidlehandle.h"

class CAutoPHandle : public CBaseIdleHandle
{
public:
    CAutoPHandle(int nType = PS_STATE_AUTO_PROVISION);
    virtual ~CAutoPHandle();

public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
};

class CAutopErrorHandle : public CBaseIdleHandle
{
public:
    CAutopErrorHandle(int nType = PS_STATE_AUTO_PROVISION_ERROR);
    virtual ~CAutopErrorHandle();
public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
};

#endif //end of _AUTOP_IDLE_HANDLE_H_

