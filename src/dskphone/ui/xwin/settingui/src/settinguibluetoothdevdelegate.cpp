#include "settinguibluetoothdevdelegate.h"

#ifdef IF_SUPPORT_BLUETOOTH

#include "translateiddefine.h"
#include "baseui/include/t2xpicpath.h"
#include "settinguibluetoothdevitemdata.h"
#include "viewmodeuimanager/include/uicommon.h"
#include "viewmodeuimanager/include/viewmodeuidefine.h"

CSettingUIBluetoothDevDelegate::CSettingUIBluetoothDevDelegate()
{

}

CSettingUIBluetoothDevDelegate::~CSettingUIBluetoothDevDelegate()
{

}

CSettingUIDelegateBase * CSettingUIBluetoothDevDelegate::CreateBluetoothDevDelegate()
{
    CSettingUIBluetoothDevDelegate * pDelegate = new CSettingUIBluetoothDevDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIItemDataBase * CSettingUIBluetoothDevDelegate::CreateDefaultItemData(
    const ViewInfo & viewInfo)
{
    CSettingUIItemDataBase * pItemData = new CSettingUIBluetoothDevItemData;

    return pItemData;
}

bool CSettingUIBluetoothDevDelegate::InitListItemData(CSettingUIItemDataBase * pItemData,
        const ViewInfo & viewInfo)
{
    if (!CSettingUIBtWifiDelegate::InitListItemData(pItemData, viewInfo))
    {
        return false;
    }

    CSettingUIBluetoothDevItemData * pDevItemData = (CSettingUIBluetoothDevItemData *)pItemData;
    if (NULL == pDevItemData)
    {
        return false;
    }

    pDevItemData->SetMac(viewInfo.strValue);

    if (UIConmonAPI_Lang(TRID_DISCONNECTED) == viewInfo.strValue1)
    {
        pDevItemData->SetConnectStatusIcon(PIC_WIRELESS_DISCONNECT_ICON);
    }
    else if (UIConmonAPI_Lang(TRID_CONNECTED) == viewInfo.strValue1)
    {
        pDevItemData->SetConnectStatusIcon(PIC_WIRELESS_CONNECTED_ICON);
    }

    if (viewInfo.strVecExtraValue.size() >= 1)
    {
        const yl::string & strValue  = viewInfo.strVecExtraValue[0];
#if IS_COLOR
        pDevItemData->SetDevIcon(PIC_COMMON_ICON_PATH + strValue);
#else
        pDevItemData->SetDevIcon(PIC_COMMON_PATH + strValue);
#endif
    }

    return true;
}

#endif //IF_SUPPORT_BLUETOOTH
