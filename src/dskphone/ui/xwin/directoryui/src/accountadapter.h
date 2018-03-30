#ifndef __ACCOUNT_ADAPTER_H__
#define __ACCOUNT_ADAPTER_H__

#include "detailadapter.h"

class CAccountAdapter : public CDetailAdapter
{
public:
    CAccountAdapter();
    virtual ~CAccountAdapter();

public:
    static CAccountAdapter * GetAccountAdapter(CDirectoryAdapterPtr & pAdapter);

public:
    void EnterAccountChoose(bool bRegisterAccount);

public:
    virtual bool ProcessKeyPress(int nKey);
    virtual bool ProcessLineKeyPress(int nKey);
    virtual bool ProcessAction(int nAction);
    virtual bool GetTitle(yl::string & strTitle);
    virtual bool GetStateSoftKey(xListViewItem * pFocusItem, CArraySoftKeyBarData & vecSoftKey);
    virtual void InitListViewLayout(xListView * pListView);
    virtual xListViewItem * PrepareItem(xListView * pListView, int nDataIndex, xListViewItem * pItem);
    virtual bool RefreshItem(xListView * pListView, xListViewItem * pItem);
protected:
    yl::string GetAccountStates(int nStates, bool bDefault);
    void FocsuDefaultAccount();
    bool OnSelect();
    int GetItemType();
    bool RefreshMenuItem(xListView * pListView, xListViewItem * pItem);
    bool RefrehDetailItem(xListView * pListView, xListViewItem * pItem);
    int GetItemAccountID(xListViewItem* pItem);
protected:
    VEC_CONTACT_ID m_vecAccountId;
    bool m_bRegisterAccount;
};

#endif
