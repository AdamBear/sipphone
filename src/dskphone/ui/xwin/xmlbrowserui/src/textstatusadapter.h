#ifndef _TEXTSTATUS_ADAPTER_H__
#define _TEXTSTATUS_ADAPTER_H__

#include "xwindow/xlistviewadapter.h"
#include "extended/xmlbrowser/include/logicanduicommon.h"
#include "xwindow/xGraphic.h"

#if XML_BROWSER_SUPPORT_UI

typedef YLVector<StatusItem>   VEC_STATUS_ITEM_DATA;

////////////////////////////////////////////////////////
class CTextStatusAdapter :  public xListViewAdapter
{

public:
    CTextStatusAdapter();
    ~CTextStatusAdapter();

    void SetData(const VEC_STATUS_ITEM_DATA & vecData);

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

    // 根据类型转换列表项高度
    int TypeToItemHeight(X_FONT_TYPE eFontSize);

private:
    VEC_STATUS_ITEM_DATA    m_vecItemData;      //要显示内容
};

#endif

#endif // _TEXTSTATUS_ADAPTER_H__

