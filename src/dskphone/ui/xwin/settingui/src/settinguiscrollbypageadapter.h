#ifndef _SETTINGUI_SCROLL_BY_PAGE_ADAPTER_
#define _SETTINGUI_SCROLL_BY_PAGE_ADAPTER_

#include "settinguiadapter.h"

class CSettinguiScrollByPageAdapter : public CSettingUIAdapter
{
public:
    CSettinguiScrollByPageAdapter();
    virtual ~CSettinguiScrollByPageAdapter();
    static CSettingUIAdapter * CreateScrollByPageAdapter();
public:
    // 初始化列表框的布局
    virtual void InitListViewLayout(xListView * pListView);
    // 根据位置获取列表项
    virtual xListViewItem * PrepareItem(xListView * pListView, int nDataIndex, xListViewItem * pItem);
    // 刷新列表项数据
    virtual bool RefreshItem(xListView * pListView, xListViewItem * pItem);
protected:
    bool IsScrollBypageDelegate();
};

#endif //_SETTINGUI_SCROLL_BY_PAGE_ADAPTER_
