#ifndef SETTING_FWDINTERNATIONAL_H_
#define SETTING_FWDINTERNATIONAL_H_

#include <ylstring.h>

#include "commondefine.h"

struct FWDInternationalData;
struct MenuItemListData;

namespace NS_SETTING
{
class CFWDInternationalModel;

// Class CFWDInternationalController.
class CFWDInternationalController
{
public:
    // Get the singleton instance.
    static CFWDInternationalController * GetInstance();
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

    // Get AES key data.
    // | pData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetFWDInternationalData(FWDInternationalData * pData) const;
    // Set AES key data.
    // | refData | is the data to set.
    void SetFWDInternationalData(const FWDInternationalData & refData);

private:
    // Ctor and Dtor.
    CFWDInternationalController();
    ~CFWDInternationalController();

    // Initialize.
    // Returns true if initialized successfully, otherwise returns false.
    bool MyInit();
    // Finalize.
    // Returns true if finalized successfully, otherwise returns false.
    bool MyFinal();

private:
    CFWDInternationalModel * m_pModel; // The model instance.

    static CFWDInternationalController * m_pInst; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CFWDInternationalController);
};

}  // namespace NS_SETTING

#define g_pFWDInternationalController (NS_SETTING::CFWDInternationalController::GetInstance())

#endif  // SETTING_FWDINTERNATIONAL_H_
