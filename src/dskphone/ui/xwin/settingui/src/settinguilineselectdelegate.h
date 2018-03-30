#ifndef _SETTINGUI_LINE_SELECT_DELEGATE_H_
#define _SETTINGUI_LINE_SELECT_DELEGATE_H_

#include "settinguixsidelegatebase.h"

class CSettinguiLineSelectDelegate : public CSettinguiXSIDelegatebase
{
public:
    CSettinguiLineSelectDelegate();
    virtual ~CSettinguiLineSelectDelegate();

    static CSettingUIDelegateBase * CreateLineSelectDelegate();
public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool OnAction(int eActionType, int iDataIndex);
    virtual void InitExtraData(void * pData);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    yl::string GetNextPageID()
    {
        return m_strNextPageID;
    }
protected:
    void AddAccountItem(const yl::string & strName, const yl::string & strID);
    void LoadWin32ListAccount();
    void LoadNormalLstAccount();
protected:
    yl::string m_strNextPageID;
};

#endif //_SETTINGUI_LINE_SELECT_DELEGATE_H_
