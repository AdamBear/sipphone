#ifndef __ACCOUNT_MANAGER_H__
#define __ACCOUNT_MANAGER_H__

#include "account_inc.h"

#include "baseaccount.h"
#include "sipaccount.h"
#ifdef IF_FEATURE_PSTN
#include "pstnaccount.h"
#endif // IF_FEATURE_PSTN
#ifdef IF_FEATURE_H323
#include "../src/h323account.h"
#endif
#include "accountlist.h"


//帐号管理类
class CAccountManager
{
public:
    //构造函数
    CAccountManager();
    //析构函数
    ~CAccountManager();

    static CAccountManager & GetInstance() DECLARE_ATTR;

public:
    //初始化帐号管理类对象
    bool InitializeObject();
    //注销帐号管理类对象
    void UninitializeObject();

public:
    //刷新所有帐号状态
    void RefreshAllLineState();
    //根据帐号Id刷新帐号状态
    void RefreshLineStateById(int iAccountId);

    //加载所有帐号
    void LoadAllLineAccount();
    //根据帐号Id加载帐号
    void LoadLineAccountById(int iAccountId);
#if IF_FEATURE_PSTN
    // 根据账号ID加载PSTN账号
    void LoadPstnAccountById(int iAccountId, int iDeviceId);
#endif // IF_FEATURE_PSTN

    //获取SIP IP直拨帐号
    CSipAccount * GetDirectIPAccount(bool bCheck = true);
    //根据帐号Id获取帐号信息
    CBaseAccount * GetAccountByAccountId(int iAccountId, bool bCheck = true);
    //由服务器名获得帐号信息
    CBaseAccount * GetAccountByServerName(const yl::string & strServer);
    //根据帐号全名获取帐号信息
    CBaseAccount * GetAccountByFullName(LPCSTR lpszFullName);
    //根据帐号获取帐号信息
    CBaseAccount * GetAccountBySIPName(LPCSTR lpszSIPName, LPCSTR lpszSIPServer = NULL);

#ifdef IF_FEATURE_PSTN
    // 根据设备ID获取账号信息
    CPstnAccount * GetAccountByDeviceId(int iDeviceId);
    //根据PSTN帐号名称获取帐号信息，格式为"PSTN%d"
    CPstnAccount * GetAccountByPstnName(const char * szPstnFullName);

    /**
     * @brief 获取与指定账号相似的可用账号
     * @details 如果首选账号可用，则返回首选账号ID。其次优先使用相同类型的可用
     * 最小编号账号。再次使用其他类型的可用账号。如果没有可用账号，则返回NULL。
     *
     * @param iFirstChoiceAccountId 首选账号ID
     *
     * @return 可用账号。无指定条件的可用账号则返回NULL。
     */
    CBaseAccount * GetUsableAccountLike(int iFirstChoiceAccountId);
#endif // IF_FEATURE_PSTN

    //自动获得一个可用的账号,bFirst为是否获取第一个可用帐号
    CBaseAccount * GetUsableAccount(bool bSearchFirst = false,
                                    int eAccountType = ACCOUNT_TYPE_SIP | ACCOUNT_TYPE_PSTN | ACCOUNT_TYPE_H323);

    // 自动获得一个已注册的账号, bFirst为是否获取第一个已注册帐号
    CBaseAccount * GetRegisteredAccount(bool bSearchFirst = false, int eAccountType = ACCOUNT_TYPE_SIP
#ifdef IF_FEATURE_PSTN
                                        | ACCOUNT_TYPE_PSTN
#endif // IF_FEATURE_PSTN
                                       );

    //根据服务器和账号名获取可用账号
    CBaseAccount * GetUsableAccount(const yl::string & strServer, const yl::string & strName = "");
    //获取紧急呼叫的账号
    CBaseAccount * GetEmergencyAccount();

    //获取不包括特殊帐号的可用帐号数
    UINT GetUsableAccountCount();
    //获取可用帐号数 注:与上面接口不能整合
    //上面接口使用地方较多且设置数量缓存, 与实际是否usable可能不符
    UINT GetUsableAccountCount(bool bExceptSpecialAccount,
                               int eAccountType = ACCOUNT_TYPE_ALL);

    // 获取第一个Server不为空的账号
    CBaseAccount * GetNoEmptyServerAccount(bool bChekEnable = false);
    // 返回第一个有用户名的账号对象
    CBaseAccount * GetFirstUserNameAccount();

    //获取有效账号数据，包括注册失败
    bool GetValidAccountList(CAccountList & listValidAccount);
    //获取可用账号数据
    bool GetAvailableAccountList(CAccountList & listValidAccount);

    //使用帐号Id设置默认帐号
    bool SetDefaultAccount(int iAccountId);
    //将默认帐号Id保存至文件中
    bool SaveDefaultAccount(int iAccountId);
    //获取默认账号ID
    int GetDefaultAccountID();
    //切换默认帐号
    int SwitchDefaultAccount(bool bForward = true);

    //处理帐号状态改变消息
    bool OnLineStateChange(msgObject & objMessage);

#ifdef IF_FEATURE_PSTN
    // 处理PSTN线路状态改变消息
    bool OnPstnLineStateChange(msgObject & objMessage);
#endif // IF_FEATURE_PSTN

    // 处理账号配置改变消息
    bool OnLineConfigChange(msgObject & objMessage);

    //收到新语音信息消息，刷新帐号的语音信息相关内容
    bool OnNewVoiceMessage(msgObject & objMessage);

    //处理网络状态改变消息
    bool OnNetworkStateChange(msgObject & objMessage);

    //打印所有帐号信息
    void PrintAllAccountInfo();

    //获取帐号列表
    void GetAccountList(CAccountList & listAccount, bool bOnlyUser = true);

    // 返回账号个数
    int GetAccountNum(bool bOnlyUser = true);

    // 获取注册上的账号数
    int GetRegisteredAccountNum(bool bOnlyUser = true, int eAccountType = ACCOUNT_TYPE_ALL);

    // 获取注册失败的账号
    YLList<int> GetRegisteFailAccountList(int eAccountType = ACCOUNT_TYPE_SIP);

    // 设置autoanswer状态
    bool SetAutoAnswer();
#ifndef _T49
    // 获取voicemail数量，bReaded = true表示要获取未读iAccount=-1表示获取所有账号的总数
    int GetVoiceMailNum(bool bReaded, int iAccountID = -1);
#endif
    // 返回最多账号数
    int GetMaxAccountNum(bool bIgnoreDirectIPAccount = true);

    // 以字符串形式返回所有账号信息
    void GetAccountInfo(yl::string & strInfoOut, int eAccountType = ACCOUNT_TYPE_ALL);
    bool SetAccountInfo(yl::string & strInfoIn);

    //备份账号配置
    void BackupRestoreCfg(int iAccountId, bool bIsRestore);

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER

    //获取ACD账号(需注册上)
    int GetACDAccountId();
    //获取Hoteling账号(需注册上)
    int GetHotelingAccountId();
#endif
#ifndef _T49
    // 获取最近的VoiceMail的账号ID
    int GetLastVMailAccountId();
#endif

    // 擦除账号链表中的账号, eAccountType -- 账号主类型, iType -- 账号子类型
    void EraseAllAccount(int eAccountType = ACCOUNT_TYPE_ALL, int iType = -1);
    // 根据账号ID擦除账号链表中的账号
    void EraseAccountById(int id);
    // 清空所有账号文件中配置指定的账号信息, eAccountType -- 账号主类型, iType -- 账号子类型
    void EmptyAllAccount(int eAccountType = ACCOUNT_TYPE_ALL, int iType = -1);
    // 根据账号ID清空账号文件中配置指定的账号信息
    void EmptyAccountById(int id);
    // 判断是否所有账号都没有开启
    bool IsAllAccountDisable();
#if 1
    // 恢复出厂配置前，通知SIP到服务器注销帐号，已经全部注销返回TRUE，否则返回FALSE
    bool LogoutBeforeReset();
    // 定时器处理
    BOOL OnTimer(UINT uTimeID) DECLARE_ATTR;
    // 是否处于等待恢复出厂配置前的账号注销阶段
    bool BeWaitLogoutBeforeReset()
    {
        return (m_iCheckLogOutTime > 0);
    }
#endif  //IF_BUG_20555

    // 判断是否所有账号都没有开启
    bool IsAllAccountEmpty();

    // 账号是否支持H264
    bool IsSupportH264(int iAccountId);

#ifdef IF_FEATURE_H323
    //////////////////////////////H323////////////////////////////////////////////
    bool LoadH323Account();

    // 账号配置变更
    bool OnH323ConfigChanged(msgObject & objMessage);

    //处理帐号状态改变消息
    bool OnH323LineStateChange(msgObject & objMessage);

    // 取H323账号
    CBaseAccount * GetH323DirectIPAccount();

    // 底层账号Id映射上层账号id
    int MapH323AccountId2Logic(int iAccountId);
#endif //#ifdef IF_FEATURE_H323

    //通知默认账号改变
    void NotifyDefaultAccountChanged(int iOldDefAcc);

    void SetTransPort(msgObject & objMessage);

protected:
    //帐号状态改变
    //bool bGuiQuery用来标识传入的state是由GUI主动向sip查询获得的
    bool LineStateChange(int iAccountId, LINE_STATE eState, account_register_reason_t & sReason,
                         bool bGuiQuery = false);
    //设置监控接口
    void RegisterMsg();
    //通知默认账号改变
    void NotifyDefaultAccountChanged();
    //检测网络状态发生改变，从而帐号状态相应修改
    void CheckNetworkStatus();
    //获取是否允许IP直播功能
    bool IsAllowIPCall();
    //处理帐号设备提示流程
    void ProcessAccountDeviceTip();
    // 自动填写Account账号配置
    void AutoSetAccount();
#ifndef _T49
    //m_iLastVoiceMailID对应的账号发生改变，
    void SetLastVMailAccountID(int iAccountID);
#endif
    //重新设置默认账号
    void ResetDefaultAccount();

private:
    CAccountList    m_listAccount;

    int             m_iDefaultAccountId;            //系统默认帐号ID号

    UINT            m_uMaxLineCount;                //最大帐号数

    int             m_iLastVoiceMailID;

    int             m_nUsableAccount;               //可用账号数

    int             m_iCfgStatus;       //配置备份状态

    bool            m_bLastNetworkReady;

#if IF_BUG_20555
    int
    m_iCheckLogOutTime;     // 恢复出厂设置前，检测账号全部注销次数，0.5s检测一次，检测30次
#endif  //IF_BUG_20555
};


// 获取全局唯一的窗口管理对象
#define _AccountManager CAccountManager::GetInstance()

#endif //__ACCOUNT_MANAGER_H__
