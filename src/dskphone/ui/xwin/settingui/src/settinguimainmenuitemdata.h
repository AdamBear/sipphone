#ifndef __SETTINGUI_MAIN_MENU_TEM_DATA_H__
#define __SETTINGUI_MAIN_MENU_TEM_DATA_H__

#include "settinguidefine.h"
#if IF_SETTING_MAINMENU_IS_TABLEVIEW
#include "ylstl/ylstring.h"
#include "ylstl/yllist.h"
#include "settinguilogic/src/settinguimenuitemdata.h"
#define  PIC_PATH = "images/menu/"
enum
{
    // 第2层子类，使用5、6位
    SETTING_ITEM_TYPE_MAIN_MENU = 0x00001500,
};

class CSettingUIMainMenuItemData : public CSettingUIMenuItemData
{
public:
    explicit CSettingUIMainMenuItemData();
    virtual ~CSettingUIMainMenuItemData();
    static int GetMainMenuDataItemType();
    static CSettingUIMainMenuItemData* GetMainMenuDataItem(CSettingUIItemDataBase * pDataItem);
    static CSettingUIItemDataBase* CreateMainMenuDataItem();
    static bool  ParserMainMenuItemData(const xml_node& node,
                                        CSettingUIItemDataBase* pItemInfo);

public:
    const yl::string& GetPicPath() const;
    void SetPicPath(const yl::string& strPicPath);

protected:
    yl::string m_strPicPath;

};

typedef CSettingUIMainMenuItemData*  CSettingUIMainMenuItemDataPtr;

#endif

#endif
