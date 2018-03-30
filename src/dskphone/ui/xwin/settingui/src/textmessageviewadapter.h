#ifndef _TEXT_MESSAGE_VIEW_ADAPTER_H_
#define _TEXT_MESSAGE_VIEW_ADAPTER_H_

#include "settinguiadapter.h"

class CTextMessageViewAdapter : public CSettingUIAdapter
{
public:
    CTextMessageViewAdapter();
    virtual ~CTextMessageViewAdapter();
    static CSettingUIAdapter * CreateTextMessageViewAdapter();
public:
    // 初始化列表框的布局
    virtual void InitListViewLayout(xListView * pListView);
    // 根据位置获取列表项
    virtual xListViewItem * PrepareItem(xListView * pListView, int nDataIndex, xListViewItem * pItem);
    // 刷新列表项数据
    virtual bool RefreshItem(xListView * pListView, xListViewItem * pItem);
};

#endif //_TEXT_MESSAGE_VIEW_ADAPTER_H_
