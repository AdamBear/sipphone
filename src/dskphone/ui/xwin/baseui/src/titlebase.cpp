#include "titlebase.h"
#include "xwindow/xWindow.h"
#include "baseui/include/baseuicommon.h"


CTitleBase::CTitleBase()
    : m_pTitle(NULL)
    , m_pText(NULL)
{
}

CTitleBase::~CTitleBase()
{
}

xView * CTitleBase::GetTitleView() const
{
    return m_pTitle;
}

void CTitleBase::SetTitleView(xView * pTitle)
{
    m_pTitle = pTitle;

    if (NULL == m_pTitle)
    {
        m_pText = NULL;

        return;
    }

    m_pText = static_cast<xTextView *>(m_pTitle->getViewById(BASE_UI_ID_TEXT_TITLE));
}

bool CTitleBase::SetTitle(const yl::string & strTitle)
{
    if (NULL == m_pText)
    {
        return false;
    }

    m_pText->SetText(strTitle);

    return true;
}

bool CTitleBase::Show()
{
    if (NULL == m_pTitle)
    {
        return false;
    }

    m_pTitle->show();

    return true;
}

bool CTitleBase::Hide()
{
    if (NULL == m_pTitle)
    {
        return false;
    }

    m_pTitle->hide(true);

    return true;
}

bool CTitleBase::IsVisible() const
{
    if (NULL == m_pTitle)
    {
        return false;
    }

    return m_pTitle->isVisible();
}
