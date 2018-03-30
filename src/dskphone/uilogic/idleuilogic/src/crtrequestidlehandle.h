#ifndef _CRT_REQUEST_IDLE_HANDLE_H_
#define _CRT_REQUEST_IDLE_HANDLE_H_

#include "baseidlehandle.h"

class CCRTRequestHandle : public CBaseIdleHandle
{
public:
    CCRTRequestHandle(int nType = PS_STATE_CTR_REQUEST);
    virtual ~CCRTRequestHandle();

public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
};

#endif //end of _CRT_REQUEST_IDLE_HANDLE_H_

