#ifndef _SETTINGUI_VOICE_MAIL_ADD_TO_PERSONAL_DELEGATE_H_
#define _SETTINGUI_VOICE_MAIL_ADD_TO_PERSONAL_DELEGATE_H_

#include "settinguidatadelegatebase.h"

#if IF_FEATURE_BROADSOFT_VOICEMAIL
class CSettinguiVoiceMailAddToPersonalDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettinguiVoiceMailAddToPersonalDelegate();
    ~CSettinguiVoiceMailAddToPersonalDelegate();

    static CSettingUIDelegateBase * CreateVoiceMailAddToPersonalDelegate();
public:
    virtual void InitExtraData(void * pData);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
protected:
    void SetValidInfo(const yl::string & strNote, int nIndex);
protected:
    int m_nVoiceMailIndex;
};

#endif //#if IF_FEATURE_BROADSOFT_VOICEMAIL
#endif //_SETTINGUI_VOICE_MAIL_ADD_TO_PERSONAL_DELEGATE_H_
