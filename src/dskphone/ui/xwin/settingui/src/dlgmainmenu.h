#ifndef _DLG_MAIN_MENU_H_
#define _DLG_MAIN_MENU_H_
#include "settinguidefine.h"
#if IF_SETTING_MAINMENU_IS_TABLEVIEW

#include "dlgsettingui.h"

class CDlgMainMenu : public CDlgSettingUI
{
public:
    static CBaseDialog* CreateInstance();

public:
    CDlgMainMenu();
    virtual ~CDlgMainMenu();
    void loadChildrenElements(xml_node& node);

#if IS_COLOR
    virtual xBrush GetBackGroundBrush();
#endif
};

#endif

#endif //_DLG_NEW_MESSAGE_H_
