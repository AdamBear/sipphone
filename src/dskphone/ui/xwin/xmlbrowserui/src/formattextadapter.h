#ifndef _FORMAT_TEXT_ADAPTER_H__
#define _FORMAT_TEXT_ADAPTER_H__

#include "xwindow/xlistviewadapter.h"
#include "extended/xmlbrowser/include/logicanduicommon.h"
#include "xwindow/xGraphic.h"

#if XML_BROWSER_SUPPORT_UI

struct stScrollData
{
    yl::string  m_strText;
    int         nAlgin;

    int         eFont;      // 字体大小
    xColor      m_Color;    // 字体颜色

    stScrollData()
    {
        Reset();
    }

    void Reset()
    {
        m_strText = "";
        nAlgin = 0;
        eFont = 18;
        m_Color = xColor(255, 255, 255);
    }
};

typedef YLVector<stScrollData>   VEC_SCROLL_ITEM_DATA;

////////////////////////////////////////////////////////
class CFormatTextAdapter :  public xListViewAdapter
{

public:
    CFormatTextAdapter();
    ~CFormatTextAdapter();

    void SetData(const VEC_SCROLL_ITEM_DATA & vecData);

public:
    // 初始化列表框的布局
    virtual void InitListViewLayout(xListView * pListView);

    // 根据位置获取列表项
    virtual xListViewItem * PrepareItem(xListView * pListView, int nDataIndex, xListViewItem * pItem);

    // 刷新列表项数据
    virtual bool RefreshItem(xListView * pListView, xListViewItem * pItem);

    // 获取列表高度
    virtual bool GetItemHeight(xListView * pListView, int nDataIndex, int& nItemHeight);

public:
    // 列表项Action处理
    virtual void OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction);

private:
    VEC_SCROLL_ITEM_DATA    m_vecItemData;      //要显示内容
};

#endif

#endif // _FORMAT_TEXT_ADAPTER_H__

