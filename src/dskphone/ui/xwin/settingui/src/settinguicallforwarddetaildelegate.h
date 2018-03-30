#ifndef __SETTINGUI_CALL_FORWARD_DETAIL_DELEGATE_H__
#define __SETTINGUI_CALL_FORWARD_DETAIL_DELEGATE_H__

#include "settinguiaccountbasedelegate.h"

class CSettingUICallForwardDetailDelegate : public CSettingUIAccountDelegateBase
{
public:
    CSettingUICallForwardDetailDelegate();
    virtual ~CSettingUICallForwardDetailDelegate();

    static CSettingUIDelegateBase * CreateCallForwardDetailDelegate();
#if IF_SUPPORT_SELECT_CONTACT_INFO
public:
    static void UpdateContactInfo(const yl::string& strContactName, const yl::string& strContactNumber);
protected:
    void UpdateCallforwardData(const yl::string& strContactNumber);
    virtual bool IsShowDirectorySoftkey();
#endif
public:
    virtual bool ProcessMsgCallBack(int nResult, int nID, void * pExtraData = NULL);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);

private:
    void LoadPrivateData(CSettingUIPageDataBase * pPagedata);
    int GetTypeById(CSettingUIPageDataBase * pPageData);

protected:
    int m_nForwardType;
};

typedef CSettingUICallForwardDetailDelegate * CSettingUICallForwardDetailDelegatePtr;

#endif // __SETTINGUI_CALL_FORWARD_DETAIL_DELEGATE_H__
