#ifndef SETTING_CALIBRATIONCONTROLLER_H_
#define SETTING_CALIBRATIONCONTROLLER_H_

#include <ylstring.h>

#include "commondefine.h"

struct MenuItemListData;

namespace NS_SETTING
{
// Class CCalibrationController.
class CCalibrationController
{
public:
    // Get the singleton instance.
    static CCalibrationController * GetInstance();
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

private:
    // Ctor and Dtor.
    CCalibrationController();
    ~CCalibrationController();

private:
    static CCalibrationController * m_pController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CCalibrationController);
};

}  // namespace NS_SETTING

#define g_pCalibrationController (NS_SETTING::CCalibrationController::GetInstance())

#endif  // SETTING_CALIBRATIONCONTROLLER_H_
