#include "settinguiwizardtitle.h"
#include "xwindow/xWindow.h"

namespace
{
const char * SETTINGUI_TITLE_PAGE_INFO = "txtPageInfo";
const char * SETTINGUI_TITLE_TEXT_GUIDE = "txtGuide";
}

CSettingUIWizardlTitle::CSettingUIWizardlTitle()
    : m_pPageInfo(NULL)
    , m_pTextGuide(NULL)
{

}

CSettingUIWizardlTitle::~CSettingUIWizardlTitle()
{

}

void CSettingUIWizardlTitle::SetTitleView(xView * pTitle)
{
    CTitleBase::SetTitleView(pTitle);

    if (NULL != m_pTitle)
    {
        m_pPageInfo = static_cast<xTextView *>(m_pTitle->getViewById(SETTINGUI_TITLE_PAGE_INFO));
        m_pTextGuide = static_cast<xTextView *>(m_pTitle->getViewById(SETTINGUI_TITLE_TEXT_GUIDE));
    }
}

void CSettingUIWizardlTitle::SetPageInfo(const yl::string & strPageInfo)
{
    if (NULL != m_pPageInfo)
    {
        m_pPageInfo->SetText(strPageInfo);
    }
}

void CSettingUIWizardlTitle::SetGuideInfo(const yl::string & strWizard)
{
    if (NULL != m_pTextGuide)
    {
        m_pTextGuide->SetText(strWizard);
    }
}
