#ifndef _NETWORK_IDLE_HANDLE_H_
#define _NETWORK_IDLE_HANDLE_H_

#include "baseidlehandle.h"
#include "idle/idle_data_define.h"

class CNetworkHandle : public CBaseIdleHandle
{
public:
    CNetworkHandle(int nType = PS_STATE_NETWORK_FAILED);
    virtual ~CNetworkHandle();
    static LRESULT OnStatusMessage(msgObject & msg);
    //刷新网络状态,返回是否正常
    static bool FlushNetStatus();

public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
    // 获取显示的图标信息
    virtual bool GetIconInfo(IdleIconInfo & IconData);
#if IF_SUPPORT_TEXT_NOTIFY
    // 获取显示的提示文本信息
    virtual bool GetTextNotifyData(ST_TextNotifyData& stNotifyData);
#endif
private:
    void GetNotifyText(NETWORK_STATUS eStatus, yl::string& strNote);

private:
    static NETWORK_STATUS   m_lastNetworkStatus;
};

#endif //end of _NETWORK_IDLE_HANDLE_H_

