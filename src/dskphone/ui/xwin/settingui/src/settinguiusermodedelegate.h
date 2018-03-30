#ifndef _SETTINGUI_USERMODE_DELEGATE_H_
#define _SETTINGUI_USERMODE_DELEGATE_H_

#include "settinguidatadelegatebase.h"

class CSettinguiUserModeDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettinguiUserModeDelegate();
    virtual ~CSettinguiUserModeDelegate();

    static CSettingUIDelegateBase * CreateUserModeDelegate();
public:
    virtual bool ProcessMsgCallBack(int nResult, int nID, void* pExtraData = NULL);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
protected:
    void LoadUserTypeData(CSettingUIPageDataBase * pPageData);
    void AddUserTypeOption(CSettingUIPageDataBase * pPageData, const yl::string & strValue,
                           const yl::string & strCurrentName);
    void SetValidInfo(const yl::string strNote);
};

#endif //_SETTINGUI_USERMODE_DELEGATE_H_
