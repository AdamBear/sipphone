#ifndef __PSTN_ACCOUNT_H__
#define __PSTN_ACCOUNT_H__

#include "baseaccount.h"

#ifdef IF_FEATURE_PSTN

class CPstnAccount : public CBaseAccount
{
public:
    CPstnAccount(int iDeviceId);
    virtual ~CPstnAccount();

    /**
     * @brief 获取自定义显示内容
     * @details 根据PSTN账号标签获取自定义内容。若标签非空，则为“label(PSTNn)”；
     * 若为空，则为“PSTNn”。其中n为PSTN账号序号，从1开始编号。
     *
     * @return 自定义显示内容
     */
    virtual yl::string GetCustomShowText() const;

    /**
     * @brief 从配置文件中加载账号信息
     *
     * @param lpszAccountPath 配置文件绝对路径
     * @param iAccountId 账号ID
     *
     * @return 加载配置是否成功
     */
    virtual bool LoadAccountFromConfig(const yl::string & lpszAccountPath,
                                       int iAccountId = INVALID_ACCOUNTID);

    /**
     * @brief 判断账号是否可用
     * @details 账号是否为可使用状态，即注册上状态
     * @return 账号是否可用
     */
    virtual bool IsAccountUsable() const;

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

    /**
     * @brief 获取PSTN Box设备ID
     * @return PSTN Box设备ID
     */
    virtual int GetDeviceId() const;

    /**
     * @brief 打印账号信息
     */
    virtual void PrintAccoutInfo() const;

    /**
     * @brief 通知账号状态改变
     *
     * @param eOldState 原有账号状态
     * @param bGuiQuery 用来标识传入的state是由GUI主动向sip查询获得的
     */
    virtual void NotifyAccountStatusChanged(LINE_STATE eOldState, bool bGuiQuery = false) const;

    /**
    * @brief 获取自动应答开关状态
    * @return 自动应答开关状态
    */
    virtual bool GetAutoAnswerSwitch() const;

    /**
    * @brief 设置自动应答开关状态
    * @param 开关状态
    */
    virtual bool SetAutoAnswerSwitch(bool bEnable, CONFIG_LEVEL_TYPE eType) const;

protected:
    const int m_iDeviceId; /// 对应PSTN Box设备ID
};

#endif // IF_FEATURE_PSTN

#endif // __PSTN_ACCOUNT_H__
