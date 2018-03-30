#ifndef MOD_UI_OPEN_LOGIC_H
#define  MOD_UI_OPEN_LOGIC_H

//直接不允许进入
bool SettingUILogic_UnableEnter();

//disposition_code
bool SettingUILogic_DispositionCodeIsEnable();

//emergency_escalation
bool SettingUILogic_EmergencyIsEnable();

//bluetooth
bool SettingUILogic_BluetoothIsEnable();

//initial_state
bool SettingUILogic_InitialStateIsEnable();

//call control
bool SettingUILogic_CallControlIsEnable();

//any where
bool SettingUILogic_AnyWhereIsEnable();

//anonymous rejection
bool SettingUILogic_AnonymousRejectionIsEnable();

#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
//bsft callwaiting
bool SettingUILogic_CallWaitingIsEnable();
#endif

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
//call record
bool SettingUILogic_CallRecordIsEnable();
#endif

#ifdef IF_FEATURE_GETBALANCE
//get balance
bool SettingUILogic_GetBanlanceIsEnable();
#endif

#endif