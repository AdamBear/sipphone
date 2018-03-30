#ifndef _SETTINGUI_PHONE_UNLOCK_CHECK_DELEGATE_H_
#define _SETTINGUI_PHONE_UNLOCK_CHECK_DELEGATE_H_

#include "settinguidatadelegatebase.h"
#include <settingui/include/settinguidefine.h>
#if !IF_UNLOCK_CHECK_POPBOX

class CSettinguiPhoneUnlockCheckDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettinguiPhoneUnlockCheckDelegate();
    virtual ~CSettinguiPhoneUnlockCheckDelegate();

    static CSettingUIDelegateBase * CreatePhoneUnlockCheckDelegate();
public:
    static LRESULT OnMessage(msgObject & msg);
    virtual bool ProcessMsgCallBack(int nResult, int nID, void * pExtraData = NULL);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual bool OnAction(int eActionType, int iDataIndex);
    virtual void InitExtraData(void * pData);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);

    bool IsEnterBySetting();

protected:
    bool IsPingRight();
private:
    bool m_bEnterBySetting;
};
#endif

#endif //_SETTINGUI_PHONE_UNLOCK_CHECK_DELEGATE_H_
