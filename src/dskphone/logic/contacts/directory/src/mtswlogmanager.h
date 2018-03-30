#if IF_FEATURE_METASWITCH_CALLLOG
#ifndef __MTSW_LOG_MANAGER_H__
#define __MTSW_LOG_MANAGER_H__

#include "basecontactmanager.h"
#include <ylhashmap.h>
#include "metaswitch/mtswcontrol/include/mtswactioncmdbase.h"

class CMTSWLogManager : public CBaseContactManger, public CMTSWActionCmdBase
{
public:
    static CMTSWLogManager * GetInstance();

    bool IsCallLogDownLoading()
    {
        return m_eStatus == FILE_DOWNLOADING;
    }

    void PreLoad();

    bool ClearCallLog();

    //重新加载历史记录
    bool ReqLoadCallLog();

    //下载失败回调
    virtual void OnRequestError(NS_TA::TaskActionErrorType eErrCode, int nType, int nId);

    //下载成功回调
    virtual bool OnLoadSucProcess(CMTSWAction * pAction);

    //通过ID获取排序字段
    virtual const yl::string & GetSortKeyById(int nId);

protected:

    // 如果初始化成功则返回true, 否则返回false.
    virtual bool MyInit();

    //加载成功则返回true, 否则返回false.
    bool MyCreatGroupsFromConfig();

    //创建配置组节点
    void CreatConfigGroup(int nGroupId, yl::string strName);

    // 执行下载
    bool MyExecDownload();

    // 解析下载到的数据
    bool ParseDownloadedFile(cJSON * pObject);

    virtual bool IsNeedShowAllGroup();

    // 解析Json数据
    bool ParseJsonData(cJSON * objJson, int nGroupId, CALL_TYPE eLogType);

private:
    CMTSWLogManager();
    virtual ~CMTSWLogManager();

private:
    FileDownloadStatus m_eStatus;
};

#define g_pMTSWLogManager (CMTSWLogManager::GetInstance())


#endif //__MTSW_LOG_MANAGER_H__

#endif //IF_FEATURE_METASWITCH_CALLLOG
