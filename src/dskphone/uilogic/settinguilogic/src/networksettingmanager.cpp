#include "networksettingmanager.h"

//静态变量初始化
bool CNetworkChangeObject::m_bNetworkChangeFlag = false;

CNetworkChangeObject & GetNetworkChangeObjectInstance()
{
    static CNetworkChangeObject s__NetworkChangeFlagManager;
    return s__NetworkChangeFlagManager;
}

bool CNetworkChangeObject::GetNetworkChangeFlag()
{
    return m_bNetworkChangeFlag;
}

void CNetworkChangeObject::SetNetworkChangeFlag(bool bPageFlag)
{
    m_bNetworkChangeFlag |=  bPageFlag;
}

void CNetworkChangeObject::ClearChangeFlag()
{
    m_bNetworkChangeFlag = false;
}

