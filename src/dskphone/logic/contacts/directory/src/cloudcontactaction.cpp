#ifdef IF_FEATURE_CLOUD
#include "cloudcontactaction.h"

CCloudContactAction::CCloudContactAction(const yl::string & strLocalFilePath,
        const yl::string & strRemoteFilePath, CLOUD_OPERATION_TYPE eType)
    : NS_TA::CNetworkFileAction(strLocalFilePath, strRemoteFilePath)
{
    // 设置TaskAction的类型.
    m_eTARequestType = NS_TA::TA_REQUEST_NETWORK_FILE;

    m_eType = eType;
}

CCloudContactAction::~CCloudContactAction()
{
}


#endif //#ifdef IF_FEATURE_CLOUD
