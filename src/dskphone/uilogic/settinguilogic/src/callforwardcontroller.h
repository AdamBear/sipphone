#ifndef SETTING_CALLFORWARDCONTROLLER_H_
#define SETTING_CALLFORWARDCONTROLLER_H_

#include "ETLLib/ETLLib.hpp"
#include "settinguilogic/include/common.h"
#include "talk/fwd_dnd/include/fwddndtypes.h"

#include "commondefine.h"

struct MenuItemListData;

// Class CCallForwardModel
class CCallForwardController
{
public:
    // Get the singleton instance.
    static CCallForwardController * GetInstance();
    // Release the singleton instance.
    static void ReleaseInstance();

    // Get forward data, such as getting always forward data, busy forward data,
    // and no answer forward data.
    // | pForwardData | [out] is the buffer.
    // | eType | is the forward type.
    // | iAccountID | is the forward account id. -1 on PhoneMode
    // Return true if retrieved successfully, otherwise return false.
    bool GetForwardDataByType(ForwardData * pForwardData, FWD_TYPE eType,
                              int iAccountID = -1);

    // Get PhoneMode forward data.
    bool GetPhoneForwardDataByType(ForwardData * pForwardData, FWD_TYPE eType);

    // Get AccountMode forward data.
    bool GetCustomForwardDataByType(ForwardData * pForwardData, FWD_TYPE eType,
                                    int iAccountID);

    // Set forward data by type.
    // | eType | is the forward type.
    // | ForwardData | is the forward data to set.
    // | iAccountID | is forward account id .  -1 on PhoneMode
    // Return true if saved successfully, otherwise return false.
    bool SetForwardDataByType(FWD_TYPE eType, const ForwardData & refData,
                              int iAccountID = -1, bool bSendMsg = true);

    // 全局
    bool SetPhoneForwardDataByType(FWD_TYPE eType, const ForwardData & refData);

    // 自定义
    bool SetCustomForwardDataByType(FWD_TYPE eType, const ForwardData & refData, int iAccountID);

    // Check forward switch.
    // | eType | is the forward type.
    // | iAccountID | accountID
    bool IsForwardEnable(FWD_TYPE eType, int iAccountID);

private:
    bool IsAlwaysForwardEnable(int iAccountID);  // Check Always forward switch.
    bool IsBusyForwardEnable(int iAccountID);    // Check Busy forward switch.
    bool IsNoAnswerForwardEnable(int iAccountID);// Check NoAnswer forward switch.

    // Ctor and Dtor.
    CCallForwardController();
    ~CCallForwardController();

private:
    static CCallForwardController * m_pController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CCallForwardController);
};

#define g_pCallForwardController (CCallForwardController::GetInstance())

#endif  // SETTING_CALLFORWARDCONTROLLER_H_
