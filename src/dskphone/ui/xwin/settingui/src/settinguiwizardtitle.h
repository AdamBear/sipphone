#ifndef __NORMAL_TITLE_H__
#define __NORMAL_TITLE_H__

#include "baseui/include/titlebase.h"

class CSettingUIWizardlTitle : public CTitleBase
{
public:
    CSettingUIWizardlTitle();
    virtual ~CSettingUIWizardlTitle();

public:
    virtual void SetTitleView(xView * pTitle);

    void SetPageInfo(const yl::string & strPageInfo);
    void SetGuideInfo(const yl::string & strWizard);

protected:
    xTextView * m_pPageInfo;
    xTextView * m_pTextGuide;
};

#endif
