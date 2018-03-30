#ifndef __CUSTOM_THEME_MANAGER_H__
#define __CUSTOM_THEME_MANAGER_H__

#include "xmlparser.hpp"
#include "customthemecommondefine.h"


class CCustomThemeManager
{
public:
    static CCustomThemeManager * CreateInstance();

public:
    CCustomThemeManager();
    virtual ~CCustomThemeManager();

    void Init();

    const CustomFontData & GetCustomFontDataByKey(int nKey);

    int GetCustomFontSizeByKey(int nFontKey);

    QColor GetCustomFontColorByKey(int nFontKey, int nFontStatus = FONT_NORMAL);

    bool IsShowFontShadowByKey(int nFontKey);

protected:
    void LoadCustomThemeMap();

    void ReloadCustomFontData(const xml_node & nodeFont);

    int GetCustomObjectId(yl::string & strId);

    bool GetColorByString(yl::string & strValue, QColor & objColor);

    void InsertCustomFontData(int nFontId, CustomFontData & pCustomFontData);

private:
    MAP_CUSTOMTHEME_DATA m_mapCustomThemeData;
    CustomFontData m_themeData;
};

#define g_pCustomThemeManager CCustomThemeManager::CreateInstance()

#endif // #ifndef __CUSTOM_THEME_MANAGER_H__
