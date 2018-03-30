#ifndef _SETTINGUI_TIME_DATE_FORMAT_DELEGATE_H_
#define _SETTINGUI_TIME_DATE_FORMAT_DELEGATE_H_

#include "settinguidatadelegatebase.h"

class CSettingUIComboBoxItemData;

class CSettinguiTimeDataFormatDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettinguiTimeDataFormatDelegate();
    virtual ~CSettinguiTimeDataFormatDelegate();

    static CSettingUIDelegateBase * CreateTimeDataFormatDelegate();
public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
protected:
    void AddDataFormatOptionList();
    void AddDataFormatOption(int Index, const yl::string & strValue,
                             CSettingUIComboBoxItemData * pComboxBox);
};

#endif //_SETTINGUI_TIME_DATE_FORMAT_DELEGATE_H_
