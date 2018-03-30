#ifndef __GENBANDBOOK_ACTION_H__
#define __GENBANDBOOK_ACTION_H__

#if IF_FEATURE_GBBOOK
#include "ETLLib.hpp"
#include "threadtask/include/networkfileaction.h"
#include "commondata.h"
#include "threadtask/include/common.h"



using namespace NS_TA;
class CGenbandBookAction : public CNetworkFileAction
{
public:

    // | strLocalFilePath | 是远程文件将要存放在本地的路径.
    // | strRemoteFilePath | 是远程文件的地址.
    CGenbandBookAction(const yl::string & strLocalFilePath,
                       const yl::string & strRemoteFilePath, GBBOOK_OPERATION_TYPE eType);
    virtual ~CGenbandBookAction();

    void SetData(BaseDirDataRecord * pData);

    void SetGabKey(const yl::string & strKey);

    const yl::string GetGabKey()
    {
        return m_strGabKey;
    }

    BaseDirDataRecord * GetData()
    {
        return m_pData;
    }

    GBBOOK_OPERATION_TYPE GetOptType()
    {
        return m_eType;
    }

#if 0
    void SetDownloadMode(TASKACTION_DL_MODE eMode);
    TASKACTION_DL_MODE GetDownlodMode();
#endif

protected:
    // 重写. 写文件.
    //virtual bool Write();

private:

    GBBOOK_OPERATION_TYPE m_eType;

    BaseDirDataRecord * m_pData;

    yl::string m_strGabKey;

    //NS_TA::CNetworkFileActionDownloadHelper* m_pHelperEx;

    TASKACTION_DL_MODE m_eDownLoadMode; // 下载模式
};
#endif

#endif
