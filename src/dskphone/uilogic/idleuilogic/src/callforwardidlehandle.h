#ifndef _CALL_FORWARD_IDLE_HANDLE_H_
#define _CALL_FORWARD_IDLE_HANDLE_H_

#include "baseidlehandle.h"

class CCallForwardHandle : public CBaseIdleHandle
{
public:
    CCallForwardHandle(int nType = PS_STATE_CALLFORWARD);
    virtual ~CCallForwardHandle();
    static LRESULT OnForwardMessage(msgObject & msg);

public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
#if IF_SUPPORT_TEXT_NOTIFY
    // 获取显示的提示文本信息
    virtual bool GetTextNotifyData(ST_TextNotifyData& stNotifyData);
    // 获取按键信息
    virtual bool GetSoftkey(VEC_SOFTKET_TYPE& m_vecSoftkey);
#endif
};

#endif //end of _CALL_FORWARD_IDLE_HANDLE_H_

