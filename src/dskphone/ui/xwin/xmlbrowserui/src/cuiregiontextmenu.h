#ifndef CUI_REGION_TEXT_MENU_H
#define CUI_REGION_TEXT_MENU_H

#include "cuiregionbaselistscreen.h"
#include "xwindow/xlistviewadapter.h"

#if XML_BROWSER_SUPPORT_UI

struct TextMenuItemData
{
    bool        m_bIsWrap;
    bool        m_bShowIndex;
    yl::string  m_strText;
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    VecSoftkeyParam m_vecSoftkey;
#endif

    TextMenuItemData()
    {
        Reset();
    }

    void Reset()
    {
        m_bIsWrap = false;
        m_bShowIndex = false;
        m_strText = "";
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
        m_vecSoftkey.clear();
#endif
    }
};

typedef YLVector<TextMenuItemData>   VEC_MENU_ITEM_DATA;

////////////////////////////////////////////////////////
class CUIRegionTextMenu : public CUIRegionBaseListScreen
{

public:
    CUIRegionTextMenu();
    ~CUIRegionTextMenu();

    // 必须实现的函数
    virtual void Update(void * pData);

    virtual void SetDefaultSoftkey(VecSoftkeyParam & softkeyParam);

    // 处理按键的事件
    virtual bool OnKeyPress(int nKey);

public:
    // 初始化列表框的布局
    virtual void InitListViewLayout(xListView * pListView);

    // 根据位置获取列表项
    virtual xListViewItem * PrepareItem(xListView * pListView, int nDataIndex, xListViewItem * pItem);

    // 刷新列表项数据
    virtual bool RefreshItem(xListView * pListView, xListViewItem * pItem);

private:
    bool LoadMenuItemData(XBTextMenuData * pMenuData);
    const VecSoftkeyParam & GetFocusSoftkey();
    const VecSoftkeyParam & GetSoftkeyByIndex(int nIndex);

private:
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    VecSoftkeyParam m_vecGlobalSoftkey;
#endif
    VEC_MENU_ITEM_DATA      m_vecItemData;      //要显示内容
};

#endif

#endif // CUI_REGION_TEXT_MENU_H

