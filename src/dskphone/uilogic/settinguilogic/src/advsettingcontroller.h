#ifndef _ADV_SETTING_CONTROLLER_H_
#define _ADV_SETTING_CONTROLLER_H_

#include "../include/common.h"

bool GetAccountDetailData(AccountDetailData * pData, int iAccountID);

void SetAccountDetailData(int iAccountID, const AccountDetailData & refData);

bool GetPhoneLockData(PhoneLockData * pData);
// Set phone lock data.
// | refData | is the data to set.
void SetPhoneLockData(const PhoneLockData & refData);

bool GetPhoneBacklightData(PhoneBacklightData * pData);
void SetPhoneBacklightData(const PhoneBacklightData & refData);

bool GetPhoneContrastData(PhoneContrastData * pData);
void SetPhoneContrastData(const PhoneContrastData & refData);

bool IsFwdInternationEnable();

void SetFwdInternationActive(bool bEnable);

bool GetAutoProvisionData(AutoProvisionData * pData);
// Set auto provision data.
// | refData | is the data to set.
void SetAutoProvisionData(const AutoProvisionData & refData, bool bSavingUrl = true);

// Begin auto provision.
// Return true begin successfully, otherwise return false.
bool BeginAutoProvision();

bool GetResetData(ResetToFactoryData * pData);
// Set reset to factory data.
// | refData | is the data to set.
void SetResetData(const ResetToFactoryData & refData);

bool GetAesKeyData(AesKeyData * pData);
// Set AES key data.
// | refData | is the data to set.
void SetAesKeyData(const AesKeyData & refData);

bool SetPassword(const PasswordData & refData, const char * pUserName);

#ifdef IF_FEATURE_HOTDESKING
// Set HotDesk Base
void SetHotDeskBaseData(const HotDeskBaseData & refData);
#endif  //IF_FEATURE_HOTDESKING

#endif
