#ifndef __MAIN_MENU_ITEM_H__
#define __MAIN_MENU_ITEM_H__
#include "settinguidefine.h"
#if IF_SETTING_MAINMENU_IS_TABLEVIEW

#include "xwindow/xlistviewitem.h"
#include "xwindow/xtableview.h"

enum
{
    LIST_ITEM_TYPE_MAIN_MENU     = 0x15000000,
};

class xTextElement;
class xTimerPieceView;
class CMainMenuItem : public xListViewItem
{

public:
    explicit CMainMenuItem(IListViewItemAction* pAction = NULL);
    virtual ~CMainMenuItem();
    static int GetMainMenuItemType();
    static CMainMenuItem* GetMainMenuItem(xListViewItem* pItem);
    static xListViewItem* CreateMainMenuItem();
    virtual bool onFocusEvent(bool bFocusIn);

public:
    virtual void loadChildrenElements(xml_node& node);
    void SetStatusIcon(const yl::string& strIcon);
    void SetName(const yl::string& strName);

protected:
    xImageView*     m_pIcon;
    xTextView*      m_pName;

};
typedef CMainMenuItem* CMainMenuItemPtr;

#endif

#endif
