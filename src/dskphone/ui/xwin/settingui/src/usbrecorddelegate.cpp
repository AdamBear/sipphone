#include "usbrecorddelegate.h"
#include "usbrecordcontroler.h"
#include "pageiddefine.h"


#ifdef IF_USB_SUPPORT

namespace
{
const char * kszUSBRecordAudio = "usbrecord_audio";
const char * kszUSBRecordStorage = "usbrecord_storage";
const char * kszUSBRecordIdleRecord = "usbrecord_idlerecord";
const char*  kszUSBRecordPhoto = "usbrecord_photo";
}

CUSBRecordDelegate::CUSBRecordDelegate()
{
    g_UsbRecordControler.Init(kszPageIDUsbRecord);
}

CUSBRecordDelegate::~CUSBRecordDelegate()
{
    g_UsbRecordControler.UnInit(kszPageIDUsbRecord);
}

CSettingUIDelegateBase * CUSBRecordDelegate::CreateUSBRecordDelegate()
{
    CUSBRecordDelegate * pUSBRecordDelegate = new CUSBRecordDelegate;

    return pUSBRecordDelegate;
}

bool CUSBRecordDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
                                        bool bReload/* = false*/)
{
    bool bResult = CSettingUIDelegateBase::PageDataFilter(pPagedata, bReload);

    if (NULL == pPagedata)
    {
        return bResult;
    }

    if (!g_UsbRecordControler.IsUsbConnect())
    {
        SetItemShow(pPagedata, kszUSBRecordAudio, false);
        SetItemShow(pPagedata, kszUSBRecordPhoto, false);
        SetItemShow(pPagedata, kszUSBRecordStorage, false);
        SetItemShow(pPagedata, kszUSBRecordIdleRecord, false);
    }
    else
    {
        if (!g_UsbRecordControler.IsIdleRecordEnabled())
        {
            SetItemShow(pPagedata, kszUSBRecordIdleRecord, false);
        }
    }

    return bResult;
}

void CUSBRecordDelegate::OnEnterPage(const yl::string& strPage, int iDataIndex)
{
    if (kszUSBRecordIdleRecord == GetItemIDByDataIndex(iDataIndex))
    {
        SetRefreshWndWhileActive(true);
    }

    CSettingUIDelegateBase::OnEnterPage(strPage, iDataIndex);
}

#endif
