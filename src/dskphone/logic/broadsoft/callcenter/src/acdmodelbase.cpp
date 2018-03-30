#include "callcenter_inc.h"

#if defined(IF_FEATURE_ACD) || defined(IF_FEATURE_BROADSOFT_CALL_CENTER) || defined(IF_FEATURE_GENESYS_ACD) || IF_FEATURE_METASWITCH_ACD

CACDModelBase::CACDModelBase(void)
{
    m_eACDType = ACD_BASE;
    m_nAccountId = -1;
    m_eACDLineState = AS_IDLE;
    m_strUserName = "";
    m_strPassword = "";
}

CACDModelBase::~CACDModelBase(void)
{
    dsskey_SetDsskeyStatusByType(DT_ACD, DS_ACD_IDLE);
}

// 消息处理
LRESULT CACDModelBase::OnMessage(msgObject & msg)
{
    switch (msg.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (ST_ACD == msg.wParam)
            {
                return ProcessAutoAvailble(GetAcdStatus());
            }
        }
        break;
    default:
        break;
    }

    return 1;
}

void CACDModelBase::SetStatus(ACD_STATUS eStatus)
{
    if (eStatus == AS_WRAPUP && m_eACDType != ACD_BROADSOFT)
    {
        eStatus = AS_IDLE;
    }

    m_eACDLineState = eStatus;

    if (m_eACDType != ACD_BROADSOFT)
    {
        configParser_SetCfgItemIntValue(kszACDStatus, GetAccountId(), (int)m_eACDLineState);
    }

    // ACD注册成功后需要更新BSACD的信息:账号和状态
    //to do -syl
    //m_iLineID = ACD_GetAccountId();
    switch (m_eACDLineState)
    {
    case AS_IDLE:
        {
            dsskey_SetDsskeyStatusByType(DT_ACD, DS_ACD_IDLE);
        }
        break;
#if IF_BUG_24860
    case AS_AVAILABLE:
    case AS_UNAVAILABLE:
    case AS_WRAPUP:
        {
            dsskey_SetDsskeyStatusByType(DT_ACD, DS_ACD_AVAILABLE);
        }
        break;
#else
    case AS_AVAILABLE:
        {
            dsskey_SetDsskeyStatusByType(DT_ACD, DS_ACD_AVAILABLE);
        }
        break;
    case AS_UNAVAILABLE:
        {
            dsskey_SetDsskeyStatusByType(DT_ACD, DS_ACD_UNAVAILABLE);
        }
        break;
    case AS_WRAPUP:
        {
            dsskey_SetDsskeyStatusByType(DT_ACD, DS_ACD_WRAPUP);
        }
        break;
#endif
    default:
        break;
    }
#ifdef IF_FEATURE_ACD
    ProcessAutoAvailble(eStatus);
#endif //IF_FEATURE_ACD

}

BOOL CACDModelBase::OnAutoAvailTimer(msgObject & refObj)
{
    m_objTimer.KillTimer();

    return ReqChangeACDStatus(AA_AVAIL);
}

// 处理定时设置状态为可用
bool CACDModelBase::ProcessAutoAvailble(ACD_STATUS eStatus)
{
    m_objTimer.KillTimer();

    if (AS_UNAVAILABLE == eStatus
            && 0 != configParser_GetConfigInt(kszEnableACDAutoAvailable))
    {
        // 启动计时器.
        int iTimer = configParser_GetConfigInt(kszACDAutoAvailableTime) * 1000;
        m_objTimer.SetTimer(iTimer, NULL, MK_MSG_ENTRY(this, CACDModelBase::OnAutoAvailTimer));
    }

    //通知更新dsskey
    etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_DSSKEY_LENGHT, 0);

    return true;
}

bool CACDModelBase::GetReasonCode(DispCodeList & listData)
{
    yl::string strCode, strCodeName;
    for (int i = 1; i <= 100; ++i)
    {
        strCode = configParser_GetCfgItemStringValue(kszAccountACDReasonCode, m_nAccountId, i);
        strCodeName = configParser_GetCfgItemStringValue(kszAccountACDReasonCodeName, m_nAccountId, i);

        if (!strCode.empty() && !strCodeName.empty())
        {
            listData.push_back(DispCode(strCode, strCodeName));
        }
    }

    return listData.size() > 0;
}

#if IF_FEATURE_METASWITCH_ACD
void CACDModelBase::GetCurReason(yl::string & strReasonName)
{

}

void CACDModelBase::SetCurReason(const yl::string & strReason)
{

}
#endif

bool CACDModelBase::CallOut(const yl::string & strNumber, const yl::string & strName,
                            int iAccountId /*= AUTO_SELECT_LINE_ID*/, CallOut_Param * pCallParam /*= NULL*/)
{
    CallOut_Param localParam;
    if (pCallParam == NULL)
    {
        pCallParam = &localParam;
    }

    pCallParam->eCallOutType = TCT_CALL_CENTER;
    return talklogic_CallOut(strNumber, strName, iAccountId, pCallParam);
}

#endif
