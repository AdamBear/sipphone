#include "baseui/framelist.h"
#include "framelistdelegate.h"

CFrameListDelegate::CFrameListDelegate(void)
{
}

CFrameListDelegate::~CFrameListDelegate(void)
{
}

bool CFrameListDelegate::OnLastItemNext()
{
    return false;
}

bool CFrameListDelegate::OnFirstItemPrevious()
{
    return false;
}

void CFrameListDelegate::OnCleanContent(CFrameListBase * pFrameList)
{

}

bool CFrameListDelegate::IsFrameListAttach(CFrameListBase * pFrameList)
{
    return IsAttach(pFrameList);
}

void CFrameListDelegate::OnFrameListDetach(CFrameListBase * pFrameList)
{
    UnInitFrameList();
    Detach(pFrameList);
}

void CFrameListDelegate::OnFrameListAttach(CFrameListBase * pFrameList)
{
    if (NULL != m_pFrameList)
    {
        UnInitFrameList();
        SetFrameList(NULL);
    }

    Attach(pFrameList);
    InitFrameList();
}

void CFrameListDelegate::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
{
    if (NULL == pFrameList || pFrameList != m_pFrameList || NULL == pItem)
    {
        return;
    }

    if (LIST_ITEM_ACTION_FOCUS == nAction)
    {
        SetFocusIndex(m_pFrameList->GetFocusItemIndex());
    }
}

void CFrameListDelegate::InitFrameList()
{

}

void CFrameListDelegate::UnInitFrameList()
{

}

IFrameListDelegatePtr CFrameListDelegate::GetDelegate() const
{
    return (IFrameListDelegatePtr)this;
}
