#ifndef _SETTINGUI_DND_LINE_SELECT_DELEGATE_H_
#define _SETTINGUI_DND_LINE_SELECT_DELEGATE_H_

#include "basesettinguidelegate.h"

class CSettinguiDNDLineSelectDelegate : public CSettingUIDelegateBase
{
public:
    CSettinguiDNDLineSelectDelegate();
    virtual ~CSettinguiDNDLineSelectDelegate();

    static CSettingUIDelegateBase * CreateDNDLineSelectDelegate();
public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual bool OnAction(int eActionType, int iDataIndex);
protected:
    void EnterDndPage(int iDataIndex);
    void AddDNDAccountItem(const yl::string strDiap, int nAccountID, const yl::string & strValue);
    bool IsAccountAllOn();
    void SetAllItemValue(const yl::string & strValue);
};

#endif //_SETTINGUI_DND_LINE_SELECT_DELEGATE_H_
