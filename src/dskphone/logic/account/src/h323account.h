#ifdef IF_FEATURE_H323
#ifndef _H323_ACCOUNT_H_
#define _H323_ACCOUNT_H_

#include "../include/baseaccount.h"

class CH323Account : public CBaseAccount
{
public:
    CH323Account();
    ~CH323Account();

    // 从配置文件中加载账号信息
    virtual bool LoadAccountFromConfig(const yl::string & lpszAccountPath,
                                       int iAccountId = INVALID_ACCOUNTID);

#ifdef IF_SUPPORT_VIDEO
    // 加载codec
    void LoadCodecInfo();
#endif //#ifdef IF_SUPPORT_VIDEO

    // 获取注册名
    virtual yl::string GetRegisterName() const;
    // 获取显示名
    virtual yl::string GetDisplayName() const;
    // 获取用户名
    virtual yl::string GetUserName() const;
    // 获取服务器名
    virtual yl::string GetServerName() const;
    // 是否帐号可用
    bool IsAccountEnable();

    virtual bool LineStateChange(LINE_STATE eState,
                                 account_register_reason_t & sReason,
                                 bool bGuiQuery/*=false*/);

    // 获取autoanswer时间
    virtual int GetAutoAnswerTime() const;

    /**
    * @brief 获取自动应答开关状态
    * @return 自动应答开关状态
    */
    virtual bool GetAutoAnswerSwitch()const;

    /**
    * @brief 设置自动应答开关状态
    * @param 开关状态
    */
    virtual bool SetAutoAnswerSwitch(bool bEnable, CONFIG_LEVEL_TYPE eType) const;

    // 获取默认呼叫方式
    virtual CALL_AV_TYPE GetDefaultCallMode() const;

#ifdef IF_SUPPORT_VIDEO
    // 是否支持H264
    bool IsSupportH264()
    {
        return m_bSupportH264;
    }
#endif //#ifdef IF_SUPPORT_VIDEO

    // 打印账号信息
    virtual void PrintAccoutInfo() const;

private:
    bool m_bSupportH264;
};


#endif //_H323_ACCOUNT_H_
#endif //#ifdef IF_FEATURE_H323
