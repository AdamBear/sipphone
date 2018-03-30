#ifndef _MTSW_ACD_MYSTATUS_DELEGATE_H_
#define _MTSW_ACD_MYSTATUS_DELEGATE_H_

#if IF_FEATURE_METASWITCH_ACD
#include "ylvector.h"
#include "settinguilogic/include/common.h"
#include "settinguidatadelegatebase.h"
#include "messagebox/messageboxcallback.h"
#include "broadsoft/callcenter/include/cccommon.h"

typedef YLList<yl::string>      LIST_STATUS;

class CMtswAcdMyStatusDelegate : public CSettinguiDataDelegateBase
{
public:
    CMtswAcdMyStatusDelegate();
    virtual ~CMtswAcdMyStatusDelegate();
    static CSettingUIDelegateBase* CreatMtswAcdMyStatusDelegate();

public:
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);

protected:
    void LoadMTSWStatus();
    void AddRadioListItem(const yl::string& strName, const yl::string& strValue);
    void OnSetAcdStatus();
};
#endif

#endif //_MTSW_ACD_MYSTATUS_DELEGATE_H_
