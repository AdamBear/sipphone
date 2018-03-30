#ifndef _ACCOUNT_IDLE_HANDLE_H_
#define _ACCOUNT_IDLE_HANDLE_H_

#include "baseidlehandle.h"

class CAccountIdleHandle : public CBaseIdleHandle
{
public:
    CAccountIdleHandle(int nType = PS_STATE_ACCOUNT);
    virtual ~CAccountIdleHandle();

public:
    // 返回数据信息
    virtual bool GetAccountInfo(yl::string & strDisplayName, ICON_TYPE & eIcon);
public:
    static LRESULT OnAccountMsg(msgObject & objMessage);
protected:
#if IF_SUPPORT_CENTER_ACD_STATUS
    static void UpdateAcdStatus();
#endif
    static bool UpdateAccountInfo();
    static bool OnAccountStatusChange(msgObject & msg);
    static void SendAccountStatusURL(int iAccount, int eOldStatus);
    static bool GetACDStatus(ACD_STATUS eAcdStatus, yl::string& strAcdStatus);
};

#endif //end of _ACCOUNT_IDLE_HANDLE_H_

