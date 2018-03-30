#include "cdsskeycallcenter.h"
#include "modcallcenter.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "talk/talklogic/include/callinfo.h"
#include "adapterbox/include/modadapterbox.h"
#include "dsskey/include/moddsskey.h"
#include "setting_page_define.h"
#include "idlescreen/include/modidlescreen.h"

#if IF_FEATURE_METASWITCH
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#endif

CDsskeyAcdTrace::CDsskeyAcdTrace()
    : IDsskeyImplAction(DT_ACD_TRACE)
{

}

CDsskeyAcdTrace::~CDsskeyAcdTrace()
{

}

bool CDsskeyAcdTrace::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                 void * pExtraData)
{
    CALLCENTER_INFO("ACD Trace OnKeyPress");
    //功能未开启，直接返回
    //for test
    if (!CallCenter_IsTraceOn() || talklogic_IsRingSessionExist()
            || configParser_GetConfigInt(kszBroadsoftActive) == 0)
    {
        return false;
    }

    if (!talklogic_SessionExist())
    {
        //非通话中，不需要callId参数
        return CallCenter_OriginatedTrace();
    }
    else
    {
        //通话中先获取当前焦点通话的callId
        int nSessionId = talklogic_GetFocusedSessionID();

        CCallInfo * pCallInfo = (CCallInfo *)talklogic_GetCallInfoBySessionID(nSessionId);
        if (pCallInfo == NULL)
        {
            return false;
        }
        return CallCenter_OriginatedTrace(pCallInfo->GetCallID());
    }
}


CDssKeyACD::CDssKeyACD()
    : IDsskeyImplAction(DT_ACD)
{

}

CDssKeyACD::~CDssKeyACD()
{

}

bool CDssKeyACD::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData)
{
    ACD_TYPE eType = Get_ACDType();
    if (!AdapterBox_IsInIdle())
    {
        // 不处于idle界面，不进入call center登录界面
        return false;
    }

#if IF_FEATURE_METASWITCH_ACD
    // 没登录commportal，则进入commportal登录界面
    if (g_CallCenterController->IsMTSWACDEnable()
            && !MTSW_IsLogined())
    {
        AdapterBox_SettingUIEnterPage(CC_PAGE_MTSW_COMMPORTAL);
        return true;
    }
#endif

    if (!CallCenter_IsACDOn())
    {
        return false;
    }

#if IF_FEATURE_METASWITCH_ACD
    if (ACD_METASWITCH == eType)
    {
        AdapterBox_SettingUIEnterPage(CC_PAGE_MTSWACD_MLHG);
        return true;
    }
#endif
    switch (ACD_GetAcdStatus())
    {
    case AS_IDLE:
        {
            if (ACD_BROADSOFT == eType)
            {
                //未登录按键登陆
                ACD_ReqLogin();
            }
            else if (ACD_COSMOCOM == eType
                     || ACD_GENESYS == eType)
            {
                //进入状态改变界面
                AdapterBox_SettingUIEnterPage(CC_PAGE_ACD_LOGIN);
            }
#if IF_FEATURE_START2START_ACD
            else if (ACD_START2START == eType)
            {
                //未登录按键登陆
                ACD_ReqLogin();
            }
#endif
        }
        break;
    case AS_AVAILABLE:
    case AS_UNAVAILABLE:
    case AS_WRAPUP:
        {
            if (ACD_BROADSOFT == eType
                    || ACD_GENESYS == eType)
            {
#if IF_BUG_24860
                ACD_ReqChangeACDStatus(AA_LOGOUT);
#else
                //进入状态改变界面
                AdapterBox_SettingUIEnterPage(CC_PAGE_CALLCENTER_ACDSELECTSTATE);
#endif
            }
            else if (ACD_COSMOCOM == eType)
            {
                //进入状态改变界面
                AdapterBox_SettingUIEnterPage(CC_PAGE_ACD_STATUS);
            }
        }
        break;
    default:
        break;
    }
    return true;
}

bool CDssKeyACD::OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
                                    void * pExtraData)
{
    if (oldType != newType
            && newType == DT_ACD)
    {
        dsskey_SetDsskeyStatus(dsskeyID, ACD_GetDsskeyStatus(ACD_GetAcdStatus()));
    }

    return true;
}

#if IF_FEATURE_METASWITCH_ACD
Dsskey_Status CDssKeyACD::GetDsskeyStatusByACDStatus(ACD_STATUS eAcdStatus)
{
    switch (eAcdStatus)
    {
    case AS_IDLE:
        return DS_ACD_IDLE;
    case AS_AVAILABLE:
        return DS_ACD_AVAILABLE;
    case AS_UNAVAILABLE:
        return DS_ACD_UNAVAILABLE;
    case AS_WRAPUP:
        return DS_ACD_WRAPUP;
    default:
        return DS_ACD_IDLE;
    }
}
#endif

CDssKeyHoteling::CDssKeyHoteling()
    : IDsskeyImplAction(DT_HOTELING)
{

}

CDssKeyHoteling::~CDssKeyHoteling()
{

}

bool CDssKeyHoteling::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                 void * pExtraData)
{
    if (!CallCenter_IsHotelingOn()
            || configParser_GetConfigInt(kszBroadsoftActive) == 0)
    {
        return false;
    }
    switch (CallCenter_GetHotelingStatus())
    {
    case HOTELING_STATUS_LOGIN:
        {
            if (CallCenter_IsGuestLock())
            {
                // S2: 弹出新UI或者刷新UI
                idleScreen_SetStatus(PS_STATE_HOTELING_GUESTOUT_CHECK);
            }
            else
            {
                // S2: 弹出新UI或者刷新UI
                idleScreen_SetStatus(PS_STATE_HOTELING_GUESTOUT);
            }

            return true;
        }
        break;
    default:
        {
            //调用显示Hoteling及ACD登陆界面
            if (CallCenter_IsACDOn())
            {
                //Acd开启的Hoteling登陆界面
                AdapterBox_SettingUIEnterPage(CC_PAGE_CALLCENTER_HOTELINGLOGIN);
            }
            else
            {
                //ACD关闭的Hoteling的登陆界面
                AdapterBox_SettingUIEnterPage(CC_PAGE_CALLCENTER_HOTELINGLOGINACDOFF);
            }
        }
        break;
    }

    return false;
}

bool CDssKeyHoteling::OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
        void * pExtraData)
{
    if (oldType != newType
            && newType == DT_HOTELING)
    {
        dsskey_SetDsskeyStatus(dsskeyID, GetDsskeyStatusByHotelStatus(CallCenter_GetHotelingStatus()));
    }

    return true;
}

Dsskey_Status CDssKeyHoteling::GetDsskeyStatusByHotelStatus(HOTELING_STATUS eHotelStatus)
{
    if (eHotelStatus == HOTELING_STATUS_LOGIN)
    {
        return DS_HOTELING_ON;
    }
    else
    {
        return DS_HOTELING_OFF;
    }
}

CDsskeyDispositionCode::CDsskeyDispositionCode()
    : IDsskeyImplAction(DT_DISPCODE)
{

}

CDsskeyDispositionCode::~CDsskeyDispositionCode()
{

}

bool CDsskeyDispositionCode::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                        void * pExtraData)
{
    //功能未开启，直接返回
    if (!CallCenter_IsDispCodeOn() || talklogic_IsRingSessionExist()
            || configParser_GetConfigInt(kszBroadsoftActive) == 0)
    {
        return false;
    }

    if (logicData.strValue.empty())
    {
        AdapterBox_SettingUIEnterPage(CC_PAGE_CALLCENTER_DISPOSITIONCODE);

    }
    else
    {
        CallCenter_SendDispCode(logicData.strValue);
    }

    return true;
}


CDsskeyEmergencyEscalation::CDsskeyEmergencyEscalation()
    : IDsskeyImplAction(DT_ESCALATE)
{

}

CDsskeyEmergencyEscalation::~CDsskeyEmergencyEscalation()
{

}

bool CDsskeyEmergencyEscalation::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
        void * pExtraData)
{
    //通话未开启直接返回
    if (!CallCenter_IsEmergencyEscalationOn() || talklogic_IsRingSessionExist()
            || configParser_GetConfigInt(kszBroadsoftActive) == 0)
    {
        return FALSE;
    }
    //只有在通话中财可以使用该功能
    if (!talklogic_SessionExist())
    {
        return FALSE;
    }

    if (logicData.strValue.empty())
    {
        AdapterBox_SettingUIEnterPage(CC_PAGE_CALLCENTER_EMERGENCYESCALATION);
    }
    else
    {
        return CallCenter_EmergencyEscalation(logicData.strValue);
    }

    return true;
}

