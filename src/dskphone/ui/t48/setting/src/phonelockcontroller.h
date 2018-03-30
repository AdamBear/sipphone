#ifndef SETTING_PHONELOCKCONTROLLER_H_
#define SETTING_PHONELOCKCONTROLLER_H_

#include <ylstring.h>

#include "commondefine.h"

struct MenuItemListData;
struct PhoneLockData;

namespace NS_SETTING
{
class CPhoneLockModel;

// Class CPhoneLockController.
class CPhoneLockController
{
public:
    // Get the singleton instance.
    static CPhoneLockController * GetInstance();
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

    // Get phone lock data.
    // | pData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetPhoneLockData(PhoneLockData * pData);
    // Set phone lock data.
    // | refData | is the data to set.
    void SetPhoneLockData(const PhoneLockData & refData);

private:
    // Ctor and Dtor.
    CPhoneLockController();
    ~CPhoneLockController();

    // Initialize.
    // Return true if initialized successfully, otherwise return false.
    bool MyInit();
    // Finalize.
    // Return true if finalized successfully, otherwise return false.
    bool MyFinal();

private:
    CPhoneLockModel * m_pModel; // The model instance.

    static CPhoneLockController * m_pController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CPhoneLockController);
};

}  // namespace NS_SETTING

#define g_pPhoneLockController (NS_SETTING::CPhoneLockController::GetInstance())

#endif  // SETTING_PHONELOCKCONTROLLER_H_
