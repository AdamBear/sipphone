#ifndef _TALKUI_COMMON_FUNCTION_H
#define _TALKUI_COMMON_FUNCTION_H

#include <ylstring.h>
#include "talkui_define.h"
#include "talkuilogic/talkuilogiccommon.h"
#include "talkuilogic/talkuilogicenum.h"
// #include "imagecachemanager/modimagecachemanager.h"

#include "uimanager/dialogviewcontroller.h"
#include "uimanager/uimanager_common.h"

namespace talkui
{

#define POOL_ACTION_LOCAL_CONTACT   "localdirectory"
#define POOL_ACTION_LOCAL_CALLLOG   "history"
#define POOL_ACTION_REMOTE_CONTACT  "remotedirectory"
#define POOL_ACTION_LDAP_CONTACT    "ldap"
#define POOL_ACTION_BST_CONTACT     "broadsoftdirectory"
#define POOL_ACTION_NETWORK_CONTACT     "networkdirectory"
#define POOL_ACTION_BST_CALLLOG     "networkcalllog"
#define POOL_ACTION_UC_BUDDIES      "uc_buddies"            //uc_buddies
#define POOL_ACTION_NETWORK_FAVORITE        "network_favorite"      //network_favorite   ucfavorite
#if IF_BUG_PLCMDIR
#define POOL_ACTION_PLCM_CONTACT    "plcmdirectory"
#endif
#ifdef IF_FEATURE_PSTN
#define POOL_ACTION_FAVORITE_DIRECTORY  "favoritedirectory"
#endif
#define POOL_ACTION_MOBILE_CONTANT  "mobilecontant"

#if IF_FEATURE_GBBOOK
#define POOL_ACTION_GENBAND_CONTACT "pabdirectory"
#define POOL_ACTION_GAB_CONTACT "gabdirectory"
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
#define POOL_ACTION_MTSW_CONTACT        "metaswitchcontacts"
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
#define POOL_ACTION_MTSW_CALLLOG        "metaswitchcalllog"
#endif

enum
{
    LIST_ITEM_TYPE_NORMAL_ITEM = 0x00100000,
    LIST_ITEM_TYPE_TALK_SEARCH = 0x00200000,
    LIST_ITEM_TYPE_CONF_MANAGER = 0x00300000,
};

//根据processor名称获取对话框的名称
yl::string GetDlgNameFromProcessorName(yl::string strProcessorName);

//chString和ylstring转换函数
// yl::string fromchString(const chConstStringA& other);

// chConstStringA tochString(const yl::string& other);

//根据路径获取Pixmap
xPixmap GetPixmapByString(const yl::string & strPath);

//根据通道类型获取通道图标
yl::string GetChannelIconPicByChannelIconType(TALKUI_CHANNEL_ICON eTalkUIChannelIcon);

LCD_ICON_TYPE GetChannelIconTypeByChannelIconType(TALKUI_CHANNEL_ICON eTalkUIChannelIcon);

//根据图标类型获取 图片路径
yl::string GetIconPathByIconType(TALKUI_ICON iconType);

//根据蓝牙类型 获取蓝牙图标路径
yl::string GetBlueToothIconPathByBlueToothIconType(TALKUI_BLUETOOTH_ICON eTalkUIBlueToothIcon);


//根据softkeytype 获取 softkeybardata
void GetSoftkeyBarDataFromSoftKeyType(CSoftKeyBarData & softkeyBarData, SoftKey_TYPE eSoftkeyType);

////根据softkeytype 获取 softkeybardata
//void GetSoftkeyBarDataFromSoftKeyType(CSoftKeyBarData& softkeyBarData, SoftKey_TYPE eSoftkeyType);


//xwindow 使用 根据控件、窗口名称 获取mostleve
//BYTE GetTopMostLevelByName(const yl::string& strWidgetName);

bool ExistLocalDirectory(YLList<DirShortcutData> & listData);
void ProcessNormalOpenPool();

void ChangeNameAndNumberBySecurityLevel(yl::string & strName, yl::string & strNumber,
                                        const yl::string strSecurityLev, bool bInsertFirstLine = false);

}       //namespace talkui


#endif
