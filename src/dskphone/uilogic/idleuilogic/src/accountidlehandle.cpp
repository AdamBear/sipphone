#include "accountidlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include <configiddefine.h>
#include "idle/idle_data_define.h"
#include "configparser/modconfigparser.h"
#include "service_config.h"

#include "account/include/modaccount.h"
#include "idlescreen/include/modidlescreen.h"
#include "extended/actionuri_url/include/modactionuri_url.h"
#include "voice/include/modvoice.h"
#include "lamp/powerled/include/modpowerledlogic.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "talk/talklogic/include/uiandlogic_common.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "idleuilogic/include/modidleul.h"
#include "settinguilogic/include/modsetuilogic.h"
#if IF_FEATURE_EVENT_MODULE
#include "extended/event/include/modevent.h"
#else
#include "extended/actionuri_url/include/modactionuri_url.h"
#endif

CAccountIdleHandle::CAccountIdleHandle(int nType /*= PS_STATE_ACCOUNT*/)
    : CBaseIdleHandle(nType)
{
    etl_RegisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED,
                          &CAccountIdleHandle::OnAccountMsg);
    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED,
                          &CAccountIdleHandle::OnAccountMsg);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          &CAccountIdleHandle::OnAccountMsg);
    etl_RegisterMsgHandle(ACD_MESSAGE_STATUS_CHANGE, ACD_MESSAGE_STATUS_UPDATE,
                          &CAccountIdleHandle::OnAccountMsg);
    etl_RegisterMsgHandle(HOTELING_MESSAGE_STATUS_CHANGE, HOTELING_MESSAGE_STATUS_CHANGE,
                          &CAccountIdleHandle::OnAccountMsg);
#ifdef IF_FEATURE_FORWARD_OPT
    etl_RegisterMsgHandle(FWD_CONFIG_RELOAD, FWD_CONFIG_RELOAD, &CAccountIdleHandle::OnAccountMsg);
#endif
}

CAccountIdleHandle::~CAccountIdleHandle()
{
    etl_UnregisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED,
                            &CAccountIdleHandle::OnAccountMsg);
    etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED,
                            &CAccountIdleHandle::OnAccountMsg);
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            &CAccountIdleHandle::OnAccountMsg);
    etl_UnregisterMsgHandle(ACD_MESSAGE_STATUS_CHANGE, ACD_MESSAGE_STATUS_UPDATE,
                            &CAccountIdleHandle::OnAccountMsg);
    etl_UnregisterMsgHandle(HOTELING_MESSAGE_STATUS_CHANGE, HOTELING_MESSAGE_STATUS_CHANGE,
                            &CAccountIdleHandle::OnAccountMsg);
#ifdef IF_FEATURE_FORWARD_OPT
    etl_UnregisterMsgHandle(FWD_CONFIG_RELOAD, FWD_CONFIG_RELOAD, &CAccountIdleHandle::OnAccountMsg);
#endif
}

LRESULT CAccountIdleHandle::OnAccountMsg(msgObject & objMessage)
{
    bool bRet = FALSE;
    switch (objMessage.message)
    {
    case ACCOUNT_DEFAULT_CHANGED:
        {
            UpdateAccountInfo();
            IdleUL_UpdateXMLBrowser();
            bRet = TRUE;
        }
        break;
    case ACCOUNT_STATUS_CHANGED:
        {
            if (OnAccountStatusChange(objMessage))
            {
                UpdateAccountInfo();
                bRet = TRUE;
            }
        }
        break;
    case CONFIG_MSG_BCAST_CHANGED:
        {
            switch (objMessage.wParam)
            {
            case ST_SIPLINE: // 网页改变刷新AUTOANSWER的图标
                {
                    UpdateAccountInfo();
                    bRet = TRUE;
                }
                break;
            default:
                break;
            }
        }
        break;
    case ACD_MESSAGE_QUEUE_STATUS:
        {
            IdleUL_UpdateACDQueueStatus();
        }
        break;
    case ACD_MESSAGE_STATUS_CHANGE:
    case ACD_MESSAGE_STATUS_UPDATE:
    case HOTELING_MESSAGE_STATUS_CHANGE:
#ifdef IF_FEATURE_FORWARD_OPT
    case FWD_CONFIG_RELOAD:
#endif
        {
            UpdateAccountInfo();
        }
        break;
    default:
        break;
    }
    return bRet;
}

bool CAccountIdleHandle::OnAccountStatusChange(msgObject & msg)
{
    //状态改变才发Code
    if (msg.lParam != acc_GetState(msg.wParam))
    {
        SendAccountStatusURL(msg.wParam, msg.lParam);
    }
    powerled_FlushPowerLed();
    return true;
}

void CAccountIdleHandle::SendAccountStatusURL(int iAccount, int eOldStatus)
{
    LINE_STATE eLineState = acc_GetState(iAccount);
    if (eLineState == eOldStatus || (LS_DISABLE == eLineState && LS_BE_UNREGISTERED == eOldStatus))
    {
        return;
    }
#if IF_FEATURE_EVENT_MODULE
    EventDataBase eventData;
    eventData.m_wParam = iAccount;

    // 发送注册成功、注册失败、取消注册等Event
    switch (acc_GetState(iAccount))
    {
    case LS_REGISTERED:
        {
            eventData.m_eEventType = EVENT_LOGIN_ON;
            event_SendEvent(&eventData);
        }
        break;
    case LS_REGISTER_FAIL:
        {
            eventData.m_eEventType = EVENT_REGISTER_FAILED;
            event_SendEvent(&eventData);
        }
        break;
    case LS_DISABLE:
        {
            eventData.m_eEventType = EVENT_LOGIN_OFF;
            event_SendEvent(&eventData);
        }
        break;
    default:
        break;
    }
#else

    // 组成数据
    ActionUrlData objValue;
    objValue.m_strSIPURLUser = acc_GetUsername(iAccount);
    objValue.m_strSIPURLHost = acc_GetServerName(iAccount);
    objValue.m_strSIPURL = "sip:" + acc_GetFullName(iAccount);
    //ACTION_INFO("CAccount::SendActionURL[%d][%d]", nAccount, eState);
    // 发送注册成功、注册失败、取消注册等ActionURL
    switch (acc_GetState(iAccount))
    {
    case LS_REGISTERED:
        {
            actURL_SendActionURL(ACTION_URL_LOGIN_ON, objValue);
        }
        break;
    case LS_REGISTER_FAIL:
        {
            actURL_SendActionURL(ACTION_URL_REGISTER_FAILED, objValue);
        }
        break;
    case LS_DISABLE:
        {
            actURL_SendActionURL(ACTION_URL_LOGIN_OFF, objValue);
        }
        break;
    default:
        break;
    }
#endif
}

bool CAccountIdleHandle::UpdateAccountInfo()
{
    idleScreen_SetStatus(PS_STATE_ACCOUNT, "");
    return true;
}

#if IF_SUPPORT_CENTER_ACD_STATUS
void CAccountIdleHandle::UpdateAcdStatus()
{
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    yl::string strAcdStatus;
    int iAccount = acc_GetDefaultAccount();

    if (iAccount == acc_GetDirectIPAccountID())
    {
        iAccount = INVALID_ACCOUNTID;
    }

    if (CallCenter_IsACDOn() && ACD_GetAccountId() == iAccount)
    {
        ACD_STATUS eAcdStatus = ACD_GetAcdStatus();
        GetACDStatus(eAcdStatus, strAcdStatus);
    }

    IdleUL_UpdateCenterNote(strAcdStatus);
#endif
}
#endif

bool CAccountIdleHandle::GetAccountInfo(yl::string & strDisplayName, ICON_TYPE & eIcon)
{

#if IF_SUPPORT_CENTER_ACD_STATUS
    UpdateAcdStatus();
#endif

    yl::string strLabel;
    int iAccount = acc_GetDefaultAccount();

    IDLEUI_INFO("CAccountIdleHandle::GetAccountInfo iAccount is %d, ipacountId is %d.", iAccount,
                acc_GetDirectIPAccountID());
    if (iAccount == acc_GetDirectIPAccountID())
    {
        iAccount = INVALID_ACCOUNTID;
    }

    bool bIsSCAAccount = false;
    int iBlaType = acc_GetBLAType(iAccount);

    if (BLA_BROADSOFT_SCA == iBlaType || BLA_DRAFT_BLA == iBlaType)
    {
        bIsSCAAccount = true;
    }


#if IF_FEATURE_CLOUD_ACCOUNT
    bool bCloud = acc_IsCloudAccount(iAccount);
#endif

    LINE_STATE eLineState = acc_GetState(iAccount);
    eIcon = IT_UNKNOWN;
    switch (eLineState)
    {
    case LS_UNKNOW://未知状态
    case LS_DISABLE://禁用状态
    case LS_INEXISTENCE:
        {
            eIcon = II_ACCOUNT_LS_DISABLE;
        }
        break;
    case LS_REGISTERING://注册中状态
    case LS_RE_REGISTERING:// 重新注册状态
        {
            eIcon = II_ACCOUNT_LS_LOGING;
        }
        break;
    case LS_REGISTERED://已注册状态
        {
            int iLineId = acc_GetDefaultAccount();
#ifdef IF_FEATURE_FORWARD_OPT
            FWD_TYPE eFWDType = FWD_TYPE_ALWAYS;
#else
            FWD_TYPE eFWDType = FWD_TYPE_ALL;
#endif
            if (FWDDND_MODE_ACCOUNT == fwd_GetMode()
                    && fwd_GetInfo(iLineId, eFWDType))
            {
                eIcon = II_ACCOUNT_LS_FORWARD;
            }
            else if (FWDDND_MODE_ACCOUNT == dnd_GetMode()
                     && dnd_IsEnable(iLineId))
            {
                eIcon = II_ACCOUNT_LS_DND;
            }

#if defined(IF_FEATURE_BROADSOFT_CALL_CENTER) && !IF_SUPPORT_CENTER_ACD_STATUS
            else if (CallCenter_IsACDOn()
                     && ACD_GetAccountId() == iAccount)
            {
                ACD_STATUS eAcdStatus = ACD_GetAcdStatus();
                switch (eAcdStatus)
                {
                case AS_IDLE:
                    {
                        eIcon = II_ACCOUNT_LS_ACD_LOGOUT;
                    }
                    break;
                case AS_AVAILABLE:
                    {
                        eIcon = II_ACCOUNT_LS_ACD_LOGIN;
                    }
                    break;
                case AS_UNAVAILABLE:
                    {
                        eIcon = II_ACCOUNT_LS_ACD_UNAVAILABLE;
                    }
                    break;
                case AS_WRAPUP:
                    {
                        eIcon = II_ACCOUNT_LS_ACD_WRAPUP;
                    }
                    break;
                default:
                    break;
                }
                GetACDStatus(eAcdStatus, strLabel);
            }
#endif
            if (IT_UNKNOWN == eIcon)
            {
                //IDLE界面耳麦开启，更改注册上的账号图标为手柄
                if (voice_GetHeadsetStatus())
                {
                    eIcon = II_ACCOUNT_LS_HEADSET;
                }
                else if (bIsSCAAccount)
                {
                    eIcon = II_ACCOUNT_LS_SCA_REGISTED;
                }
#warning II_ACCOUNT_LS_CLOUD_REGISTED
//#if IF_FEATURE_CLOUD_ACCOUNT
//                else if (bCloud)
//                {
//                    eIcon = II_ACCOUNT_LS_CLOUD_REGISTED;
//                }
//#endif
                else
                {
                    eIcon = II_ACCOUNT_LS_REGISTED;
                }
            }
        }
        break;
    case LS_REGISTER_FAIL://注册失败状态
        {
            eIcon = II_ACCOUNT_LS_REGISTEDFAIL;
        }
        break;
    case LS_BE_UNREGISTERED://注销状态
        {
            eIcon = II_ACCOUNT_LS_UNREGISTER;
        }
        break;
    case LS_SYSTEM_DEFAULT_ACCOUNT://系统默认Account状态
        {
            if (bIsSCAAccount)
            {
                eIcon = II_ACCOUNT_LS_SCA_REGISTED;
            }
#warning II_ACCOUNT_LS_CLOUD_REGISTED
//#if IF_FEATURE_CLOUD_ACCOUNT
//            else if (bCloud)
//            {
//                eIcon = II_ACCOUNT_LS_CLOUD_REGISTED;
//            }
//#endif
            else
            {
                eIcon = II_ACCOUNT_LS_REGISTED;
            }
        }
        break;
    default:
        break;
    }//end of switch

    if (INVALID_ACCOUNTID == iAccount)
    {
        strLabel = _UILOGIC_LANG_TRAN(TRID_NO_SERVICE);
#if IF_HIDE_ACOUNT_DISABLE_ICON
        eIcon = IT_UNKNOWN;
#endif
    }
    else
    {
        if (strLabel.empty())
        {
            strLabel = acc_GetAccountShowText(iAccount);
        }
    }
    strDisplayName = strLabel;
    // 更新一次后，清除账号更新状态
    idleScreen_DropStatus(PS_STATE_ACCOUNT);
    return true;
}

bool CAccountIdleHandle::GetACDStatus(ACD_STATUS eAcdStatus, yl::string& strAcdStatus)
{
    bool bRet = true;

#if IF_BUG_24860
    //更新状态提示信息
    switch (eAcdStatus)
    {
    case AS_AVAILABLE:
    case AS_UNAVAILABLE:
    case AS_WRAPUP:
        {
            strAcdStatus = _UILOGIC_LANG_TRAN(TRID_LOGIN);
        }
        break;
    case AS_IDLE:
        {
            strAcdStatus = _UILOGIC_LANG_TRAN(TRID_LOGOUT);
        }
        break;
    default:
        {
            bRet = false;
        }
        break;
    }
#else
    bRet = SettingUILogic_GetACDStatusString(eAcdStatus, strAcdStatus);
#endif

    return bRet;
}

