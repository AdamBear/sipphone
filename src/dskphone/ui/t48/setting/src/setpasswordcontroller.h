#ifndef SETTING_SETPASSWORDCONTROLLER_H_
#define SETTING_SETPASSWORDCONTROLLER_H_

#include <ylstring.h>

#include "commondefine.h"

struct MenuItemListData;
struct PasswordData;

namespace NS_SETTING
{
// Class CSetPasswordController.
class CSetPasswordController
{
public:
    // Get the singleton instance.
    static CSetPasswordController * GetInstance();
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

    // Set password.
    // It first checks the old password, return false if not equaled.
    // Then set password into config file.
    // | refData | is the data to set.
    // Return true if set successfully, otherwise return false.
    bool SetPassword(const yl::string strUser, const PasswordData & refData) const;

private:
    // Ctor and Dtor.
    CSetPasswordController();
    ~CSetPasswordController();

private:
    static CSetPasswordController * m_pController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CSetPasswordController);
};

}  // namespace NS_SETTING

#define g_pSetPasswordController (NS_SETTING::CSetPasswordController::GetInstance())

#endif  // SETTING_SETPASSWORDCONTROLLER_H_
