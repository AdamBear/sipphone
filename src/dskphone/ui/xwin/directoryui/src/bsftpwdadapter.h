#ifndef __BSFT_PWD_ADAPTER_H__
#define __BSFT_PWD_ADAPTER_H__

#include "directoryadapter.h"

class CDetailListItem;
class CEditListItem;

class CBsftPwdAdapter : public CDirectoryAdapter
{
public:
    CBsftPwdAdapter();
    virtual ~CBsftPwdAdapter();

public:
    static CBsftPwdAdapter * GetBsftPwdAdapter(CDirectoryAdapterPtr & pAdapter);

public:
    // 恢复初始化
    virtual void UnInit();

    virtual bool GetTitle(yl::string & strTitle);

    virtual bool GetStateSoftKey(xListViewItem * pFocusItem, CArraySoftKeyBarData & vecSoftKey);

    // 初始化列表框的布局
    virtual void InitListViewLayout(xListView * pListView);

    // 进行数据切换时进行数据索引校验，如果nDataIndex不是有效的数据，则返回false,并且将nDataIndex改为正确的值
    virtual bool PrecheckDataIndex(int & nDataIndex);
    // 根据位置获取列表项
    virtual xListViewItem * PrepareItem(xListView * pListView, int nDataIndex, xListViewItem * pItem);
    // 刷新列表项数据
    virtual bool RefreshItem(xListView * pListView, xListViewItem * pItem);

    virtual bool ProcessKeyPress(int nKey);
    virtual bool ProcessLineKeyPress(int nKey);
    virtual bool ProcessAction(int nAction);

    void EnterPwdSetting();

protected:
    void ReleaseListItem();

protected:
    CDetailListItem * m_pUserItem;
    CEditListItem * m_pPwdItem;
};

#endif
