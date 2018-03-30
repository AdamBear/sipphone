#ifndef __CLOUD_ACCOUNT_INFO_MANAGER_H__
#define __CLOUD_ACCOUNT_INFO_MANAGER_H__

#ifdef IF_FEATURE_CLOUD
#include "account_datadefine.h"
#include "taskaction/taskactioncallbackbase.h"

using namespace NS_TA;

class CCloudAccountInfoManager : public CTaskActionCallbackBase
{
public:
    //构造函数
    CCloudAccountInfoManager();
    //析构函数
    ~CCloudAccountInfoManager();

    static CCloudAccountInfoManager & GetInstance() DECLARE_ATTR;

public:
    bool Init();

    void AddUrlParameter(yl::string & strUrl, const yl::string & strName,
                         const yl::string & strValue, bool isPreMatch, bool isCaseSensitive = true,
                         bool isAddUrlParamer = true);

    void ProcessCloudLoginMsg(msgObject & objMessage);

    // 液晶登录请求
    bool ReqLogin(CloudAccountInfo & stInfo);

    // 云账号登录请求
    void ProcessCloudLogin();

    // yealink云账号登录
    bool CloudYLAccountLogin(const CloudAccountInfo & stInfo);

    // 第三方云平台账号登录
    bool Cloud3rdAccountLogin(const CloudAccountInfo & stInfo);

    // Yealink PIN登录-UI使用
    bool CloudYLPinLogin(int nAccountId, const yl::string & strPinCode);

    // Yealink PIN登录
    bool CloudYLPinLogin(CloudAccountInfo & stInfo);

    // yealink云账号登录
    bool CloudYLUserLogin(const CloudAccountInfo & stInfo);

    // 登出
    bool CloudAccountLogout(int nAccountId);

    // 回调函数. 当一次异步读写完毕时, 会调用此函数.
    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

    bool GetLoginCloudAccountInfo(CloudAccountInfo & refData);

    // 取历史记录
    bool GetCloudAccountInfo(VectorInfo & refData, SipAccountType eType,
                             OrderType eOrderType = REVERSE_ORDER);

    bool RemoveHistoryCloudAccountInfo(const yl::string & strUserName,
                                       SipAccountType eType = ACCOUNT_YEALINK, bool bSaveToFile = true);

    bool ReplaceCloudAccountInfo(CloudAccountInfo & refData);

    bool ProcessLoginHistory(int nAccount, LINE_STATE eState);

    bool AddHistoryAccountInfo(const CloudAccountInfo & refData, bool bSaveToFile);

    bool SaveToFile(YLVector<CloudAccountInfo> & list, const yl::string & strPath);

    bool ProcessYLLoginChange(int nAccountId, LINE_STATE eState, account_register_reason_t & sReason);

    bool Process3rdLoginChange(int nAccountId, LINE_STATE eState, account_register_reason_t & sReason);

    bool ProcessCloudStateChange(int iAccountId, LINE_STATE eState,
                                 account_register_reason_t & sReason);

    bool ProcessContactChange(int nAccountId, LINE_STATE eState);

    bool CancelLogin(int nAccount);

    // 处理网络状态改变消息
    bool OnNetworkStateChange();

private:
    bool ProcessBeforeLogin(int nAccountId, SipAccountType eType);

    bool LoadCloudAccount(YLVector<CloudAccountInfo> & listAccount, const yl::string & strPath);

    bool SaveAccountLoginData(const CloudAccountInfo & stAccountInfo, bool bIsLogin = true);

    bool OpenXMLFile(const char * lpFile, xml_document & doc);

    bool MyLoadEmailFromFile(const yl::string & strFilePath, yl::string & strEmail, int & code);

    bool MyLoadAccountInfoFromFile(const yl::string & strFilePath, CloudAccountInfo & stAccountInfo,
                                   int & code);

    int GetResultCodeFromXmlFile(const xml_document & doc); // code 0 success

    yl::string GetRandomNumString();

    CloudAccountRegFailCode GetFailCodeByReasonCode(int ReasonCode);

    bool RemovePinCode();

    // 检测云功能
    void CheckCloud(bool bNotify);

    // 清空账号
    void ClearCloudAccount(YLVector<CloudAccountInfo> & listData);

    // 根据类型取数据
    bool GetCloudInfoByType(YLVector<CloudAccountInfo> & listInfo, CloudAccountInfo & refData);

    void FixDefaultInfo(CloudAccountInfo & info);

    // 将没有设置的参数，补全默认值
    void FixCloudAccountInfo(CloudAccountInfo & infoTo, CloudAccountInfo & infoFrom);

private:
    // 保存历史记录
    YLVector<CloudAccountInfo> m_listHistory;

    // 保存登录请求
    YLVector<CloudAccountInfo> m_listReq;

    // 各云平台默认配置：数量量不大，常驻内存
    YLVector<CloudAccountInfo> m_listDefault;

    CloudAccountInfo m_stAccountInfo;

    CloudAccountLoginStatus m_eStatus;

    // 记录当前异步执行的指针，用于区分是否账号变更
    UINT m_nTaskId;
};

#define _CloudAccountInfoManager CCloudAccountInfoManager::GetInstance()
#endif //#ifdef IF_FEATURE_CLOUD
#endif

