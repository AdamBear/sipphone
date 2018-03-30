#include "usbstorageinfodelegate.h"
#include "translateiddefine.h"
#include "commonapi/stringhelperapi.h"
#include "languagehelper/modlangtranslate.h"
#ifdef IF_USB_SUPPORT
#include "record/include/modrecord.h"
#endif
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "usbrecordcontroler.h"
#include "pageiddefine.h"


#ifdef IF_USB_SUPPORT
namespace
{
const char * kszUSBRSTotal = "usbrs_total";
const char * kszUSBRSUsed = "usbrs_used";
const char * kszUSBRSFree = "usbrs_free";
const char * kszUSBRSAvaiableRecordTime = "usbrs_avaiable_record_time";
}

CUSBStorageInfoDelegate::CUSBStorageInfoDelegate()
{
    g_UsbRecordControler.Init(kszPageIDUsbStorageInfo);
}

CUSBStorageInfoDelegate::~CUSBStorageInfoDelegate()
{
    g_UsbRecordControler.UnInit(kszPageIDUsbStorageInfo);
}

CSettingUIDelegateBase * CUSBStorageInfoDelegate::CreateStorageInfoDelegate()
{
    CUSBStorageInfoDelegate * pStorageDelegate = new CUSBStorageInfoDelegate;

    return pStorageDelegate;
}

CSettingUIPageDataBase * CUSBStorageInfoDelegate::LoadPageData(bool bReLoad/* = false*/)
{
    CSettingUIPageDataBase * pPageData = CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (NULL == pPageData || !bReLoad)
    {
        return pPageData;
    }

    StorageInfo stStorage;

    modRecord_GetStorageInfo(stStorage);

    UpdateItemValue(kszUSBRSTotal, modRecord_TransSizeToString(stStorage.m_ullTotal));

    yl::string strUsed = modRecord_TransSizeToString(stStorage.m_ullUsed);

    strUsed += commonAPI_FormatString(" (%.1f%% %s)",
                                      (float)stStorage.m_ullUsed * 100 / stStorage.m_ullTotal,
                                      _LANG(TRID_USB_PROPERTY_USED_HINT).c_str());
    UpdateItemValue(kszUSBRSUsed, strUsed);

    yl::string strFree = modRecord_TransSizeToString(stStorage.m_ullFree);

    strFree += commonAPI_FormatString(" (%.1f%% %s)",
                                      (float)stStorage.m_ullFree * 100 / stStorage.m_ullTotal,
                                      _LANG(TRID_USB_PROPERTY_FREE_HINT).c_str());
    UpdateItemValue(kszUSBRSFree, strFree);

    UpdateItemValue(kszUSBRSAvaiableRecordTime, modRecord_GetAvailableRecordingTime());

    return pPageData;
}

void CUSBStorageInfoDelegate::UpdateItemValue(const char * pszID, const yl::string & strValue)
{
    if (NULL == pszID || NULL == m_pPageData)
    {
        return;
    }

    CSettingUIItemDataBase * pItemData = m_pPageData->GetItemByItemId(pszID);

    if (NULL != pItemData)
    {
        pItemData->SetValue(strValue);
    }
}
#endif
