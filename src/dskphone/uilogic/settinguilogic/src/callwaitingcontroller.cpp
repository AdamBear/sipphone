#include "callwaitingcontroller.h"

#include "settinguilogic/include/common.h"
#include "ETLLib.hpp"
#include "etlmsghandler/modetlmsghandler.h"
#include "service_config.h"
#include "settinguilogic/include/messagedefine.h"

#include "interfacedefine.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "commondefine.h"
//////////////////////////////////////////////////////////////////////////
// Class CallWaitingController.
CCallWaitingController * CCallWaitingController::m_pCallWaitController = NULL;

CCallWaitingController * CCallWaitingController::GetInstance()
{
    // Instantiate the singleton instance if not exists.
    if (NULL == m_pCallWaitController)
    {
        m_pCallWaitController = new CCallWaitingController();
    }
    return m_pCallWaitController;
}

void CCallWaitingController::ReleaseInstance()
{
    // Release the singleton instance if exists.
    if (NULL != m_pCallWaitController)
    {
        delete m_pCallWaitController;
        m_pCallWaitController = NULL;
    }
}

bool CCallWaitingController::GetCallWaitingData(CallWaitingData * pCallWaitData) const
{
    if (NULL == pCallWaitData)
    {
        return false;
    }

    // Retrieve the call waiting data.
    pCallWaitData->m_isOn = IsOn();
    pCallWaitData->m_isPlayTone = IsPlayTone();
    pCallWaitData->m_strOnCode = GetOnCode();
    pCallWaitData->m_strOffCode = GetOffCode();
    return true;
}

void CCallWaitingController::SetCallWaitingData(const CallWaitingData & refData)
{
    // Set call waiting data.
    SetStatus(refData.m_isOn);
    SetPlayTone(refData.m_isPlayTone);
    SetCode(refData.m_strOnCode, refData.m_strOffCode);

    etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_CALLWAIT, 0);
}

bool CCallWaitingController::IsOn() const
{
    return (StrictCheck(configParser_GetConfigInt(kszCallWaitingSwitch)) == CONFIG_ON);
}

void CCallWaitingController::SetStatus(bool isOn)
{
    ConfigSwitch eCurrentStatus = StrictCheck(configParser_GetConfigInt(kszCallWaitingSwitch));
    ConfigSwitch eStatusToSet = isOn ? CONFIG_ON : CONFIG_OFF;
    if (eCurrentStatus != eStatusToSet)
    {
        // Saves the value to config file.
        configParser_SetConfigInt(kszCallWaitingSwitch, isOn ? 1 : 0, CONFIG_LEVEL_PHONE);

        // Post message.
        etl_NotifyApp(false, CALLWAITING_STATUS_CHANGED, 0, isOn ? 1 : 0);
    }
}

bool CCallWaitingController::IsPlayTone() const
{
    return StrictCheck(configParser_GetConfigInt(kszCallWaitingToneSwitch)) == CONFIG_ON;
}

void CCallWaitingController::SetPlayTone(bool isPlay)
{
    ConfigSwitch eCurrentStatus = StrictCheck(configParser_GetConfigInt(kszCallWaitingToneSwitch));
    ConfigSwitch eStatusToSet = isPlay ? CONFIG_ON : CONFIG_OFF;
    // Only the status if not equals to the status before, we set the play tone.
    if (eCurrentStatus != eStatusToSet)
    {
        // Saves the value to config file.
        configParser_SetConfigInt(kszCallWaitingToneSwitch, isPlay ? 1 : 0, CONFIG_LEVEL_PHONE);

        // Post message.
        etl_NotifyApp(false, CALLWAITING_PLAYTONE_CHANGED, 0, isPlay ? 1 : 0);
    }
}

yl::string CCallWaitingController::GetOnCode() const
{
    return configParser_GetConfigString(kszCallWaitingOnCode);
}

yl::string CCallWaitingController::GetOffCode() const
{
    return configParser_GetConfigString(kszCallWaitingOffCode);
}

bool CCallWaitingController::SetCode(yl::string strOnCode, yl::string strOffCode)
{
    bool bSuccessed = false;
    if (configParser_GetConfigString(kszCallWaitingOnCode) != strOnCode)
    {
        bSuccessed = configParser_SetConfigString(kszCallWaitingOnCode, strOnCode.c_str(),
                     CONFIG_LEVEL_PHONE);
    }
    if (configParser_GetConfigString(kszCallWaitingOffCode) != strOnCode)
    {
        bSuccessed = configParser_SetConfigString(kszCallWaitingOffCode, strOffCode.c_str(),
                     CONFIG_LEVEL_PHONE) && bSuccessed;
    }

    return bSuccessed;
}

CCallWaitingController::CCallWaitingController()
{

}

CCallWaitingController::~CCallWaitingController()
{

}


