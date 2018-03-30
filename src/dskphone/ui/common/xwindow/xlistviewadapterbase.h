#ifndef __X_LIST_VIEW_ADAPTER_BASE_H__
#define __X_LIST_VIEW_ADAPTER_BASE_H__

#include <xwindow/xlistviewdefine.h>
#include "ilistviewadapter.h"


class xListView;

class xListViewAdapterBase
{
public:
    xListViewAdapterBase();
    virtual ~xListViewAdapterBase();

public:
    // 设置代理的列表框
    void SetListView(xListView* pListView);

    // 获取代理
    xListView* GetListView() const;

#if IF_XLISTVIEW_SWITCH_ITEM_FOCUS
    int AdjustPageDataIndex(int nTotalCount, int nDataIndex);

    // 设置起始数据序号
    void SetTotalCount(int nTotalCount);
    void SetPageDataIndex(int nDataIndex);
    void SetContentPos(int nContentPos);

    void ResetIndex();
    void UpdateIndex();

    int GetTotalCount() const;
    int GetPageDataIndex() const;
#endif

    // 设置焦点序号
    void SetFocusIndex(int nIndex);

    int GetFocusIndex() const;

    // 刷新列表框
    virtual void RefreshListView();

protected:
    // 是否绑定列表框
    bool IsAttach(xListView* pListView);
    // 绑定列表框
    void Attach(xListView* pListView);
    // 取消绑定列表框
    void Detach(xListView* pListView);

    void ClearListView();

    // 获取真正代理指针
    virtual IListViewAdapterPtr GetAdapter() const          = 0;
    virtual void InitListView();
    virtual void UnInitListView();

public:
    // 释放
    bool Release();

protected:
    // 列表框实例指针
    xListView* m_pListView;
    // 列表框起始数据序号
    int m_nPageDataIndex;
    int m_nContentPos;
    // 数据总数量
    int m_nTotalCount;
    // 焦点序号
    int m_nFocusIndex;
};

#endif
