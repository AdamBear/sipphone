#ifndef _SETTINGUI_LOCATION_LIST_DELEGATE_H_
#define _SETTINGUI_LOCATION_LIST_DELEGATE_H_

#ifdef IF_FEATURE_BROADSOFT_ANYWHERE

#include "settinguixsidelegatebase.h"
#include "broadsoft/anywhere/include/anywherecommon.h"

class CSettinguiLocationListDelegate : public CSettinguiXSIDelegatebase
{
public:
    CSettinguiLocationListDelegate();
    virtual ~CSettinguiLocationListDelegate();

    static CSettingUIDelegateBase * CreateLocationListDelegate();
public:
    static LRESULT OnMessage(msgObject & msg);
    void OnMessageReqFinish(msgObject & msg);
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    virtual void InitExtraData(void * pData);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual void OnEnterPage(const yl::string & strPage, int iDataIndex);
    virtual bool OnPageAction(int eActionType, int iDataIndex);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual bool IsResetFocusWhileReload()
    {
        return true;
    }
protected:
    virtual void OnCancleClick();
    void OnLocationDelete(bool bDeleteAll);
    void LoadLocationListItem();
    // 返回是否可以增加新的Location
    bool IsEnableAddLocation() const;
    void OnReturnLocationList(msgObject & msg);
    void OnDeleteFinish(msgObject & msg);
    int GetItemRealIndex(int nIndex);
protected:
    //Location数据
    SAYWLocationArray m_LocationArry;
    bool m_bLocationListCanEidit;
};

#endif //IF_FEATURE_BROADSOFT_ANYWHERE
#endif //_SETTINGUI_LOCATION_LIST_DELEGATE_H_
