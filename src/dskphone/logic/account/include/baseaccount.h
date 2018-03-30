#ifndef __BASE_ACCOUNT_H__
#define __BASE_ACCOUNT_H__

#include "account_inc.h"
#include "account_datadefine.h"


/**
 * @brief 账号基类
 */
class CBaseAccount
{
public:
    CBaseAccount(ACCOUNT_TYPE eAccountType, AccountProtocolType eProtocol);
    virtual ~CBaseAccount();

    /**
     * @brief 比较运算
     * @details 主要比较账号ID
     *
     * @param lhs 左值
     * @param rhs 右值
     *
     * @return true: lhs<rhs; false: lhs>=rhs
     */
    static bool Compare(const CBaseAccount * lhs, const CBaseAccount * rhs);

    /**
     * @brief 获取账号ID
     * @return 账号ID
     */
    int GetAccountId() const;

    /**
    * @brief 获取账号类型
    * @return 账号类型
    */
    ACCOUNT_TYPE GetAccountType() const;

    /**
     * @brief 判断账号是否为IP直拨账号
     * @return 账号是否为IP直拨账号
     */
    bool IsDirectIPAccount() const;
    /**
     * @brief 判断账号是否为特殊账号
     * @details 特殊账号包括PSTN账号或IP直拨账号
     * @return 账号是否为特殊账号
     */
    bool IsSpecialAccount() const;
    /**
     * @brief 判断账号是否已注册
     * @details 账号是否为注册上/无需注册状态
     * @return 账号是否已注册
     */
    bool IsRegistered() const;
    /**
     * @brief 判断账号是否可用
     * @details 账号是否为可使用状态，即注册上状态
     * @return 账号是否可用
     */
    virtual bool IsAccountUsable() const;

    /**
     * @brief 判断账号是否注册失败
     */
    bool CBaseAccount::IsRegisteFailed() const;

    /**
     * @brief 从配置文件中加载账号信息
     *
     * @param lpszAccountPath 配置文件绝对路径
     * @param iAccountId 账号ID
     *
     * @return 加载配置是否成功
     */
    virtual bool LoadAccountFromConfig(const yl::string & lpszAccountPath,
                                       int iAccountId = INVALID_ACCOUNTID) = 0;

    /**
     * @brief 打印账号信息
     */
    virtual void PrintAccoutInfo() const = 0;

    /**
     * @brief 获取账号状态
     * @return 账号状态
     */
    virtual LINE_STATE GetAccountState() const;
    /**
     * @brief 设置账号状态
     *
     * @param eLineState 账号状态
     * @param bGuiQuery 用来标识传入的state是由GUI主动向sip查询获得的
     */
    virtual void SetAccountState(LINE_STATE eLineState, bool bGuiQuery = false);
    /**
     * @brief 通知账号状态改变
     *
     * @param eOldState 原有账号状态
     * @param bGuiQuery 用来标识传入的state是由GUI主动向sip查询获得的
     */
    virtual void NotifyAccountStatusChanged(LINE_STATE eOldState, bool bGuiQuery = false) const;

    /**
     * @brief 获取自动应答延迟时间
     * @return 自动应答延迟时间(单位ms)
     */
    virtual int GetAutoAnswerTime() const = 0;

    /**
    * @brief 获取自动应答开关状态
    * @return 自动应答开关状态
    */
    virtual bool GetAutoAnswerSwitch() const = 0;

    /**
    * @brief 设置自动应答开关状态
    * @param 开关状态
    */
    virtual bool SetAutoAnswerSwitch(bool bEnable, CONFIG_LEVEL_TYPE eType) const = 0;

    /**
     * @brief 获取默认呼叫方式
     * @return 默认呼叫方式
     */
    virtual CALL_AV_TYPE GetDefaultCallMode() const = 0;

    /**
     * @brief 设置自定义显示内容
     *
     * @param strShow 自定义的显示内容
     * @return 设置是否成功
     */
    virtual bool SetCustomShowText(const yl::string & strShow);
    /**
     * @brief 获取自定义显示内容
     * @return 自定义显示内容
     */
    virtual yl::string GetCustomShowText() const;

    /**
    * @brief  判断是否需要注册
    * @return 是否需要注册
    */
    virtual bool NeedRegister();

    /**
    * @brief  判断是否需要注册
    * @return 是否需要注册
    */
    void SetRegisterReason(account_register_reason_t & sReason);

    /**
    * @brief  账号状态变更处理
    * @return 处理结果
    */
    virtual bool LineStateChange(LINE_STATE eState,
                                 account_register_reason_t & sReason,
                                 bool bGuiQuery/*=false*/);

    /**
    * @brief  获取显示名
    * @return 显示名称
    */
    virtual yl::string GetDisplayName() const
    {
        return "";
    };

    /**
    * @brief  获取注册名
    * @return 注册名
    */
    virtual yl::string GetRegisterName() const
    {
        return "";
    };

    /**
    * @brief  获取用户名
    * @return 用户名
    */
    virtual yl::string GetUserName() const
    {
        return "";
    };

    /**
    * @brief  获取服务器名
    * @return 服务器名
    */
    virtual yl::string GetServerName() const
    {
        return "";
    }

    /**
    * @brief  获取账号所属协议类型
    * @return 协议类型
    */
    AccountProtocolType GetProtocol() const
    {
        return m_eProtocol;
    }

    /**
    * @brief  获取注册失败Id
    * @return 失败Id
    */
    int GetRegisterFailId() const
    {
        return m_iFailId;
    }

    /**
    * @brief  获取注册失败信息
    * @return 失败信息
    */
    yl::string GetRegisterFailInfo() const
    {
        return m_strFailInfo;
    }

protected:
    int                     m_iAccountId;           ///< 账号ID
    ACCOUNT_TYPE            m_eAccountType;         ///< 账号类型
    AccountProtocolType     m_eProtocol;            ///< 账号所属协议
    LINE_STATE              m_eAccountState;        ///< 账号状态
    yl::string              m_strConfigPath;        ///< 账号配置文件路径
    yl::string              m_strCustomShowText;    ///< 自定义显示文本

    int                     m_iFailId;              //注册失败的类型
    yl::string              m_strFailInfo;          //注册失败的信息
};

#endif // __BASE_ACCOUNT_H__
