#include "intercommodel.h"

#include "ETLLib.hpp"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "setting/include/messagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"

#include "commondefine.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CIntercomModel.
bool CIntercomModel::IsAllowed() const
{
    return StrictCheck(configParser_GetConfigInt(kszIntercomAllowance)) == CONFIG_ON;
}

void CIntercomModel::SetAllowance(bool isAllowed)
{
    ConfigSwitch eCurrentStatus =
        StrictCheck(configParser_GetConfigInt(kszIntercomAllowance));
    ConfigSwitch eSwitchToSet = isAllowed ? CONFIG_ON : CONFIG_OFF;
    if (eCurrentStatus != eSwitchToSet)
    {
        // Save the value to config file.
        configParser_SetConfigInt(kszIntercomAllowance, isAllowed ? 1 : 0, CONFIG_LEVEL_PHONE);
    }
}

bool CIntercomModel::IsMuteOn() const
{
    return StrictCheck(configParser_GetConfigInt(kszIntercomMuteSwitch)) == CONFIG_ON;
}

void CIntercomModel::SetMute(bool isOn)
{
    ConfigSwitch eCurrentStatus =
        StrictCheck(configParser_GetConfigInt(kszIntercomMuteSwitch));
    ConfigSwitch eSwitchToSet = isOn ? CONFIG_ON : CONFIG_OFF;
    if (eCurrentStatus != eSwitchToSet)
    {
        // Save the value to config file.
        configParser_SetConfigInt(kszIntercomMuteSwitch, isOn ? 1 : 0, CONFIG_LEVEL_PHONE);
    }
}

bool CIntercomModel::IsToneOn() const
{
    return StrictCheck(configParser_GetConfigInt(kszIntercomToneSwitch)) == CONFIG_ON;
}

void CIntercomModel::SetTone(bool isOn)
{
    ConfigSwitch eCurrentStatus =
        StrictCheck(configParser_GetConfigInt(kszIntercomToneSwitch));
    ConfigSwitch eSwitchToSet = isOn ? CONFIG_ON : CONFIG_OFF;
    if (eCurrentStatus != eSwitchToSet)
    {
        // Save the value to config file.
        configParser_SetConfigInt(kszIntercomToneSwitch, isOn ? 1 : 0, CONFIG_LEVEL_PHONE);
    }
}

bool CIntercomModel::IsBargeOn() const
{
    return StrictCheck(configParser_GetConfigInt(kszIntercomBargeSwitch)) == CONFIG_ON;
}

void CIntercomModel::SetBarge(bool isOn)
{
    ConfigSwitch eCurrentStatus =
        StrictCheck(configParser_GetConfigInt(kszIntercomBargeSwitch));
    ConfigSwitch eSwitchToSet = isOn ? CONFIG_ON : CONFIG_OFF;
    if (eCurrentStatus != eSwitchToSet)
    {
        // Save the value to config file.
        configParser_SetConfigInt(kszIntercomBargeSwitch, isOn ? 1 : 0, CONFIG_LEVEL_PHONE);
    }
}

CIntercomModel::CIntercomModel()
{

}

CIntercomModel::~CIntercomModel()
{

}

}  // namespace NS_SETTING
