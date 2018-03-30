#include "callwaitingmodel.h"

#include <ETLLib/ETLLib.hpp>
#include "interfacedefine.h"
#include "configparser/modconfigparser.h"
#include "setting/include/messagedefine.h"
#include "setting/include/common.h"
#include <configiddefine.h>
#include "etlmsghandler/modetlmsghandler.h"

#include "commondefine.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CCallWaitingModel.
bool CCallWaitingModel::IsOn() const
{
    return StrictCheck(configParser_GetConfigInt(kszCallWaitingSwitch)) == CONFIG_ON;
}

void CCallWaitingModel::SetStatus(bool isOn)
{
    ConfigSwitch eCurrentStatus =
        StrictCheck(configParser_GetConfigInt(kszCallWaitingSwitch));
    ConfigSwitch eStatusToSet = isOn ? CONFIG_ON : CONFIG_OFF;
    if (eCurrentStatus != eStatusToSet)
    {
        // Saves the value to config file.
        configParser_SetConfigInt(kszCallWaitingSwitch, isOn ? 1 : 0, CONFIG_LEVEL_PHONE);

        // Post message.
        etl_NotifyApp(false, CALLWAITING_STATUS_CHANGED, 0, isOn ? 1 : 0);
    }
}

bool CCallWaitingModel::IsPlayTone() const
{
    return StrictCheck(configParser_GetConfigInt(kszCallWaitingToneSwitch)) == CONFIG_ON;
}

void CCallWaitingModel::SetPlayTone(bool isPlay)
{
    ConfigSwitch eCurrentStatus =
        StrictCheck(configParser_GetConfigInt(kszCallWaitingToneSwitch));
    ConfigSwitch eStatusToSet = isPlay ? CONFIG_ON : CONFIG_OFF;
    // Only the status if not equals to the status before, we set the play tone.
    if (eCurrentStatus != eStatusToSet)
    {
        // Saves the value to config file.
        configParser_SetConfigInt(kszCallWaitingToneSwitch, isPlay ? 1 : 0, CONFIG_LEVEL_PHONE);

        // Post message.
        etl_NotifyApp(false, CALLWAITING_PLAYTONE_CHANGED, 0,
                      isPlay ? 1 : 0);
    }
}

CCallWaitingModel::CCallWaitingModel()
{

}

CCallWaitingModel::~CCallWaitingModel()
{

}

//get callwaitting oncode from config file
yl::string CCallWaitingModel::GetOnCode()
{
    return configParser_GetConfigString(kszCallWaitingOnCode);
}

//get callwaitting offcode from config file
yl::string CCallWaitingModel::GetOffCode()
{
    return configParser_GetConfigString(kszCallWaitingOffCode);
}

//set callwaitting oncode and offcode to config file
bool CCallWaitingModel::SetCode(yl::string strOnCode, yl::string strOffCode)
{
    bool bSuccessed = false;
    bool bChanged = false;
    if (configParser_GetConfigString(kszCallWaitingOnCode) != strOnCode)
    {
        bSuccessed = configParser_SetConfigString(kszCallWaitingOnCode, strOnCode.c_str(),
                     CONFIG_LEVEL_PHONE);
        bChanged = bChanged || bSuccessed;
    }
    if (configParser_GetConfigString(kszCallWaitingOffCode) != strOnCode)
    {
        bSuccessed = configParser_SetConfigString(kszCallWaitingOffCode, strOffCode.c_str(),
                     CONFIG_LEVEL_PHONE) && bSuccessed;
        bChanged = bChanged || bSuccessed;
    }

    //在controller当中修改页面配置后发消息，单个配置保存不再发
    /*if (bChanged)
    {
        //msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), CONFIG_MSG_BCAST_CHANGED, ST_CALLWAIT, 0);
    }*/

    return bSuccessed;
}

}  // namespace NS_SETTING
