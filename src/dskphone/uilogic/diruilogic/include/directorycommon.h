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
#include <diruilogic/include/diruilogicdefine.h>


// 联系人ID数组
typedef YLVector<int> VEC_CONTACT_ID;

#define DIR_INVALID_AMOUNT -1

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
    DIRECTORY_TYPE_BLUETOOTH    = 6,                    // 蓝牙联系人
    DIRECTORY_TYPE_REMOTE_SEARCH    = 7,                // 远程联系人服务器搜索
    DIRECTORY_TYPE_PLCM         = 8,                    // PLCM联系人
    DIRECTORY_TYPE_GENBAND      = 9,                    // GB联系人
    DIRECTORY_TYPE_METASWICTH   = 10,                   // MTSW联系人
    DIRECTORY_TYPE_MAX,

    CALL_LOG_TYPE_LOCAL         = 100,                  // 本地历史记录
    CALL_LOG_TYPE_BROADSOFT     = 101,                  // Broadsoft联系人
    CALL_LOG_TYPE_MTSW          = 102,                  // MTSW历史记录
};

// 联系人搜索类型
enum
{
    DIRECTORY_SEARCH_TYPE_NONE          = -1,
    DIRECTORY_SEARCH_TYPE_NORMAL        = 0,            // 联系人普通搜索
    DIRECTORY_SEARCH_TYPE_LDAP          = 1,            // LDAP搜索
    DIRECTORY_SEARCH_TYPE_BROADSOFT     = 2,            // BroadSoft高级搜索
    DIRECTORY_SEARCH_TYPE_MAX
};

// 联系人UI状态类型
enum
{
    DIR_UI_STATE_NONE           = 0,
    DIR_UI_STATE_BROWSER,
    DIR_UI_STATE_SEARCH,
    DIR_UI_STATE_DETAIL,
    DIR_UI_STATE_ADDITION,
    DIR_UI_STATE_PRECISE_SEARCH,
#ifdef DIR_SUPPORT_SETTING
    DIR_UI_STATE_SETTING,
#endif
    DIR_UI_STATE_CHOOSE_NUMBER,
    DIR_UI_STATE_CHG_PWD,
    DIR_UI_STATE_CHOOSE_PHOTO,

    DIR_UI_STATE_LDAP_CHG_PWD,
    DIR_UI_STATE_CHOOSE_ACCOUNT,
};

// 联系人历史记录
enum
{
    DIR_ACTION_NONE                         = 0,
    DIR_ACTION_COMMON_DETAIL                = 1,
    DIR_ACTION_COMMON_SELECT                = 2,
    DIR_ACTION_COMMON_SEND                  = 3,
    DIR_ACTION_COMMON_BACK                  = 4,

    DIR_ACTION_LOCAL_ENTER_ADD_GROUP,
    DIR_ACTION_LOCAL_ADD_GROUP,
    DIR_ACTION_LOCAL_ADD_GROUP_FINISH,
    DIR_ACTION_LOCAL_EDIT_GROUP,
    DIR_ACTION_LOCAL_EDIT_GROUP_FINISH,
    DIR_ACTION_LOCAL_DEL_GROUP,
    DIR_ACTION_LOCAL_DEL_ALL_GROUP,
    DIR_ACTION_LOCAL_ENTER_ADD_CONTACT,
    DIR_ACTION_LOCAL_ADD_CONTACT,
    DIR_ACTION_LOCAL_DSSKEY_ADD_CONTACT,
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
    DIR_ACTION_LOCAL_EDIT_FAVORITE,
    DIR_ACTION_LOCAL_ENTER_CHOOSE_PHOTO,

    DIR_ACTION_REMOTE_UPDATE,
    DIR_ACTION_REMOTE_DOWNLOAD,
    DIR_ACTION_REMOTE_SERVER_SEARCH,
    DIR_ACTION_REMOTE_PREVPAGE,
    DIR_ACTION_REMOTE_NEXTPAGE,

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
    DIR_ACTION_BSFT_ENTER_PRECISE_SEARCH,
    DIR_ACTION_BSFT_PRECISE_SEARCH,
    DIR_ACTION_BSFT_BACK_TO_MENU_LIST,
    DIR_ACTION_BSFT_BACK_TO_SEARCH_RESULT,

    DIR_ACTION_PLCM_UPDATE,
    DIR_ACTION_PLCM_DOWNLOAD,

    DIR_ACTION_LCLOG_DEL_CALL_LOG,
    DIR_ACTION_LCLOG_DEL_ALL_CALL_LOG,
    DIR_ACTION_LCLOG_BATCH_DEL_CALL_LOG,
    DIR_ACTION_LCLOG_EDIT,

    DIR_ACTION_BSFTLOG_EDIT,
    DIR_ACTION_BSFTLOG_CLEAR,
    DIR_ACTION_BSFTLOG_DELETE,

    DIR_ACTION_BLUETOOTH_UPDATE,
    DIR_ACTION_BLUETOOTH_DOWNLOAD,

    DIR_ACTION_NAME_INVALID_TIP,
    DIR_ACTION_NUMBER_INVALID_TIP,
    DIR_ACTION_GROUP_NAME_INVALID_TIP,

    DIR_ACTION_LDAP_ENTER_CHG_PWD,
    DIR_ACTION_LDAP_CHG_PWD,

    DIR_ACTION_GBBOOK_SEARCH_MODE_CHANGE,
    DIR_ACTION_GENBAND_UPDATE,
    DIR_ACTION_GENBAND_ENTER_MANAGER_GROUP,

    DIR_ACTION_GENBAND_SEARCH_BY_NAME,
    DIR_ACTION_GENBAND_SEARCH_BY_FIRSTNAME,
    DIR_ACTION_GENBAND_SEARCH_BY_LASTNAME,
    DIR_ACTION_GENBAND_SEARCH_BY_NUMBER,
    DIR_ACTION_GENBAND_PRECISE_SEARCH,
    DIR_ACTION_GENBAND_DOWNLOAD,
    DIR_ACTION_GENBAND_SELECT_ACCOUNT,

    DIR_ACTION_GENBAND_ENTER_ADD_GROUP,
    DIR_ACTION_GENBAND_ADD_GROUP,
    DIR_ACTION_GENBAND_ENTER_EDIT_GROUP,
    DIR_ACTION_GENBAND_EDIT_GROUP,
    DIR_ACTION_GENBAND_DEL_GROUP,

    DIR_ACTION_GENBAND_ENTER_ADD_PAB,
    DIR_ACTION_GENBAND_ADD_PAB,
    DIR_ACTION_GENBAND_ENTER_EDIT_PAB,
    DIR_ACTION_GENBAND_EDIT_PAB,
    DIR_ACTION_GENBAND_DEL_PAB,

    DIR_ACTION_GENBAND_ENTER_ADD_TO_PAB,
    DIR_ACTION_GENBAND_ADD_TO_PAB,
    DIR_ACTION_GENBAND_COPY_PERSONAL_FINISH,

    DIR_ACTION_MTSW_UPDATE,
    DIR_ACTION_MTSW_DOWNLOAD,
    DIR_ACTION_MTSW_ENTER_ADD_CONTACTLISTL,
    DIR_ACTION_MTSW_ADD_CONTACTLIST,
    DIR_ACTION_MTSW_ADD_CONTACTLIST_FINISH,
    DIR_ACTION_MTSW_ENTER_EDIT_CONTACTLISTL,
    DIR_ACTION_MTSW_EDIT_CONTACTLIST,
    DIR_ACTION_MTSW_EDIT_CONTACTLIST_FINISH,
    DIR_ACTION_MTSW_ADD_TO_CONTACTLIST,
    DIR_ACTION_MTSW_DEL_CONTACTLIST,
    DIR_ACTION_MTSW_DEL_ALL_CONTACTLIST,
    DIR_ACTION_MTSW_ENTER_ADD_TO_LOCAL,
    DIR_ACTION_MTSW_ADD_TO_LOCAL,
    DIR_ACTION_MTSW_ENTER_CHG_PWD,
    DIR_ACTION_MTSW_CHG_PWD,
    DIR_ACTION_MTSW_SORT,

    DIR_ACTION_COMMON_BTRANSFER,
    DIR_ACTION_MSGBOX_DETAIL_DELETE_BACK,
};

enum DIR_DIAL_ACTION
{
    DIR_DIAL_ACTION_NONE                    = 0,
    DIR_DIAL_ACTION_DIGIT_MAP_HISTORY       = 1,
    DIR_DIAL_ACTION_DIGIT_MAP_DIRECTORY     = 2,
    DIR_DIAL_ACTION_ADD_AREA_CODE           = 4,
};

enum
{
    DIR_UI_ACTION_NONE              = 0,
    DIR_UI_ACTION_COMMON_BACK,
    DIR_UI_ACTION_COMMON_HOME,
    DIR_UI_ACTION_RING_SETTING,
    DIR_UI_ACTION_NUMBER_CHOOSE,
    DIR_UI_ACTION_COMMON_SELECT_BOX,

    DIR_UI_ACTION_COMMON_END,
};

enum
{
    DIR_UI_REFRESH_TYPE_NONE            = 0,
    DIR_UI_REFRESH_TYPE_TITLE           = 1,
    DIR_UI_REFRESH_TYPE_SOFTKEY         = 2,
    DIR_UI_REFRESH_TYPE_LIST_VIEW       = 4,
    DIR_UI_REFRESH_TYPE_ALL             = 7,
};

enum
{
    DIR_UI_REFRESH_PARAM_NONE           = 0,
    DIR_UI_REFRESH_PARAM_DATA_CHANGE    = 1,
    DIR_UI_REFRESH_PARAM_RESET_FOCUS    = 2,
    DIR_UI_REFRESH_PARAM_ALL            = 3
};

#if IF_FEATURE_CALLLIST_SHOW_METHOD
//Calllog List显示记录的方式
enum
{
    CL_SHOW_ONLY_NAME                   = 0,    //仅显示名字
    CL_SHOW_ONLY_NUMBER                 = 1,    //仅显示号码
    CL_SHOW_NAME_AND_NUMBER             = 2,    //名字在前，号码在后
    CL_SHOW_NUMBER_AND_NAME             = 3,    //号码在前，名字在后
    CL_SHOW_FULL_CONTACT_INFO           = 4,    //显示完成的信息
};
#endif

enum
{
    CL_NORMAL_SHOW_ONLY_NAME = 0,  //仅显示名字
    CL_NORMAL_SHOW_ONLY_NUMBER = 1, //仅显示号码
    CL_NORMAL_SHOW_NAME_AND_NUMBER = 2, // 名字在上，号码在下
};

// Auto帐号
#define AUTO_ACCOUNT_IDENTIFY -1

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
    DETAIL_DATA_TYPE_NONE   = 0,
    DETAIL_DATA_TYPE_NUMBER = 1,
};

struct ST_DETAIL_DATA
{
    int nType;
    ST_CONTACT_ATTRIBUTE stAttr;
};
typedef YLVector<ST_DETAIL_DATA> VEC_DETAIL_DATA;

// 界面状态链表
typedef YLList<int> LIST_DIR_UI_STATE;

enum
{
    DIR_RES_TYPE_NONE   = 0,
    DIR_RES_TYPE_RING   = 1,
    DIR_RES_TYPE_PHOTO  = 2,
};

bool Res_GetResType(int nResType, yl::string & strResType);
bool Res_GetNameByPath(int nResType, const yl::string & strPath, yl::string & strName);
bool ParseFileName(const yl::string & strFilePath, yl::string & strFile);
bool ParseFilePath(const yl::string & strFilePath, yl::string & strPath, yl::string & strFile);
bool IsBsftAllowSipAuth();
void TransBsftFACText(const yl::string & strValue, yl::string & strText);
time_t TransBsftCallLogTime(const char * pszCallLog, bool bHaveSecond);
#if IF_FEATURE_METASWITCH_CALLLOG
time_t TransMTSWCallLogTime(const char * pszCallLog, bool bHaveSecond);
#endif
#ifndef WIN32
#define _snprintf snprintf
#endif

#if DIR_SUPPORT_SELECT_CONTACT
typedef void(*DIR_SELECT_CONTACT_FUN)(yl::string strContactName, yl::string strContactNumber);
#endif

// 定义联系人属性名
extern const char * DIR_ATTR_NAME;
extern const char * DIR_ATTR_OFFICENUMBER;
extern const char * DIR_ATTR_MOBILENUMBER;
extern const char * DIR_ATTR_OTHERNUMBER;
extern const char * DIR_ATTR_NUMBER;
extern const char * DIR_ATTR_LINE;
extern const char * DIR_ATTR_RING;
extern const char * DIR_ATTR_GROUP;
extern const char * DIR_ATTR_FAVORITE_INDEX;

// 定义broadsoft联系人属性名
extern const char * DIR_ATTR_BSFT_LASTNAME;
extern const char * DIR_ATTR_BSFT_FIRSTNAME;
extern const char * DIR_ATTR_BSFT_USERID;
extern const char * DIR_ATTR_BSFT_GROUPID;
extern const char * DIR_ATTR_BSFT_EXTENSION;
extern const char * DIR_ATTR_BSFT_NUMBER;
extern const char * DIR_ATTR_BSFT_MOBILE;
extern const char * DIR_ATTR_BSFT_YAHOOID;
extern const char * DIR_ATTR_BSFT_PAGER;
extern const char * DIR_ATTR_BSFT_EMAIL;
extern const char * DIR_ATTR_BSFT_IMPID;
extern const char * DIR_ATTR_BSFT_LOCATION;
extern const char * DIR_ATTR_BSFT_TITLE;
extern const char * DIR_ATTR_BSFT_ADDRESS1;
extern const char * DIR_ATTR_BSFT_ADDRESS2;
extern const char * DIR_ATTR_BSFT_CITY;
extern const char * DIR_ATTR_BSFT_STATE;
extern const char * DIR_ATTR_BSFT_COUNTRY;
extern const char * DIR_ATTR_BSFT_CODE;
extern const char * DIR_ATTR_BSFT_DEPARTMENT;
extern const char * DIR_ATTR_BSFT_PASSWORD;

extern const char * DIR_ATTR_MTSW_FIRST_NAME;
extern const char * DIR_ATTR_MTSW_LAST_NAME;
extern const char * DIR_ATTR_MTSW_NICK_NAME;
extern const char * DIR_ATTR_MTSW_JOBTITLE;
extern const char * DIR_ATTR_MTSW_ORGANISATION;
extern const char * DIR_ATTR_MTSW_HOMENUMBER;
extern const char * DIR_ATTR_MTSW_WORKNUMBER;
extern const char * DIR_ATTR_MTSW_MOBILENUMBER;
extern const char * DIR_ATTR_MTSW_FAXNUMBER;
extern const char * DIR_ATTR_MTSW_OTHERNUMBER;
extern const char * DIR_ATTR_MTSW_DEPARTMENT;
extern const char * DIR_ATTR_MTSW_EXTENTION;
extern const char * DIR_ATTR_MTSW_NAME;
extern const char * DIR_ATTR_MTSW_NUMBER;
extern const char * DIR_ATTR_MTSW_EXTENSION;

#endif
