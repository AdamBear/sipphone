#ifndef _EDKUI_DEF_
#define _EDKUI_DEF_

#include <ylstl/ylstring.h>
#include <ylstl/ylhashmap.h>
#include "dsklog/log.h"
#include "common/common_data_define.h"
#include "include/features-define.h"
#include "dsskey/include/dsskey_enumtypes.h"

// menu id和 dir type映射文件
// NODE
#define ROOT_NODE       "edk"
#define KEY_NODE        "key"
#define SETTING_NODE    "setting_ui"
#define DIRECTORY_NODE  "directory_ui"
// ATTRIBUTE
#define ATTRI_ITEM              "item"
#define ATTRI_MENU_ID           "menu_id"
#define ATTRI_DEV               "dev"
#define ATTRI_PAGE              "page_name"
#define ATTRI_TYPE              "type"
#define ATTRI_SOFTKEY_TYPE      "softkey_type"
#define ATTRI_PROGRAMKEY_TYPE   "programkey_type"
#define ATTRI_KEY_ID            "key_id"

enum TipNameType
{
    TIPNAME_SOFTKEY = 1,
    TIPNAME_PROGRAMKEY,
};

// 判断是否允许进入UI的函数指针
typedef bool(*FuncIsAllowEnterMenu)(const yl::string & strPageName);

typedef bool (*FuncEdkActionCallBack)(bool, int, yl::string &);

// 包含softkey和program key
struct KeyType
{
    SoftKey_TYPE eSoftkeyType;
    DssKey_Type  eDsskeyType;

    KeyType()
    {
        eSoftkeyType = ST_EMPTY;
        eDsskeyType  = DT_NONE;
    }
};

struct ST_EDK_DATA
{
    int                     iIndex;         // 输入框索引
    int                     iId;            // 命令id
    FuncEdkActionCallBack   pCallBackFun;   // ui响应后的回调函数

    ST_EDK_DATA()
    {
        iIndex = -1;
        iIndex = -1;
        pCallBackFun = NULL;
    }
};

// softkey id ----> key type
typedef YLHashMap<yl::string, KeyType>      typeSoftkeyIDToTypeMap;
typedef typeSoftkeyIDToTypeMap::iterator    Iter_SoftkeyID_Type_Map;

// menu id ----> setting
typedef YLHashMap<yl::string, yl::string>   typeMenuIDToPageMap;
typedef typeMenuIDToPageMap::iterator       Iter_MenuID_Page_Map;

// menu id ----> directory type
typedef YLHashMap<yl::string, int>          typeMenuIDToDirTypeMap;
typedef typeMenuIDToDirTypeMap::iterator    Iter_MenuID_DirType_Map;

// menu id ----> condition func pointer
typedef YLHashMap<yl::string, FuncIsAllowEnterMenu> typeMenuIDToAllowFunc;
typedef typeMenuIDToAllowFunc::iterator             Iter_MenuID_AllowFunc_Map;

#define kszEdkPageIDCallcontrolAnonymousCallselectline          "broadsoftanonymouscallselectline"
#define kszEdkPageIDCallcontrolAnonymousRejectionselectline     "broadsoftanonymousrejectionselectline"
#define kszEdkPageIDCallcontrolRemoteOfficeSelectline           "broadsoftremoteofficeselectline"
#define kszEdkPageIDCallcontrolAnywhereSelectline               "anywhereselectline"
#define kszEdkPageIDCallcontrolForwardNotReachableSelectline    "broadsoftforwardnotreachableselectline"
#define kszEdkPageIDCallcontrolSimultaneousRingSelectline       "simultaneousringselectline"
#define kszEdkPageIDCallcontrolAssistantSelectline              "assistantselectline"
#define kszEdkPageIDCallcontrolCallWaitingselectline            "broadsoftcallwaitingselectline"
#define kszEdkPageIDCallcontrolCallRecordSelectline             "broadsoftcallrecordselectline"
#define kszEdkPageIDCallcontrolMobilityselectline               "broadsoftmobilityselectline"
#define kszEdkPageIDCallcontrolGroupNightForwardSelectline      "broadsoftgroupnightforwardselectline"
#define kszEdkPageIDCallcontrolforwardselectiveselectline       "broadsoftforwardselectiveselectline"

#define kszEdkPageIDCallForwardLineselect                       "callforwardlineselect"
#define kszEdkPageIDCustomAccountForward                        "customaccountforward"
#define kszEdkPageIDCustomAlwaysForward                         "customalwaysforward"
#define kszEdkPageIDCustomBusyForward                           "custombusyforward"
#define kszEdkPageIDCustomNoAnswerForward                       "customnoanswerforward"
#define kszEdkPageIDDNDLineselect                               "dndlineselect"
#define kszEdkPageIDCustomAccountDND                            "customaccountdnd"

#define ID_INVALID          -1
#define ID_DEFAULT          0

#endif
