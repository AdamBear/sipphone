#ifndef _DLG_BOOT_H_
#define _DLG_BOOT_H_

#ifdef IF_SETTING_WIZARD

#include "dlgsettingui.h"
#include "settinguiwizardtitle.h"

class CDlgBoot : public CDlgSettingUI
{
public:
    static CBaseDialog * CreateInstance();

public:
    CDlgBoot();
    virtual ~CDlgBoot();

public:
    virtual void loadChildrenElements(xml_node & node);
    //刷新Title
    virtual void RefreshTitle(const yl::string & strTitle);

protected:
    CSettingUIWizardlTitle          m_GuideTitle;
};

#endif //IF_SETTING_WIZARD
#endif //_DLG_BOOT_H_
