#include "cloudaccountaction.h"

#ifdef IF_FEATURE_CLOUD
CCloudAccountAction::CCloudAccountAction(const yl::string & strLocalFilePath,
        const yl::string & strRemoteFilePath)
    : CNetworkFileAction(strLocalFilePath, strRemoteFilePath)
{
    // 设置TaskAction的类型.
    m_eTARequestType = NS_TA::TA_REQUEST_NETWORK_FILE;

    m_eType = CLOUD_NONE;
}

CCloudAccountAction::~CCloudAccountAction()
{
}

void CCloudAccountAction::SetOptType(CLOUD_OPERATION_TYPE eType)
{
    m_eType = eType;
}
#endif //#ifdef IF_FEATURE_CLOUD

