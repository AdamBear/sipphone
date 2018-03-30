#ifndef __DIRECTORY_DATA_DEFINE_H__
#define __DIRECTORY_DATA_DEFINE_H__

// #define  MAX_PHONEBOOK_URL_LENGTH 548        //远程地址簿URL长度
// #ifndef size_DISPLAY_NAME
// #define size_DISPLAY_NAME        100
// #endif
// #ifndef size_USER_NAME
// #define size_USER_NAME       100
// #endif
// #ifndef size_SERVER_NAME
// #define size_SERVER_NAME         100
// #endif

/*
#define MAX_CALLLIST_NUM    100     //最大呼叫记录数目
#define MAX_CONTACT_NUM     300     //最大联系人数目
#define MAX_BLACKLIST_NUM   30      //最大黑名单数目
#define MAX_REMOTEBOOK_NUM  32000   //最大远程地址簿的记录数目
#define MAX_REMOTE_NUM      5       //最大远程地址簿个数
#define MAX_LOCALGROUP_NUM  5       //最大本地分组数(不包括联系人和黑名单)
#define MAX_BROADSOFT_NUM   6       //最大BroadSoft地址簿个数
#define MAX_CALLLOG_NUM     3       //最大Call Log地址簿个数
#define MAX_LDAP_SEARCH_NUM 32      //最大LDAP允许搜索字符个数

//Contact Menu类型
enum CONTACT_MENU_TYPE
{
    CMT_LOCAL = 0,
    CMT_BLACK,
    CMT_REMOTE,
    CMT_BROADSOFT_,
    CMT_LDAP,
    CMT_CALLLOG_,
    CMT_LAST, // 此值也被作为联系人选项隐藏列表数字大小,不应被设置为0或者负值
};


//contact item 类型
enum CONTACT_ITEM_TYPE
{
    CIT_EDIT = 0,
    CIT_ADD,
    CIT_SHOWXML,
    CIT_UNKNOW,
};
*/

/*
enum CALLLIST_TYPE
{
    CL_FIRST = 0x100,
    CL_MAIN,
    CL_MISS,
    CL_ANSWERED,
    CL_DIALED,
    CL_FORWARDED,
    CL_FRAME,   // 从Frame进入,AccountId为-1
    CL_CONTACT, // CallList保存到Contact时进入
    CL_LAST,
};

enum CONTACTLIST_TYPE
{
    CNTL_FIRST = 0x1000,
    CNTL_CALL_TO_CONTACT,
    CNTL_FRAME_TO_CONTACT,  //从Frame进入，AccountId为-1
    CNTL_IDLE_TO_MAINMENU,  //Idle界面按Dir进入联系人主界面
    //Joyce T2X 所有最新的软件，需要修改翻译
    //http://192.168.1.99/Bug.php?BugID=13630
    CNTL_IDLE_TO_LOCAL_CONTACT,// Idle界面按Dir直接进入本地联系人界面
    CNTL_IDLE_TO_REMOTE,
    CNTL_LDAP_TO_CONTACT,
    CNTL_POOL_TO_LOCAL,
    CNTL_POOL_TO_REMOTE,
    CNTL_POOL_TO_BROADSOFT,
    CNTL_ENTER_TO_LOCAL,        //进入某个本地群组联系人
    CNTL_ENTER_TO_REMOTE,       //进入某个远程联系人
    CNTL_ENTER_TO_BROADSOFT,    //进入某个BroadSoft联系人
    CNTL_ENTER_TO_CALLLOG,      //进入Call Log菜单界面
    CNTL_ENTER_TO_BS_MENU,      //进入BroadSoft菜单界面
    CNTL_FRAME_TO_CALLLOG,      //Frame进入Call Log菜单界面
    CNTL_LAST
};
*/

// 发给呼叫记录管理程序的消息
// enum CALLLIST_MSG_CODE
// {
//  CALLLIST_MSG_FIRST  = 0x50000   , // 327680
//  CALLLIST_MSG_ADD_RECORD         , //01 wParam:0             lParam:0            extra:  struct call_record_t
//  CALLLIST_MSG_PHONEWORK_STATUS   , //02 wParam:phone work status(0-free, 1-busy) lParam:0
//  CALLLIST_MSG_CONTACT_TO_CALL    , //03 wParam:0/1(0-CallList active)     lParam:0   extra: struct contact_to_call
//  CALLLIST_MSG_SAVE_TO_FILE       , //04 wParam:0             lParam:0
//  CALLLIST_MSG_CLEAR_CALLLIST     , //05 wParam:CT_CALLALL    lParam:0
// #if IF_ATLINK_SPECIFIC
//  CALLLIST_MSG_REDIAL,
// #endif
//  CALLLIST_MSG_LAST
// };

/*
//发给ContactList管理程序的消息
enum CONTACTLIST_MSG_CODE
{
    CONTACTLIST_MSG_FIRST   = 0xa0000   , // 327680
    CONTACTLIST_MSG_CALL_TO_CONTACT     , //01 wParam:0 lParam:0/1(0:联系人,1:黑名单)     extra:  struct call_saveto_contact
    CONTACTLIST_MSG_SEARCH_DISPLAYNAME  , //02 wParam:0/1(1:查黑名单)   lParam:0/1(1:查显示名)  extra:  char[SIZE_USER_NAME] 呼叫号码
    CONTACTLIST_MSG_ENTER_XMLPHONEBOOK  , //03 wParam:xml phonebook index   lParam:0
    CONTACTLIST_MSG_CLEAR_DIRECTORY     , //04 wParam:0                     lParam:0
    CONTACTLIST_MSG_PHONEWORK_STATUS    , //05 wParam:0/1(1:正在通话)       lParam:0
    CONTACTLIST_MSG_GET_GROUPNAME       , //06 wParam:0                     lParam:0
    CONTACTLIST_MSG_SAVE_DIR2FILE       , //07 wParam:0                     lParam:0
    CONTACTLIST_MSG_REMOTE_LOAD         , //08 wParam:0(下载失败)/1(下载成功)   lParam:下载ID
    CONTACTLIST_MSG_SEARCH_REMOTE       , //09 wParam:Call ID               lParam:0            extra: char* Num
    CONTACTLIST_MSG_LAST
};
*/

/*
// 此结构的使用者 vpPhone(S),CallList(R),
struct call_record_t
{
    int iCALLID;            // 通话ID
    CALL_TYPE eCallType;    // 通话类型
    int uBornTick;          // 呼叫建立时间, UTC tick

    char szCallDuration[16];                        //通话时长
    char szLocaleSIPName[size_USER_NAME];           // 本地使用帐号对应的注册名
    char szLocaleSIPServer[size_SERVER_NAME];       // 本地使用帐号对应的主机名

    char szRemoteSIPName[size_USER_NAME];           // 对方注册名
    char szRemoteDisplayName[size_DISPLAY_NAME];    // 对方显示名
    char szRemoteSIPServer[size_SERVER_NAME];       // 对方注册主机名
};

// 此结构的使用者 CallList(R), ContactList
struct call_saveto_contact
{
    int iTopIndex;                          // CallList的顶点索引
    int iFocusIndex;                        // CallList的焦点索引
    int iCallType;                          // CallList的呼叫类型
    //BOOL bSave2Contact;                       // 是否保存到联系人:TRUE:保存到联系人;FALSE:保存到黑名单
    char szDisplayName[size_DISPLAY_NAME];  // 显示名
    char szSIPName[size_USER_NAME];         // 呼叫号码
};

// 此结构的使用者 CallList(R), ContactList
struct contact_to_call
{
    int  iTopIndex;                         // CallList的顶点索引
    int  iFocusIndex;                       // CallList的焦点索引
    int  iCallType;                         // CallList的呼叫类型
    //BOOL bChangeName;                     // 是否更改呼叫记录显示名
    char szDisplayName[size_DISPLAY_NAME];  // 显示名
    char szSIPName1[size_DISPLAY_NAME];     // 号码1
    char szSIPName2[size_DISPLAY_NAME];     // 号码2
    char szSIPName3[size_DISPLAY_NAME];     // 号码3
};

// 此结构体的使用者 TalkLogic(T), ContactList
struct active_contact
{
    int iLineID;    //传送的帐号ID,当前界面的帐号ID,如果Idle界面则为-1
//  UINT uIndex;    //传送的焦点序号,用于进入某个本地群组或远程地址簿
};


// 此结构体的使用者 TalkLogic(T), ContactList
#define size_URL_USERNAME  100
#define size_URL_PASSWORD  100

struct broadsoft_send
{
    char pServer[MAX_PHONEBOOK_URL_LENGTH];     //下载BroadSoft类型的远程地址簿发送的URL
    char pUserName[size_URL_USERNAME];  //下载BroadSoft类型的远程地址簿用到的用户名
    char pPassWord[size_URL_PASSWORD];  //下载BroadSoft类型的远程地址簿用到的密码
};
*/

// struct remote_name_info
// {
//  char szSIPName[size_USER_NAME];         // 搜索远程地址簿的号码
//  char szDisplayName[size_DISPLAY_NAME];  // 在远程地址簿中搜索到得显示名
// };

#include <ylstring.h>  // use memset.
#include <ylvector.h>
#include <yllist.h>

/*

#ifndef _T48
struct CommonDataBase
{
    virtual ~CommonDataBase(){}
};

// Contact group data.
struct ContactGroupData
{
    yl::string m_strDisplayName;  // Group display name.
    yl::string m_strRing;  // Group ring name.

    ContactGroupData()
    {
        Reset();
    }

    void Reset()
    {
        m_strDisplayName = "";
        m_strRing = "";
    }
};

struct ContactGroupSimpleData
{
    yl::string m_strDisplayName;  // 组显示名.
    int m_nGroupId;  // 组id.

    ContactGroupSimpleData()
    {
        Reset();
    }

    void Reset()
    {
        m_strDisplayName.clear();
        m_nGroupId = -1;
    }
};

// Contact general data.
struct ContactGeneralItemData
{
    yl::string m_strDisplayName;  // Contact display name.
    yl::string m_strGroupName;  // Group name.
    yl::string m_strOfficePhoneNumber;  // Office phone number.
    yl::string m_strMobilePhoneNumber;  // Mobile phone number.
    yl::string m_strOtherPhoneNumber;  // Other phone number.
    yl::string m_strRing;  // Ring name(no path).
    yl::string m_strDefaultPhoto;  // Default photo.
    int m_nLine;  // Line.
    int m_nSelectedPhoto;  // Selected photo.

    ContactGeneralItemData()
    {
        Reset();
    }

    void Reset()
    {
        m_strDisplayName = "";
        m_strGroupName = "";
        m_strOfficePhoneNumber = "";
        m_strMobilePhoneNumber = "";
        m_strOtherPhoneNumber = "";
        m_strRing = "";
        m_strDefaultPhoto = "";
        m_nLine = 0;
        m_nSelectedPhoto = 0;
    }
};

// Contact group list data, if m_nIndexFrom == 0 && m_nCountToGet == 0,
// Retrieve all group.
struct ContactGroupListData
{
    YLList<ContactGroupData> m_listGroup;  // Contact group list.
    int m_nIndexFrom;  // Retrieve from which index.
    int m_nCountToGet;  // Count to retrieve.
    int m_nTotalCount;  // Total group count;

    ContactGroupListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listGroup.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};

// Contact group list data, if m_nIndexFrom == 0 && m_nCountToGet == 0,
// Retrieve all group.
struct ContactGroupSimpleListData
{
    YLList<ContactGroupSimpleData> m_listGroup;  // Contact group list.
    int m_nIndexFrom;  // Retrieve from which index.
    int m_nCountToGet;  // Count to retrieve.
    int m_nTotalCount;  // Total group count;

    ContactGroupSimpleListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listGroup.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};

// Contact list data, if m_nIndexFrom == 0 && m_nCountToGet == 0,
// Retrieve all contact.
struct ContactGeneralItemListData : public CommonDataBase
{
    YLList<ContactGeneralItemData> m_listContact;  // Contact list.
    int m_nIndexFrom;  // Retrieve from which index.
    int m_nCountToGet;  // Count to retrieve.
    int m_nTotalCount;  // Total member count;

    ContactGeneralItemListData()
    {
        Reset();
    }

    void Reset()
    {
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};

// Contact searched list data, if m_nIndexFrom == 0 && m_nCountToGet == 0,
// Retrieve all contact.
struct ContactGeneralSearchItemListData : public CommonDataBase
{
    YLList<ContactGeneralItemData> m_listContact;  // Searched result list.
    yl::string m_strSearch;  // Search string.
    int m_nIndexFrom;  // Retrieve from which index.
    int m_nCountToGet;  // Count to retrieve.
    int m_nTotalCount;  // Total item count;

    ContactGeneralSearchItemListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listContact.clear();
        m_strSearch = "";
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};
#endif



enum _PRESENCE_TYPE
{
    _PRESENCE_TYPE_NONE = -1,
    _PRESENCE_TYPE_ONLINE,  // 用户在线而且准备好沟通
    _PRESENCE_TYPE_CHATTY,  // 用户在线而且希望和人聊天
    _PRESENCE_TYPE_AWAY,       // 用户在线,但是已经离开电脑10分钟
    _PRESENCE_TYPE_EXTENDED_AWAY,  // Extended Away
    _PRESENCE_TYPE_BUSY,       // 用户处于忙碌状态
    _PRESENCE_TYPE_OFFLINE, // 用户处于脱机状态
    _PRESENCE_TYPE_UNKNOWN, // 未知状态
    _PRESENCE_TYPE_MAX,
};

// 超级搜索结果.
struct SuperSearchItemData
{
    yl::string m_strName;  // 名字.
    yl::string m_strNumber;  // 号码.

    yl::string m_strContactId;
    yl::string m_strPic;
    _PRESENCE_TYPE m_eStatus;

    SuperSearchItemData()
    {
        Reset();
    }

    void Reset()
    {
        m_strName.clear();
        m_strNumber.clear();
        m_strContactId.clear();
        m_strPic.clear();
        m_eStatus = _PRESENCE_TYPE_NONE;
    }
};

// 超级搜索结果链表.
struct SuperSearchListData
{
    YLList<SuperSearchItemData> m_listResult;  // 结果链表.
    yl::string m_strKey;  // 搜索关键字.
    int m_nIndexFrom;  // 从何处下标开始取.
    int m_nCountToGet;  // 取多少个.
    int m_nTotalCount;  // 结果总数.

    SuperSearchListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listResult.clear();
        m_strKey.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};

/*

// Remote contact item type.
enum ContactRemoteItemType
{
    CONTACT_REMOTE_TYPE_NONE = 0,  // Invalid type.
    CONTACT_REMOTE_MENU = 1,  // Remote menu contact.
    CONTACT_REMOTE_UNIT = 2,  // Remote contact unit, is a contact.
    CONTACT_REMOTE_HOTKEY = 3,  // Remote contact hot-key.
    CONTACT_REMOTE_BROADSOFT = 4,
    CONTACT_REMOTE_PAGEKEY = 5,

    CONTACT_REMOTE_TYPE_MAX,
};

// Contact remote item data.
struct ContactRemoteItemData
{
    ContactRemoteItemType m_eType;  // Item type.
    yl::string m_strDisplayName;  // Display name.
    YLList<yl::string> m_listNumber;  // Phone number list.

    yl::string m_strPageUrl;

    ContactRemoteItemData()
    {
        Reset();
    }

    void Reset()
    {
        m_eType = CONTACT_REMOTE_TYPE_NONE;
        m_strDisplayName = "";
        m_listNumber.clear();
        m_strPageUrl.clear();
    }
};

// BroadSoft联系人配置项结构体.
struct BroadSoftConfigItem
{
    yl::string m_strDisplayName;  // 显示名.
    yl::string m_strServer;  // 服务器地址.
    yl::string m_strPort;  // 服务器端口.
    yl::string m_strUsername;  // 用户名.
    yl::string m_strPassword;  // 密码.
    yl::string m_strFullUrl;

    BroadSoftConfigItem()
    {
        Reset();
    }

    void Reset()
    {
        m_strDisplayName = "";
        m_strServer = "";
        m_strPort = "";
        m_strUsername = "";
        m_strPassword = "";
        m_strFullUrl = "";
    }

    // Override operator =.
    BroadSoftConfigItem& operator= (const BroadSoftConfigItem& rhs)
    {
        if (this != &rhs)
        {
            m_strDisplayName = rhs.m_strDisplayName;
            m_strServer = rhs.m_strServer;
            m_strPort = rhs.m_strPort;
            m_strUsername = rhs.m_strUsername;
            m_strPassword = rhs.m_strPassword;
            m_strFullUrl = rhs.m_strFullUrl;
        }
        return *this;
    }
};

// Remote contact item list. if m_nIndexFrom == 0 && m_nCountToGet == 0,
// Retrieve all contact.
struct ContactRemoteListData : public CommonDataBase
{
    YLList<ContactRemoteItemData> m_listContact;  // Remote contact list.
    int m_nIndexFrom;  // Retrieve from which index.
    int m_nCountToGet;  // Count to retrieve.
    int m_nSelelectedIndex;  // Current selected index.
    int m_nTotalCount;  // Total item count.

    bool m_bIsHaveNext;
    bool m_bIsHavePre;

    ContactRemoteListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listContact.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nSelelectedIndex = -1;
        m_nTotalCount = 0;
        m_bIsHaveNext = false;
        m_bIsHavePre = false;
    }
};

// BroadSoft contact type.
enum ContactBroadSoftItemType
{
    CONTACT_BROADSOFT_ITEM_TYPE_NONE = 0,  // Invalid type.
    CONTACT_BROADSOFT_ITEM_GROUP = 1,  // Group broadsoft contact.
    CONTACT_BROADSOFT_ITEM_PERSONAL = 2,  // Personal broadsoft contact.
    CONTACT_BROADSOFT_ITEM_COMMON = 3,
    CONTACT_BROADSOFT_ITEM_ENTERPRISE = 4,
    CONTACT_BROADSOFT_ITEM_CUSTOM =5,
    CONTACT_BROADSOFT_ITEM_TYPE_MAX
};

#ifndef _T48
// BroadSoft contact item.
struct ContactBroadSoftItemData
{
    ContactBroadSoftItemType m_eType;  // type.
    yl::string m_strDisplayName;  // Display name.
    yl::string m_strNumber;  // 电话号码.
    yl::string m_strUserId;  // 用户id.
    yl::string m_strGroupId;  // 组id.
    yl::string m_strExtension;  // 扩展.

    yl::string m_strMobile;  // 移动号码.
    yl::string m_strYahooId;  // Yahoo id.
    yl::string m_strPaper;  // Paper.
    yl::string m_strEmail;  // E-mail.
    yl::string m_strTitle;  // Title.
    yl::string m_strLocation;  // 位置.
    yl::string m_strAddressLine1;  // Address line 1.
    yl::string m_strAddressLine2;  // Address line 2.
    yl::string m_strCity;  // 城市.
    yl::string m_strState;  // 镇.
    yl::string m_StrZip;  // Zip号.
    yl::string m_strCountry;  // 国家.
    yl::string m_strDepartment;  // 部门.
    yl::string m_strImpId;

    ContactBroadSoftItemData()
    {
        Reset();
    }

    void Reset()
    {
        m_eType = CONTACT_BROADSOFT_ITEM_TYPE_NONE;
        m_strDisplayName = "";
        m_strUserId = "";
        m_strGroupId = "";
        m_strExtension = "";
        m_strNumber = "";
        m_strMobile = "";
        m_strYahooId = "";
        m_strPaper = "";
        m_strEmail = "";
        m_strTitle = "";
        m_strLocation = "";
        m_strAddressLine1 = "";
        m_strAddressLine2 = "";
        m_strCity = "";
        m_strState = "";
        m_StrZip = "";
        m_strCountry = "";
        m_strDepartment = "";
        m_strImpId = "";
    }
};

// BroadSoft contact list. if m_nIndexFrom == 0 && m_nCountToGet == 0,
// Retrieve all contact.
struct ContactBroadSoftListData : public CommonDataBase
{
    YLList<ContactBroadSoftItemData> m_listContact;  // BroadSoft contact list.
    int m_nIndexFrom;  // Retrieve from which index.
    int m_nCountToGet;  // Count to retrieve.
    int m_nTotalCount;  // Total item count.

    ContactBroadSoftListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listContact.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};

// Get contact list result value.
enum GetListResult
{
    GET_LIST_OK = 0,  // List has downloaded
    GET_LIST_DOWNLOADING = 1,  // List is downloading.
    GET_LIST_DOWNLOADED_FAIL = 2,  // Fail to download.
    GET_LIST_NONE_DOWNLOAD = 3,  // No page entered, so none file downloaded.
    GET_LIST_READ_CONFIG_FAIL = 4,  // Fail to read config file.
    GET_LIST_CONFIG_FILE_IS_EMPTY = 5,  // Config file is empty.
    GET_LIST_CONFIG_FILE_INVALID = 6,  // Config file is invalid.
    GET_LIST_NETWORK_NOT_CONNECTED = 7,  // Network is not connected.
    GET_LIST_USER_CANCELED = 8,  // 取消下载.
    GET_LIST_UPPER_LIMIT_REACHED = 9,  // 级数超过上限.
    GET_LIST_DOWNLOAD_OTHER_ERROR,  // Other error.
    GET_LIST_RESULT_TYPE_MAX
};
#endif

// 联系人图像链表结构体, 如果m_nIndexFrom == 0 && m_nCountToGet == 0,
// 则获取所有的图像.
struct ContactImageListData
{
    YLList<yl::string> m_listImage;  // 联系人图像链表.
    int m_nIndexFrom;  // 从何处index开始获取.
    int m_nCountToGet;  // 获取多少个.

    ContactImageListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listImage.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
    }
};

// 历史记录项结构体, 只显示简单数据.
struct CallLogSimpleData
{
    yl::string m_strDisplayName;  //  对方显示名.
#ifndef _T48
    yl::string m_strBornTick;  // 通话建立时间.
#else
    time_t m_tBornTick;  // 通话建立时间.
#endif
    yl::string m_strNumber;
    int m_nCallType;  // 通话类型. 是CALL_TYPE枚举类型.

    CallLogSimpleData()
    {
        Reset();
    }

    void Reset()
    {
        m_strDisplayName.clear();
#ifndef _T48
        m_strBornTick.clear();
#else
        m_tBornTick = time_t(NULL);
#endif
        m_strNumber.clear();
        m_nCallType = 0;  // CT_UNKNOW.
    }
};

// 历史记录链表结构体.
struct CallLogSimpleListData : public CommonDataBase
{
    YLList<CallLogSimpleData> m_listCallLog;
    int m_nIndexFrom;  // 从何处index开始获取.
    int m_nCountToGet;  // 获取多少个.
    int m_nTotalCount;  // 历史记录总数.

    CallLogSimpleListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listCallLog.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};

#ifndef _T48
// 历史记录详细信息.
struct CallLogDetailData : public CommonDataBase
{
    yl::string m_strName;  // 名称.
    yl::string m_strNumber;  // 号码.
    yl::string m_strServer;  // 服务器IP.
    yl::string m_strLocalSip; // 本地呼出号码
    yl::string m_strLocalSipServer; // 本地号码服务器
    yl::string m_strBornTick;  // 建立时间.
    time_t m_tBornTick;     //time_t格式的时间
    yl::string m_strDuration;  // 通话时长.
    yl::string m_strCountryCode; //
    yl::string m_strPicture;  // 头像(全路径).
    int m_nCallType;  // 通话类型. 是CALL_TYPE枚举类型.

    CallLogDetailData()
    {
        Reset();
    }

    void Reset()
    {
        m_strName.clear();
        m_strNumber.clear();
        m_strServer.clear();
        m_strLocalSip.clear();
        m_strLocalSipServer.clear();
        m_strBornTick.clear();
        m_strDuration.clear();
        m_strPicture.clear();
        m_nCallType = 0;  // CT_UNKOWN.
    }
};

// 历史记录增加项.
struct CallLogAddItemData
{
    int m_nCallType;  // 通话类型, 为CALL_TYPE枚举值.

    time_t m_nBornTick;  // 通话建立时间.
    int m_nDuration;  // 通话时长.

    yl::string m_strLocaleSIPName;  //  本地账号对应的注册名.
    yl::string m_strLocaleSIPServer;  //  本地账号对应的主机名.

    yl::string m_strRemoteSIPName;  //  对方注册名.
    yl::string m_strRemoteSIPServer;  //  对方主机名.
    yl::string m_strRemoteDisplayName;  //  对方显示名.

    yl::string m_strForwardSIPName;  // 转移至什么号码.
    yl::string m_strForwardSIPServer;  // 转移至什么服务器.
    yl::string m_StrForwardDisplayName;  // 转移至谁.

    CallLogAddItemData()
    {
        Reset();
    }

    void Reset()
    {
        m_nCallType = 0;  // CT_UNKNOW.
        m_nBornTick = 0;
        m_nDuration = 0;
        m_strLocaleSIPName.clear();
        m_strLocaleSIPServer.clear();
        m_strRemoteSIPName.clear();
        m_strRemoteSIPServer.clear();
        m_strRemoteDisplayName.clear();
        m_strForwardSIPName.clear();
        m_strForwardSIPServer.clear();
        m_StrForwardDisplayName.clear();
    }
};
#endif

// 历史记录更新项.
struct CallLogUpdateData
{
    yl::string m_strNumber1;  // 号码1.
    yl::string m_strNumber2;  // 号码2.
    yl::string m_strNumber3;  // 号码3.
    yl::string m_strNewName;  // 新名字.

    CallLogUpdateData()
    {
        Reset();
    }

    void Reset()
    {
        m_strNumber1.clear();
        m_strNumber2.clear();
        m_strNumber3.clear();
        m_strNewName.clear();
    }
};

// 未接来电记录.
struct MissedCallData
{
    yl::string m_strCaller;  // 拨打者名.
    yl::string m_strCallerNumber;  // 拨打者号码.
    int m_nTimes;  // 未接次数.

    MissedCallData()
    {
        Reset();
    }

    void Reset()
    {
        m_strCaller.clear();
        m_strCallerNumber.clear();
        m_nTimes = 0;
    }
};

// 未接来电提示列表.
struct MissedNotifyListData
{
    YLList<MissedCallData> m_listMissed;  // 未接来电列表.

    MissedNotifyListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listMissed.clear();
    }
};

// 转接来电记录.
struct ForwardedCallData
{
    yl::string m_strCaller;  // 拨打者名.
    yl::string m_strForwarder;  // 转向名.
    yl::string m_strCallerNumber;  // 拨打者号码.
    int m_nTimes;  // 转接次数.

    ForwardedCallData()
    {
        Reset();
    }

    void Reset()
    {
        m_strCaller.clear();
        m_strForwarder.clear();
        m_strCallerNumber.clear();
        m_nTimes = 0;
    }
};

// 转接来电提示列表.
struct ForwardedNotifyList
{
    YLList<ForwardedCallData> m_listForwarded;  // 转接来电列表.

    ForwardedNotifyList()
    {
        Reset();
    }

    void Reset()
    {
        m_listForwarded.clear();
    }
};

// 历史记录搜索结果项.
struct CallLogSearchData
{
    yl::string m_strName;  // 名字.
    yl::string m_strNumber;  // 号码.

    CallLogSearchData()
    {
        Reset();
    }

    void Reset()
    {
        m_strName.clear();
        m_strNumber.clear();
    }
};

// 历史记录搜索结果链表.
struct CallLogSearchListData
{
    YLList<CallLogSearchData> m_listResult;  // 搜索结果链表.
    int m_nIndexFrom;  // 从何处下标开始获取.
    int m_nCountToGet;  // 获取的数量.
    int m_nTotalCount;  // 总数.

    CallLogSearchListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listResult.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};

// 远程联系人搜索结果.
struct RemoteSearchItemData
{
    yl::string m_strName;  // 名字.
    ContactRemoteItemType m_eType;  // 远程联系人类型.
    YLVector<yl::string> m_listNumber;  // 号码列表.

    RemoteSearchItemData()
    {
        Reset();
    }

    void Reset()
    {
        m_strName.clear();
        m_listNumber.clear();
        m_eType = CONTACT_REMOTE_TYPE_NONE;
    }
};

// 远程搜索结果链表.
struct RemoteSearchListData
{
    YLList<RemoteSearchItemData> m_listResult;  // 结果链表.
    yl::string m_strKey;  // 搜索关键字.
    int m_nIndexFrom;  // 从何处下标开始取.
    int m_nCountToGet;  // 取多少个.
    int m_nTotalCount;  // 结果总数.

    RemoteSearchListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listResult.clear();
        m_strKey.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};

// 联系人快捷菜单的数据.
struct DirShortcutData
{
    yl::string m_strIdName;  // ID名.

    DirShortcutData()
    {
        Reset();
    }

    void Reset()
    {
        m_strIdName.clear();
    }
};

// 联系人快捷菜单列表.
struct DirShortcutListData
{
    YLList<DirShortcutData> m_listData;  // [out] 获取到的列表.
    int m_nIndexFrom;  // 开始下标.
    int m_nCountToGet;  // 获取数量.
    int m_nTotalCount;  // 总数.

    DirShortcutListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listData.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};
/*
#ifdef _T48
enum AttrType
{
    ATTR_NONE,
    ATTR_ALL,
    ATTR_NUMBER,
    ATTR_NAME_AND_NUM,
};

struct Attribute
{
    yl::string m_strName; //属性名
    yl::string m_strValue; //属性值
    AttrType m_eType;  //属性类型，如号码

    Attribute()
    {
        m_strName.clear();
        m_strValue.clear();
        m_eType = ATTR_NONE;
    }
};

enum eSortType
{
    SORT_BY_DEFOULT,
    SORT_BY_NAME
};

struct PresenceInfo
{
    yl::string   strContactID; //jid
    yl::string   strContactName;//名字
    yl::string  strAvatarInfo;  //头像文件名
    yl::string  strAvatarSignature;     //头像图片的特征码
    _PRESENCE_TYPE ePresenceType; //状态

    yl::string m_strTitle;//签名

    YLList<Attribute> m_listAttribute;//属性列表

    bool m_bFirst; //是否第一次

    yl::string m_strSeq;//排序字符串

#ifdef _T48
    int m_nLocalId;
#endif

    yl::string& GetSortKey()
    {
        if (!m_strSeq.empty())
        {
            return m_strSeq;
        }
        else
        {
            return strContactName;

        }
    }

    bool operator < (PresenceInfo& item)
    {
        if (ePresenceType == _PRESENCE_TYPE_NONE)
        {
            return false;
        }
        else if (item.ePresenceType == _PRESENCE_TYPE_NONE)
        {
            return true;
        }

        if ((int)ePresenceType < (int)item.ePresenceType)
        {
            return true;
        }
        else if (ePresenceType == item.ePresenceType)
        {
            return strcasecmp(strContactName.c_str(),item.strContactName.c_str()) < 0;
        }

        return false;
    }

    yl::string GetAttributeValue(AttrType eType)
    {
        for (YLList<Attribute>::const_iterator iter = m_listAttribute.begin();iter != m_listAttribute.end();iter++)
        {
            const Attribute& refData = iter->dataEntry;
            if (refData.m_eType == eType)
            {
                return refData.m_strValue;
            }
        }

        return "";
    }

    bool GetAttributeValue(int iIndex, AttrType eType, Attribute& stAtt)
    {
        int i = 0;
        for (YLList<Attribute>::const_iterator iter = m_listAttribute.begin();iter != m_listAttribute.end();iter++)
        {
            const Attribute& refData = iter->dataEntry;
            if (refData.m_eType == eType)
            {
                if (i == iIndex)
                {
                    stAtt = refData;
                    return true;
                }

                i++;
            }
        }

        return false;
    }

    PresenceInfo()
    {
        Reset();
    }

    void Reset()
    {
        m_bFirst = true;
        ePresenceType = _PRESENCE_TYPE_NONE;
        strAvatarInfo   = "";
        strContactID  = "";
        strContactName = "";
        m_listAttribute.clear();
        strAvatarSignature = "";
        m_strTitle.clear();
#ifdef _T48
        m_nLocalId = -1;
#endif
        m_strSeq.clear();
    }
};

#ifdef _T48
typedef YLList<int> ListPresenceInfo;
#else
typedef YLList<PresenceInfo>    ListPresenceInfo;
#endif

typedef ListPresenceInfo::iterator       IterPresenceInfo;
typedef ListPresenceInfo::const_iterator CIterPresenceInfo;

struct UCFavoriteData
{
    yl::string m_strPhotoPath;
    yl::string m_strName;
    yl::string m_strNumber;
    _PRESENCE_TYPE eStatus;

    UCFavoriteData()
    {
        m_strPhotoPath = "";
        m_strName = "";
        m_strNumber = "";
        eStatus = _PRESENCE_TYPE_NONE;
    }

};

enum PresenceType
{
    PRESENCE_BASE,
    PRESENCE_LYNC,
    PRESENCE_UC,
};

struct ContactBuddyListData
{
    ListPresenceInfo m_list;
    int m_nIndexFrom; // Retrieve from which index.
    int m_nCountToGet; // Count to retrieve.
    int m_nTotalCount; // Total item count.
    PresenceType m_eType;

    ContactBuddyListData()
    {
        Reset();
    }

    void Reset()
    {
        m_eType = PRESENCE_UC;
#ifdef _T48
        m_list.clear();
#else
        m_list.clear();
#endif
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};
#endif

*/


#endif //__DIRECTORY_DATA_DEFINE_H__
