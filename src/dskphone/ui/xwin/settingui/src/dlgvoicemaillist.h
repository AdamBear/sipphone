#ifndef _DLG_VOICE_MAIL_LIST_H_
#define _DLG_VOICE_MAIL_LIST_H_

#if IF_FEATURE_BROADSOFT_VOICEMAIL

#include "dlgsettingui.h"
#include "settinguiwizardtitle.h"

class CDlgVoiceMailList : public CDlgSettingUI
{
public:
    static CBaseDialog * CreateInstance();

public:
    CDlgVoiceMailList();
    virtual ~CDlgVoiceMailList();

public:
    virtual void loadChildrenElements(xml_node & node);
    //刷新Title
    virtual void RefreshTitle(const yl::string & strTitle);
    virtual bool onActiveEvent(bool bActive);

protected:
    CSettingUIWizardlTitle          m_VoiceMailTitle;
};

#endif //IF_FEATURE_BROADSOFT_VOICEMAIL
#endif //_DLG_VOICE_MAIL_LIST_H_
