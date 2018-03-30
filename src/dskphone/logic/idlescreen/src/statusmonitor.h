#ifndef __IDLESCREEN_STATUS_MONITOR_H__
#define __IDLESCREEN_STATUS_MONITOR_H__

#ifdef IF_FEATURE_IDLESCREEN
#ifndef IF_NOTIFICATION_CENTER_ON_IDLE
#include <singletonclassdefine.h>
#include "idlescreen_inc.h"


class CStatusMonitor
{
    DECLARE_SINGLETON_CLASS(CStatusMonitor);
public:
    static LRESULT OnStatusMessage(msgObject & msg);

    static LRESULT OnMissCallMsg(msgObject & msg);

    static LRESULT OnForwardMsg(msgObject & msg);

    static LRESULT OnAccountStatusChange(msgObject & msg);

    static LRESULT OnWifiStatusMsg(msgObject & msg);
#ifdef IF_SUPPORT_BLUETOOTH
    static LRESULT OnBtStatusMsg(msgObject & msg);
#endif //#ifdef IF_SUPPORT_BLUETOOTH
    static LRESULT OnDndStatusMsg(msgObject & msg);
    static LRESULT OnFwdStatusMsg(msgObject & msg);

    //刷新网络状态,返回是否正常
    bool FlushNetStatus();

    //初始化音量状态
    void InitVoiceStatus();

    // http://192.168.1.99/Bug.php?BugID=24993
    // 初始化 DHCP option
    void InitDHCPOption();

#ifdef IF_FEATURE_PSTN
    BOOL ProcessPstnAccountMsg(msgObject & objMsg);
#endif

private:
    //发送Account状态的ActionURL
    void SendAccountStatusURL(int iAccount, int eOldStatus);


    // 根据DHCP Option 120设置第一个账号的sip server
    void SetAccountServerByDHCP();
private:
    NETWORK_STATUS m_lastNetworkStatus;

};

#define _StatusMonitor GET_SINGLETON_INSTANCE(CStatusMonitor)

#endif // IF_FEATURE_IDLESCREEN
#endif // ndef T49
#endif

