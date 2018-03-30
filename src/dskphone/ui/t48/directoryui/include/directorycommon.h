
#ifndef __DIRECTORY_COMMON_H__
#define __DIRECTORY_COMMON_H__

#include <yllist.h>
#include <ylvector.h>
#include <ylstring.h>
#include "ETLLib.hpp"
#include "contacts/directory/include/commondata.h"
#include "contacts/directory/include/directoryenumtypes.h"
#include "contacts/directory/include/directorymessagedefine.h"
#include "contacts/directory/include/directorystructure.h"
#include "contacts/directory/include/moddirinterface.h"
#include "contacts/ldaplogic/include/modldap.h"
#include "baseui/framelist.h"


// 联系人ID数组
typedef YLVector<int> VEC_CONTACT_ID;

// 模块类型
enum
{
    DIR_MOD_TYPE_NONE           = 0,
    DIR_MOD_TYPE_CONTACT        = 1,                    // 联系人模块
    DIR_MOD_TYPE_CALLLOG        = 2,                    // 历史记录模块
};

// 联系人类型
enum
{
    DIRECTORY_TYPE_NONE         = 0,
    DIRECTORY_TYPE_LOCAL        = 1,                    // 本地联系人
    DIRECTORY_TYPE_REMOTE       = 2,                    // 远程联系人
    DIRECTORY_TYPE_LDAP         = 3,                    // LDAP联系人
    DIRECTORY_TYPE_BROADSOFT    = 4,                    // Broadsoft联系人
    DIRECTORY_TYPE_BTUC         = 5,                    // UC联系人
    DIRECTORY_TYPE_PLCM         = 6,                    // 远程联系人
    DIRECTORY_TYPE_REMOTE_SEARCH    = 7,                // 远程联系人服务器搜索
    DIRECTORY_TYPE_BLUETOOTH    = 8,                    // 蓝牙联系人
    DIRECTORY_TYPE_GENBAND      = 9,
    DIRECTORY_TYPE_METASWICTH   = 10,
    DIRECTORY_TYPE_MAX,

    CALL_LOG_TYPE_LOCAL         = 100,                  // 本地历史记录
    CALL_LOG_TYPE_BROADSOFT     = 101,                  // Broadsoft联系人
    CALL_LOG_TYPE_MTSW      = 102,                  //
};

// 联系人搜索类型
enum
{
    DIRECTORY_SEARCH_TYPE_NONE          = -1,
    DIRECTORY_SEARCH_TYPE_NORMAL        = 0,            // 联系人普通搜索
    DIRECTORY_SEARCH_TYPE_LDAP          = 1,            // LDAP搜索
    DIRECTORY_SEARCH_TYPE_BROADSOFT     = 2,            // BroadSoft高级搜索
    DIRECTORY_SEARCH_TYPE_GENBAND       = 3,            // Genband高级搜索
    DIRECTORY_SEARCH_TYPE_MAX
};

// 是否支持列表框
#define FL_SUPPORT
// 是否支持标题栏
#define FL_SUPPORT_TITLE
// 是否支持设置界面
#define DIR_SUPPORT_SETTING
// 是否支持逐个浏览联系人
#define DIR_SUPPORT_SWITCH_DETAIL
// 是否支持批量操作
#define DIR_SUPPORT_BATCH_OPERATE
// 是否只有单一的UIHelper
// #define DIR_SUPPORT_SINGLE_UIHELPER

#ifdef FL_SUPPORT_TITLE
#define DIR_UI_LIST_TITLE_ITEM_COUNT 5
#define DIR_UI_LIST_NONE_TITLE_ITEM_COUNT 6
#define DIR_UI_LIST_DETAIL_ITEM_COUNT 6
#else
//#后面多机型改用函数调用
#define DIR_UI_LIST_NORMAL_ITEM_COUNT 5
#define DIR_UI_LIST_SEARCH_ITEM_COUNT 4
#define DIR_UI_LIST_DETAIL_ITEM_COUNT 5
#endif

// 是否支持从联系人界面选取一个联系人
#define DIR_SUPPORT_SELECT_CONTACT  1
// 是否支持未接来电总是显示红色
#define DIR_SUPPORT_MISSCALL_ALWAYS_RED 1
// 是否支持net call log删除
#define DIR_SUPPORT_NET_CALLLOG_DELETE      1

// 联系人UI状态类型
enum
{
    DIR_UI_STATE_NONE           = 0,
    DIR_UI_STATE_BROWSER        = 1,
    DIR_UI_STATE_SEARCH         = 2,
    DIR_UI_STATE_DETAIL         = 3,
    DIR_UI_STATE_ADDITION       = 4,
    DIR_UI_STATE_PRECISE_SEARCH = 5,
#ifdef DIR_SUPPORT_SETTING
    DIR_UI_STATE_SETTING        = 6,
#endif
    DIR_UI_STATE_CHOOSE_NUMBER,
    DIR_UI_STATE_CHG_PWD,
    DIR_UI_STATE_LDAP_CHG_PWD,
};

// 联系人历史记录
enum
{
    DIR_ACTION_NONE                         = 0,
    DIR_ACTION_COMMON_DETAIL                = 1,
    DIR_ACTION_COMMON_SELECT                = 2,

    DIR_ACTION_LOCAL_ENTER_ADD_GROUP,
    DIR_ACTION_LOCAL_ADD_GROUP,
    DIR_ACTION_LOCAL_ADD_GROUP_FINISH,
    DIR_ACTION_LOCAL_EDIT_GROUP,
    DIR_ACTION_LOCAL_EDIT_GROUP_FINISH,
    DIR_ACTION_LOCAL_DEL_GROUP,
    DIR_ACTION_LOCAL_DEL_ALL_GROUP,
    DIR_ACTION_LOCAL_ENTER_ADD_CONTACT,
    DIR_ACTION_LOCAL_ADD_CONTACT,
    DIR_ACTION_LOCAL_ADD_CONTACT_FINISH,
    DIR_ACTION_LOCAL_DEL_CONTACT,
    DIR_ACTION_LOCAL_DEL_ALL_CONTACT,
    DIR_ACTION_LOCAL_BATCH_DEL_CONTACT,
    DIR_ACTION_LOCAL_BATCH_MOVE_CONTACT,
    DIR_ACTION_LOCAL_BATCH_MOVE_TO_BLACKLIST,
    DIR_ACTION_LOCAL_EDIT_CONTACT,
    DIR_ACTION_LOCAL_EDIT_CONTACT_FINISH,
    DIR_ACTION_LOCAL_MOVE_TO_CONTACTS,
    DIR_ACTION_LOCAL_MOVE_TO_BLACKLIST,
    DIR_ACTION_LOCAL_MOVE_CONTACT_FINISH,
    DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS,
    DIR_ACTION_LOCAL_COPY_TO_CONTACTS,
    DIR_ACTION_LOCAL_COPY_TO_CONTACTS_OVERWRITE,
    DIR_ACTION_LOCAL_ENTER_COPY_TO_BLACKLIST,
    DIR_ACTION_LOCAL_COPY_TO_BLACKLIST,
    DIR_ACTION_LOCAL_COPY_TO_BLACKLIST_OVERWRITE,
    DIR_ACTION_LOCAL_COPY_CONTACT_FINISH,
    DIR_ACTION_LOCAL_COPY_TO_FAVORITE,
    DIR_ACTION_LOCAL_REMOVE_FROM_FAVORITE,

    DIR_ACTION_REMOTE_UPDATE,
    DIR_ACTION_REMOTE_DOWNLOAD,
    DIR_ACTION_REMOTE_SERVER_SEARCH,
    DIR_ACTION_REMOTE_PREVPAGE,
    DIR_ACTION_REMOTE_NEXTPAGE,
    DIR_ACTION_REMOTE_OPEN_APPOINT_GROUP,

    DIR_ACTION_BSFT_UPDATE,
    DIR_ACTION_BSFT_DOWNLOAD,
    DIR_ACTION_BSFT_ENTER_ADD_PERSONAL,
    DIR_ACTION_BSFT_ADD_PERSONAL,
    DIR_ACTION_BSFT_ADD_PERSONAL_FINISH,
    DIR_ACTION_BSFT_EDIT_PERSONAL,
    DIR_ACTION_BSFT_EDIT_PERSONAL_FINISH,
    DIR_ACTION_BSFT_DEL_PERSONAL,
    DIR_ACTION_BSFT_ENTER_ADD_TO_PERSONAL,
    DIR_ACTION_BSFT_ADD_TO_PERSONAL,
    DIR_ACTION_BSFT_ADD_TO_PERSONAL_OVERWRITE,
    DIR_ACTION_BSFT_COPY_PERSONAL_FINISH,
    DIR_ACTION_BSFT_ENTER_CHG_PWD,
    DIR_ACTION_BSFT_CHG_PWD,
    DIR_ACTION_BSFT_BATCH_MOVE_TO_PERSONAL,
    DIR_ACTION_BSFT_BATCH_DEL_PERSONAL,
    DIR_ACTION_BSFT_CLEAR_GROUP,
    DIR_ACTION_BSFT_DELETE_CALLLOG,

    DIR_ACTION_PLCM_UPDATE,
    DIR_ACTION_PLCM_DOWNLOAD,

    DIR_ACTION_BLUETOOTH_UPDATE,
    DIR_ACTION_BLUETOOTH_DOWNLOAD,

    DIR_ACTION_GENBAND_UPDATE,
    DIR_ACTION_GENBAND_EDIT_CONTACT,
    DIR_ACTION_GENBAND_ENTER_MANAGER_GROUP,
    DIR_ACTION_GENBAND_DOWNLOAD,
    DIR_ACTION_GENBAND_ENTER_ADD_GROUP,
    DIR_ACTION_GENBAND_ADD_GROUP,
    DIR_ACTION_GENBAND_ADD_GROUP_FINISH,
    DIR_ACTION_GENBAND_EDIT_GROUP,
    DIR_ACTION_GENBAND_EDIT_GROUP_FINISH,
    DIR_ACTION_GENBAND_DEL_GROUP,
    DIR_ACTION_GENBAND_ENTER_ADD_PAB,
    DIR_ACTION_GENBAND_ADD_PAB,
    DIR_ACTION_GENBAND_ADD_PAB_FINISH,
    DIR_ACTION_GENBAND_EDIT_PAB,
    DIR_ACTION_GENBAND_EDIT_PAB_FINISH,
    DIR_ACTION_GENBAND_DEL_PAB,
    DIR_ACTION_GENBAND_DEL_FINISH,
    DIR_ACTION_GENBAND_ENTER_ADD_TO_PAB,
    DIR_ACTION_GENBAND_ADD_TO_PAB,
    DIR_ACTION_GENBAND_ADD_TO_PAB_OVERWRITE,
    DIR_ACTION_GENBAND_COPY_PERSONAL_FINISH,
    DIR_ACTION_GENBAND_BATCH_MOVE_TO_PERSONAL,
    DIR_ACTION_GENBAND_BATCH_DEL_PERSONAL,
    DIR_ACTION_GENBAND_SEARCH_BY_NAME,
    DIR_ACTION_GENBAND_SEARCH_BY_FIRSTNAME,
    DIR_ACTION_GENBAND_SEARCH_BY_LASTNAME,
    DIR_ACTION_GENBAND_SEARCH_BY_NUMBER,

    DIR_ACTION_MTSW_ENTER_ADD_GROUP,
    DIR_ACTION_MTSW_ADD_GROUP,
    DIR_ACTION_MTSW_ADD_GROUP_FINISH,
    DIR_ACTION_MTSW_EDIT_GROUP,
    DIR_ACTION_MTSW_EDIT_GROUP_FINISH,
    DIR_ACTION_MTSW_DEL_GROUP,

    DIR_ACTION_MTSW_UPDATE,
    DIR_ACTION_MTSW_DOWNLOAD,
    DIR_ACTION_MTSW_ENTER_ADD_CONTACTLISTL,
    DIR_ACTION_MTSW_ADD_CONTACTLIST,
    DIR_ACTION_MTSW_ADD_CONTACTLIST_FINISH,
    DIR_ACTION_MTSW_EDIT_CONTACTLIST,
    DIR_ACTION_MTSW_EDIT_CONTACTLIST_FINISH,
    DIR_ACTION_MTSW_DEL_CONTACTLIST,
    DIR_ACTION_MTSW_DEL_ALL_CONTACTLIST,
    DIR_ACTION_MTSW_ENTER_ADD_TO_CONTACTLIST,
    DIR_ACTION_MTSW_ADD_TO_LOCAL,
    DIR_ACTION_MTSW_ADD_TO_LOCAL_OVERWRITE,
    DIR_ACTION_MTSW_BATCH_MOVE_TO_CONTACT,
    DIR_ACTION_MTSW_BATCH_DEL_CONTACT,
    DIR_ACTION_MTSW_SORT,
    DIR_ACTION_MTSW_CHG_PWD,
    DIR_ACTION_MTSW_ERROR_PWD,

    DIR_ACTION_LCLOG_DEL_CALL_LOG,
    DIR_ACTION_LCLOG_DEL_ALL_CALL_LOG,
    DIR_ACTION_LCLOG_BATCH_DEL_CALL_LOG,
    DIR_ACTION_LCLOG_EDIT,

    DIR_ACTION_BSFTLOG_EDIT,


    DIR_ACTION_LDAP_ENTER_CHG_PWD,
    DIR_ACTION_LDAP_CHG_PWD,
};

enum
{
    DIR_UI_ACTION_NONE              = 0,
    DIR_UI_ACTION_COMMON_BACK,
    DIR_UI_ACTION_COMMON_HOME,
    DIR_UI_ACTION_RING_SETTING,
    DIR_UI_ACTION_NUMBER_CHOOSE,
    DIR_UI_ACTION_COMMON_SELECT_BOX,
    DIR_UI_ACTION_ACCOUNT_CHOOSE,
    DIR_UI_ACTION_TRANSFER_NUMBER_CHOOSE,

    DIR_UI_ACTION_COMMON_END,
};

#if IF_FEATURE_CALLLIST_SHOW_METHOD
//Calllog List显示记录的方式
enum
{
    CL_SHOW_ONLY_NAME = 0,  //仅显示名字
    CL_SHOW_ONLY_NUMBER = 1, //仅显示号码
    CL_SHOW_NAME_AND_NUMBER = 2, //名字在前，号码在后
    CL_SHOW_NUMBER_AND_NAME = 3,    //号码在前，名字在后
    CL_SHOW_FULL_CONTACT_INFO = 4,  //显示完成的信息
};
#endif

// Auto帐号
#define AUTO_ACCOUNT_IDENTIFY -1
#define DEAD_LOOP_BREAK(nLoop) ++nLoop;\
    if (nLoop > 1000000) {DIRUI_ERR("Dead loop break at %s %d", __FILE__, __LINE__); break;}

// 铃声播放延迟防抖时间
#define TIME_DELAY_PLAY_RING 500

// 铃声焦点延迟播放时间
#define TIME_DELAY_FOCUS_RING 2000

#define CONTACT_PHOTO_FILE_DEFAULT "default_contact_image.png"
#define CONTACT_PHOTO_FILE_FRIEND "friend.png"
#define CONTACT_PHOTO_FILE_FAMILY "family.png"
#define CONTACT_PHOTO_FILE_CUSTOMER "customer.png"

#define CONTACT_PHOTO_DEFAULT_CONTACT_IMAGE "Default:default_contact_image.png"
#define CONTACT_PHOTO_ICON_PATH "icon/"

#ifndef FL_SUPPORT
#define DIR_DEFAULT_HEAD "Contact.dob"
#define CALL_LOG_ICON_OUT "calllist_out.dob"
#define CALL_LOG_ICON_IN "calllist_in.dob"
#define CALL_LOG_ICON_MISS "calllist_miss.dob"
#define CALL_LOG_ICON_FORWARD "calllist_forward.dob"

// 列表项界面下的操作类型
enum ITEM_OPERATION
{
    ITEM_UNKNOWN,
    ITEM_ADD_CONTACT,
    ITEM_EDIT_CONTACT,
    ITEM_ADD_BROADSOFT_PERSONAL,
    ITEM_EDIT_BROADSOFT_PERSONAL,
    ITEM_ADD_TO_PERSONAL,
    ITEM_BROADSOFT_SEARCH,
    ITEM_ADD_TO_CONTACT,
    ITEM_ADD_TO_BLACKLIST,
    ITEM_ADD_BLACKLIST,
    ITEM_ADD_GROUP,
    ITEM_EDIT_GROUP,
    ITEM_DETAIL,
    ITEM_NUMBER,
    ITEM_BROADSOFT_PASSWORD,
};

// 显示类型,目前是菜单,列表及列表项
enum DISPLAY_TYPE
{
    DISPLAY_NONE = 0,
    DISPLAY_MENU,
    DISPLAY_LIST,
    DISPLAY_ITEM
};

// 保存每个页面的信息
struct PageType
{
    int nDirType;               // 联系人类型
    int nUIState;               // UI状态
    DISPLAY_TYPE eDisplayType;  // 显示类型
    ITEM_OPERATION eItemOperation;
    int iFromIndex; // 获取数据的起始位置
    int iFocusIndex; // 界面焦点

    PageType()
    {
        Reset();
    }

    PageType & operator= (const PageType & pageType)
    {
        if (this != &pageType)
        {
            nDirType = pageType.nDirType;
            nUIState = pageType.nUIState;
            eDisplayType = pageType.eDisplayType;
            eItemOperation = pageType.eItemOperation;
            iFromIndex = pageType.iFromIndex;
            iFocusIndex = pageType.iFocusIndex;
        }

        return *this;
    }

    void Reset()
    {
        nDirType = DIRECTORY_TYPE_NONE;
        nUIState = DIR_UI_STATE_NONE;
        eDisplayType = DISPLAY_NONE;
        eItemOperation = ITEM_UNKNOWN;
        iFromIndex = 0;
        iFocusIndex = 0;
    }
};
#endif

// LDAP联系人列表
typedef YLList<LdapContactData> LIST_LDAP_CONTACT_DATA;

// 联系人属性
struct ST_CONTACT_ATTRIBUTE
{
    yl::string m_strName;
    yl::string m_strValue;
};

// 联系人属性链表定义
typedef YLList<ST_CONTACT_ATTRIBUTE> LIST_ATTRIBUTE;
typedef YLVector<ST_CONTACT_ATTRIBUTE> VEC_ATTRIBUTE;

enum
{
    DIR_RES_TYPE_NONE   = 0,
    DIR_RES_TYPE_RING   = 1,
    DIR_RES_TYPE_PHOTO  = 2,
};

class QLineEdit;

bool Res_GetResType(int nResType, yl::string & strResType);
bool Res_GetNameByPath(int nResType, const yl::string & strPath, yl::string & strName);
bool ParseFileName(const yl::string & strFilePath, yl::string & strFile);
bool ParseFilePath(const yl::string & strFilePath, yl::string & strPath, yl::string & strFile);
bool IsBsftAllowSipAuth();

void DirUI_SetNameIME(QLineEdit * pNameEdit);

bool DirUI_AdjustFrameList(CFrameList* m_pFrameList, bool bReset, QRect rcDefault, int nOffset, int nTop);

#define CALLMGR_PAGE_NAME_CALL_LOG "history"
#define CALLMGR_PAGE_NAME_DIRECTORY "directory"

#if DIR_SUPPORT_SELECT_CONTACT
typedef void(*DIR_SELECT_CONTACT_FUN)(yl::string strContactName, yl::string strContactNumber);
#endif

#endif
