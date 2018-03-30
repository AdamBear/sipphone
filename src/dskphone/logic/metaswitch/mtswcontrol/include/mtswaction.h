#if IF_FEATURE_METASWITCH
#ifndef MTSW_ACTION_H_
#define MTSW_ACTION_H_

#include "taskaction/taskaction.h"
#include "threadtask/include/networkfileaction.h"
#include "json/cjson.h"
#include "mtswcommondefine.h"

class CMTSWActionCmdBase;

enum SessionErrorType
{
    ST_NO_ERROR = 0,
    ST_ERROR_SESSIONEXPIRED,
    ST_ERROR_OTHER,
};

class CMTSWAction : public CNetworkFileAction
{
public:
    // 构造函数.
    // | strLocalFilePath | 是远程文件将要存放在本地的路径.
    // | strRemoteFilePath | 是远程文件的地址.
    CMTSWAction(const yl::string & strLocalFilePath, const yl::string & strRemoteFilePath);
    virtual ~CMTSWAction();

    // Override.
    virtual bool Exec();

    /***
     * @brief : Exec过程中产生的资源回收
     * @param : |bCallBackSucceed| 是否回调成功
     * @notice: 涉及异步, 只能操作自身资源
     */
    virtual bool ReleaseData(bool bCallBackSucceed)
    {
        return false;
    }

    bool IsSessionExpired()
    {
        return m_bIsSessionExpired;
    }

    void SetActionCallback(CMTSWActionCmdBase * pCallback)
    {
        m_pCallback = pCallback;
    }
    CMTSWActionCmdBase * GetActionCallback()
    {
        return m_pCallback;
    }

    void SetMTSWActionType(MtswActionType nType)
    {
        m_nType = nType;
    }
    MtswActionType GetMTSWActionType()
    {
        return m_nType;
    }

    bool IsAsnycParse()
    {
        return m_bAsyncParse;
    }

    cJSON * GetJsonObject();

private:

    bool ProcessServerError();

private:

    CMTSWActionCmdBase * m_pCallback;
    cJSON * m_pcJSON;
    MtswActionType m_nType;
    bool m_bAsyncParse;

    bool m_bIsSessionExpired;
};

#endif  // MTSW_ACTION_H_
#endif
