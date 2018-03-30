#ifndef CUI_REGION_TEXTSCREEN_H
#define CUI_REGION_TEXTSCREEN_H

#include "cuiregionbaselistscreen.h"
#include "xwindow/xlistviewadapter.h"

#if XML_BROWSER_SUPPORT_UI

typedef YLVector<yl::string> VEC_TEXT;


////////////////////////////////////////////////////////
class CUIRegionTextScreen : public CUIRegionBaseListScreen
{

public:
    CUIRegionTextScreen();
    ~CUIRegionTextScreen();

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

public:
    virtual void RelayoutByContent();

private:
    bool LoadMenuItemData(XBTextScreenData * pTextScreenData);
    void GetShowText(const yl::string & strText, VEC_TEXT & vecText);
    yl::string GetValueByIndex(int nIndex);
    int GetTotalPage();

private:
    VEC_TEXT            m_vecShowText;      //要显示内容
    int                 m_nSelect;
    yl::string          m_strTitle;
    int                 m_nCurrentPage;
};

#endif

#endif // CUI_REGION_TEXTSCREEN_H

