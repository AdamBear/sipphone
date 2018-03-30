#ifndef __DETAIL_ADAPTER_H__
#define __DETAIL_ADAPTER_H__

#include "directorycommon.h"
#include "directoryadapter.h"


class CDetailLayout;

class CDetailAdapter : public CDirectoryAdapter
{
public:
    explicit CDetailAdapter();
    virtual ~CDetailAdapter();

    static CDetailAdapter * GetDetailAdapter(CDirectoryAdapterPtr & pAdapter);

public:
    virtual void UnInit();

    virtual bool GetTitle(yl::string & strTitle);

    virtual bool GetStateSoftKey(xListViewItem * pFocusItem, CArraySoftKeyBarData & vecSoftKey);

    // 处理普通按键
    virtual bool ProcessKeyPress(int nKey);
    // 处理数字键、#、*
    virtual bool ProcessDigitKeyPress(int nKey);
    // 处理LineKey
    virtual bool ProcessLineKeyPress(int nKey);
    // 处理SoftKey
    virtual bool ProcessAction(int nAction);

    void EnterDetail(int nAction);
    void SetDetailLayout(CDetailLayout * pDetailLayout);

    CDetailLayout * GetDetailLayout() const;

public:
    // 初始化列表框的布局
    virtual void InitListViewLayout(xListView * pListView);

    // 进行数据切换时进行数据索引校验，如果nDataIndex不是有效的数据，则返回false,并且将nDataIndex改为正确的值
    virtual bool PrecheckDataIndex(int & nDataIndex);
    // 根据位置获取列表项
    virtual xListViewItem * PrepareItem(xListView * pListView, int nDataIndex, xListViewItem * pItem);
    // 刷新列表项数据
    virtual bool RefreshItem(xListView * pListView, xListViewItem * pItem);

protected:
    bool ProcessDetailDial(int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    int GetTotalItemCount();

protected:
    CDetailLayout * m_pDetailLayout;
};

#endif
