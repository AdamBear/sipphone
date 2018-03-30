#ifndef _SETTINGUI_STATUS_RTP_DELEGATE_H_
#define _SETTINGUI_STATUS_RTP_DELEGATE_H_

#include "settinguiscrollbypagedelegatebase.h"

class CSettinguiStatusRtpDelegate : public CSettinguiScrollByPageDelegateBase
{
public:
    CSettinguiStatusRtpDelegate();
    virtual ~CSettinguiStatusRtpDelegate();
    static CSettingUIDelegateBase * CreatStatusRtpDelegate();
public:
    // 打开界面附加操作
    virtual void InitExtraData(void * pData);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
protected:
    void GetShowText(VEC_TEXT & vecText);
};

#endif //_SETTINGUI_STATUS_RTP_DELEGATE_H_
