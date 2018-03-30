#include "normaltitle.h"
#include "xwindow/xWindow.h"
#include "directoryuicommon.h"


CNormalTitle::CNormalTitle()
    : m_pPageInfo(NULL)
{

}

CNormalTitle::~CNormalTitle()
{

}

void CNormalTitle::SetTitleView(xView * pTitle)
{
    CTitleBase::SetTitleView(pTitle);

    if (NULL != m_pTitle)
    {
        m_pPageInfo = static_cast<xTextView *>(m_pTitle->getViewById(DIR_UI_ID_TITLE_PAGE_INFO));
    }
}

bool CNormalTitle::SetPageInfo(const yl::string & strPageInfo)
{
    if (NULL == m_pPageInfo || NULL == m_pTitle)
    {
        return false;
    }

    m_pPageInfo->SetText(strPageInfo);

    int nTextWrapLimited = m_pTitle->geometry().Width();

    if (strPageInfo.empty())
    {
        m_pPageInfo->hide(true);
    }
    else
    {
        m_pPageInfo->show();

        nTextWrapLimited = nTextWrapLimited - (m_pPageInfo->wrapFrameWidth() << 1);
    }

    if (NULL != m_pText)
    {
        m_pText->SetWrapLimited(nTextWrapLimited);
    }

    return true;
}
