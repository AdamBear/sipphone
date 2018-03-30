#ifndef _CALLCOMPLETION_IDLE_HANDLE_H_
#define _CALLCOMPLETION_IDLE_HANDLE_H_

#include "baseidlehandle.h"

class CallCompletionData;
class CCallCompletionHandle : public CBaseIdleHandle
{
public:
    CCallCompletionHandle(int nType = PS_STATE_CALL_COMPLETION);
    virtual ~CCallCompletionHandle();

public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
private:
    CallCompletionData * m_pCallCompletionData;
};

#endif //end of _CALLCOMPLETION_IDLE_HANDLE_H_

