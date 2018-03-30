#include "callcenter_inc.h"

#if defined(IF_FEATURE_ACD) || defined(IF_FEATURE_GENESYS_ACD)

CACDModelGenesys::CACDModelGenesys()
{
    Set_ACDType(ACD_GENESYS);

    m_bReqChang = false;
    m_eActionWanted = AA_LOGIN;
}

CACDModelGenesys::~CACDModelGenesys()
{
}

// 消息处理
LRESULT CACDModelGenesys::OnMessage(msgObject & msg)
{
    switch (msg.message)
    {
    case TALK_MSG_TALK_START:
        {
            if (msg.lParam < 0
                    || msg.lParam != GetAccountId()
                    || 0 == configParser_GetConfigInt(kszEnableWrapup))
            {
                break;
            }

            // 通话建立时, 保存起来
            m_mapTalkInfo[msg.wParam] = msg.lParam;
        }
        break;
    case TALK_MSG_SESSION_DESTORY:
        {
            if (msg.lParam < 0
                    || msg.lParam != GetAccountId()
                    || 0 == configParser_GetConfigInt(kszEnableWrapup))
            {
                break;
            }

            // 没建立过通话, 则不处理
            YLHashMap<int, int>::iterator itr = m_mapTalkInfo.find(msg.wParam);
            if (itr == m_mapTalkInfo.end())
            {
                break;
            }

            // 清除数据
            m_mapTalkInfo.erase(itr);

            bool bEnterWarpup = true;
            for (itr = m_mapTalkInfo.begin(); itr != m_mapTalkInfo.end(); ++itr)
            {
                if (itr->second == msg.lParam)
                {
                    bEnterWarpup = false;
                    break;
                }
            }

            // 若允许进入Warpup且状态更改成功, 通知UI刷新
            if (bEnterWarpup && ReqChangeACDStatus(AA_WRAPUP))
            {
                OnACDNotify();
            }
        }
        break;
    default:
        CACDModelBase::OnMessage(msg);
        break;
    }

    return 1;
}

// 登录ACD
bool CACDModelGenesys::ReqLogin(acd_accout * pAcd)
{
    if (NULL == pAcd
            || -1 == GetAccountId())
    {
        return false;
    }

    m_strUserName = pAcd->user;
    m_strPassword = pAcd->password;

    m_bReqChang = true;
    m_eActionWanted = AA_LOGIN;
    return CACDModelCosmocom::ReqLogin(pAcd);
}

void CACDModelGenesys::OnACDNotify()
{
    CALLCENTER_INFO("CACDModelGenesys::OnACDNotify [%d]", m_bReqChang);
    if (m_bReqChang)
    {
        ACD_STATUS eStatus = Action2Status();
        SetStatus(eStatus);
        etl_NotifyApp(false, ACD_MESSAGE_STATUS_UPDATE, GetAccountId(), eStatus);
    }
}

void CACDModelGenesys::SetStatus(ACD_STATUS eStatus)
{
    m_bReqChang = false;
    m_eACDLineState = eStatus;

    //ACD_STATUS eTmpStatus = (AA_WRAPUP == eStatus ? AS_IDLE : eStatus);
    configParser_SetCfgItemIntValue(kszACDStatus, GetAccountId(), (int)m_eACDLineState);

    dsskey_SetDsskeyStatusByType(DT_ACD, ACD_GetDsskeyStatus(m_eACDLineState));

    ProcessAutoAvailble(eStatus);
}

bool CACDModelGenesys::ReqChangeACDStatus(ACD_ACTION eACDAction)
{
    bool bChangeToWrapup = (AA_WRAPUP == eACDAction);
    bool bACDStatusInIdle = (AS_IDLE == m_eACDLineState);

    if (!bChangeToWrapup)
    {
        m_bReqChang = true;
        m_eActionWanted = eACDAction;
        return CACDModelCosmocom::ReqChangeACDStatus(eACDAction);
    }
    else if (!bACDStatusInIdle)
    {
        m_bReqChang = true;
        m_eActionWanted = eACDAction;
        return true;
    }

    return false;
}

ACD_STATUS CACDModelGenesys::Action2Status()
{
    switch (m_eActionWanted)
    {
    case AA_AVAIL:
        return AS_AVAILABLE;
    case AA_UNAVAIL:
        return AS_UNAVAILABLE;
    case AA_WRAPUP:
        return AS_WRAPUP;
    default:
        break;
    }

    return AS_IDLE;
}
#endif
