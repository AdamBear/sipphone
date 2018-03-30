#ifndef _MISSCALL_IDLE_HANDLE_H_
#define _MISSCALL_IDLE_HANDLE_H_

#include "baseidlehandle.h"

class CMissCallHandle : public CBaseIdleHandle
{
public:
    CMissCallHandle(int nType = PS_STATE_MISSCALL);
    virtual ~CMissCallHandle();
    static LRESULT OnMissCallMessage(msgObject & msg);

public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
    // 获取显示的图标信息
    virtual bool GetIconInfo(IdleIconInfo & IconData);
#if IF_SUPPORT_TEXT_NOTIFY
    // 获取显示的提示文本信息
    virtual bool GetTextNotifyData(ST_TextNotifyData& notifyData);
    // 获取按键信息
    virtual bool GetSoftkey(VEC_SOFTKET_TYPE& m_vecSoftkey);
#endif
public:
    static int m_nMisscallCounts;
};

#endif //end of _MISSCALL_IDLE_HANDLE_H_

