#ifndef SETTING_INTERCOMCONTROLLER_H_
#define SETTING_INTERCOMCONTROLLER_H_

#include <ylstring.h>

#include "commondefine.h"

struct IntercomData;
struct MenuItemListData;

namespace NS_SETTING
{

class CIntercomModel;

// Class CIntercomController
class CIntercomController
{
public:
    // Get the singleton instance.
    static CIntercomController * GetInstance();
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

    // Get intercom data.
    // | pData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetIntercomData(IntercomData * pData);
    // Set intercom data.
    // | refData | is the data to set.
    void SetIntercomData(const IntercomData & refData);

private:
    // Initialize.
    // Returns true if initialized successfully, otherwise returns false.
    bool MyInit();
    // Finalize.
    // Returns true if finalized successfully, otherwise returns false.
    bool MyFinal();

    // Ctor and Dtor.
    CIntercomController();
    ~CIntercomController();

private:
    CIntercomModel * m_pIntercomModel; // The intercom model instance.
    static CIntercomController * m_pIntercomController; // The intercom singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CIntercomController);
};

}  // namespace NS_SETTING

#define g_pIntercomController (NS_SETTING::CIntercomController::GetInstance())

#endif  // SETTING_INTERCOMCONTROLLER_H_
