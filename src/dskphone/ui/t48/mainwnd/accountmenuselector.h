
#ifndef __ACCOUNT_MENU_SELECTOR_H__
#define __ACCOUNT_MENU_SELECTOR_H__

#include "account/include/modaccount.h"
#include "baseui/framelistdelegatebase.h"
#include "baseui/ipopupmenudelegate.h"
#include "uikernel/ilistitemaction.h"

class CAccountMenuSelector : public CFrameListDelegateBase, public IPopupMenuDelegate,
    public IListItemAction
{
public:
    CAccountMenuSelector(void);
    virtual ~CAccountMenuSelector(void);

public:
    void Init();

    int GetAccountId();
    int GetAccountSize();

    void SetSelectIndexByAccountID(int iAccountID);

// CFrameListDelegateBase
protected:
    // 获取真正代理指针
    virtual IFrameListDelegatePtr GetDelegate() const;

// IFrameListDelegate
public:
    // 翻页时处理最后一页（如果处理完返回true）
    virtual bool OnLastItemNext();
    // 翻页时处理第一页（如果处理完返回true）
    virtual bool OnFirstItemPrevious();
    // 数据清空
    virtual void OnCleanContent(CFrameListBase * pFrameList);
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);
    // 是否已经绑定特定的列表框
    virtual bool IsFrameListAttach(CFrameListBase * pFrameList);
    // 数据代理解除绑定
    virtual void OnFrameListDetach(CFrameListBase * pFrameList);
    // 数据代理绑定
    virtual void OnFrameListAttach(CFrameListBase * pFrameList);

// IPopupMenuDelegate
public:
    // 获得Menu总数量
    virtual int GetTotalMenuCount();

// IListItemAction
public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

protected:
    int             m_nSelIndex;
    ListAccounts    m_listAccount;
};

#endif
