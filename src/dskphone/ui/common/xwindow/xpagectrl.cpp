#include "xpagectrl.h"

namespace
{
#define PAGE_NO_DEFAULT 0
#define PAGE_COUNT_DEFAULT 1
}

#if IF_XLISTVIEW_SCROLL_BY_PAGE_CTRL
xPageCtrl::xPageCtrl(IScrollNotify* pScrollNotify)
    : m_pScrollNotify(pScrollNotify)
    , m_nPageNo(PAGE_NO_DEFAULT)
    , m_nPageCount(PAGE_COUNT_DEFAULT)
{
    ReLayout();
}

xPageCtrl::~xPageCtrl()
{

}

void xPageCtrl::SetScrollCtrlNotify(IScrollNotify* pScrollNotify)
{
    m_pScrollNotify = pScrollNotify;
}

void xPageCtrl::SetRect(const chRect& rect)
{
    setGeometry(rect);

    ReLayout();
    update();
}

void xPageCtrl::SetScrollInfo(int nPageRange, int nRange, int nPos)
{
    if (nRange < PAGE_COUNT_DEFAULT)
    {
        nRange = PAGE_COUNT_DEFAULT;
    }

    if (nPos < PAGE_NO_DEFAULT)
    {
        nPos = PAGE_NO_DEFAULT;
    }

    m_nPageCount = nRange;
    m_nPageNo = nPos;

    char szBuffer[32];

    snprintf(szBuffer, sizeof(szBuffer), "%d/%d", (m_nPageNo + 1), m_nPageCount);
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    m_strPageInfo = szBuffer;
}

void xPageCtrl::SetScrollPos(int nPos)
{
    if (nPos < PAGE_NO_DEFAULT)
    {
        nPos = PAGE_NO_DEFAULT;
    }

    m_nPageNo = nPos;

    char szBuffer[32];

    snprintf(szBuffer, sizeof(szBuffer), "%d/%d", (m_nPageNo + 1), m_nPageCount);
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    m_strPageInfo = szBuffer;

    update();
}

int xPageCtrl::GetPageNo()
{
    return m_nPageNo;
}

int xPageCtrl::GetPageCount()
{
    return m_nPageCount;
}

void xPageCtrl::Relayout()
{
}
#endif
