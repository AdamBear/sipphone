#ifndef __PRECISE_SEARCH_ADAPTER_H__
#define __PRECISE_SEARCH_ADAPTER_H__

#include "directoryadapter.h"

class CDetailLayout;
class CPreciseSearchControlBase;

class CPreciseSearchAdapter : public CDirectoryAdapter
{
public:
    CPreciseSearchAdapter();
    virtual ~CPreciseSearchAdapter();

    static CPreciseSearchAdapter * GetPreciseSearchAdapter(CDirectoryAdapterPtr & pAdapter);

public:
    // 初始化
    virtual bool Init(int nDirType, int nGroupId = knInvalidRId);

    virtual void UnInit();

    virtual bool IsShowPageInfo();

    virtual bool GetTitle(yl::string & strTitle);

	virtual bool ProcessKeyPress(int nKey);
    // 处理SoftKey
    virtual bool ProcessAction(int nAction);

    // 初始化列表框的布局
    virtual void InitListViewLayout(xListView * pListView);

    // 进行数据切换时进行数据索引校验，如果nDataIndex不是有效的数据，则返回false,并且将nDataIndex改为正确的值
    virtual bool PrecheckDataIndex(int & nDataIndex);
    // 根据位置获取列表项
    virtual xListViewItem * PrepareItem(xListView * pListView, int nDataIndex, xListViewItem * pItem);
    // 刷新列表项数据
    virtual bool RefreshItem(xListView * pListView, xListViewItem * pItem);

public:
    void EnterPreciseSearch();
    void SetSearchLayout(CDetailLayout* pSearchLayout);

    bool IsPreciseSearchResult();
    bool IsPreciseSearchCondition();

protected:
    int GetTotalItemCount();
    CPreciseSearchControlBase* GetPreciseSearchControll();

protected:
    CDetailLayout* m_pSearchLayout;
};

#endif
