#ifndef SETTING_CALLWAITINGCONTROLLER_H_
#define SETTING_CALLWAITINGCONTROLLER_H_

#include <ylstring.h>

#include "commondefine.h"

struct CallWaitingData;
struct MenuItemListData;

namespace NS_SETTING
{
class CCallWaitingModel;

// Class CCallWaitingController
class CCallWaitingController
{
public:
    // Get the singleton instance, it's a lazy singleton.
    static CCallWaitingController * GetInstance();
    // Release the singleton instance.
    static void ReleaseInstance();

    // Get page menu list by click action.
    // | pData | [out] is the data retrieved.
    // | strClickAction | is the click action, it must begin with OpenPage.
    // Such as "OpenPage("account_list.xml")".
    // Returns true if successfully, otherwise return false.
    bool GetPageMenuListByClickAction(MenuItemListData * pData,
                                      const yl::string & strClickAction) const;

    // Get title by click action.
    // | strClickAction | is the click action.
    // Return the title if Retrieved successfully, otherwise return empty.
    const yl::string GetTitleByClickAction(
        const yl::string & strClickAction) const;

    // Get call waiting data, including call waiting status and play-tone.
    // | pCallWaitingData | is the data retrieved.
    // Returns true if retrieved successfully, otherwise returns false.
    bool GetCallWaitingData(CallWaitingData * pCallWaitData) const;
    // Set call waiting data.
    // | refData | is the data to set.
    void SetCallWaitingData(const CallWaitingData & refData);

private:
    // Ctor and Dtor.
    CCallWaitingController();
    ~CCallWaitingController();

    // Initialize.
    // Returns true if initialized successfully, otherwise returns false.
    bool MyInit();
    // Finalize.
    // Returns true if finalized successfully, otherwise returns false.
    bool MyFinal();

private:
    CCallWaitingModel * m_pCallWaitingModel; // The call waiting model instance.
    static CCallWaitingController * m_pCallWaitController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CCallWaitingController);
};

}  // namespace NS_SETTING

#define g_pCallWaitingController (NS_SETTING::CCallWaitingController::GetInstance())

#endif  // SETTING_CALLWAITINGCONTROLLER_H_
