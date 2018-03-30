#ifndef _SETTINGUI_MAIL_CODE_LIST_DELEGATE_H_
#define _SETTINGUI_MAIL_CODE_LIST_DELEGATE_H_

#include "settinguidatadelegatebase.h"

class CVoiceMailCodeListDelegate : public CSettinguiDataDelegateBase
{
public:
    CVoiceMailCodeListDelegate();
    ~CVoiceMailCodeListDelegate();

    static CSettingUIDelegateBase * CreateVoiceMailCodeListDelegate();
public:
    static LRESULT OnMessage(msgObject & msg);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
protected:
    yl::string GetValueByIndex(int nIndex);
    int GetRealIndex(int nIndex);
    void LoadMailCodeItem(CSettingUIPageDataBase * pPageData);
};

#endif //_SETTINGUI_MAIL_CODE_LIST_DELEGATE_H_
