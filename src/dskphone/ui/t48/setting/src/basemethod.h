#ifndef SETTING_BASEMETHOD_H_
#define SETTING_BASEMETHOD_H_

#include <ylstring.h>

struct MenuItemListData;

namespace ETL_FileHelper
{
class xmlBaseNode;
class xmlElement;
}

namespace NS_SETTING
{
// Get page menu list by click action.
// | pData | [out] is the data retrieved.
// | strClickAction | is the click action, it must begin with OpenPage.
// Such as "OpenPage("account_list.xml")".
// Returns true if successfully, otherwise return false.
bool GetPageMenuListByClickAction(MenuItemListData * pData,
                                  const yl::string & strClickAction);

// Get title by click action.
// | strClickAction | is the click action.
// Return the title if Retrieved successfully, otherwise return empty.
const yl::string GetTitleByClickAction(const yl::string & strClickAction);

}  // namespace NS_SETTING

#endif  // SETTING_BASEMETHOD_H_
