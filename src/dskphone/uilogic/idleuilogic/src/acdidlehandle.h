#ifndef _ACD_IDLE_HANDLE_H_
#define _ACD_IDLE_HANDLE_H_

#include "baseidlehandle.h"
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER

class CAcdBaseHandle : public CBaseIdleHandle
{
public:
    CAcdBaseHandle(int nType = PS_STATE_NONE);
    virtual ~CAcdBaseHandle();

public:
    // 获取按键信息
    void GetLogoutLockSoftkey(VEC_SOFTKET_TYPE& m_vecSoftkey, int& nSoftkeyIndex, SoftKey_TYPE eSoftkey);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
};

class CAcdHandle : public CAcdBaseHandle
{
public:
    CAcdHandle(int nType = PS_STATE_ACD_CHANGE);
    virtual ~CAcdHandle();
    static LRESULT OnACDMessage(msgObject & objMessage);
    static LRESULT OnACDMessagelogintimeout(msgObject & objMessage);
    static LRESULT OnACDMessageloginfail(msgObject & objMessage);
    static LRESULT OnACDMessagestatuschange(msgObject & objMessage);
    static LRESULT OnACDMessagequeuestatus(msgObject & objMessage);

public:
    // 获取按键信息
    virtual bool GetSoftkey(VEC_SOFTKET_TYPE& m_vecSoftkey);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
};

//////////////////CHotelingHandle//////////////////
class CHotelingHandle : public CAcdBaseHandle
{
public:
    CHotelingHandle(int nType = PS_STATE_HOTELING_CHANGE);
    virtual ~CHotelingHandle();
    static LRESULT OnHotelingMessage(msgObject & objMessage);

public:
    // 获取按键信息
    virtual bool GetSoftkey(VEC_SOFTKET_TYPE& m_vecSoftkey);
};

//////////////////CHotelingGuestOutHandle//////////////////
class CHotelingGuestOutHandle : public CBaseIdleHandle
{
public:
    CHotelingGuestOutHandle(int nType = PS_STATE_HOTELING_GUESTOUT);
    virtual ~CHotelingGuestOutHandle();

public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);

};

//////////////////CHotelingGuestOutCheckHandle//////////////////
class CHotelingGuestOutCheckHandle : public CBaseIdleHandle
{
public:
    CHotelingGuestOutCheckHandle(int nType = PS_STATE_HOTELING_GUESTOUT_CHECK);
    virtual ~CHotelingGuestOutCheckHandle();

public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData);
};

//////////////////CFlexibleSeatingHandle//////////////////
class CFlexibleSeatingHandle : public CBaseIdleHandle
{
public:
    CFlexibleSeatingHandle(int nType = PS_STATE_FLEXIBLE_UPDATE);
    virtual ~CFlexibleSeatingHandle();
    static LRESULT OnFlexibleMessage(msgObject & objMessage);

public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
};

#endif // end of IF_FEATURE_BROADSOFT_CALL_CENTER
#endif //end of _ACD_IDLE_HANDLE_H_

