#ifndef __NUMBER_ADAPTER_H__
#define __NUMBER_ADAPTER_H__

#include "detailadapter.h"

class CNumberAdapter : public CDetailAdapter
{
public:
    CNumberAdapter();
    virtual ~CNumberAdapter();

public:
    static CNumberAdapter * GetNumberAdapter(CDirectoryAdapterPtr & pAdapter);

public:
    void EnterNumberChoose();

public:
    virtual bool GetStateSoftKey(xListViewItem * pFocusItem, CArraySoftKeyBarData & vecSoftKey);

    // 初始化列表框的布局
    virtual void InitListViewLayout(xListView * pListView);

    // 进行数据切换时进行数据索引校验，如果nDataIndex不是有效的数据，则返回false,并且将nDataIndex改为正确的值
    virtual bool PrecheckDataIndex(int & nDataIndex);

    // 刷新列表项数据
    virtual bool RefreshItem(xListView * pListView, xListViewItem * pItem);
};

#endif
