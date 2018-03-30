#ifndef __ACCOUNT_H__
#define __ACCOUNT_H__

#include "baseaccount.h"

//帐号信息类
class CSipAccount : public CBaseAccount
{
public :
    //构造函数
    CSipAccount();

    //析构函数
    ~CSipAccount();

public:

    //打印帐号信息
    virtual void    PrintAccoutInfo() const;

    //从配置文件中加载帐号信息
    virtual bool    LoadAccountFromConfig(const yl::string & lpszAccountPath, int iAccountId = -1);

#if defined(IF_FEATURE_BROADSOFT_CALL_CENTER) || defined(IF_FEATURE_GENESYS_ACD)
    //获取账号ACD开关值
    bool GetACDSwitch() const;

    //获取账号Hoteling开关值
    bool GetHotelingSwitch() const;

    //获取账号FlexibleSeating开关值
    bool GetFlexibleSeatingSwitch() const;
#endif

public:
    //处理帐号状态改变消息内容
    //bool bGuiQuery用来标识传入的state是由GUI主动向sip查询获得的
    virtual bool LineStateChange(LINE_STATE eState, account_register_reason_t & sReason,
                                 bool bGuiQuery = false);

    /**
    * @brief 获取自动应答延迟时间
    * @return 自动应答延迟时间(单位ms)
    */
    virtual int GetAutoAnswerTime() const;

    /**
    * @brief 获取默认呼叫方式
    * @return 默认呼叫方式
    */
    virtual CALL_AV_TYPE GetDefaultCallMode() const;

    //获取注册失败Id
    int GetRegisterFailId() const;
    //获取注册失败信息
    yl::string GetRegisterFailInfo() const;

    //获取用户名
    yl::string GetUserName() const;

    //获取服务器名
    yl::string GetServerName() const;
    //获取服务器ID
    int GetServerID() const;
    //设置服务器名
    void SetServerName(const char * lpszServerName);
    //获取服务器序号
    void SetServerId(int iServerID);

    //获取服务器端口
    int GetServerPort() const;
    // 重置服务器端口
    void ResetServerPort();

    //获取显示名
    yl::string GetDisplayName() const;
    //获取注册名
    yl::string GetRegisterName() const;

    //获取服务器类型
    SIP_SERVER_TYPE GetServerType() const;

    //获取配置路径
    yl::string GetConfigPath() const;

#ifndef _T49
    //获取未读语音信息数量
    UINT GetUnReadVoiceMailCount() const;
    //设置未读语音信息数量
    void SetUnReadVoiceMailCount(UINT nCount);

    //获取已读语音信息数量
    UINT GetReadedVoiceMailCount() const;
    //设置已读语音信息数量
    void SetReadedVoiceMailCount(UINT nCount);
#endif

    // 获取同步订阅是否成功
    bool GetSyncSubscribeResult() const;
    // 设置同步订阅是否成功
    void SetSyncSubscribeResult(bool bSuccess);

    //获取BLA类型
    BLA_TYPE GetBLAType() const;

    //获取是否处理VoiceMail开关
    bool GetVoiceMailSwitch() const;

    // 是否是特殊的云账号
    bool IsSpecialCloudAccount();

    virtual bool NeedRegister();

#ifdef IF_FEATURE_GETBALANCE
    inline bool GetBalanceSwtich() const
    {
        return m_bBalanceEnable;
    }

    inline yl::string GetBalanceLink() const
    {
        return m_strBalanceLink;
    }
    // 获取密码
    inline yl::string GetPassword() const
    {
        return m_strPasswd;
    }
#endif

#ifdef IF_SUPPORT_VIDEO
    // 加载codec
    void LoadCodecInfo(int iAccountId);

    bool IsSupportH264()
    {
        return m_bSupportH264;
    }
#endif //#ifdef IF_SUPPORT_VIDEO

    bool IsNoNeedRegisterStatus() const
    {
        return m_eAccountState == LS_DISABLE_REG;
    }

    // 判断是否空账号
    bool IsEmptyAccount();

    void SetTransPort(int nTransPort);

    int GetTransPort();

    /**
    * @brief 获取自动应答开关状态
    * @return 自动应答开关状态
    */
    virtual bool GetAutoAnswerSwitch() const;

    /**
    * @brief 设置自动应答开关状态
    * @param 开关状态
    */
    virtual bool SetAutoAnswerSwitch(bool bEnable, CONFIG_LEVEL_TYPE eType)const;

    bool SetServerByServerID(int iServerID);

#if IF_BUG_39041
    void SetSupportSCA(bool bSupport)
    {
        m_bIsSupportSCA = bSupport;
    }
    bool IsSupportSCA()
    {
        return m_bIsSupportSCA;
    }
#endif
protected:
    //读取ServerName配置
    void InitServerName();
private:
    yl::string          m_strServerName;        //服务器地址
    int                 m_iServerID;            //服务器ID
    int                 m_iServerPort;          //服务器端口


#ifndef _T49
    UINT                m_uReadedVoiceMailCount;    //已读语音信息数
    UINT                m_uUnReadVoiceMailCount;    //未读语音信息数
#endif

    bool                m_bSyncSubscribe;   // 是否同步订阅成功
    bool                m_bDNDSyncAndOn;    // DND功能是否与服务器同步, 且开启
    bool                m_bCFASyncAndOn;    // FWD Always功能是否与服务器同步, 且开启
    SipAccountType      m_eSipAccountType;  // sip帐号类型
#ifdef IF_FEATURE_GETBALANCE
    bool                m_bBalanceEnable;   // 是否开启了余额功能
    yl::string          m_strBalanceLink;   // 获取花费的请求地址
    yl::string          m_strPasswd;        // 账号密码
#endif

#ifdef IF_SUPPORT_VIDEO
    bool                m_bSupportH264;     // 账号codec是否配置了H264
#endif //#ifdef IF_SUPPORT_VIDEO
#if IF_BUG_39041
    bool                m_bIsSupportSCA;
#endif
public:
    SIP_SERVER_TYPE     m_eServerType;      // 账号服务器类型

    int m_nTransPort; //注册的方式，0 UDP，1 TCP，2 TLS
};

#endif // __ACCOUNT_H__
