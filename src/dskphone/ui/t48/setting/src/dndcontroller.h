#ifndef SETTING_DNDCONTROLLER_H_
#define SETTING_DNDCONTROLLER_H_

#include <ylstring.h>

#include "commondefine.h"

struct DndData;
struct MenuItemListData;

namespace NS_SETTING
{
// Class CDndController.
class CDndController
{
public:
    // Get the singleton instance.
    static CDndController * GetInstance();
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

    // Get DND data.
    // | pDndData | [out] is the buffer.
    // | iAccount | [in] is the account id. -1 on PhoneMode
    // Return true if retrieved successfully, otherwise return false.
    bool GetDndData(DndData * pDndData, int iAccountID = -1);
    // Set DND data.
    // | refDndData | is the DND data to set.
    // | iAccount | [in] is the account id. -1 on PhoneMode
    // Return true if saved successfully, otherwise return false
    bool SetDndData(const DndData & refDndData, int iAccountID = -1);

private:
    // Ctor and Dtor.
    CDndController();
    ~CDndController();

private:
    static CDndController * m_pController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CDndController);
};

}  // namespace NS_SETTING

#define g_pDndController (NS_SETTING::CDndController::GetInstance())

#endif  // SETTING_DNDCONTROLLER_H_
