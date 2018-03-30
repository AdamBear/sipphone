#ifndef __USB_PHOTO_LIST_DELEGATE_H__
#define __USB_PHOTO_LIST_DELEGATE_H__

#include "settinguilogic/include/settingdefine.h"
#if defined(IF_USB_SUPPORT) && IF_SUPPORT_USB_PHOTO
#include "basesettinguidelegate.h"
#include "messagebox/messageboxcallback.h"
#include "record/include/modrecord.h"
#include "xwindow-kernel/include/xthreadtimer.h"

class CUSBPhotoListDelegate : public CSettingUIDelegateBase, public CMsgBoxCallBackAction, public xTimerHandler
{
public:
    CUSBPhotoListDelegate();
    virtual ~CUSBPhotoListDelegate();

    static CSettingUIDelegateBase* CreateUSBPhotoListDelegate();
    static LRESULT OnUSBPhotoMsg(msgObject& msg);

public:
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey);

public:
    virtual bool OnAction(int eActionType, int nDataIndex);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);

public:
    virtual void MessageBoxCallBack(CMessageBoxBase* pMessageBox);

protected:
    void AddPhotoFile(int nFileIndex, const RecordFileInfo& infoFile);
    void LoadPhotoData();
    void OnDelayLoadList(xThreadTimer* timer, LPVOID pExtraData);

private:
    xThreadTimer m_timerDelayLoad;
};
#endif // defined(IF_USB_SUPPORT) && IF_SUPPORT_USB_PHOTO

#endif // __USB_PHOTO_LIST_DELEGATE_H__
