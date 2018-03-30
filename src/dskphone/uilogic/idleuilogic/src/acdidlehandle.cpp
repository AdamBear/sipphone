#include "acdidlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "translateiddefine.h"

#include "idle/idle_data_define.h"
#include "account/include/accountmessagedefine.h"
#include "broadsoft/callcenter/include/ccmessagedefine.h"
#include "broadsoft/callcenter/include/cccommon.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include <configiddefine.h>
#include "talk/talklogic/include/uiandlogic_common.h"
#include "commonunits/commonunits_def.h"
#include "service_autop.h"

#include "idlescreen/include/modidlescreen.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "configparser/modconfigparser.h"
#include "idleuilogic/include/modidleul.h"

#if defined(IF_FEATURE_BROADSOFT_CALL_CENTER) || defined(IF_FEATURE_GENESYS_ACD)

namespace
{
#define SOFTKEY_INDEX_FIRST     0
#define SOFTKEY_INDEX_SECOND    1
}

//////////////////CAcdHandle//////////////////

CAcdBaseHandle::CAcdBaseHandle(int nType /*= PS_STATE_NONE*/)
    : CBaseIdleHandle(nType)
{

}

CAcdBaseHandle::~CAcdBaseHandle()
{

}

bool CAcdBaseHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    bool bRet = true;

    if (CBaseIdleHandle::HandleKeyProcess(eSoftkey, eKeyCode))
    {
        return true;
    }

    switch (eSoftkey)
    {
    case ST_GUEST_LOCK:
        {
            // 启用键盘锁
            if (!CallCenter_IsGuestLock())
            {
                CallCenter_GuestLock(true);
                IdleUL_UpdateSoftkey();
            }
        }
        break;
    case ST_UNLOCK:
        {
            // 关闭键盘锁
            if (CallCenter_IsGuestLock())
            {
                idleScreen_SetStatus(PS_STATE_HOTELING_GUESTOUT_CHECK);
            }
        }
        break;
    case ST_HOTELING_GUESTIN:
        {
            // 黑白屏彩屏调用的接口参数不一致，需要确认接口最新调用方式，
            IDLEUI_INFO("CallCenter_IsHotelingOn() = %d", CallCenter_IsHotelingOn());
            bool bAcdOff = true;
            if (CallCenter_IsHotelingOn())
            {
                //开启了hoteling，进行登录界面
                IdleUL_EnterPageByType(IEPT_HOTELING_LOGIN_ACD_OFF);
            }
            else
            {
                ACD_TYPE eType = Get_ACDType();
                if (ACD_BROADSOFT == eType)
                {
                    //未登录按键登陆
                    ACD_ReqLogin();
                }
                else if (ACD_COSMOCOM == eType)
                {
                    //进入登录改变界面
                    IdleUL_EnterPageByType(IEPT_HOTELING_LOGIN_ACD_OFF);
                }
            }
            bRet = true;
        }
        break;
    case ST_HOTELING_GUESTOUT:
        {
            // 产品部新需求：Hoteling或FlexibleSeating有开, 退出时都需要弹窗
            if (CallCenter_IsHotelingOn() || CallCenter_IsFlexibleSeatingOn())
            {
                idleScreen_SetStatus(PS_STATE_HOTELING_GUESTOUT);
            }
            else
            {
                //ACD和HotelIng登出
                CallCenter_Logout();
            }
            bRet = true;
        }
        break;
    default:
        {
            bRet = false;
        }
        break;
    }
    return bRet;
}

void CAcdBaseHandle::GetLogoutLockSoftkey(VEC_SOFTKET_TYPE& m_vecSoftkey, int& nSoftkeyIndex,
        SoftKey_TYPE eSoftkey)
{
    if (!CallCenter_EnableGuestLock())
    {
        SwapSoftKeyToEnd(m_vecSoftkey, nSoftkeyIndex, eSoftkey);
        ++nSoftkeyIndex;
    }
    else if (!CallCenter_IsGuestLock())
    {
        SwapSoftKeyToEnd(m_vecSoftkey, nSoftkeyIndex, eSoftkey);
        ++nSoftkeyIndex;
        SwapSoftKeyToEnd(m_vecSoftkey, nSoftkeyIndex, ST_GUEST_LOCK);
        ++nSoftkeyIndex;
    }
    else
    {
        SwapSoftKeyToEnd(m_vecSoftkey, nSoftkeyIndex, ST_UNLOCK);
        ++nSoftkeyIndex;
    }
}


//////////////////CAcdHandle//////////////////
CAcdHandle::CAcdHandle(int nType /*= PS_STATE_ACD_CHANGE*/)
    : CAcdBaseHandle(nType)
{
    etl_RegisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED, &CAcdHandle::OnACDMessage);
    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED, &CAcdHandle::OnACDMessage);
    etl_RegisterMsgHandle(ACD_MESSAGE_STATUS_CHANGE, ACD_MESSAGE_STATUS_CHANGE,
                          &CAcdHandle::OnACDMessage);
    etl_RegisterMsgHandle(ACD_MESSAGE_QUEUE_STATUS, ACD_MESSAGE_QUEUE_STATUS,
                          &CAcdHandle::OnACDMessage);
    etl_RegisterMsgHandle(ACD_MESSAGE_STATUS_UPDATE, ACD_MESSAGE_STATUS_UPDATE,
                          &CAcdHandle::OnACDMessage);
    etl_RegisterMsgHandle(ACD_MESSAGE_LOGIN_TIMEOUT, ACD_MESSAGE_LOGIN_TIMEOUT,
                          &CAcdHandle::OnACDMessage);
    etl_RegisterMsgHandle(ACD_MESSAGE_LOGIN_FAIL, ACD_MESSAGE_LOGIN_FAIL, &CAcdHandle::OnACDMessage);
}

CAcdHandle::~CAcdHandle()
{
    etl_UnregisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED,
                            &CAcdHandle::OnACDMessage);
    etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED, &CAcdHandle::OnACDMessage);
    etl_UnregisterMsgHandle(ACD_MESSAGE_STATUS_CHANGE, ACD_MESSAGE_STATUS_CHANGE,
                            &CAcdHandle::OnACDMessage);
    etl_UnregisterMsgHandle(ACD_MESSAGE_QUEUE_STATUS, ACD_MESSAGE_QUEUE_STATUS,
                            &CAcdHandle::OnACDMessage);
    etl_UnregisterMsgHandle(ACD_MESSAGE_STATUS_UPDATE, ACD_MESSAGE_STATUS_UPDATE,
                            &CAcdHandle::OnACDMessage);
    etl_UnregisterMsgHandle(ACD_MESSAGE_LOGIN_TIMEOUT, ACD_MESSAGE_LOGIN_TIMEOUT,
                            &CAcdHandle::OnACDMessage);
    etl_UnregisterMsgHandle(ACD_MESSAGE_LOGIN_FAIL, ACD_MESSAGE_LOGIN_FAIL, &CAcdHandle::OnACDMessage);
}

bool CAcdHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    if (CAcdBaseHandle::HandleKeyProcess(eSoftkey, eKeyCode))
    {
        return true;
    }

    bool bRet = true;
    switch (eSoftkey)
    {
    case ST_ACD_AVAILABLE:
        {
            ACD_ReqChangeACDStatus(AA_AVAIL);
        }
        break;
    case ST_ACD_UNAVAILABLE:
        {
            if (CallCenter_IsUnavailReasonCodeOn()
                    && ACD_BROADSOFT == Get_ACDType())
            {
                IdleUL_EnterPageByType(IEPT_ACD_UNAVAILABLE_REASON_CODE);
            }
            else
            {
                ACD_ReqChangeACDStatus(AA_UNAVAIL);
            }
        }
        break;
    case ST_ACD_LOGIN:
    case ST_S2SACD_LOGIN:
        {
            IDLEUI_INFO("CallCenter_IsHotelingOn() = %d", CallCenter_IsHotelingOn());
#if IF_FEATURE_START2START_ACD
            if (ACD_START2START == Get_ACDType())
            {
                ACD_ReqLogin();
            }
            else
#endif
                if (CallCenter_IsHotelingOn())
                {
                    IdleUL_EnterPageByType(IEPT_HOTELING_LOGIN);
                }
                else
                {
                    ACD_TYPE eType = Get_ACDType();
                    if (ACD_BROADSOFT == eType)
                    {
                        ACD_ReqLogin();
                    }
                    else if (ACD_COSMOCOM == eType)
                    {
                        IdleUL_EnterPageByType(IEPT_HOTELING_LOGIN);
                    }
                }
        }
        break;
    case ST_ACD_LOGOUT:
        {
            // 产品部新需求：Hoteling或FlexibleSeating有开, 退出时都需要弹窗
            if (CallCenter_IsHotelingOn() || CallCenter_IsFlexibleSeatingOn())
            {
                idleScreen_SetStatus(PS_STATE_HOTELING_GUESTOUT);
            }
            else
            {
                //ACD和HotelIng登出
                CallCenter_Logout();
            }
        }
        break;
    case ST_ACD_DISPCODE:
        {
            if (CallCenter_IsDispCodeOn()
                    && 0 != configParser_GetConfigInt(kszBroadsoftActive))
            {
                //进入Disposition code界面
                IdleUL_EnterPageByType(IEPT_ACD_DISPCODE);
            }
        }
        break;
    case ST_ACD_TRACE:
        {
            if (CallCenter_IsTraceOn()
                    && 0 != configParser_GetConfigInt(kszBroadsoftActive))
            {
                //发起上一路来电追踪
                CallCenter_OriginatedTrace();
            }
        }
        break;
#if IF_FEATURE_START2START_ACD
    case ST_S2SACD_REFRESH:
        {
            if (ACD_START2START == Get_ACDType())
            {
                ACD_ReqChangeACDStatus(AA_REFRESH);
            }
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_ACD
    case ST_MTSW_ACD_STATE:
        {
            // 进入状态切换界面
            IdleUL_EnterPageByType(IEPT_MTSW_ACD_MYSTATUS);
        }
        break;
#endif
    default:
        {
            bRet = false;
        }
        break;
    }
    return bRet;
}

LRESULT CAcdHandle::OnACDMessage(msgObject & objMessage)
{
    BOOL bRet = FALSE;
    switch (objMessage.message)
    {
    //账号改变会引起acd状态的改变，但是acd模块没有发送消息通知，因此这边对账号改变消息进行监听
    case ACCOUNT_DEFAULT_CHANGED:
    case ACCOUNT_STATUS_CHANGED:
        {
            bRet = OnACDMessagequeuestatus(objMessage);
        }
        break;
#if IF_BUG_24363
    case ACD_MESSAGE_LOGIN_TIMEOUT:
        {
            bRet = OnACDMessagelogintimeout(objMessage);
        }
        break;
#endif
    case ACD_MESSAGE_LOGIN_FAIL:
        {
            bRet = OnACDMessageloginfail(objMessage);
        }
        break;
    case ACD_MESSAGE_STATUS_CHANGE:
        {
            bRet = OnACDMessagestatuschange(objMessage);
        }
        break;
    //不要break因为账号disable之后只会发送 ACD_MESSAGE_STATUS_CHANGE
    //而不发送 ACD_MESSAGE_QUEUE_STATUS,
    case ACD_MESSAGE_QUEUE_STATUS:
        {
            bRet = OnACDMessagequeuestatus(objMessage);
        }
        break;
    default:
        break;
    }
    return bRet;
}

LRESULT CAcdHandle::OnACDMessagelogintimeout(msgObject & objMessage)
{
    PopupBoxInfo popboxInfo;
    popboxInfo.m_iType = IDLE_MSGBOX_TYPE_NOTE;//tips
    popboxInfo.m_nTimes = POPUP_BOX_SHOWING_SHORT_TIME;
    popboxInfo.m_strNote = _UILOGIC_LANG_TRAN(TRID_ACD);
    popboxInfo.m_strNote += "";
    if (AS_IDLE == ACD_GetAcdStatus())
    {
        popboxInfo.m_strNote += _UILOGIC_LANG_TRAN(TRID_LOGIN_FAILED);
    }
    else
    {
        popboxInfo.m_strNote += _UILOGIC_LANG_TRAN(TRID_LOGOUT_FAILED);
    }

    IdleUL_PopupBox(popboxInfo);
    return TRUE;
}

LRESULT CAcdHandle::OnACDMessageloginfail(msgObject & objMessage)
{
    PopupBoxInfo popboxInfo;
    popboxInfo.m_iType = IDLE_MSGBOX_TYPE_NOTE;//tips
    popboxInfo.m_nTimes = POPUP_BOX_SHOWING_SHORT_TIME;
    popboxInfo.m_strNote = yl::string(_UILOGIC_LANG_TRAN(TRID_ACD)) + yl::string(" ") + yl::string(
                               _UILOGIC_LANG_TRAN(TRID_LOGIN_FAILED));
    IdleUL_PopupBox(popboxInfo);
    return TRUE;
}

LRESULT CAcdHandle::OnACDMessagestatuschange(msgObject & objMessage)
{
    yl::string strAcdTip;
    ICON_TYPE eIconType = IT_UNKNOWN;
    idleScreen_SetStatus(PS_STATE_ACD_CHANGE, "");
    switch (objMessage.lParam)
    {
    case AS_AVAILABLE:
        {
            strAcdTip = _UILOGIC_LANG_TRAN(TRID_AVAILABLE);
        }
        break;
    case AS_UNAVAILABLE:
        {
            yl::string strUnavailCode;
            ACD_GetUnavailCode(strUnavailCode);
            if (strUnavailCode.empty())
            {
                strAcdTip = _UILOGIC_LANG_TRAN(TRID_UNAVAILABLE);
            }
            else
            {
                strAcdTip = strUnavailCode.c_str();
            }
        }
        break;
    case AS_WRAPUP:
        {
            strAcdTip = _UILOGIC_LANG_TRAN(TRID_WRAP_UP);
        }
        break;
    case AS_IDLE:
    default:
        {
            // SetACDQueStatActivate(false);
            strAcdTip = "";
#ifndef _WIN32
#warning "UpdateAcdQueueStatus"
#endif
            //账号注销或者退出的时候，队列状态并不会改变，而acd状态会改变，因此在这边进行更新队列信息操作。
//          if(NULL != _UiIdleLogicManager.m_pIdleUi)
//          {
//              _UiIdleLogicManager.m_pIdleUi->UpdateAcdQueueStatus("");
//          }
        }
        break;
    }
    return TRUE;
}

LRESULT CAcdHandle::OnACDMessagequeuestatus(msgObject & objMessage)
{
    typeCusNotifyList listCusNotify;
    if (CallCenter_IsACDOn() && ACD_BROADSOFT == Get_ACDType()
            && CallCenter_IsEnableQueueStatus() && AS_IDLE != ACD_GetAcdStatus())
    {
        yl::string strQest;
        const CallCenterQueueStatus & qst = ACD_GetQueueStatus();
        strQest = qst.m_strName;
        switch (qst.m_eStatus)
        {
        case QUEUE_EMPTY:
            {
                strQest += ": ";
                strQest += _UILOGIC_LANG_TRAN(TRID_EMPTY);
            }
            break;
        case QUEUE_NORMAL:
            {
                strQest += ": ";
                strQest += _UILOGIC_LANG_TRAN(TRID_QUEUEING);
            }
            break;
        case QUEUE_THRESHOLD_EXCEEDED:
            {
                strQest += ": ";
                strQest += _UILOGIC_LANG_TRAN(TRID_ALERT);
            }
            break;
        default:
            {
                //SetACDQueStatActivate(false);
                strQest += "";
            }
            break;
        }
        IDLEUI_INFO("OnACDMessagequeuestatus: m_eStatus = %d, strDisplay = %s", qst.m_eStatus,
                    strQest.c_str());
        if (!strQest.empty())
        {
            CustomNotify stNotify;
            stNotify.strText = strQest.c_str();
            listCusNotify.push_back(stNotify);
        }
    }

    idleScreen_UpdateCustomNotify(listCusNotify);
    idleScreen_SetQueueStatusCusNotify(true);

    return TRUE;
}

bool CAcdHandle::GetSoftkey(VEC_SOFTKET_TYPE& m_vecSoftkey)
{
    m_vecSoftkey.clear();
    CAcdBaseHandle::GetSoftkey(m_vecSoftkey);

    m_vecSoftkey.push_back(ST_IDLE_HOTKEY1);
    m_vecSoftkey.push_back(ST_IDLE_HOTKEY2);
    m_vecSoftkey.push_back(ST_IDLE_HOTKEY3);
    m_vecSoftkey.push_back(ST_IDLE_HOTKEY4);
    //ACD开启时Idle界面的Softkey
    if (CallCenter_IsACDOn()
            && 0 != configParser_GetConfigInt(kszSoftkeyIdleACDEnable))
    {
        //根据ACD状态设置SoftKey
        ACD_STATUS eAcdState = ACD_GetAcdStatus();

#if IF_FEATURE_START2START_ACD
        if (ACD_START2START == Get_ACDType())
        {
            m_vecSoftkey.push_back(ST_S2SACD_REFRESH);
            SwapSoftKeyToEnd(m_vecSoftkey, SOFTKEY_INDEX_SECOND, ST_S2SACD_LOGIN);
            if (AS_AVAILABLE == eAcdState)
            {
                SwapSoftKeyToEnd(m_vecSoftkey, SOFTKEY_INDEX_FIRST, ST_S2SACD_UNAVAILABLE);
            }
            else if (AS_UNAVAILABLE == eAcdState)
            {
                SwapSoftKeyToEnd(m_vecSoftkey, SOFTKEY_INDEX_FIRST, ST_S2SACD_AVAILABLE);
            }
            return true;
        }
#endif
#if IF_FEATURE_METASWITCH_ACD
        if (ACD_METASWITCH == Get_ACDType())
        {
            if (AS_IDLE != eAcdState
                    && MTSWACD_IsEnableShowState())
            {
                SwapSoftKeyToEnd(m_vecSoftkey, SOFTKEY_INDEX_FIRST, ST_MTSW_ACD_STATE);
            }
            return TRUE;
        }
#endif

        int nSoftKeyIndex = SOFTKEY_INDEX_FIRST;
        if (AS_IDLE == eAcdState)
        {
            //只单独登出ACD,Hoteling还是登录状态，softkey2为GuestOut
            if (CallCenter_IsHotelingOn() &&
                    0 != configParser_GetConfigInt(kszSoftkeyIdleHotelingEnable))
            {
                // 如果hoteling也开启的情况
                if (HOTELING_STATUS_LOGIN == CallCenter_GetHotelingStatus())
                {
                    GetLogoutLockSoftkey(m_vecSoftkey, nSoftKeyIndex, ST_HOTELING_GUESTOUT);
                }
                else
                {
                    SwapSoftKeyToEnd(m_vecSoftkey, SOFTKEY_INDEX_SECOND, ST_HOTELING_GUESTIN);
                }
            }
            else
            {
                SwapSoftKeyToEnd(m_vecSoftkey, SOFTKEY_INDEX_SECOND, ST_ACD_LOGIN);
            }
        }
        else
        {
            bool bAvailabilityState = false;
            if (1 == configParser_GetCfgItemIntValue(kszAccountACDAvailabilityState, ACD_GetAccountId()))
            {
                bAvailabilityState = true;
            }
            // 判断是否开启ACD的登录登出
            if (bAvailabilityState)
            {
                if (AS_AVAILABLE == eAcdState)
                {
                    SwapSoftKeyToEnd(m_vecSoftkey, nSoftKeyIndex, ST_ACD_UNAVAILABLE);
                    ++nSoftKeyIndex;
                }
                else if (AS_UNAVAILABLE == eAcdState
                         || AS_WRAPUP == eAcdState)
                {
                    SwapSoftKeyToEnd(m_vecSoftkey, nSoftKeyIndex, ST_ACD_AVAILABLE);
                    ++nSoftKeyIndex;
                }
            }

            GetLogoutLockSoftkey(m_vecSoftkey, nSoftKeyIndex, ST_ACD_LOGOUT);

            //如果Disposition Code功能没开启，不显示Softkey
            if (CallCenter_IsDispCodeOn()
                    && ACD_BROADSOFT == Get_ACDType()
                    && 0 != configParser_GetConfigInt(kszBroadsoftActive))
            {
                SwapSoftKeyToEnd(m_vecSoftkey, nSoftKeyIndex, ST_ACD_DISPCODE);
                ++nSoftKeyIndex;
            }
            //如果Originated Trace功能没开启，不显示Softkey
            if (CallCenter_IsTraceOn()
                    && ACD_BROADSOFT == Get_ACDType()
                    && 0 != configParser_GetConfigInt(kszBroadsoftActive))
            {
                SwapSoftKeyToEnd(m_vecSoftkey, nSoftKeyIndex, ST_ACD_TRACE);
                ++nSoftKeyIndex;
            }
        }
        return true;
    }//end of IsAcdON

    return false;
}

//////////////////CHotelingHandle//////////////////
CHotelingHandle::CHotelingHandle(int nType /*= PS_STATE_HOTELING_CHANGE*/)
{
    etl_RegisterMsgHandle(HOTELING_MESSAGE_STATUS_CHANGE, HOTELING_MESSAGE_STATUS_CHANGE,
                          &CHotelingHandle::OnHotelingMessage);
}

CHotelingHandle::~CHotelingHandle()
{
    etl_UnregisterMsgHandle(HOTELING_MESSAGE_STATUS_CHANGE, HOTELING_MESSAGE_STATUS_CHANGE,
                            &CHotelingHandle::OnHotelingMessage);
}

LRESULT CHotelingHandle::OnHotelingMessage(msgObject & objMessage)
{
    if (HOTELING_MESSAGE_STATUS_CHANGE == objMessage.message)
    {
        idleScreen_SetStatus(PS_STATE_HOTELING_CHANGE, "");
        return TRUE;
    }
    return FALSE;
}

bool CHotelingHandle::GetSoftkey(VEC_SOFTKET_TYPE& m_vecSoftkey)
{
    m_vecSoftkey.clear();
    CAcdBaseHandle::GetSoftkey(m_vecSoftkey);
    m_vecSoftkey.push_back(ST_IDLE_HOTKEY1);
    m_vecSoftkey.push_back(ST_IDLE_HOTKEY2);
    m_vecSoftkey.push_back(ST_IDLE_HOTKEY3);
    m_vecSoftkey.push_back(ST_IDLE_HOTKEY4);
    if (CallCenter_IsHotelingOn()
            && 0 != configParser_GetConfigInt(kszBroadsoftActive)
            && 0 != configParser_GetConfigInt(kszSoftkeyIdleHotelingEnable))
    {
        //Hoteling开启，ACD关闭时Idle界面的Softkey
        //根据Hoteling状态设置Softkey
        if (HOTELING_STATUS_LOGIN == CallCenter_GetHotelingStatus())
        {
            int nSoftkeyIndex = SOFTKEY_INDEX_SECOND;
            GetLogoutLockSoftkey(m_vecSoftkey, nSoftkeyIndex, ST_HOTELING_GUESTOUT);
        }
        else
        {
            SwapSoftKeyToEnd(m_vecSoftkey, SOFTKEY_INDEX_SECOND, ST_HOTELING_GUESTIN);
        }
        return true;
    }
    return false;
}

//////////////////CHotelingGuestOutHandle//////////////////
CHotelingGuestOutHandle::CHotelingGuestOutHandle(int nType /*= PS_STATE_HOTELING_GUESTOUT*/)
{

}

CHotelingGuestOutHandle::~CHotelingGuestOutHandle()
{

}

bool CHotelingGuestOutHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL;//messagebox
    popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_WANTT_LOGOUT);
    popData.m_vecSoftkey[0] = ST_CANCEL;
    popData.m_vecSoftkey[1] = ST_EMPTY;
    popData.m_vecSoftkey[2] = ST_EMPTY;
    popData.m_vecSoftkey[3] = ST_OK;
    return true;
}

bool CHotelingGuestOutHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    bool bRet = false;
    idleScreen_DropStatus(PS_STATE_HOTELING_GUESTOUT);
    if ((ST_OK == eSoftkey || PHONE_KEY_OK == eKeyCode))
    {
        CallCenter_Logout();
        idleScreen_SetStatus(PS_STATE_ACCOUNT);
        bRet = true;
    }
    return bRet;
}

//////////////////CHotelingGuestOutCheckHandle//////////////////
CHotelingGuestOutCheckHandle::CHotelingGuestOutCheckHandle(int
        nType /*= PS_STATE_HOTELING_GUESTOUT_CHECK*/)
{

}

CHotelingGuestOutCheckHandle::~CHotelingGuestOutCheckHandle()
{

}

bool CHotelingGuestOutCheckHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    // 先清除标记
    idleScreen_DropStatus(PS_STATE_HOTELING_GUESTOUT_CHECK);
    IdleUL_EnterPageByType(IEPT_GUEST_UNLOCK_CHECK);
    return true;
}

//////////////////CFlexibleSeatingHandle//////////////////
CFlexibleSeatingHandle::CFlexibleSeatingHandle(int nType /*= PS_STATE_FLEXIBLE_UPDATE*/)
{
    etl_RegisterMsgHandle(COMMONUNITS_MESSAGE_CHECKSYNC_UPDATECONFIG,
                          COMMONUNITS_MESSAGE_CHECKSYNC_UPDATECONFIG,
                          &CFlexibleSeatingHandle::OnFlexibleMessage);
    etl_RegisterMsgHandle(ATP_MSG_NOTIFY_AUTOP_END, ATP_MSG_NOTIFY_AUTOP_END,
                          &CFlexibleSeatingHandle::OnFlexibleMessage);
}

CFlexibleSeatingHandle::~CFlexibleSeatingHandle()
{
    etl_UnregisterMsgHandle(COMMONUNITS_MESSAGE_CHECKSYNC_UPDATECONFIG,
                            COMMONUNITS_MESSAGE_CHECKSYNC_UPDATECONFIG,
                            &CFlexibleSeatingHandle::OnFlexibleMessage);
    etl_UnregisterMsgHandle(ATP_MSG_NOTIFY_AUTOP_END, ATP_MSG_NOTIFY_AUTOP_END,
                            &CFlexibleSeatingHandle::OnFlexibleMessage);
}

LRESULT CFlexibleSeatingHandle::OnFlexibleMessage(msgObject & objMessage)
{
    if (!CallCenter_IsFlexibleSeatingOn())
    {
        return FALSE;
    }

    switch (objMessage.message)
    {
    case COMMONUNITS_MESSAGE_CHECKSYNC_UPDATECONFIG:
        {
            CallCenter_ClearFlexibleSeating();
            idleScreen_SetStatus(PS_STATE_FLEXIBLE_UPDATE);
        }
        break;
    case ATP_MSG_NOTIFY_AUTOP_END:
        {
            IdleStatusItem phonestate = idleScreen_GetTopStatus(SNT_POPUP);
            if (PS_STATE_FLEXIBLE_UPDATE == phonestate.m_nId)
            {
                idleScreen_DropStatus(PS_STATE_FLEXIBLE_UPDATE);
                idleScreen_SetStatus(PS_STATE_FLEXIBLE_UPDATE_END);
            }
        }
        break;
    default:
        break;
    }
    return TRUE;
}

bool CFlexibleSeatingHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    bool bRet = false;
    if (PS_STATE_FLEXIBLE_UPDATE == m_iType)
    {
        popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL;//tips
        popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_DOWNLOAD_CONFIG_BEGIN);
        popData.m_vecSoftkey[0] = ST_CANCEL;
        popData.m_vecSoftkey[1] = ST_EMPTY;
        popData.m_vecSoftkey[2] = ST_EMPTY;
        popData.m_vecSoftkey[3] = ST_EMPTY;
        bRet = true;
    }
    else if (PS_STATE_FLEXIBLE_UPDATE_END == m_iType)
    {
        popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL;//tips
        popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_DOWNLOAD_CONFIG_END);
        popData.m_nTimes = POPUP_BOX_SHOWING_SHORT_TIME;
        popData.m_vecSoftkey[0] = ST_CANCEL;
        popData.m_vecSoftkey[1] = ST_EMPTY;
        popData.m_vecSoftkey[2] = ST_EMPTY;
        popData.m_vecSoftkey[3] = ST_EMPTY;
        idleScreen_ClearNotifyTypeFlag(PS_STATE_FLEXIBLE_UPDATE_END, SNT_POPUP);
        bRet = true;
    }
    return bRet;
}

bool CFlexibleSeatingHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    bool bRet = false;
    if (ST_CANCEL == eSoftkey || ST_EXIT == eSoftkey
            || PHONE_KEY_CANCEL == eKeyCode)
    {
        idleScreen_ClearNotifyTypeFlag(m_iType, SNT_POPUP);
        bRet = true;
    }
    return bRet;
}
#endif
