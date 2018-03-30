#include "networkidlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "translateiddefine.h"
#include "service_system.h"
#include "account/include/accountmessagedefine.h"

#include "idlescreen/include/modidlescreen.h"
#include "feature/include/modfeature.h"
#include "lamp/backlight/include/modbacklight.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "idleuilogic/include/modidleul.h"
#include "account/include/modaccount.h"

NETWORK_STATUS CNetworkHandle::m_lastNetworkStatus = NS_OK;

CNetworkHandle::CNetworkHandle(int nType /*= PS_STATE_NETWORK_FAILED*/)
    : CBaseIdleHandle(nType)
{
    etl_RegisterMsgHandle(SYS_MSG_NOTIFY_PHY_STATUS, SYS_MSG_NOTIFY_TIME_CHANGED,
                          &CNetworkHandle::OnStatusMessage);
    etl_RegisterMsgHandle(SYS_MSG_OPENVPN_STATUS, SYS_MSG_OPENVPN_STATUS,
                          &CNetworkHandle::OnStatusMessage);
    etl_RegisterMsgHandle(SYS_MSG_NOTIFY_NETWORK_STATUS, SYS_MSG_NOTIFY_NETWORK_STATUS,
                          &CNetworkHandle::OnStatusMessage);
    etl_RegisterMsgHandle(ACCOUNT_PSTN_CREATED, ACCOUNT_PSTN_CREATED, &CNetworkHandle::OnStatusMessage);
    etl_RegisterMsgHandle(ACCOUNT_PSTN_REMOVED, ACCOUNT_PSTN_REMOVED, &CNetworkHandle::OnStatusMessage);
    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED,
                          &CNetworkHandle::OnStatusMessage);
    FlushNetStatus();
}

CNetworkHandle::~CNetworkHandle()
{
    etl_UnregisterMsgHandle(SYS_MSG_NOTIFY_PHY_STATUS, SYS_MSG_NOTIFY_TIME_CHANGED,
                            &CNetworkHandle::OnStatusMessage);;
    etl_UnregisterMsgHandle(SYS_MSG_OPENVPN_STATUS, SYS_MSG_OPENVPN_STATUS,
                            &CNetworkHandle::OnStatusMessage);
    etl_UnregisterMsgHandle(SYS_MSG_NOTIFY_NETWORK_STATUS, SYS_MSG_NOTIFY_NETWORK_STATUS,
                            &CNetworkHandle::OnStatusMessage);
    etl_UnregisterMsgHandle(ACCOUNT_PSTN_CREATED, ACCOUNT_PSTN_CREATED,
                            &CNetworkHandle::OnStatusMessage);
    etl_UnregisterMsgHandle(ACCOUNT_PSTN_REMOVED, ACCOUNT_PSTN_REMOVED,
                            &CNetworkHandle::OnStatusMessage);
    etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED,
                            &CNetworkHandle::OnStatusMessage);
}

bool CNetworkHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    popData.m_vecSoftkey[0] = ST_EMPTY;
    popData.m_vecSoftkey[1] = ST_EMPTY;
    popData.m_vecSoftkey[2] = ST_EMPTY;
    popData.m_vecSoftkey[3] = ST_OK;
#if IF_SUPPORT_MIDDLE_POPBOX
    popData.m_iType = IDLE_MSGBOX_TYPE_POPUP_MIDDLE;
#else
    popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL;//messagebox
#endif
    // 遍历获取提示信息
    NETWORK_STATUS eNetState = idleScreen_GetNetworkStatus();
    if (NS_OK == eNetState)
    {
        return false;
    }

    GetNotifyText(eNetState, popData.m_strNote);

    return true;
}

bool CNetworkHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    bool bRet = false;
    unsigned int nNotifyType = GetNeedClearNotifyType();
    if (ST_OK == eSoftkey
            || PHONE_KEY_OK == eKeyCode)
    {
        idleScreen_ClearNotifyTypeFlag(PS_STATE_NETWORK_FAILED, nNotifyType);
        bRet = true;
    }
    else if (ST_CANCEL == eSoftkey || ST_EXIT == eSoftkey
             || PHONE_KEY_CANCEL == eKeyCode)
    {
        idleScreen_ClearNotifyTypeFlag(PS_STATE_NETWORK_FAILED, nNotifyType);
        bRet = true;
    }
    return bRet;
}

LRESULT CNetworkHandle::OnStatusMessage(msgObject & msg)
{
    BOOL bRet = FALSE;
    switch (msg.message)
    {
    case SYS_MSG_NOTIFY_NETWORK_STATUS:
    case SYS_MSG_NOTIFY_PHY_STATUS:
        {
            if (FlushNetStatus())
            {
                backlight_RegisterEvent(BACK_LIGHT_NETWORK, false);
            }
            else
            {
                backlight_RegisterEvent(BACK_LIGHT_NETWORK, true);
            }

            bRet = TRUE;
        }
        break;
    case SYS_MSG_OPENVPN_STATUS:
        {
            if (1 == msg.wParam)
            {
                idleScreen_SetStatus(PS_STATE_VPN, "");
            }
            else
            {
                idleScreen_DropStatus(PS_STATE_VPN);
            }
            bRet = TRUE;
        }
        break;
#ifdef IF_FEATURE_PSTN
    case ACCOUNT_PSTN_CREATED:
    case ACCOUNT_PSTN_REMOVED:
    case ACCOUNT_STATUS_CHANGED:
        {
            if (!netIsCableConnected())
            {
                FlushNetStatus();
            }
        }
        break;
#endif
    default:
        break;
    }
    return bRet;
}

bool CNetworkHandle::FlushNetStatus()
{
    //刷新网络状态,返回是否正常
    NETWORK_STATUS networkStatus = idleScreen_GetNetworkStatus(true);
    bool bret = false;
#ifdef IF_FEATURE_PSTN
    bool bHasPstnAccount = false;
    if (acc_AccountNum(ACCOUNT_TYPE_PSTN) > 0)
    {
        bHasPstnAccount = true;
    }
    IDLEUI_INFO("Current networkStatus is %d, bHasPstnAccount[%d].", networkStatus, bHasPstnAccount);
#endif

    if (NS_OK != networkStatus
#ifdef IF_FEATURE_PSTN
            && !bHasPstnAccount
#endif
       )
    {
        //防抖，网络改变才刷新状态
        if (m_lastNetworkStatus != networkStatus)
        {
            idleScreen_SetStatus(PS_STATE_NETWORK_FAILED, "");
            idleScreen_DropStatus(PS_STATE_VPN);
        }
        bret = false;
    }
    else
    {
#ifdef IF_FEATURE_PSTN
        if (bHasPstnAccount)
        {
            networkStatus = NS_OK;
        }
#endif
        idleScreen_DropStatus(PS_STATE_NETWORK_FAILED);
        // 判断VPN是否连接
        bool bIsVPNLink = (0 != configParser_GetConfigInt(kszOpenvpnStatus));
        if (bIsVPNLink)
        {
            idleScreen_SetStatus(PS_STATE_VPN);
        }
        bret = true;
    }
    m_lastNetworkStatus = networkStatus;
    return bret;
}

bool CNetworkHandle::GetIconInfo(IdleIconInfo & IconData)
{
    CBaseIdleHandle::GetIconInfo(IconData);
#if IF_SUPPORT_NOTIFY_ICON_FLASH
    IconData.m_bFlash = true;
#endif
    return true;
}

void CNetworkHandle::GetNotifyText(NETWORK_STATUS eStatus, yl::string& strNote)
{
    switch (eStatus)
    {
    case NS_CHECKING_MAC:
        {
            strNote = _UILOGIC_LANG_TRAN(TRID_CHECKING_MAC);
        }
        break;
    case NS_INVALID_MAC:
        {
            strNote = _UILOGIC_LANG_TRAN(TRID_NEED_UPDATE_MAC);
        }
        break;
    case NS_UPDATING:
        {
            strNote = _UILOGIC_LANG_TRAN(TRID_UPDATING_NETWORK);
        }
        break;
    case NS_CABLE_DISCONNECTED:
    case NS_NETWORK_STOP:
        {
            strNote = _UILOGIC_LANG_TRAN(TRID_NETWORK_UNAVAILABLE);
        }
        break;
    case NS_IP_CONFLICT:
        {
            strNote = _UILOGIC_LANG_TRAN(TRID_IP_CONFLICTED);
        }
        break;
    case NS_REQUEST_IP:
        {
            strNote = _UILOGIC_LANG_TRAN(TRID_OBTAINING_IP_ADDRESS);
        }
        break;
    case NS_ALL_IP_CONFLICT:
        {
            strNote = _UILOGIC_LANG_TRAN(TRID_ALL_IP_CONFLICT);
        }
        break;
    case NS_IPV4_CONFLICT:
        {
            strNote = _UILOGIC_LANG_TRAN(TRID_IPV4_CONFLICT);
        }
        break;
    case NS_IPV6_CONFLICT:
        {
            strNote = _UILOGIC_LANG_TRAN(TRID_IPV6_CONFLICT);
        }
        break;
    default:
        strNote = "";
        break;
    }
}

#if IF_SUPPORT_TEXT_NOTIFY
bool CNetworkHandle::GetTextNotifyData(ST_TextNotifyData& stNotifyData)
{
    NETWORK_STATUS eNetState = idleScreen_GetNetworkStatus();

    if (NS_OK == eNetState)
    {
        return false;
    }

    GetNotifyText(eNetState, stNotifyData.m_strNote);

    if (NS_INVALID_MAC == eNetState)
    {
        stNotifyData.m_bFlashNote = TRUE;
    }
    else
    {
        stNotifyData.m_bFlashIcon = TRUE;
#warning IT_STATE_CABLE_OFF
        //stNotifyData.m_listIconType.push_back(IT_STATE_CABLE_OFF);
#warning IT_STATE_CABLE_ON
        //stNotifyData.m_listIconType.push_back(IT_STATE_CABLE_ON);
    }

    return true;
}
#endif
