#ifndef _BALANCE_IDLE_HANDLE_H_
#define _BALANCE_IDLE_HANDLE_H_

#include "baseidlehandle.h"

#ifdef IF_FEATURE_GETBALANCE
class CBalanceHandle : public CBaseIdleHandle
{
public:
    CBalanceHandle(int nType = PS_STATE_BALANCE_LOADING);
    virtual ~CBalanceHandle();
    static LRESULT OnBalanceMessage(msgObject & msg);

public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
};
#endif

#endif //end of _BALANCE_IDLE_HANDLE_H_

