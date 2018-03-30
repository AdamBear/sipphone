#ifndef _CLOUD_ACCOUNT_ACTION_H_
#define _CLOUD_ACCOUNT_ACTION_H_

#ifdef IF_FEATURE_CLOUD

#include "taskaction/taskaction.h"
#include "threadtask/include/networkfileaction.h"
#include "commonunits/commonunits_def.h"
#include "../include/account_datadefine.h"

class CCloudAccountAction : public CNetworkFileAction
{
public:
    // 构造函数.
    // | strLocalFilePath | 是远程文件将要存放在本地的路径.
    // | strRemoteFilePath | 是远程文件的地址.
    CCloudAccountAction(const yl::string & strLocalFilePath, const yl::string & strRemoteFilePath);
    virtual ~CCloudAccountAction();

    CLOUD_OPERATION_TYPE GetOptType()
    {
        return m_eType;
    }
    void SetOptType(CLOUD_OPERATION_TYPE eType);

private:
    CLOUD_OPERATION_TYPE m_eType;
};
#endif //#ifdef IF_FEATURE_CLOUD
#endif  // _CLOUD_ACCOUNT_ACTION_H_
