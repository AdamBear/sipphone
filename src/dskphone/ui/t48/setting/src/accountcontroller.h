#ifndef SETTING_ACCOUNTCONTROLLER_H_
#define SETTING_ACCOUNTCONTROLLER_H_

#include <ylstring.h>

#include "commondefine.h"

struct AccountDetailData;
struct MenuItemListData;

namespace NS_SETTING
{
// Class CAccountController.
class CAccountController
{
public:
    // Get the singleton instance.
    static CAccountController * GetInstance();
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

    // Get account by click action.
    // | pData | [out] is the buffer.
    // | strClickAction | is the click action.
    // Return true if retrieved successfully, otherwise return false.
    bool GetAccountByClickAction(AccountDetailData * pData,
                                 const yl::string & strClickAction) const;
    // Set account by click action.
    // | strClickAction | is the click action.
    // | refData | is the data to set.
    void SetAccountByClickAction(const yl::string & strClickAction,
                                 const AccountDetailData & refData);

private:
    // Ctor and Dtor.
    CAccountController();
    ~CAccountController();

private:
    static CAccountController * m_pController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CAccountController);
};

}  // namespace NS_SETTING

#define g_pAccountController (NS_SETTING::CAccountController::GetInstance())

#endif  // SETTING_ACCOUNTCONTROLLER_H_
