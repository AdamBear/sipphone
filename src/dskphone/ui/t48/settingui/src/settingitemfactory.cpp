#include "settingitemfactory.h"
#include "bluetoothitem.h"
#include "settingitembase.h"
#include "messageui/src/messagelistitem.h"
#include "wifiui/src/wifiitem.h"
#include "wifiui/src/wirelesssearchitem.h"
#include "usbaudioitem.h"
#include "settinganimationitem.h"
#include "usbphotoitem.h"

#if IF_FEATURE_BROADSOFT_VOICEMAIL
#include "vvmitem.h"
#endif

#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "metaswitchui/mtswvoicemaillistitem.h"
#endif

/******************Class  CSettingItemManager**********************/
static CSettingItemManager * MngInstance = NULL;

CSettingItemManager::CSettingItemManager()
{
    //
}

CSettingItemManager * CSettingItemManager::GetInstance()
{
    if (MngInstance == NULL)
    {
        MngInstance = new CSettingItemManager();
    }

    return MngInstance;
}

CSettingItem * CSettingItemManager::CreateItemByType(int eType, QWidget * pParent,
        QWidgetPair & pair)
{
    switch (eType)
    {
    case SETTING_ITEM_NORMAL:
        {
            return new CSettingItem(pParent, pair);
        }
        break;
    case SETTING_ITEM_MENU:
        {
            return new CSettingMenuItem(pParent, pair);
        }
        break;
    case SETTING_ITEM_RIDIO:
        {
            return new CSettingRadioItem(pParent, pair);
        }
        break;
    case SETTING_ITEM_BLUETOOTH:
        {
            return new CSettingBluetoothItem(pParent, pair, true);
        }
        break;
    case SETTING_ITEM_BLUETOOTHDEVICELIST:
        {
            return new CSettingBluetoothDeviceListItem(pParent, pair);
        }
        break;
    case MESSAGE_LIST_ITEM:
        {
            return new CMessageListItem(pParent, pair);
        }
        break;
    case WIRELESS_ITEM_SEARCH:
        {
            return new WirelessSearchStatisticsItem(pParent, pair);
        }
        break;
    case SETTING_ITEM_WIFI:
        {
            return new CSettingWifiItem(pParent, pair);
        }
        break;
    case SETTING_ITEM_ANIMATION:
        {
            return new CSettingAnimationItem(pParent, pair);
        }
        break;
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    case SETTING_ITEM_USB_AUDIO:
        {
            return new CUSBAudioItem(pParent, pair);
        }
        break;
    case  SETTING_ITEM_USB_PHOTO:
        {
            return new CUSBPhotoItem(pParent, pair);
        }
        break;
#endif
#if IF_FEATURE_BROADSOFT_VOICEMAIL
    case SETTING_ITEM_VVM:
        {
            return new CVVMItem(pParent, pair);
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_ACD
    case SETTING_ITEM_MTSWACDLIST:
        {
            return new CMTSWAcdListItem(pParent, pair);
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_VOICEMAIL
    case VOICEMAIL_LIST_ITEM:
        {
            return new CVoiceMailListItem(pParent, pair);
        }
        break;
#endif
#if IF_FEATURE_GENBAND_E911
    case SETTING_ITEM_GENBAND_E911:
        {
            return new CGenbandE911ListItem(pParent, pair);
        }
        break;
#endif
    default:
        {
            return NULL;
        }
    }
}
