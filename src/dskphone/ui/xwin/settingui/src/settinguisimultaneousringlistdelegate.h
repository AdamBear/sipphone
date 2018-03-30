#ifndef _SETTINGUI_SIMULTANEOUS_RING_LIST_DELEGATE_H_
#define _SETTINGUI_SIMULTANEOUS_RING_LIST_DELEGATE_H_

#ifdef IF_FEATURE_BROADSOFT_PRIORITY_ALERT

#include "settinguixsidelegatebase.h"
#include "broadsoft/bwcallcontrol/include/bwcommon.h"

class CSettinguiSimultaneousRingListDelegate : public CSettinguiXSIDelegatebase
{
public:
    CSettinguiSimultaneousRingListDelegate();
    virtual ~CSettinguiSimultaneousRingListDelegate();

    static CSettingUIDelegateBase * CreateSimultaneousRingListDelegate();
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
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);
    virtual bool IsResetFocusWhileReload()
    {
        return true;
    }
protected:
    virtual void OnCancleClick();
    void OnLocationDelete(bool bDeleteAll);
    // 返回是否可以增加新的Location
    bool IsEnableAddLocation();
    void OnReturnLocationList();
    int GetItemRealIndex(int nIndex);
protected:
    SSimultaneousRing m_simRingInfo;
    bool m_bLocationListCanEidit;
};

#endif //IF_FEATURE_BROADSOFT_PRIORITY_ALERT
#endif //_SETTINGUI_SIMULTANEOUS_RING_LIST_DELEGATE_H_
