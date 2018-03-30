#ifndef _AUTOANSWER_DELEGATE_H_
#define _AUTOANSWER_DELEGATE_H_

#include "settinguiaccountbasedelegate.h"

class CSettingUIComboBoxItemData;

class CAutoAnswerDelegate : public CSettingUIAccountDelegateBase
{
public:
    CAutoAnswerDelegate();
    ~CAutoAnswerDelegate();
    static CSettingUIDelegateBase * CreateAutoAnswerDelegate();

public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual void OnComboBoxValueChange(int nIndex);
private:
    bool AddLineIDOption(CSettingUIComboBoxItemData * pCmbItemData, const yl::string & strDisp,
                         int nLineID);
    void LoadLineIDItem();
};

#endif //_AUTOANSWER_DELEGATE_H_
