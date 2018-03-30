#ifndef _E911_LOCATION_IDLE_HANDLE_H_
#define _E911_LOCATION_IDLE_HANDLE_H_

#include "baseidlehandle.h"

#if IF_FEATURE_GENBAND_E911
class CE911LocationHandle : public CBaseIdleHandle
{
public:
    CE911LocationHandle(int nType = PS_STATE_E911_LOCATION);
    virtual ~CE911LocationHandle();
    static LRESULT OnE911LocationMessage(msgObject& msg);

public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData& popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
};
#endif

#endif //end of _E911_LOCATION_IDLE_HANDLE_H_

