#ifndef _WIFI_IDLE_HANDLE_H_
#define _WIFI_IDLE_HANDLE_H_

#ifdef IF_SUPPORT_WIFI
#include "baseidlehandle.h"

class CWifiHandle : public CBaseIdleHandle
{
public:
    CWifiHandle(int nType = PS_STATE_WIFI);
    virtual ~CWifiHandle();
    static LRESULT OnWifiMessageMessage(msgObject& msg);

public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData& popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
};

#if IF_SUPPORT_WIFI_ADAPTER
class CWifiAdapterHandle : public CBaseIdleHandle
{
public:
    CWifiAdapterHandle(int nType = PS_STATE_WIFI_ADAPTER_ADD);
    virtual ~CWifiAdapterHandle();
    static LRESULT OnWifiMessage(msgObject& msg);
public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData& popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);

};
#endif

#endif
#endif //end of _WIFI_IDLE_HANDLE_H_

