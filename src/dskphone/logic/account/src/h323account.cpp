#ifdef IF_FEATURE_H323
#include "h323account.h"
#include "modaccount.h"

CH323Account::CH323Account()
    : CBaseAccount(ACCOUNT_TYPE_H323, PROTOCOL_H323)
{
#ifdef IF_SUPPORT_VIDEO
    m_bSupportH264 = false;
#endif //#ifdef IF_SUPPORT_VIDEO
}

CH323Account::~CH323Account()
{
}

bool CH323Account::LoadAccountFromConfig(const yl::string & lpszAccountPath,
        int iAccountId/* = INVALID_ACCOUNTID*/)
{
    ACCOUNT_INFO("load h323 account[%d]", iAccountId);

    m_iAccountId = iAccountId;

    //m_H323Protocol.OpenKey("/config/user/user.ini", "H323Protocol", "bEnable");

    // 和user name、display name等一样,不需要成员变量
    //if (m_iAccountId == H323ACCOUNTID_IP)
    //{
    //  m_strServerName = "127.0.0.1";
    //}
    //else
    //{
    //  // 注意：configParser 会默认+1 ----->> v81读取的直接是传入的index
    //  m_strServerName = configParser_GetCfgItemStringValue(kszAccountH323Server, 0);
    //}

    if (!IsSpecialAccount())
    {
        m_eAccountState = (configParser_GetConfigInt(kszAccountH323Enable) != 0) ? LS_BE_UNREGISTERED :
                          LS_DISABLE;
    }
    else
    {
        m_eAccountState = LS_REGISTERED;
    }

    // kszH323Switch 暂时保留：需求不要求，但是可能后续有需要
    if (configParser_GetConfigInt(kszH323ProtocolSwitch) == 0
            /*|| configParser_GetConfigInt(kszH323Switch) == 0*/
            || configParser_GetConfigInt(kszVCAccountActive) == 0
            || configParser_GetConfigInt(kszVCAccountEnable) == 0)
    {
        m_eAccountState = LS_DISABLE;
    }

#ifdef IF_SUPPORT_VIDEO
    LoadCodecInfo();
#endif //#ifdef IF_SUPPORT_VIDEO

    return true;
}

#ifdef IF_SUPPORT_VIDEO
void CH323Account::LoadCodecInfo()
{
    for (int i = 0; i < 3; i++)
    {
        yl::string strCodecType = configParser_GetCfgItemStringValue(kszAccountH323CodecType, i);
        if (strCodecType.compare("H264") == 0
                && configParser_GetCfgItemIntValue(kszAccountH323CodecEnable, i) == 1)
        {
            m_bSupportH264 = true;
        }
    }
}
#endif //#ifdef IF_SUPPORT_VIDEO

// 获取注册名
yl::string CH323Account::GetRegisterName() const
{
    return m_iAccountId == H323ACCOUNTID_IP ? "SIP Phone" : configParser_GetConfigString(
               kszAccountH323GkUserName);
}

//获取显示名
yl::string CH323Account::GetDisplayName() const
{
    return m_iAccountId == H323ACCOUNTID_IP ? "SIP Phone" : configParser_GetConfigString(
               kszAccountH323DisplayName);
}

//获取用户名
yl::string CH323Account::GetUserName() const
{
    return m_iAccountId == H323ACCOUNTID_IP ? "SIP Phone" : configParser_GetConfigString(
               kszAccountH323Extension);
}

yl::string CH323Account::GetServerName() const
{
    // 主备服务器地址
    return m_iAccountId == H323ACCOUNTID_IP ? "127.0.0.1" : configParser_GetCfgItemStringValue(
               kszAccountH323Server, 1);
}

//是否帐号可用
bool CH323Account::IsAccountEnable()
{
    if (configParser_GetConfigInt(kszH323ProtocolSwitch) == 0
            /*|| configParser_GetConfigInt(kszH323Switch) == 0*/
            || configParser_GetConfigInt(kszVCAccountActive) == 0
            || configParser_GetConfigInt(kszVCAccountEnable) == 0)
    {
        return false;
    }

    if (m_iAccountId == H323ACCOUNTID)
    {
        return (configParser_GetConfigInt(kszAccountH323Enable) != 0);
    }

    return true;
}

bool CH323Account::LineStateChange(LINE_STATE eState, account_register_reason_t & sReason,
                                   bool bGuiQuery/*=false*/)
{
    if (!IsAccountEnable())
    {
        TALK_WARN("h323 account disable");
        //LogoutBeforeReset不生效
        if (eState == LS_BE_UNREGISTERED)
        {
            SetAccountState(LS_DISABLE, bGuiQuery);
        }

        SetRegisterReason(sReason);
        //帐号未启用，不处理注册状态消息
        return false;
    }

    //将Line状态重新设置
    SetAccountState(eState, bGuiQuery);

    SetRegisterReason(sReason);

    return true;
}

int CH323Account::GetAutoAnswerTime() const
{
    int nAutoAnswerTime = 0;
    if (acc_IsAccountAutoAnswerEnable(m_iAccountId))
    {
        nAutoAnswerTime = configParser_GetCfgItemIntValue(kszAutoAnswerTimeout, m_iAccountId);
        //由秒转换为毫秒
        nAutoAnswerTime = chMAX(200, nAutoAnswerTime * 1000);
    }
    return nAutoAnswerTime;
}

CALL_AV_TYPE CH323Account::GetDefaultCallMode() const
{
    CALL_AV_TYPE eDefCallMode = (CALL_AV_TYPE)configParser_GetCfgItemIntValue(
                                    kszAccountCallMode, m_iAccountId);
    if (eDefCallMode == CAVT_UNKNOWN)
    {
        eDefCallMode = CAVT_VIDEO;
    }
    return eDefCallMode;
}

bool CH323Account::GetAutoAnswerSwitch() const
{
    return configParser_GetConfigInt(kszAccountH323AutoAnswerSwitch) != 0 ;
}

bool CH323Account::SetAutoAnswerSwitch(bool bEnable, CONFIG_LEVEL_TYPE eType) const
{
    if (m_iAccountId == H323ACCOUNTID_IP)
    {
        // H323 ip直播是虚拟的账号；
        return false;
    }
    else if (m_iAccountId == H323ACCOUNTID)
    {
        return configParser_SetConfigInt(kszAccountH323AutoAnswerSwitch, bEnable, eType);
    }

    return false;
}


void CH323Account::PrintAccoutInfo() const
{
    ACCOUNT_INFO("$$$$$$$$PrintAccoutInfo$$$$$$$$");
    ACCOUNT_INFO("Account ID[%d]", m_iAccountId);
    ACCOUNT_INFO("Account State[%d]", m_eAccountState);
    ACCOUNT_INFO("DTMF Type[%d]", configParser_GetConfigInt(kszVoipUserCapEnable));
    ACCOUNT_INFO("Account Name[%s]", GetUserName().c_str());
    ACCOUNT_INFO("Server Name[%s]", GetServerName().c_str());
    ACCOUNT_INFO("Display Name[%s]", GetDisplayName().c_str());
    ACCOUNT_INFO("Fail Id[%d], Fail Info[%s]", m_iFailId, m_strFailInfo.c_str());
    ACCOUNT_INFO("Is Direct IP[%d]", IsDirectIPAccount());
    //ACCOUNT_INFO("Auto Answer[%d], Auto Answer Time[%d]", IsAutoAnswerEnable(), GetAutoAnswerTime());
    //ACCOUNT_INFO("Account Ring File[%s]", configParser_GetCfgItemStringValue(kszAccountRing, m_iAccountId).c_str());
    ACCOUNT_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
}
#endif //#ifdef IF_FEATURE_H323
