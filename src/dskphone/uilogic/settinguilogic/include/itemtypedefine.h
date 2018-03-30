#ifndef __ITEM_TYPE_DEFINE_H__
#define __ITEM_TYPE_DEFINE_H__
#include <settinguilogic/include/settingdefine.h>

const char * const kszItemTypeNameLabel      = "label";
const char * const kszItemTypeNameInupt      = "edit";
const char * const kszItemTypeNameCombobox   = "combobox";
const char * const kszItemTypeNameSwitch     = "switch";
const char * const kszItemTypeNameMenu       = "menu";
const char * const kszItemTypeNameLink       = "link";
const char * const kszItemTypeNameRadio      = "radio";
const char * const kszItemTypeNameDateedit   = "dateedit";
const char * const kszItemTypeNameTimeedit   = "timeedit";

//CP920
const char * const kszItemTypeNameDetailMenu = "detailmenu";
const char * const kszItemTypeNameCustomCombobox = "customcombobox";
const char * const kszItemTypeNameBluetoothDev   = "bluetoothdev";
const char * const kszItemTypeNameWifiNet    = "wifinet";
const char * const kszItemTypeNameTextMessage    = "textmessage";
const char * const kszItemTypeBtChannelControll  = "BtChannelControll";
const char* const kszItemTypeNameVoiceMailTimer = "voicemailtimer";

#if IF_SETTING_MAINMENU_IS_TABLEVIEW
const char* const kszItemTypeNameMainMenu = "mainmenu";
#endif
#endif // __ITEM_TYPE_DEFINE_H__
