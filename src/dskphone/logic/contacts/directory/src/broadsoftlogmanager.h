#ifndef __BROADSOFT_LOG_MANAGER_H__
#define __BROADSOFT_LOG_MANAGER_H__
#include "basecontactmanager.h"

//BroadSoft历史记录配置绑定账号id
#define knAccountId 0

#define kszLogBeginId 100

class CBroadSoftLogManager : public CBaseContactManger
{
public:
    static CBroadSoftLogManager * GetInstance();

    // 删除一条BSFT通话记录.
    DIRECTORY_RET_CODE RemoveSingleLog(int nId);

    // 删除一组BSFT通话记录.
    DIRECTORY_RET_CODE RemoveGroupLog(int uGroupId);

    //请求下载联系人
    bool ReqLoadCallLog(int nGroupId);

    CALL_TYPE GetCallTypeByGroupID(int nGroupID);

#if IF_FEATURE_MUTI_CALLLOG
    int ChangeToActualGroup(int nGroupID, CALL_TYPE eType = CT_CALLALL);

    int GetGroupIDByAccountID(int nAccountID, CALL_TYPE eType = CT_CALLALL);

    void ClearBsftCallLog();

    void ClearBsftGroupLog(int nGroupId);

    //获取最外层组的id列表
    virtual bool GetConfigMenuIdList(VectorId & output);

    bool IsAccountGroup(int nGroupID);

    int GetAccountIDByGroupID(int nGroupID);

    int GetAccGroupIDByAccountID(int nAccountID);
#endif

    // 回调函数
    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

    void ReloadConfig();

    //通过ID获取排序字段
    virtual const yl::string & GetSortKeyById(int nId);

    void LoadAllCalllog();

protected:

    // 初始化.
    // 如果初始化成功则返回true, 否则返回false.
    virtual bool MyInit();

    //加载成功则返回true, 否则返回false.
    bool MyCreatGroupsFromConfig();

    //创建配置组节点
    void CreatConfigGroup(int nGroupId, const yl::string & strServer, yl::string strName,
                          int nAccountID = knAccountId, bool bAddToShowList = true);

    // 执行下载
    int MyExecDownload(int nGrouopID);

    // 删除BSFT历史记录 ,type为BSFT_DELETE_SINGLE删除单条历史记录，type为BSFT_DELETE_GROUP删除整个组历史记录
    bool MyExecDelete(int nId, BSFT_DELETE_TYPE eType);

    // 解析下载到的数据
    bool ParseDownloadedFile(const yl::string & strFilePath, int nParentID = 0);

    virtual bool IsNeedShowAllGroup();

    int GetGroupIdByNodeName(const yl::string & strName);

    // 解析组
    bool ParseXmlData(const xml_node & refNode, int nGroupId, CALL_TYPE eLogType,
                      bool bNeedLocalSort = false);

#if IF_FEATURE_MUTI_CALLLOG
    bool CreatChildGroup(int nParentID);

    bool CreatGroupByParentID(int nParentID, CALL_TYPE eLogType);

    CALL_TYPE GetLogTypeByNodeName(const yl::string & strName);

    int GetGroupIdByLogType(CALL_TYPE eLogType, int nParentID);

    yl::string GetServerUrlByAccountID(int nAccountID);

    bool GetUserNameAndPasswordByAccountID(yl::string & strUserName, yl::string & strPassword,
                                           int nAccountID = knAccountId);

    // 获取对应根节点下的组ID
    //virtual int GetAllGroupIDByRootID(VectorId& vGroupID, int iRootID);

    //联系人是否有配置
    virtual bool IsDirAvailable();
#endif

private:
    CBroadSoftLogManager();
    virtual ~CBroadSoftLogManager();

private:
    // XSI password
    yl::string m_strPassword;
    // XSI port
    yl::string m_strPort;
    // XSI userName
    yl::string m_strUser;

    int m_nLastReqGroupID;
};

#define g_pBroadSoftLogManager (CBroadSoftLogManager::GetInstance())


#endif
