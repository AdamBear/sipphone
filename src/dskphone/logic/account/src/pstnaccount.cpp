#include "pstnaccount.h"

#include "modaccount.h"

#ifdef IF_FEATURE_PSTN

CPstnAccount::CPstnAccount(int iDeviceId)
    : CBaseAccount(ACCOUNT_TYPE_PSTN, PROTOCOL_PSTN)
    , m_iDeviceId(iDeviceId)
{
}

CPstnAccount::~CPstnAccount()
{
}

yl::string CPstnAccount::GetCustomShowText() const
{
    // "custom(PSTNn)" or "label(PSTNn)" or "PSTNn"
    yl::string strCustomShowText(m_strCustomShowText);
    if (strCustomShowText.empty())
    {
        strCustomShowText = acc_GetAccountLabel(m_iAccountId);
    }

    char szAccountKey[32] = {0};
    snprintf(szAccountKey, sizeof(szAccountKey), "PSTN%d", m_iAccountId - PSTN_LINE_BEGIN_ID + 1);

    if (strCustomShowText.empty())
    {
        strCustomShowText = szAccountKey;
    }
    else
    {
        strCustomShowText.append("(");
        strCustomShowText.append(szAccountKey);
        strCustomShowText.append(")");
    }
    return strCustomShowText;
}

bool CPstnAccount::LoadAccountFromConfig(const yl::string & lpszAccountPath, int iAccountId)
{
    if (iAccountId < PSTN_LINE_BEGIN_ID || iAccountId > PSTN_LINE_END_ID)
    {
        ACCOUNT_WARN("Invalid PSTN account ID [%d]!", iAccountId);
        return false;
    }

    // 赋值账号关联配置文件
    m_strConfigPath = lpszAccountPath;
    // 赋值账号Id
    m_iAccountId = iAccountId;
    bool bEnable = acc_IsAccountEnable(iAccountId);
    // 启用pstnadapter对应设备
    pstnadapter_SetEnable(m_iDeviceId, bEnable);
    // 设置状态
    m_eAccountState = bEnable ? LS_REGISTERED : LS_DISABLE;
    return true;
}

bool CPstnAccount::IsAccountUsable() const
{
    // 该账号已注册上或者不需要注册，则可用，否则不可用
    bool bRet = m_eAccountState == LS_REGISTERED && pstnadapter_IsAvailable(m_iDeviceId);
    ACCOUNT_INFO("PSTN account status is: %d,%s usable",
                 m_eAccountState, (bRet ? "" : " not"));
    return bRet;
}

int CPstnAccount::GetAutoAnswerTime() const
{
    int nAutoAnswerTime = 0;
    if (GetAutoAnswerSwitch())
    {
        nAutoAnswerTime = 200;
    }
    return nAutoAnswerTime;
}

CALL_AV_TYPE CPstnAccount::GetDefaultCallMode() const
{
    return CAVT_PSTN;
}

int CPstnAccount::GetDeviceId() const
{
    return m_iDeviceId;
}

void CPstnAccount::PrintAccoutInfo() const
{
    ACCOUNT_INFO("$$$$$$$$PrintPstnAccoutInfo$$$$$$$$");
    ACCOUNT_INFO("Account ID[%d] Device ID[%d]", m_iAccountId, m_iDeviceId);
    ACCOUNT_INFO("Account State[%d]", m_eAccountState);
    ACCOUNT_INFO("Auto Answer[%d], Auto Answer Time[%d]", acc_IsAccountAutoAnswerEnable(m_iAccountId),
                 GetAutoAnswerTime());
    ACCOUNT_INFO("Account Ring File[%s]", acc_GetAccountRing(m_iAccountId).c_str());
    ACCOUNT_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
}

void CPstnAccount::NotifyAccountStatusChanged(LINE_STATE eOldState, bool bGuiQuery) const
{
    pstnadapter_SetEnable(m_iDeviceId, m_eAccountState == LS_REGISTERED);
    CBaseAccount::NotifyAccountStatusChanged(eOldState, bGuiQuery);
}

bool CPstnAccount::GetAutoAnswerSwitch() const
{
    return configParser_GetCfgItemIntValue(kszPSTNAccountAutoAnswer,
                                           m_iAccountId - PSTN_LINE_BEGIN_ID) ;
}

bool CPstnAccount::SetAutoAnswerSwitch(bool bEnable, CONFIG_LEVEL_TYPE eType) const
{
    return configParser_SetCfgItemIntValue(kszPSTNAccountAutoAnswer, m_iAccountId - PSTN_LINE_BEGIN_ID,
                                           bEnable, eType);
}


#endif // IF_FEATURE_PSTN
