#ifndef _NETWORK_SETTING_MANAGER_H_
#define _NETWORK_SETTING_MANAGER_H_

class CNetworkChangeObject
{
public:
    CNetworkChangeObject() {}
    ~CNetworkChangeObject() {}

public:
    //获取当前网络变化标记值
    bool GetNetworkChangeFlag();

    //设置网络变化标记值，将当前值与bPageFlag取或
    void SetNetworkChangeFlag(bool bPageFlag);

    //重置网络变化标记
    void ClearChangeFlag();

private:
    static bool m_bNetworkChangeFlag;

};

#define  _NetworkChangeFlagManager GetNetworkChangeObjectInstance()

CNetworkChangeObject & GetNetworkChangeObjectInstance();

#endif  //_NETWORK_SETTING_MANAGER_H_

