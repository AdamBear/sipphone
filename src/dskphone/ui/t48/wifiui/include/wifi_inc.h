#ifndef __WIFI_INC_H
#define __WIFI_INC_H

#include "wifiui/src/wifiuimanager.h"
#include "wifiui/include/wifiuimanagerhelp.h"
#include "wifiui/include/wificommondata.h"
#include "wifiui/include/modwifiui.h"

#include "wifiui/src/dlgwifibase.h"
#include "wifiui/src/dlgwifiaddedit.h"
#include "wifiui/src/dlgwifidetail.h"
#include "wifiui/src/dlgwifistatus.h"
#include "wifiui/src/dlgwifistorage.h"
#include "wifiui/src/wifidelegate.h"
#include "wifiui/src/wifisaveddelegate.h"
//#include "wifiui/src/wifidetailselector.h"
#include "wifiui/src/wifidetailtitle.h"
// #include "wifiui/src/wifistatusdelegate.h"

#include "settingui/src/settingitembase.h"
#include "settingui/src/settingitemfactory.h"
#include "settingui/include/modsettingui.h"
#include "settingui/src/settinguifeaturesdefine.h"

#include "wifiui/src/wifiitem.h"
// #include "wifiui/src/wifisignalitem.h"
#include "wifiui/src/notelabel.h"
#include "wifiui/src/wirelesssearchitem.h"
//#include "wifiui/src/popupboxitem.h"

#include "uikernel/languagehelper.h"
#include "uikernel/qwidgetutility.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qcomboboxutility.h"
#include "uikernel/qcheckbox.h"
#include "uikernel/ilistitemdelegate.h"

#include "baseui/t4xpicpath.h"
#include "baseui/fontcolor.h"
#include "baseui/framelistaction.h"

#include "inputmethod/inputmethod.h"

#include "etlmsghandler/modetlmsghandler.h"

#include "messagebox/modmessagebox.h"
#include "messagebox/selectmessagebox.h"
#include "messagebox/messageboxcallback.h"

#include "qtcommon/qmisc.h"

#include "uimanager/moduimanager.h"
#include "uimanager/rectdefine.h"

#include "mainwnd/modmainwnd.h"
#include "mainwnd/mainwnd.h"

#include "imagemanager/modimagemanager.h"

#include "talkui/src/qpictureanimation.h"
#include "talk/talklogic/include/modtalklogic.h"

#include "translateiddefine.h"
#include "uicommon.h"

//#define LOGIC_WIFI_SUPPORT

#ifdef IF_SUPPORT_WIFI
#include "wireless/wifi/include/modwifi.h"
#include "wireless/wifi/include/wifidefines.h"

#include "extended/wifimessagedefine.h"
#endif

// logic interface
#ifdef LOGIC_WIFI_SUPPORT
#include "wifi/modwifi.h"
#include "define/wifimessagedefine.h"
#endif

#endif
