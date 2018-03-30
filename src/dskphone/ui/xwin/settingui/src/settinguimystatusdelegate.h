#ifndef _SETTINGUI_MY_STATUS_DELEGATE_H_
#define _SETTINGUI_MY_STATUS_DELEGATE_H_

#include "settinguidefine.h"
#if FEATURE_BROADTOUCH_UC
#include "settinguidatadelegatebase.h"

class CSettingUIMyStatusDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettingUIMyStatusDelegate();
    ~CSettingUIMyStatusDelegate();

    static CSettingUIDelegateBase* CreateSettingUIMyStatusDelegate();

public:
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
    // 保存配置数据
    virtual bool SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual bool ProcessMsgCallBack(int nResult, int nID, void* pExtraData = NULL);
private:
    int GetPresenceStatus();

protected:
    void SetCheckedByIndex(int nDataIndex);
    void SetItemCheckByDataIndex(int nDataIndex, bool bCheck);

private:
    bool        m_bPresenceChange;
    int         m_nCheckIndex;
};
#endif

#endif //_SETTINGUI_MY_STATUS_DELEGATE_H_
