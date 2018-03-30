#ifndef CUI_REGION_BASE_LIST_SCREEN_H
#define CUI_REGION_BASE_LIST_SCREEN_H

#include "cuiregionbase.h"
#include "xwindow/xlistviewadapter.h"

#if XML_BROWSER_SUPPORT_UI

struct ST_BASE_SCREEN_DATA
{
    bool        m_bIsWrapTitle;
    yl::string  m_strTitle;
    int         m_nDefaultIndex;    // 默认选中的索引
    bool        m_bIsDefaultIndex;  // 是否使用默认索引

    ST_BASE_SCREEN_DATA()
    {
        Reset();
    }

    void Reset()
    {
        m_bIsWrapTitle = false;
        m_strTitle = "";
        m_nDefaultIndex = 0;
        m_bIsDefaultIndex = true;
    }
};

////////////////////////////////////////////////////////
class CUIRegionBaseListScreen : public CUIRegionBase
    , public xListViewAdapter
{

public:
    CUIRegionBaseListScreen(bool bLoadXml = true);
    ~CUIRegionBaseListScreen();

    virtual int GetCurrentItemIndex();
    virtual void loadChildrenElements(xml_node & node);

public:
    // 列表项Action处理
    virtual void OnItemAction(xListView* pListView, xListViewItem* pItem, int nAction);
    // 初始化列表框的布局
    virtual void InitListViewLayout(xListView* pListView);
    virtual void LayoutContentInRect(const chRect& rectParentContent);
    virtual int GetItemHeight();

public:
    virtual void UpdateContent();
    virtual void RelayoutByContent();

protected:
    xTextView       *       m_pTxtTitle;
    xListView       *       m_pListView;
    xScrollBar       *      m_pScroll;
    ST_BASE_SCREEN_DATA     m_stBaseData;       // 基础界面数据
};

#endif

#endif // CUI_REGION_BASE_LIST_SCREEN_H

