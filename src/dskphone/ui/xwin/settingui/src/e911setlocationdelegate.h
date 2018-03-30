#ifndef _SET_LOCATION_STATUS_DELEGATE_H_
#define _SET_LOCATION_STATUS_DELEGATE_H_

#if IF_FEATURE_GENBAND_E911
#include "basesettinguidelegate.h"
#include "baseui/include/baseuicommon.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "e911address/include/e911address_def.h"

typedef YLVector<int>               VEC_INT;

class CE911SetLocationDelegate : public CSettingUIDelegateBase
{
public:
    CE911SetLocationDelegate();
    virtual ~CE911SetLocationDelegate();
    static CSettingUIDelegateBase* CreateE911SetLocationDelegate();
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);
    static LRESULT OnE911LocationMsg(msgObject& msg);
public:
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad /*= false*/);
    virtual bool SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    // 处理弹出框回调
    virtual bool ProcessMsgCallBack(int nResult, int nID, void* pExtraData = NULL);

private:
    int GetFocusItemID();
    void LoadAddressList(int nId);
    void NextPage();
    void BackToPrePage();
    bool GetPreFocusIndex(int& nFocusIndex);
    bool IsLastAddress();

private:
    VEC_INT                 m_vecFocusIndex;
    int                     m_nId;
};
#endif

#endif //_SET_LOCATION_STATUS_DELEGATE_H__