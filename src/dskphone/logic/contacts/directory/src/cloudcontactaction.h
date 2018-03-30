#ifndef __CLOUDCONTACT_ACTION_H__
#define __CLOUDCONTACT_ACTION_H__
#ifdef IF_FEATURE_CLOUD

#include "ETLLib.hpp"
#include "threadtask/include/networkfileaction.h"
#include "commondata.h"
#include "threadtask/include/common.h"
#include "account/include/account_datadefine.h"
//#include "commonunits/commonunits_def.h"


using namespace NS_TA;
class CCloudContactAction : public NS_TA::CNetworkFileAction
{
public:

    // | strLocalFilePath | 是远程文件将要存放在本地的路径.
    // | strRemoteFilePath | 是远程文件的地址.
    CCloudContactAction(const yl::string & strLocalFilePath,
                        const yl::string & strRemoteFilePath, CLOUD_OPERATION_TYPE eType);
    virtual ~CCloudContactAction();

    CLOUD_OPERATION_TYPE GetOptType()
    {
        return m_eType;
    }

protected:

private:
    CLOUD_OPERATION_TYPE m_eType;
};
#endif //#ifdef IF_FEATURE_CLOUD
#endif

