#include "basemethod.h"

#include <assert.h>

#include "commonapi/stringhelperapi.h"
#include "setting/include/common.h"
#include "menu/menucontroller.h"

#include "ETLLib.hpp"

namespace NS_SETTING
{
bool GetPageMenuListByClickAction(MenuItemListData * pData,
                                  const yl::string & strClickAction)
{
    // Check the click action.
    if (pData == NULL || strClickAction.empty())
    {
        return false;
    }

    // Only when the action is "OpenPage", we can get page menu list.
    yl::string strAction;
    yl::string strContent;
    if (!commonAPI_ParseActionInfo(&strAction, &strContent, strClickAction)
            || strAction != kszOpenPageAction)
    {
        return false;
    }

    // Get page menu list from menu controller.
    MenuItemListData stMILD;
    if (g_pMenuController != NULL)
    {
        stMILD = g_pMenuController->GetMenuListByClickAction(strClickAction);
    }

    if (stMILD.IsValid())
    {
        *pData = stMILD;
        return true;
    }

    return false;
}

const yl::string GetTitleByClickAction(const yl::string & strClickAction)
{
    if (strClickAction.empty() || g_pMenuController == NULL)
    {
        return "";
    }

    // Call menu controller's method to get title.
    return g_pMenuController->GetTitleByClickAction(strClickAction);
}

}  // namespace NS_SETTING
