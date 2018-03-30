#ifndef SETTING_RESOURCECONTROLLER_H_
#define SETTING_RESOURCECONTROLLER_H_

#include <ylstring.h>

#include "commondefine.h"

struct MenuItemListData;
struct SdFileListData;
struct UsbFileListData;

namespace NS_SETTING
{
// Class CResourceController.
class CResourceController
{
public:
    // Get the singleton instance.
    static CResourceController * GetInstance();
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

    // Get USB file list.
    // | pData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetUsbFileListData(UsbFileListData * pData);

    // Get SD file list.
    // | pData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetSdFileListData(SdFileListData * pData);

private:
    // Ctor and Dtor.
    CResourceController();
    ~CResourceController();

private:
    static CResourceController * m_pController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CResourceController);
};
}  // namespace NS_SETTING

#define g_pResourceController (NS_SETTING::CResourceController::GetInstance())

#endif  // SETTING_RESOURCECONTROLLER_H_
