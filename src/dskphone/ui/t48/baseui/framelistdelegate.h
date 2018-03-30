
#ifndef __FRAME_LIST_DELEGATE_H__
#define __FRAME_LIST_DELEGATE_H__

#include "uikernel/iframelistdelegate.h"
#include "uikernel/ilistitemaction.h"
#include "baseui/framelistdelegatebase.h"

class CFrameListBase;

class CFrameListDelegate : public CFrameListDelegateBase, public IFrameListDelegate,
    public IListItemAction
{
protected:
    CFrameListDelegate(void);
    virtual ~CFrameListDelegate(void);

// IFrameListDelegate
public:
    // 翻页时处理最后一页（如果处理完返回true）
    virtual bool OnLastItemNext();
    // 翻页时处理第一页（如果处理完返回true）
    virtual bool OnFirstItemPrevious();
    // 数据清空
    virtual void OnCleanContent(CFrameListBase * pFrameList);
    // 是否已经绑定特定的列表框
    virtual bool IsFrameListAttach(CFrameListBase * pFrameList);
    // 数据代理解除绑定
    virtual void OnFrameListDetach(CFrameListBase * pFrameList);
    // 数据代理绑定
    virtual void OnFrameListAttach(CFrameListBase * pFrameList);

// IListItemAction
public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

protected:
    virtual void InitFrameList();
    virtual void UnInitFrameList();
    // 获取真正代理指针
    virtual IFrameListDelegatePtr GetDelegate() const;
};
typedef CFrameListDelegate * CFrameListDelegatePtr;

#endif
