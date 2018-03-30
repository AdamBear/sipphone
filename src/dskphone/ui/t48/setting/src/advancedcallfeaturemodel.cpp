#include "advancedcallfeaturemodel.h"

#include <assert.h>

#include "ETLLib/ETLLib.hpp"
#include "configparser/modconfigparser.h"
#include "setting/include/messagedefine.h"
#include "setting/include/common.h"
#include <configiddefine.h>
#include "etlmsghandler/modetlmsghandler.h"
#include "service_config.h"

#include "commondefine.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CAdvancedCallFeatureModel
int CAdvancedCallFeatureModel::GetKeyAsSend() const
{
    return configParser_GetConfigInt(kszKeyAsSend);
}

void CAdvancedCallFeatureModel::SetKeyAsSend(int nType)
{
    int nTypePrev = configParser_GetConfigInt(kszKeyAsSend);
    if (nType != nTypePrev)
    {
        // Save value to config file.
        configParser_SetConfigInt(kszKeyAsSend, nType, CONFIG_LEVEL_PHONE);
    }
}

const yl::string CAdvancedCallFeatureModel::GetHotlineNumber() const
{
    return configParser_GetConfigString(kszHotlineNumber);
}

void CAdvancedCallFeatureModel::SetHotlineNumber(const yl::string & strHotlineNumber)
{
    yl::string strCurrentHotlineNumber = configParser_GetConfigString(kszHotlineNumber);
    if (strCurrentHotlineNumber != strHotlineNumber)
    {
        // Save value to config file.
        configParser_SetConfigString(kszHotlineNumber, strHotlineNumber.c_str(), CONFIG_LEVEL_PHONE);

        // Post message.
        etl_NotifyAppEx(false,
                        ADVANCED_CALL_FEATURE_HOTLINE_NUMBER_CHANGED,
                        0, 0, (int)strHotlineNumber.length() + 1, strHotlineNumber.c_str());
    }
}

int CAdvancedCallFeatureModel::GetHotlineDelay() const
{
    return configParser_GetConfigInt(kszHotlineDelay);
}

void CAdvancedCallFeatureModel::SetHotlineDelay(int nHotlineDelay)
{
    int nCurrentHotlineDelay = configParser_GetConfigInt(kszHotlineDelay);
    if (nCurrentHotlineDelay != nHotlineDelay)
    {
        // Save value to config file.
        configParser_SetConfigInt(kszHotlineDelay, nHotlineDelay, CONFIG_LEVEL_PHONE);

        // Post message.
        etl_NotifyApp(false,
                      ADVANCED_CALL_FEATURE_HOTLINE_DELAY_CHANGED,
                      0, nHotlineDelay);
    }
}

int CAdvancedCallFeatureModel::GetAutoRedialInterval() const
{
    return configParser_GetConfigInt(kszAutoRedialInterval);
}

void CAdvancedCallFeatureModel::SetAutoRedialInterval(int nAutoRedialInterval)
{
    int nCurrentAutoRedialInterval = configParser_GetConfigInt(kszAutoRedialInterval);
    if (nCurrentAutoRedialInterval != nAutoRedialInterval)
    {
        // Save value to config file.
        configParser_SetConfigInt(kszAutoRedialInterval, nAutoRedialInterval, CONFIG_LEVEL_PHONE);

        // Post message.
        etl_NotifyApp(false,
                      ADVANCED_CALL_FEATURE_AUTO_REDIAL_INTERVAL_CHANGED,
                      0, nAutoRedialInterval);
    }
}

int CAdvancedCallFeatureModel::GetAutoRedialTimes() const
{
    return configParser_GetConfigInt(kszAutoRedialTimes);
}

void CAdvancedCallFeatureModel::SetAutoRedialTimes(int nAutoRedialTimes)
{
    int nCurrentAutoRedialTimes = configParser_GetConfigInt(kszAutoRedialTimes);
    if (nCurrentAutoRedialTimes != nAutoRedialTimes)
    {
        // Save value to config file.
        configParser_SetConfigInt(kszAutoRedialTimes, nAutoRedialTimes, CONFIG_LEVEL_PHONE);

        // Post message.
        etl_NotifyApp(false, ADVANCED_CALL_FEATURE_AUTO_REDIAL_TIMES_CHANGED,
                      0, nAutoRedialTimes);
    }
}

bool CAdvancedCallFeatureModel::IsHistorySaved() const
{
    return StrictCheck(configParser_GetConfigInt(kszSaveCallLogSwitch)) == CONFIG_ON;
}

void CAdvancedCallFeatureModel::SetHistorySaved(bool isSave)
{
    ConfigSwitch eCurrentStatus =
        StrictCheck(configParser_GetConfigInt(kszSaveCallLogSwitch));
    ConfigSwitch eSwitchToSet = isSave ? CONFIG_ON : CONFIG_OFF;
    if (eCurrentStatus != eSwitchToSet)
    {
        // Save value to config file.
        configParser_SetConfigInt(kszSaveCallLogSwitch, isSave ? 1 : 0, CONFIG_LEVEL_PHONE);

        // Post message.
        etl_NotifyApp(false,
                      ADVANCED_CALL_FEATURE_HISTORY_SAVE_SWITCH_CHANGED,
                      0, isSave ? 1 : 0);
    }
}

bool CAdvancedCallFeatureModel::IsAutoRedialOn() const
{
    return StrictCheck(configParser_GetConfigInt(kszAutoRedialSwitch)) == CONFIG_ON;
}

void CAdvancedCallFeatureModel::SetAutoRedial(bool isOn)
{
    ConfigSwitch eCurrentStatus =
        StrictCheck(configParser_GetConfigInt(kszAutoRedialSwitch));
    ConfigSwitch eSwitchToSet = isOn ? CONFIG_ON : CONFIG_OFF;
    if (eCurrentStatus != eSwitchToSet)
    {
        // Save value to config file.
        configParser_SetConfigInt(kszAutoRedialSwitch, isOn ? 1 : 0, CONFIG_LEVEL_PHONE);

        // Post message.
        etl_NotifyApp(false,
                      ADVANCED_CALL_FEATURE_AUTO_REDIAL_SWITCH_CHANGED,
                      0, isOn ? 1 : 0);
    }
}

bool CAdvancedCallFeatureModel::IsCallCompletionOn() const
{
    return StrictCheck(configParser_GetConfigInt(kszCallCompletionSwitch)) == CONFIG_ON;
}

void CAdvancedCallFeatureModel::SetCallCompletion(bool isOn)
{
    ConfigSwitch eCurrentStatus =
        StrictCheck(configParser_GetConfigInt(kszCallCompletionSwitch));
    ConfigSwitch eSwitchToSet = isOn ? CONFIG_ON : CONFIG_OFF;
    if (eCurrentStatus != eSwitchToSet)
    {
        // Save value to config file.
        configParser_SetConfigInt(kszCallCompletionSwitch, isOn ? 1 : 0, CONFIG_LEVEL_PHONE);

        // Post message.
        etl_NotifyApp(false,
                      ADVANCED_CALL_FEATURE_CALL_COMPLETION_SWITCH_CHANGED,
                      0, isOn ? 1 : 0);
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_CALLCOMPLETION, 0);
    }
}

CAdvancedCallFeatureModel::CAdvancedCallFeatureModel()
{

}

CAdvancedCallFeatureModel::~CAdvancedCallFeatureModel()
{

}


}  // namespace NS_SETTING
