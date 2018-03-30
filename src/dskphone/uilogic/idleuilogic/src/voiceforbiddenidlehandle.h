#ifndef _VOICE_FORBIDDEN_IDLE_HANDLE_H_
#define _VOICE_FORBIDDEN_IDLE_HANDLE_H_

#include "baseidlehandle.h"


class CVoiceForbiddenHandle : public CBaseIdleHandle
{
public:
    CVoiceForbiddenHandle(int nType = PS_STATE_VOICE_FORBIDDEN);
    virtual ~CVoiceForbiddenHandle();
    static LRESULT OnVoiceMessage(msgObject & msg);

public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
};


#endif //end of _VOICE_FORBIDDEN_IDLE_HANDLE_H_

