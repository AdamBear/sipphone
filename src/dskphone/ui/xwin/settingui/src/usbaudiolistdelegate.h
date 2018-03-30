#ifndef __USB_AUDIO_LIST_DELEGATE_H__
#define __USB_AUDIO_LIST_DELEGATE_H__

#include "basesettinguidelegate.h"
#include "messagebox/messageboxcallback.h"
#ifdef IF_USB_SUPPORT
#include "record/include/modrecord.h"
#endif


#ifdef IF_USB_SUPPORT
class CUSBAudioListDelegate : public CSettingUIDelegateBase, public CMsgBoxCallBackAction
{
public:
    CUSBAudioListDelegate();
    virtual ~CUSBAudioListDelegate();

    static CSettingUIDelegateBase * CreateUSBAudioListDelegate();

public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);

public:
    virtual bool OnAction(int eActionType, int nDataIndex);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);

public:
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

protected:
    void AddAudioRecordFile(int nFileIndex, const RecordFileInfo & infoFile);
};
#endif

#endif
