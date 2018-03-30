#include "xlistviewtitle.h"


#if IF_XLISTVIEW_SUPPORT_TITLE
xListViewTitle::xListViewTitle(int nType, IListViewTitleAction* pAction/* = NULL*/)
    : m_nType(nType)
    , m_pAction(pAction)
    , m_pListView(NULL)
{
}

xListViewTitle::~xListViewTitle()
{
}

void xListViewTitle::SetAction(IListViewTitleAction* pAction)
{
    m_pAction = pAction;
}

void xListViewTitle::SetListView(xListView* pListView)
{
    m_pListView = pListView;
}

int xListViewTitle::GetType() const
{
    return m_nType;
}

int xListViewTitle::GetTitleHeight()
{
    if (!IsToBeRelayout() && !m_rectLayout.IsEmpty())
    {
        return m_rectLayout.Height();
    }

    chASSERT(match_parent != layout_height);

    if (wrap_content == layout_height)
    {
        return wrapContentHeight();
    }

    return layout_height;
}

void xListViewTitle::DoAction(int nAction)
{
    if (NULL == m_pAction || !isVisible())
    {
        return;
    }

    m_pAction->OnTitleAction(m_pListView, this, nAction);
}
#endif
