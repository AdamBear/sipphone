#ifndef _DIR_ENUM_TYPE_HEDADER
#define _DIR_ENUM_TYPE_HEDADER
// T48特制
//#define DIRECTORY_T48

#define DEBUG_CONTACT 0

enum DIR_TYPE
{
    BASE_DIR,
    LOCAL_DIR,
    LOCAL_LOG,
    BSFT_LOG,
    BSFT_DIR,
    REMOTE_DIR,
    BLUETOOTH_DIR,
    PLCM_DIR,
    GB_DIR,
    MTSW_LOG,
    MTSW_DIR,
    //以下类型不能使用联系人公共接口，有自己的接口实现
    LDAP_DIR,
    UC_DIR,
    CLOUD_DIR,
};

enum DIR_INHERIT_TYPE
{
    DIR_INHERIT_BASE,
    DIR_INHERIT_GROUP,
    DIR_INHERIT_CONTACT
};

enum DIR_DATA_ENTRY_TYPE
{
    DIR_TYPE_NONE,
    DIR_TYPE_ERROR,
    DIR_DATA_ROOT,

    DIR_TYPE_GROUP,

    DIR_TYPE_LOCAL_GROUP,
    DIR_TYPE_LOCAL_CONTACT,
    DIR_TYPE_BROADSOFT_MENU,
    DIR_TYPE_BROADSOFT_ITEM_GROUP,   // Group broadsoft contact.
    DIR_TYPE_BROADSOFT_ITEM_PERSONAL,  // Personal broadsoft contact.
    DIR_TYPE_BROADSOFT_ITEM_COMMON,
    DIR_TYPE_BROADSOFT_ITEM_ENTERPRISE,
    DIR_TYPE_BROADSOFT_ITEM_CUSTOM,
    DIR_TYPE_METASWITCH_MENU,
    DIR_TYPE_METASWITCH_ITEM_CONTACTLIST,
    DIR_TYPE_METASWITCH_ITEM_EXTENTION,
    DIR_TYPE_METASWITCH_ITEM_MLHG,

    DIR_TYPE_REMOTE_MENU,  // Remote menu contact.
    DIR_TYPE_REMOTE_UNIT,  // Remote contact unit, is a contact.
    DIR_TYPE_REMOTE_HOTKEY,

    DIR_TYPE_BSFT_LOG_GROUP,
    DIR_TYPE_BSFT_CALLLOG,
    DIR_TYPE_LOCALLOG_GROUP,    // 本地历史记录组
    DIR_TYPE_LOCALLOG_CALLLOG,  // 本地历史记录
    DIR_TYPE_MTSW_LOG_GROUP,
    DIR_TYPE_MTSW_CALLLOG,
    DIR_TYPE_BT_CONTACT,


    DIR_TYPE_CLOUD_ITEM_ENTERPRISE,

    DIR_TYPE_SHORTCUT,  // 联系人快捷方式
#if IF_FEATURE_GBBOOK
    DIR_TYPE_GB_MENU,
    DIR_TYPE_GB_UNIT,
#endif
    DIR_TYPE_LDAP,
};

enum SearchMethod
{
    INVALID_METHOD = 0,
    SEARCH_BY_T9 = 1,  // T9搜索.
    SEARCH_BY_NAME = 2,  // 普通搜索, 按名字搜索.
    SEARCH_BY_AUTO //自动
};

enum SearchHitType//搜索命中类型
{
    HIT_TYPE_NONE,//没有命中
    HIT_TYPE_NAME,//名字
    HIT_TYPE_NUM//号码
};

// 文件下载状态.
enum FileDownloadStatus
{
    FILE_DOWNLOAD_STATUS_NONE = 0,  // 不正确的状态.
    FILE_DOWNLOAD_READY = 1,  // 准备好.
    FILE_DOWNLOADING = 2,  // 文件正在下载.
    FILE_DOWNLOADED_OK = 3,  // 文件下载成功.
    FILE_DOWNLOAD_FAIL = 4,  // 文件下载失败.
    FILE_DOWNLOAD_CANCELED = 5,  // 取消下载.
    FILE_DOWNLOAD_MENU_UPPER_LIMIT_REACHED = 6,  // 超过菜单联系人级数上限.
};

// enum RemoteHotKeyStatus
// {
//  HOTKEY_NONE,
//  HOTKEY_DOWNLOADING,
//  HOTKEY_DOWNLOADED_OK,  // 文件下载成功.
//  HOTKEY_DOWNLOAD_FAIL,  // 文件下载失败.
//  HOTKEY_DOWNLOAD_CANCELED,  // 取消下载.
// };


// 导入联系人文件的错误类型, 主要给网页用.
enum ImportErrorType
{
    IMPORT_ERROR_TYPE_NONE = -1,  // 不正确的错误值.
    IMPORT_NO_ERROR = 0,  // 无错误.
    IMPORT_INVALID_FILE = 1,  // 不正确的文件格式.
    IMPORT_DUPLICATE_GROUP = 2,  // 重复的组.
    IMPORT_DUPLCATE_CONTACT = 3,  // 重复的联系人.
    IMPORT_TOO_MANY_GROUP = 4,  // 组数量超过上限.
    IMPORT_TOO_MANY_LOCAL_CONTACT = 5,  // 本地联系人数量超过上限.
    IMPORT_TOO_MANY_BLACKLIST = 6,  // 黑名单数量超过上限.
    IMPORT_INVALID_PATH = 7,  // 不正确的文件路径.
    IMPORT_GROUP_EMPTY = 8,  // 组id或者名为空.
    IMPORT_CONTACT_NAME_EMPTY = 9,  // 联系人名为空.
    IMPORT_NO_DEFAULT_GROUP = 10,  // 默认组丢失.
    IMPORT_NO_CONTACT_GROUP = 11,  // 文件无记录.
    IMPORT_CONTACT_IS_ASYNCING = 12,  // 联系人正在同步.
    IMPORT_TALK_EXIST = 16,  // 当前存在通话，禁止导入，避免通话阻塞.
    IMPORT_OTHER_ERROR
};


// 联系人文件类型.
enum ContactFileType
{
    CONTACT_FILE_TYPE_NONE = -1,  // 不正确的文件类型.
    CONTACT_FILE_T3X = 0,  // T3X.
    CONTACT_FILE_V40 = 1,  // V40.
    CONTACT_FILE_V50 = 2,  // V50.
    CONTACT_FILE_BLACKLIST, // dect blacklist
    CONTACT_FILE_TYPE_UNKOWN,  // 未知类型.
    CONTACT_FILE_TYPE_MAX
};

enum DIRECTORY_RET_CODE
{
    DIR_RET_SUCC = 0,//操作成功
    DIR_RET_FAIL = -1,//操作失败

    DIR_RET_GROUP_DUPLICATE = -2,//组重复
    DIR_RET_CONTACT_NOT_BLACK = -3,//联系人并非黑名单
    DIR_RET_INVALID = -4,//数据格式错误或者非法
    DIR_RET_EMPTY = -5,//非空字段存在空字符
    DIR_RET_GROUP_UNEXISTED = -6, //联系组不存在
    DIR_RET_GROUP_SIZE_UNEXPECTED = -7,//超过最大允许组范围
    DIR_RET_GROUP_NAME_EMPTY = -8, //组名为空
    DIR_RET_CONTACT_EXIST = -9,

    DIR_RET_RERROR_NETWORK_ERROR = -10,

    DIR_RET_SEARCH_PARM_ERROR = -11,

    DIR_RET_FAVORITE_INDEX_MAX = -12,
    DIR_RET_FAVORITE_INDEX_EXIST = -13,

    DIR_RET_CONTACTID_UNEXISTED = -14,
    DIR_RET_FAVORITE_INDEX_INVALID = -15,

    GET_LIST_DOWNLOADING = 1,  // List is downloading.
    GET_LIST_DOWNLOADED_FAIL = 2,  // Fail to download.
    GET_LIST_NONE_DOWNLOAD = 3,  // No page entered, so none file downloaded.
    GET_LIST_READ_CONFIG_FAIL = 4,  // Fail to read config file.
    GET_LIST_CONFIG_FILE_IS_EMPTY = 5,  // Config file is empty.
    GET_LIST_CONFIG_FILE_INVALID = 6,  // Config file is invalid.
    GET_LIST_NETWORK_NOT_CONNECTED = 7,  // Network is not connected.
    GET_LIST_USER_CANCELED = 8,  // 取消下载.
    GET_LIST_UPPER_LIMIT_REACHED = 9,  // 级数超过上限.
    GET_LIST_DOWNLOAD_OTHER_ERROR = 10,  // Other error.
    GET_LIST_RESULT_TYPE_MAX
};

enum BSFT_SEARCH_TYPE
{
    SEARCH_NONE,
    SEARCH_LOCAL,
    SEARCH_SERVER,
};

enum BSFT_DELETE_TYPE
{
    BSFT_DELETE_SINGLE = 1,//删除一条记录
    BSFT_DELETE_GROUP = 2,//删除一组记录
};

//抽象跟节点
#define kszRootId -1

// 联系人组ID定义
#define kszAllContactId 0

#define kszFavoriteGroupId 16

#define kszBlackListGroupId 17

#define kszServerSearchGroupId 18

#define kszSearchGroupId 19
//连续ID便于操作，误轻易改动
#define kszPersonalId           1   //Personal组ID
#define kszGroupId              2   //Group组id
#define kszEnterPriseId         3   //EnterPrise组ID
#define kszGroupCommonId        4   //Common组ID
#define kszEnterPriseCommonId   5   //EnterPrise Common组ID
#define kszCustomId             6   //Custom组ID\

#define kszViewMoreId           -11

//metaswitch 联系人使用分组
#define kszContactList 7
#define kszExtention 8
#define kszMLHG 9
#define kszMtswAllContact 10
#define kszTempContact 11
#define kszPlacedLogId      1
#define kszReceivedLogId    2
#define kszMissLogId        3
#define kszForWardedLogId   4
#define kszMissNotifId      5
#define kszForwardNotifId   6

#define kszPlcmDirGroupId 1

#define kszBluethoothGroupId 1

#define kszBsftLogRootGroupFirstId 1

#define kszLDAPGroupID 1

//预加载组ID
#define kszPreLoadId  777

// 不正确的id值.
#define knInvalidRId  -16

#define kszAllBuddiesId 1
#define kszFavouriteId 2

// 允许的组数量
#define DIRECTORY_MAX_GROUP_SIZE        49 ////不包括BlackList
// 联系人最大数量
#define DIRECTORY_MAX_CONTACT_SIZE      1000
// 黑名单最大数量
#define DIRECTORY_MAX_BLACKLIST_SIZE    30

#define LOCAL_FAVORITE_INDEX_MAX        999999999


#define kszOfficeNumber  "office_number"
#define kszMobileNumber  "mobile_number"
#define kszOtherNumber "other_number"


#define kszBsftNumber "Number"
#define kszBsftExtension "Extension"
#define kszBsftMobile "Mobile"

#define kszCloudNumber "Number"

#define kszMtswHomeNumber "_HOME"
#define kszMtswWorkNumber "_WORK"
#define kszMtswMobile "_CELL"
#define kszMtswFax "_FAX"
#define kszMtswOtherNumber "NUMBER"

#define kszMtswDirectoryNumber "DirectoryNumber"
#define kszMtswExtention "Extention"

#define kszMtswMLHGNumber "MLHGNumber"

/************************************************************************/
/*                          For     Dect                                */
/************************************************************************/

enum GET_GROUP_TYPE
{
    GET_REQUESTID,
    GET_ALLCONTACT,
    GET_SEARCH,
    GET_BSFT_SEARCH
};

// 默认的任务ID
#define INVALID_TASK_ID -1
#define DEFSESSIONID    1000

// dect 手柄个数
#ifdef _DECT
#define DECT_HANDSET_NUM    5
#else
#define DECT_HANDSET_NUM    0
#endif

// dect 根节点起始ID
#define kszDectGroupID      20
// dect All Contact 组起始ID
#define kszDectAllContactID         (kszDectGroupID + DECT_HANDSET_NUM)
// dect 搜索组起始ID
#define kszDectSearchGroupID        (kszDectAllContactID +  DECT_HANDSET_NUM)
// dect BSFT搜索组起始ID
#define kszDectServerSearchGroupID  (kszDectSearchGroupID + DECT_HANDSET_NUM)

#define kszFreeID                   (kszDectServerSearchGroupID + DECT_HANDSET_NUM)

enum REQUEST_ID
{
    INVALID_REQUEST = -2,
    DEFAULT_ID = -1,        // 全局联系人使用这个ID，以下是区分DECT手柄的ID
    DECT_HANDSET_1 = kszDectGroupID,
#ifdef _DECT
    DECT_HANDSET_2 = kszDectGroupID + 1,
    DECT_HANDSET_3 = kszDectGroupID + 2,
    DECT_HANDSET_4 = kszDectGroupID + 3,
    DECT_HANDSET_5 = kszDectGroupID + 4,
#else
    DECT_HANDSET_2 = DECT_HANDSET_1,
    DECT_HANDSET_3 = DECT_HANDSET_1,
    DECT_HANDSET_4 = DECT_HANDSET_1,
    DECT_HANDSET_5 = DECT_HANDSET_1,
#endif
};

#if IF_FEATURE_GBBOOK

enum GBBOOK_OPERATION_TYPE
{
    PAB_GAB_NONE,
    PAB_OPERATION_ADD_GROUP,
    PAB_OPERATION_REMOVE_GROUP,
    PAB_OPERATION_MODIFY_GROUP,

    PAB_OPERATION_ADD_CONTACT,
    PAB_OPERATION_REMOVE_CONTACT,
    PAB_OPERATION_MODIFY_CONTACT,

    PAB_OPERATION_GET_CONTACTLIST,
    PAB_OPERATION_GET_GROUPLIST,

    GAB_SEARCH_NAME,
    GAB_SEARCH_FIRSTNAME,
    GAB_SEARCH_LASTNAME,
    GAB_SEARCH_NUMBER
};

#define kszGabBookGroupId 1

#define kszPabBookGroupId 2

#define kszGbNAGroupId 3

#define kszGbBookFax "fax"

#define kszGbBookbusinessNumber "businessPhoneNumber"

#define kszGbBookhomeNumber "homePhoneNumber"

#define kszGbBookmobile "mobile"

#define kszGbBookpager "pager"

#define kszGbBookprimaryContact "primaryContact"

#define kszGbconferenceURL "conferenceURL"

#endif

#endif
