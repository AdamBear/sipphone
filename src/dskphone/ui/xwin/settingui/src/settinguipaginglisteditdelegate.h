#ifndef _SETTINGUI_PAGING_LIST_EDIT_DELEGATE_H
#define _SETTINGUI_PAGING_LIST_EDIT_DELEGATE_H

#include "settinguiaccountbasedelegate.h"

class CSettingUIDataValidInfo;

class CSettinguiPagingListEditDelegate : public CSettingUIAccountDelegateBase
{
public:
    CSettinguiPagingListEditDelegate();
    ~CSettinguiPagingListEditDelegate();
    static CSettingUIDelegateBase * CreatPagingListEditDelegate();

public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
protected:
    bool IsInputDataValid(const yl::string & strAddress, const yl::string & strLabel,
                          CSettingUIDataValidInfo & validInfo);
};

#endif //_SETTINGUI_PAGING_LIST_EDIT_DELEGATE_H
