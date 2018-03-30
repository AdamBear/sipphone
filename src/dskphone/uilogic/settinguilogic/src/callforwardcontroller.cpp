#include "callforwardcontroller.h"

#include "account/include/modaccount.h"

#include <path_define.h>
#include <configiddefine.h>
#include <configparser/modconfigparser.h>
#include "talk/fwd_dnd/include/modfwddnd.h"

//////////////////////////////////////////////////////////////////////////
// Class CCallForwardController.
CCallForwardController * CCallForwardController::m_pController = NULL;

CCallForwardController * CCallForwardController::GetInstance()
{
    // Instantiate the singleton instance if not existed.
    if (NULL == m_pController)
    {
        m_pController = new CCallForwardController();
    }
    return m_pController;
}

void CCallForwardController::ReleaseInstance()
{
    // Release the singleton instance if existed.
    if (NULL != m_pController)
    {
        delete m_pController;
        m_pController = NULL;
    }
}

bool CCallForwardController::GetForwardDataByType(ForwardData * pForwardData, FWD_TYPE eType,
        int iAccountID/* = -1*/)
{
    if (NULL == pForwardData)
    {
        return false;
    }

    if (-1 == iAccountID)
    {
        GetPhoneForwardDataByType(pForwardData, eType);
    }
    else
    {
        GetCustomForwardDataByType(pForwardData, eType, iAccountID);
    }
    return true;
}

bool CCallForwardController::GetPhoneForwardDataByType(ForwardData * pForwardData, FWD_TYPE eType)
{
    if (NULL == pForwardData)
    {
        return false;
    }

    switch (eType)
    {
    case FWD_TYPE_ALWAYS:
        {
            pForwardData->m_isOn = configParser_GetConfigInt(kszForwardAlwaysSwitch);
            pForwardData->m_strTarget = configParser_GetConfigString(kszForwardAlwaysTarget);
            pForwardData->m_strOnCode = configParser_GetConfigString(kszForwardAlwaysOnCode);
            pForwardData->m_strOffCode = configParser_GetConfigString(kszForwardAlwaysOffCode);
        }
        break;
    case FWD_TYPE_BUSY:
        {
            pForwardData->m_isOn = configParser_GetConfigInt(kszForwardBusySwitch);
            pForwardData->m_strTarget = configParser_GetConfigString(kszForwardBusyTarget);
            pForwardData->m_strOnCode = configParser_GetConfigString(kszForwardBusyOnCode);
            pForwardData->m_strOffCode = configParser_GetConfigString(kszForwardBusyOffCode);
        }
        break;
    case FWD_TYPE_NO_ANSWER:
        {
            pForwardData->m_isOn = configParser_GetConfigInt(kszForwardNoAnswerSwitch);
            pForwardData->m_strTarget = configParser_GetConfigString(kszForwardNoAnswerTarget);
            pForwardData->m_nRingTimes = configParser_GetConfigInt(kszForwardNoAnswerDelayTime);
            pForwardData->m_strOnCode = configParser_GetConfigString(kszForwardNoAnswerOnCode);
            pForwardData->m_strOffCode = configParser_GetConfigString(kszForwardNoAnswerOffCode);
        }
        break;
    default:
        {
            return false;
        }
    }

    return true;
}

bool CCallForwardController::GetCustomForwardDataByType(ForwardData * pForwardData,
        FWD_TYPE eType,
        int iAccountID)
{
    if (NULL == pForwardData)
    {
        return false;
    }

    switch (eType)
    {
    case FWD_TYPE_ALWAYS:
        {
            pForwardData->m_isOn = configParser_GetCfgItemIntValue(kszAccountAlwaysFWDStatus, iAccountID) == 1 ? TRUE : FALSE;
            pForwardData->m_strTarget = configParser_GetCfgItemStringValue(kszAccountAlwaysFWDTarget, iAccountID);
            pForwardData->m_strOnCode = configParser_GetCfgItemStringValue(kszAccountAlwaysFWDOnCode, iAccountID);
            pForwardData->m_strOffCode = configParser_GetCfgItemStringValue(kszAccountAlwaysFWDOffCode, iAccountID);
        }
        break;
    case FWD_TYPE_BUSY:
        {
            pForwardData->m_isOn = configParser_GetCfgItemIntValue(kszAccountBusyFWDStatus, iAccountID) == 1 ? TRUE : FALSE;
            pForwardData->m_strTarget = configParser_GetCfgItemStringValue(kszAccountBusyFWDTarget, iAccountID);
            pForwardData->m_strOnCode = configParser_GetCfgItemStringValue(kszAccountBusyFWDOnCode, iAccountID);
            pForwardData->m_strOffCode = configParser_GetCfgItemStringValue(kszAccountBusyFWDOffCode, iAccountID);
        }
        break;
    case FWD_TYPE_NO_ANSWER:
        {
            pForwardData->m_isOn = configParser_GetCfgItemIntValue(kszAccountNoASWFWDStatus, iAccountID) == 1 ? TRUE : FALSE;
            pForwardData->m_strTarget = configParser_GetCfgItemStringValue(kszAccountNoASWFWDTarget, iAccountID);
            pForwardData->m_strOnCode = configParser_GetCfgItemStringValue(kszAccountNoASWFWDOnCode, iAccountID);
            pForwardData->m_strOffCode = configParser_GetCfgItemStringValue(kszAccountNoASWFWDOffCode, iAccountID);
            pForwardData->m_nRingTimes = configParser_GetCfgItemIntValue(kszAccountNoASWFWDTimeout, iAccountID);
        }
        break;
    default:
        {
            return false;
        }
    }
    return true;
}

bool CCallForwardController::SetForwardDataByType(FWD_TYPE eType,
        const ForwardData & refData,
        int iAccountID/* = -1*/, bool bSendMsg/* = true*/)
{
    bool bChanged = false;
    if (-1 == iAccountID)
    {
        bChanged = SetPhoneForwardDataByType(eType, refData);

#ifdef IF_FEATURE_FORWARD_OPT
        if (fwd_IsCustomMode())
        {
            for (int i = 0; i < acc_AccountNum(); ++i)
            {
                bChanged |= SetCustomForwardDataByType(eType, refData, i);
            }
        }
#endif
    }
    else
    {
        bChanged = SetCustomForwardDataByType(eType, refData, iAccountID);
    }

    // 如果有修改,则发消息
    if (bChanged && bSendMsg)
    {
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_FORWARD, 0);
    }
    return bChanged;
}

// 全局
bool CCallForwardController::SetPhoneForwardDataByType(FWD_TYPE eType, const ForwardData & refData)
{
    bool bChanged = false;

    switch (eType)
    {
    case FWD_TYPE_ALWAYS:
        {
            bChanged = configParser_SetConfigInt(kszForwardAlwaysSwitch, refData.m_isOn, CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetConfigString(kszForwardAlwaysTarget, refData.m_strTarget.c_str(), CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetConfigString(kszForwardAlwaysOnCode, refData.m_strOnCode.c_str(), CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetConfigString(kszForwardAlwaysOffCode, refData.m_strOffCode.c_str(), CONFIG_LEVEL_PHONE) || bChanged;
        }
        break;
    case FWD_TYPE_BUSY:
        {
            bChanged = configParser_SetConfigInt(kszForwardBusySwitch, refData.m_isOn, CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetConfigString(kszForwardBusyTarget, refData.m_strTarget.c_str(), CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetConfigString(kszForwardBusyOnCode, refData.m_strOnCode.c_str(), CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetConfigString(kszForwardBusyOffCode, refData.m_strOffCode.c_str(), CONFIG_LEVEL_PHONE) || bChanged;
        }
        break;
    case FWD_TYPE_NO_ANSWER:
        {
            bChanged = configParser_SetConfigInt(kszForwardNoAnswerSwitch, refData.m_isOn, CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetConfigString(kszForwardNoAnswerTarget, refData.m_strTarget.c_str(), CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetConfigInt(kszForwardNoAnswerDelayTime, refData.m_nRingTimes, CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetConfigString(kszForwardNoAnswerOnCode, refData.m_strOnCode.c_str(), CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetConfigString(kszForwardNoAnswerOffCode, refData.m_strOffCode.c_str(), CONFIG_LEVEL_PHONE) || bChanged;
        }
        break;
    default:
        {
            return false;
        }
    }

    return bChanged;
}

// 自定义
bool CCallForwardController::SetCustomForwardDataByType(FWD_TYPE eType, const ForwardData & refData,
        int iAccountID)
{
    bool bChanged = false;

    switch (eType)
    {
    case FWD_TYPE_ALWAYS:
        {
            bChanged = configParser_SetCfgItemIntValue(kszAccountAlwaysFWDStatus, iAccountID, refData.m_isOn, CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetCfgItemStringValue(kszAccountAlwaysFWDTarget, iAccountID, refData.m_strTarget, CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetCfgItemStringValue(kszAccountAlwaysFWDOnCode, iAccountID, refData.m_strOnCode, CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetCfgItemStringValue(kszAccountAlwaysFWDOffCode, iAccountID, refData.m_strOffCode, CONFIG_LEVEL_PHONE) || bChanged;
        }
        break;
    case FWD_TYPE_BUSY:
        {
            bChanged = configParser_SetCfgItemIntValue(kszAccountBusyFWDStatus, iAccountID, refData.m_isOn, CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetCfgItemStringValue(kszAccountBusyFWDTarget, iAccountID, refData.m_strTarget, CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetCfgItemStringValue(kszAccountBusyFWDOnCode, iAccountID, refData.m_strOnCode, CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetCfgItemStringValue(kszAccountBusyFWDOffCode, iAccountID, refData.m_strOffCode, CONFIG_LEVEL_PHONE) || bChanged;
        }
        break;
    case FWD_TYPE_NO_ANSWER:
        {
            bChanged = configParser_SetCfgItemIntValue(kszAccountNoASWFWDStatus, iAccountID, refData.m_isOn, CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetCfgItemStringValue(kszAccountNoASWFWDTarget, iAccountID, refData.m_strTarget, CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetCfgItemStringValue(kszAccountNoASWFWDOnCode, iAccountID, refData.m_strOnCode, CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetCfgItemStringValue(kszAccountNoASWFWDOffCode, iAccountID, refData.m_strOffCode, CONFIG_LEVEL_PHONE) || bChanged;
            bChanged = configParser_SetCfgItemIntValue(kszAccountNoASWFWDTimeout, iAccountID, refData.m_nRingTimes, CONFIG_LEVEL_PHONE) || bChanged;
        }
        break;
    default:
        {
            return false;
        }
    }

    return bChanged;
}

bool CCallForwardController::IsForwardEnable(FWD_TYPE eType, int iAccountID/* = -1*/)
{
    bool bEnable = false;

    switch (eType)
    {
    case FWD_TYPE_ALWAYS:
        {
            bEnable = IsAlwaysForwardEnable(iAccountID);
        }
        break;
    case FWD_TYPE_BUSY:
        {
            bEnable = IsBusyForwardEnable(iAccountID);
        }
        break;
    case FWD_TYPE_NO_ANSWER:
        {
            bEnable = IsNoAnswerForwardEnable(iAccountID);
        }
        break;
    case FWD_TYPE_ALL:
        {
            bEnable = IsAlwaysForwardEnable(iAccountID)
                      || IsBusyForwardEnable(iAccountID)
                      || IsNoAnswerForwardEnable(iAccountID);
        }
        break;
    default:
        {
            bEnable = false;
        }
        break;
    }

    return bEnable;
}

bool CCallForwardController::IsAlwaysForwardEnable(int iAccountID)
{
    ForwardData objData;

    return (GetForwardDataByType(&objData, FWD_TYPE_ALWAYS, iAccountID)
            && objData.m_isOn);
    return false;
}

bool CCallForwardController::IsBusyForwardEnable(int iAccountID)
{
    ForwardData objData;
    return (GetForwardDataByType(&objData, FWD_TYPE_BUSY, iAccountID)
            && objData.m_isOn);
    return false;
}

bool CCallForwardController::IsNoAnswerForwardEnable(int iAccountID)
{
    ForwardData objData;
    return (GetForwardDataByType(&objData, FWD_TYPE_NO_ANSWER, iAccountID)
            && objData.m_isOn);
    return false;
}

CCallForwardController::CCallForwardController()
{

}

CCallForwardController::~CCallForwardController()
{

}
