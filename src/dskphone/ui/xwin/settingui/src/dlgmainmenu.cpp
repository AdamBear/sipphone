#include "settinguidefine.h"
#if IF_SETTING_MAINMENU_IS_TABLEVIEW
#include "dlgmainmenu.h"
#include "xwindow/xtableview.h"
#include "baseui/include/t2xpicpath.h"
#include "xwindow/xWindowManager.h"

namespace
{
#define  MAIN_MENU_XML            "setting/dlgmainmenu.xml"
}

CBaseDialog* CDlgMainMenu::CreateInstance()
{
    CDlgMainMenu* pDlg = new CDlgMainMenu;

    if (NULL != pDlg)
    {
        pDlg->loadContent(MAIN_MENU_XML);
    }

    return pDlg;
}

CDlgMainMenu::CDlgMainMenu()
{

}

CDlgMainMenu::~CDlgMainMenu()
{

}

void CDlgMainMenu::loadChildrenElements(xml_node& node)
{
    xLinearLayout::loadChildrenElements(node);
    m_pListView = static_cast<xTableView*>(getViewById("TableView"));
    BindTitle(getViewById("boxTitle"));
}

#if IS_COLOR
xBrush CDlgMainMenu::GetBackGroundBrush()
{
    xBrush objBrush;

    g_WorkStation.GetImageByPath(PIC_DEFAULT_THEME_BG, objBrush.pixmap());

    return objBrush;
}
#endif
#endif
