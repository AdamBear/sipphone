#ifndef _SETTING_ITEM_MANAGER_H_
#define  _SETTING_ITEM_MANAGER_H_

#include "settingitembase.h"

//CSettingItemManager
class CSettingItemManager
{
public:
    CSettingItemManager();
    static CSettingItemManager * GetInstance();
    CSettingItem * CreateItemByType(int eType, QWidget * pParent, QWidgetPair & pair);
};

#define g_SettingItemManger CSettingItemManager::GetInstance()

#endif //_SETTING_ITEM_MANAGER_H_
