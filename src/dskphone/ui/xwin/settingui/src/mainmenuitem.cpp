#include "settinguidefine.h"
#if IF_SETTING_MAINMENU_IS_TABLEVIEW
#include "mainmenuitem.h"
#include "baseui/include/baseuicommon.h"
#include "baseui/include/t2xpicpath.h"
#include "xwindow/xWindowManager.h"

CMainMenuItem::CMainMenuItem(IListViewItemAction* pAction/* = NULL*/)
    : xListViewItem(LIST_ITEM_TYPE_MAIN_MENU, pAction)
{

}

CMainMenuItem::~CMainMenuItem()
{

}

int CMainMenuItem::GetMainMenuItemType()
{
    return (LIST_ITEM_TYPE_MAIN_MENU);
}

CMainMenuItem* CMainMenuItem::GetMainMenuItem(xListViewItem* pItem)
{
    if (NULL == pItem || GetMainMenuItemType() != pItem->GetType())
    {
        return NULL;
    }

    return (CMainMenuItem *)pItem;
}

void CMainMenuItem::loadChildrenElements(xml_node& node)
{
    xListViewItem::loadChildrenElements(node);
    m_pIcon = static_cast<xImageView*>(getViewById("picIcon"));
    m_pName = static_cast<xTextView*>(getViewById("txtTitle"));
}

xListViewItem* CMainMenuItem::CreateMainMenuItem()
{
    CMainMenuItem* pMainMenuItem = new CMainMenuItem;

    if (NULL != pMainMenuItem)
    {
        pMainMenuItem->loadContent("setting/mainmenuitem.xml");
    }

    return pMainMenuItem;
}

void CMainMenuItem::SetStatusIcon(const yl::string& strIcon)
{
    if (NULL == m_pIcon)
    {
        return;
    }
    m_pIcon->setPixmap(strIcon);
}

void CMainMenuItem::SetName(const yl::string& strName)
{
    if (NULL == m_pName)
    {
        return;
    }
    m_pName->SetText(strName);
}

bool CMainMenuItem::onFocusEvent(bool bFocusIn)
{
    if (NULL != m_pIcon)
    {
        if (bFocusIn)
        {
            xPixmap pmBG;

            g_WorkStation.GetImageByPath(PIC_DEFAULT_MAINMENU_ITEM_BG, pmBG);

            m_pIcon->setBackground(pmBG);
            m_pName->EnableScroll(true);
            m_pName->SetOverLengthStyle(OVERLENGTH_TYPE_SCROLL);
        }
        else
        {
            xPixmap pmBG;
            m_pIcon->setBackground(pmBG);
            m_pName->SetOverLengthStyle(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
        }
    }

    return xListViewItem::onFocusEvent(bFocusIn);
}

#endif
