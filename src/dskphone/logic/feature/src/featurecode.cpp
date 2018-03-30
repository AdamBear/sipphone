#include "feature_inc.h"
#include "voip/modvoip.h"
#include "featurecodefwddndconfig.h"
#if IF_BUG_30062
#include "extended/actionuri_url/include/action_def.h"
#endif
#include "talk/dialplan/include/moddialplan.h"

//////////////////////////////////////////////////////////////////////////
// class CCodeInfo
// Write
bool CCodeInfo::SetOnCode(const char * pOnCode)
{
    if (NULL == pOnCode
            || m_strOnCode == pOnCode)
    {
        return false;
    }
    m_strOnCode = pOnCode;
    m_bSendOn = true;
    return true;
}

bool CCodeInfo::SetOffCode(const char * pOffCode)
{
    if (NULL == pOffCode
            || m_strOffCode == pOffCode)
    {
        return false;
    }
    m_strOffCode = pOffCode;
    m_bSendOff = true;
    return true;
}

// Read
const char * CCodeInfo::GetOnCode() const
{
    return m_strOnCode.c_str();
}

const char * CCodeInfo::GetOffCode() const
{
    return m_strOffCode.c_str();
}

const char * CCodeInfo::GetSendOffCode() const
{
    if (!m_strOffCode.empty())
    {
        return m_strOffCode.c_str();
    }
    // 如果offcode有备份，则获取
    if (!m_strLastOffCode.empty())
    {
        return m_strLastOffCode.c_str();
    }
    return "";
}

// Status
// Get
bool CCodeInfo::NeedSendOn()
{
    return (m_bSendOn && strlen(GetOnCode()) > 0);
}

bool CCodeInfo::NeedSendOff()
{
    return (m_bSendOff && strlen(GetOffCode()) > 0);
}

// Set
void CCodeInfo::SetSendFlag(BOOL bOn)
{
    if (bOn)
    {
        m_bSendOn = true;
        m_bSendOff = true;
    }
    else
    {
        m_bSendOff = true;
    }
}

void CCodeInfo::CleanSendFlag()
{
    m_bSendOn = false;
    m_bSendOff = false;
}

CodeSendFlag::CodeSendFlag()
{
    iCodeType = Unknown;
    for (int i = 0; i < acc_AccountNum(); ++i)
    {
        arrSendFlag.push_back(false);
    }
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_GETINSTANCE(CFeatureCode)

// 将对象地址作为定时器ID
#define INIT_TIMER 15*1000

// 构造函数
CFeatureCode::CFeatureCode()
{
    AddInitSendType(CodeSendFlag::FWD);
    AddInitSendType(CodeSendFlag::DND);
    AddInitSendType(CodeSendFlag::Anonymous);

    m_uInitTimer = INIT_TIMER;

    timerSetThreadTimer(TIMER_ID(m_uInitTimer), m_uInitTimer);
}

CFeatureCode::~CFeatureCode()
{
    ListSendCodeCallID::iterator iter = m_pListCodeCallID.begin();
    for (; iter != m_pListCodeCallID.end(); ++iter)
    {
        if ((*iter) == NULL)
        {
            continue;
        }
        delete (*iter);
        (*iter) = NULL;
    }
    m_pListCodeCallID.clear();

    // Release
    ListCodeSendFlag::iterator iter2 = m_listInitSendFlag.begin();
    for (; iter2 != m_listInitSendFlag.end(); ++iter2)
    {
        if ((*iter2) == NULL)
        {
            continue;
        }
        delete (*iter2);
        (*iter2) = NULL;
    }
    m_listInitSendFlag.clear();

    timerKillThreadTimer(TIMER_ID(m_uInitTimer));
}

// 发送开启特征码
BOOL CFeatureCode::SendOnCode(int iLineId, CCodeInfo & refCode)
{
    if (refCode.NeedSendOn())
    {
        COMMONUNITS_INFO("SendOnCode: %s", refCode.GetOnCode());

        if (SendCodeToServer(iLineId, refCode.GetOnCode()))
        {
            // 发送OnCode时要备份OffCode，以避免配置直接改掉时对应的OffCode丢失
            refCode.OnSendOnCode();
            return TRUE;
        }
    }
    return FALSE;
}
// 发送关闭的特征码
BOOL CFeatureCode::SendOffCode(int iLineId, CCodeInfo & refCode)
{
    if (refCode.NeedSendOff())
    {
        COMMONUNITS_INFO("FeatureCode SendOffCode: %s", refCode.GetSendOffCode());
        if (SendCodeToServer(iLineId, refCode.GetSendOffCode()))
        {
            refCode.OnSendOffCode();
            return TRUE;
        }
    }
    return FALSE;
}

//
BOOL CFeatureCode::SendCodeToServer(int iLineId, const yl::string & strCode)
{
    if (strCode.empty())
    {
        return FALSE;
    }
    if (iLineId < -1 || iLineId >= acc_AccountNum())
    {
        return FALSE;
    }
    // 活动账号
    if (iLineId == -1)
    {
        iLineId = acc_GetDefaultAccount();
    }

    if (acc_GetState(iLineId) == LS_REGISTERED)
    {
        UINT uCallSessionID = modVoip_NewCallId();
        Code_CallInfo * pCodeCallInfo = new Code_CallInfo;
        if (pCodeCallInfo != NULL)
        {
            pCodeCallInfo->iLineId = iLineId;
            pCodeCallInfo->uCallId = uCallSessionID;
            m_pListCodeCallID.push_back(pCodeCallInfo);
        }
#if IF_BUG_30062
        yl::string strNewCode = strCode;
        if (1 == configParser_GetConfigInt(kszEnableUseDialPlanInCode))
        {
            dialplan_GetReplaceNumber(strNewCode, iLineId);
        }

        COMMONUNITS_INFO("Send Code:%s ", strNewCode.c_str());

        modVoip_CallInvite(iLineId, uCallSessionID, strNewCode.c_str());
#else
        COMMONUNITS_INFO("Send Code:%s", strCode.c_str());

        modVoip_CallInvite(iLineId, uCallSessionID, strCode.c_str());
#endif
        return TRUE;
    }
    return FALSE;
}

// 是否跳过某类型的发送
bool CFeatureCode::IsSkipCodeSend(int iType, int iLineId)
{
    static int i = 0;
    bool bSkip = false;

    if (m_uInitTimer == -1)
    {
        // 允许的初始化时间到
        return false;
    }

    ListCodeSendFlag::iterator it = m_listInitSendFlag.begin();
    for (; it != m_listInitSendFlag.end(); ++it)
    {
        if ((*it) == NULL
                || (*it)->iCodeType != iType)
        {
            continue;
        }

        if (iLineId == -1)
        {
            // 所有账号
            for (int j = 0; j < acc_AccountNum(); j++)
            {
                if (!(*it)->arrSendFlag[j])
                {
                    bSkip = true;
                    (*it)->arrSendFlag[j] = true;
                    j++;
                }
            }
            break;
        }
        if (iLineId < 0 || iLineId >= acc_AccountNum())
        {
            break;
        }
        // 指定账号
        if (!(*it)->arrSendFlag[iLineId])
        {
            bSkip = true;
            (*it)->arrSendFlag[iLineId] = true;
            i++;
        }
        break;
    }
    return bSkip;
}

BOOL CFeatureCode::SetCodeCallTimer(msgObject & objMessage)
{
    ListSendCodeCallID::iterator it = m_pListCodeCallID.begin();
    for (; it != m_pListCodeCallID.end(); ++it)
    {
        Code_CallInfo * pCodeCallInfo = (Code_CallInfo *)((*it));
        if (pCodeCallInfo != NULL)
        {
            if (objMessage.lParam == (WPARAM)pCodeCallInfo->uCallId)
            {
                //Sistek Entel T20/T26 DND/Forward与华为服务器同步不成功
                //http://10.2.1.199/Bug.php?BugID=14377
                int iDelay = 10000;
                if (objMessage.message == SIP_CALL_RINGBACK)
                {
                    iDelay = 1500;
                }

                COMMONUNITS_INFO("SetCodeCallTimer:iDelay[%d], CallID:[%d]", iDelay, objMessage.lParam);
                timerSetThreadTimer((UINT)pCodeCallInfo, iDelay);

                return TRUE;
            }
        }
    }

    return FALSE;
}

void CFeatureCode::AddInitSendType(int iType)
{
    if (iType == CodeSendFlag::Unknown)
    {
        return;
    }
    CodeSendFlag * pNew = new CodeSendFlag;
    if (pNew == NULL)
    {
        return;
    }
    pNew->iCodeType = iType;
    m_listInitSendFlag.push_back(pNew);
}

BOOL CFeatureCode::OnTimer(UINT uTimerID)
{
    if (uTimerID == TIMER_ID(m_uInitTimer))
    {
        timerKillThreadTimer(TIMER_ID(m_uInitTimer));

        m_uInitTimer = -1;
        return TRUE;
    }

    ListSendCodeCallID::iterator it = m_pListCodeCallID.begin();
    for (; it != m_pListCodeCallID.end(); ++it)
    {
        if ((*it) == NULL)
        {
            continue;
        }
        Code_CallInfo * pCodeCallInfo = (Code_CallInfo *)((*it));
        if (uTimerID == (UINT)pCodeCallInfo)
        {
            timerKillThreadTimer((UINT)pCodeCallInfo);
            modVoip_HangupCall(pCodeCallInfo->iLineId, pCodeCallInfo->uCallId);

            delete pCodeCallInfo;
            pCodeCallInfo = NULL;

            m_pListCodeCallID.erase(it);
            return TRUE;
        }
    }

    return FALSE;
}

BOOL CFeatureCode::ProcessSIPMsg(msgObject & objMessage)
{
    return SetCodeCallTimer(objMessage);
}

BOOL CFeatureCode::ProcessDNDMsg(int iAccountId, bool bEnable)
{
    yl::string strCode = CFeatureCodeFwdDndConfig::GetDndCode(iAccountId, bEnable);
    if (!strCode.empty())
    {
        COMMONUNITS_INFO("SendDNDCode: %s", strCode.c_str());
        return SendCodeToServer(iAccountId, strCode);
    }
    return FALSE;
}

BOOL CFeatureCode::ProcessFWDMsg(int iAccountId, FWD_TYPE eType, bool bEnable, yl::string strTarget)
{
    yl::string strCode = CFeatureCodeFwdDndConfig::GetFwdCode(iAccountId, eType, bEnable);
    if (!strCode.empty())
    {
        strCode += strTarget;
        COMMONUNITS_INFO("SendForwardCode: %s", strCode.c_str());
        return SendCodeToServer(iAccountId, strCode);
    }
    return FALSE;
}

