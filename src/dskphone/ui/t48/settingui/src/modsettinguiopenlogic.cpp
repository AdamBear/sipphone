#include "modsettinguiopenlogic.h"
#include "baselibs/include/configparser/modconfigparser.h"
#include <configiddefine.h>
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "wireless/bluetoothNew/include/modbluetooth.h"

#include "contacts/directory/include/moddirinterface.h"
#include "account/include/modaccount.h"

bool SettingUILogic_UnableEnter()
{
    return false;
}

bool SettingUILogic_DispositionCodeIsEnable()
{
    return (configParser_GetConfigInt(kszBroadsoftActive) != 0) && CallCenter_IsACDOn()
           && CallCenter_IsHotelingOn();
}

bool SettingUILogic_EmergencyIsEnable()
{
    return (configParser_GetConfigInt(kszBroadsoftActive) != 0) && CallCenter_IsACDOn()
           && CallCenter_IsEmergencyEscalationOn();
}

bool SettingUILogic_BluetoothIsEnable()
{
    return Bluetooth_IsEnable() && (Bluetooth_GetDeviceState() == DS_OK);
}

bool SettingUILogic_InitialStateIsEnable()
{
    return (configParser_GetConfigInt(kszBroadsoftActive) != 0) && CallCenter_IsACDOn()
           && (configParser_GetConfigInt(kszXSISwitch) != 0);
}

bool SettingUILogic_CallControlIsEnable()
{
    return (configParser_GetConfigInt(kszXSISwitch) != 0)
           && (configParser_GetConfigInt(kszBroadsoftActive) != 0);
}

bool SettingUILogic_AnyWhereIsEnable()
{
    return SettingUILogic_CallControlIsEnable() && (acc_GetRegisteredAccountNum() == 1);
}

bool SettingUILogic_AnonymousRejectionIsEnable()
{
    return SettingUILogic_CallControlIsEnable() && (acc_GetRegisteredAccountNum() == 1);
}

#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
//bsft callwaiting
bool SettingUILogic_CallWaitingIsEnable()
{
    return SettingUILogic_AnonymousRejectionIsEnable()
           && (configParser_GetConfigInt(kszCallWaitingMode) == 1);
}
#endif

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
//call record
bool SettingUILogic_CallRecordIsEnable()
{
    return SettingUILogic_AnonymousRejectionIsEnable()
           && (configParser_GetConfigInt(kszCallRecordModeEnable) == 1);
}
#endif

#ifdef IF_FEATURE_GETBALANCE
//get balance
bool SettingUILogic_GetBanlanceIsEnable()
{
    return configParser_GetConfigInt(kszGetBalanceEnable) != 0;
}
#endif
