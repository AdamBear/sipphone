#include "baseaccount.h"

CBaseAccount::CBaseAccount(ACCOUNT_TYPE eAccountType, AccountProtocolType eProtocol) :
    m_iAccountId(INVALID_ACCOUNTID),
    m_eAccountType(eAccountType),
    m_eProtocol(eProtocol),
    m_eAccountState(LS_DISABLE),
    m_iFailId(0),
    m_strFailInfo("")
{
}

CBaseAccount::~CBaseAccount()
{
}

bool CBaseAccount::Compare(const CBaseAccount * lhs, const CBaseAccount * rhs)
{
    return lhs->m_iAccountId < rhs->m_iAccountId;
}

int CBaseAccount::GetAccountId() const
{
    return m_iAccountId;
}

ACCOUNT_TYPE CBaseAccount::GetAccountType() const
{
    return m_eAccountType;
}

bool CBaseAccount::IsDirectIPAccount() const
{
    return m_iAccountId == H323ACCOUNTID_IP ?
           true : configParser_GetCfgItemIntValue(kszAccountDirectIP, m_iAccountId);
}

bool CBaseAccount::IsSpecialAccount() const
{
    return IsDirectIPAccount()
#ifdef IF_FEATURE_PSTN
           || m_eAccountType == ACCOUNT_TYPE_PSTN
#endif // IF_FEATURE_PSTN
           ;
}

bool CBaseAccount::IsRegistered() const
{
    // 该账号已注册上或者不需要注册
    return m_eAccountState == LS_REGISTERED || m_eAccountState == LS_DISABLE_REG;
}

bool CBaseAccount::IsAccountUsable() const
{
    // 该账号已注册上或者不需要注册，则可用，否则不可用
    return m_eAccountState == LS_REGISTERED || m_eAccountState == LS_DISABLE_REG;
}

bool CBaseAccount::IsRegisteFailed() const
{
    return m_eAccountState == LS_REGISTER_FAIL;
}

LINE_STATE CBaseAccount::GetAccountState() const
{
    return m_eAccountState;
}

void CBaseAccount::SetAccountState(LINE_STATE eLineState, bool bGuiQuery)
{
    if (m_eAccountState != eLineState)
    {
        LINE_STATE eOldState = m_eAccountState;
        m_eAccountState = eLineState;
        NotifyAccountStatusChanged(eOldState, bGuiQuery);
    }
}

void CBaseAccount::NotifyAccountStatusChanged(LINE_STATE eOldState, bool bGuiQuery) const
{
    if (!IsDirectIPAccount())
    {
        // 此处需要同步通知账号内容改变
        ACCOUNT_INFO("EtlMsgHandler_NotifyApp ACCOUNT_STATUS_CHANGED(%d, %d, %d, %d)!",
                     m_iAccountId, (int)eOldState, (int)m_eAccountState, bGuiQuery);
        etl_NotifyAppEx(true, ACCOUNT_STATUS_CHANGED, m_iAccountId, eOldState, sizeof(bool), &bGuiQuery);
    }
}

bool CBaseAccount::SetCustomShowText(const yl::string & strShow)
{
    m_strCustomShowText = strShow;
    return true;
}

yl::string CBaseAccount::GetCustomShowText() const
{
    return m_strCustomShowText;
}

bool CBaseAccount::NeedRegister()
{
#ifdef IF_FEATURE_PSTN
    if (ACCOUNT_TYPE_PSTN == m_eAccountType)
    {
        return false;
    }
#endif

#ifdef IF_FEATURE_H323
    if (ACCOUNT_TYPE_H323 == m_eAccountType)
    {
        return H323ACCOUNTID_IP != m_iAccountId;
    }
#endif // IF_FEATURE_H323

    return true;
}

// 设置注册失败原因
void CBaseAccount::SetRegisterReason(account_register_reason_t & sReason)
{
    m_iFailId = sReason.m_iCode;
    m_strFailInfo = sReason.m_strReason;
}

bool CBaseAccount::LineStateChange(LINE_STATE eState,
                                   account_register_reason_t & sReason,
                                   bool bGuiQuery/*=false*/)
{
    return false;
}
