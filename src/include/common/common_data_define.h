#ifndef __COMMON_DATA_DEFINE_H__
#define __COMMON_DATA_DEFINE_H__

#include "ylvector.h"
#include "ylstring.h"

#define     MAX_URL_INPUT   512
#define     PASSWORD_LENGTH     160

// 将对象地址作为定时器ID
#define TIMER_ID(timer)      (UINT(&(timer)))

// 删除堆对象
#define  DEL_OBJ(pObject)   do \
                            { \
                            if (pObject != NULL) \
                                {delete pObject; pObject = NULL;} \
                            } while (0);

// 检查空指针
#define CHECK_NULL(pointer, ret) do{\
    if(pointer == NULL){ \
    printf("Unexpected NULL pointer ! File:[%s] Line:[%d]", __FILE__, __LINE__);\
    return ret; \
    }}while(0)

//时间格式
enum TIME_FORMAT
{
    TF_12HOUR = 0,
    TF_24HOUR,
};

//日期格式
enum DATE_FORMAT
{
    DF_WWW_MMM_DD = 0,
    DF_DD_MMM_YY,
    DF_YYYY_MM_DD,
    DF_DD_MM_YYYY,
    DF_MM_DD_YY,
    DF_DD_MMM_YYYY,
    DF_WWW_DD_MMM,
    // http://192.168.1.99/Bug.php?BugID=20490
    DF_T20_MM_DD_YY = 7,//显示格式： MM/DD/YY
    DF_T20_DD_MM_YY = 8,//显示格式： DD/MM/YY
    DF_T20_YY_MM_DD = 9,//显示格式： YY/MM/DD
};

//快捷键类型
enum SHORTCUT_TYPE
{
    SCT_NONE = 0,
    SCT_HISTORY,
    SCT_CONTACTS,
    SCT_MESSAGE,
    SCT_DND,
    SCT_CREATEMSG,
    SCT_FWD,
    SCT_SWITCHACCOUNT,
    SCT_LOCK,
    SCT_DAILEDCALLS,
    SCT_MENU,
    SCT_STATUS,
    SCT_SPEEDDIAL, //for t80
    SCT_XMLPHONEBOOK,
    SCT_INTERCOM,  //for t6x
    SCT_SWITCHACCOUNT_UP,
    SCT_SWITCHACCOUNT_DOWN,
};


enum T80_ARROW_TYPE
{
    AT_NONE,
    AT_LEFT,
    AT_RIGHT
};

// Video支持情况
enum TALK_VIDEO_OPT
{
    TVO_DISABLE     = 0,  // 禁用
    TVO_FIRST       = 1,  // 优先
    TVO_OPTIONAL    = 2,  // 可选
};

enum SoftKey_TYPE
{
    ST_EMPTY                    = 0x00, //0: "Empty",       空键
    ST_SEND                     = 1,    //1: "Send",        发起呼叫
    ST_IMECHANGE                = 2,    //2: "IME",         切换输入法
    ST_DELETE                   = 3,    //3: "Delete",      删除光标前字符
    ST_LINE                     = 4,    //4: "Line",        进入帐号选择界面
    ST_HISTORY                  = 5,    //5: "History",     进入呼叫列表
    ST_DIRECTORY                = 6,    //6: "Directory",   进入联系人
    ST_CANCEL                   = 7,    //7: "Cancel",      取消当前界面或退出Routine等
    ST_ANSWER                   = 8,    //8: "Answer",      接起来电
    ST_CALLINFORWARD            = 9,    //9: "Forward",     转接来电
    ST_SILENCE                  = 10,   //10:"Silence",     来电静音,即取消响铃
    ST_REJECT                   = 11,   //11:"Reject",      拒绝来电
    ST_TRANSFER                 = 12,   //12:"Transfer",    转接通话
    ST_NEWCALL                  = 13,   //13:"NewCall",     新起一路通话
    ST_OK                       = 14,   //14:"Ok",          确认
    ST_SWITCH                   = 15,   //15:"Switch",      用于切换通话或Combox焦点等
    ST_CCOMPLETION              = 16,   //16:"CC",          启用Call Completion功能
    ST_HOLD                     = 17,   //17:"Hold",        启用Hold功能
    ST_SWAP                     = 18,   //18:"SWAP",        转换通话对象,即由与A通话转到与B通话
    ST_CONFERENCE               = 19,   //19:"Conference",  开启会议
    ST_RESUME                   = 20,   //20:"Resume",      取消Hold
    ST_SPLIT                    = 21,   //21:"Split",       拆开会议为两路通话
    ST_MUTE                     = 22,   //22:"Mute",        通话静音
    ST_MORE                     = 23,   //23:"More"         按键翻页(该项用户不可设,且放在最后)
    ST_ADD                      = 24,   //24:"Add"          网络会议界面增加与会方
    ST_REMOVE                   = 25,   //25:"Remove"       网络会议界面踢出与会方
    ST_PRI_CHAT                 = 26,   //26:"PriChat"      网络会议界面与对方私聊
    ST_EXIT_PRICHAT             = 27,   //27:"ExitPri"      网络会议界面取消与对方私聊
    ST_POOL                     = 28,   //28:"Pool"         拨号界面的Pool工具
    ST_PRIVATEHOLD              = 29,   //29:"PriHold"      BroadSoft Share Line的private hold功能
    ST_BACK                     = 30,   //30:"Back"         pool界面的返回
    ST_ENTER                    = 31,   //31:"Enter"        pool界面的进入
    ST_SELECT                   = 32,   //32:"Select"       账号选择界面
    ST_EXIT                     = 33,   //33:"Exit"         idlel界面的提示
    ST_VIEW                     = 34,   //34:"View"         idlel界面的提示
    ST_CONNECT                  = 35,   //35:"Connect"      idlel界面的提示
    ST_CONFIRM                  = 36,   //36:"confirm"      确认
    ST_SAVE                     = 37,   //37:"Save"         保存
    ST_DPICKUP                  = 38,   //38:"DPickup"
    ST_GPICKUP                  = 39,   //39:"GPickup"
    ST_DIAL                     = 40,   //40:"Dial"         呼出
    ST_EDIT                     = 41,   //41:"Edit"         LDAP搜索界面的编辑
    ST_BARGEIN                  = 42,   //42:"Barge In"
    ST_BLOCK                    = 43,   //43: "Block"
    ST_REPLY                    = 44,   //44: "reply"
    ST_ALLOFF                   = 45,   //45: "all off"
    ST_ALLON                    = 46,   //46: "all on"
    ST_ALL_LINES                = 47,   //47: "All Lines"
    ST_SEARCH                   = 48,   //48: "search"
    ST_ADDGROUP                 = 49,   //49: "addgroup"
    ST_OPTION                   = 50,   //50: "option"
    ST_DETAIL                   = 51,   //51: "Detail"
    ST_DELETEALL                = 52,   //52: "Delete All"
    ST_PARK                     = 53,   //53: "Park"
    ST_GROUP_PARK               = 54,   //54: "GroupPark"
    ST_RETRIEVE_PARK            = 55,   //55: "RetrievePark"
    ST_PICKUP                   = 56,   //56: "Pickup"
    ST_CALLPULL                 = 57,   //57: "CallPull"
    ST_RETRIEVE                 = 58,   //58: "Retrieve"
    ST_ENDCALL                  = 59,   //59: "End Call"

    //call center
    ST_ACD_LOGIN                = 60,   //60: "Login"
    ST_ACD_LOGOUT               = 61,   //61: "Logout"
    ST_ACD_AVAILABLE            = 62,   //62: "Available"
    ST_ACD_UNAVAILABLE          = 63,   //63: "Unavailable"
    ST_HOTELING_GUESTIN         = 64,   //64: "GuestIn"
    ST_HOTELING_GUESTOUT        = 65,   //65: "GuestOut"

    ST_ACD_DISPCODE             = 66,   //66: "Disp Code"
    ST_ACD_TRACE                = 67,   //67: "Trace"
    ST_ACD_CALLINFO             = 68,   //68: "Call Info"
    ST_ACD_EMERGENCY            = 69,   //69: "Emergcy"

    ST_IDLE_HOTKEY1             = 70,   // softkey1
    ST_IDLE_HOTKEY2             = 71,   // softkey2
    ST_IDLE_HOTKEY3             = 72,   // softkey3
    ST_IDLE_HOTKEY4             = 73,   // softkey4
    ST_UPDATE                   = 74,
    ST_FAVORITE                 = 75,
    ST_STATUS                   = 76,
    ST_SEND_SMS                 = 77,   //74: "Send_SMS", 发送短信
    ST_NEXT_PAGE                = 78,
    ST_PREV_PAGE                = 79,
    ST_NEXT                     = 80,
    ST_CONF_MANAGE              = 81,
    ST_CONF_FAR_MUTE            = 82,
    ST_CONF_REMOVE              = 83,
    ST_CONF_UNMUTE              = 84,
    ST_RTP_STATUS               = 85,   //82: "RTP Status"
    ST_RTP_SWITCH               = 86,
    ST_PAGING                   = 87,
    ST_LEFT                     = 88,
    ST_RIGHT                    = 89,
    ST_PLAYLEFT                 = 90,
    ST_PLAYRIGHT                = 91,
    ST_CLEAR                    = 92,
    ST_SCAMENU_RETRIEVE         = 93,   // SCAMenu界面使用softkey, 这个softkey 的图标和 resume显示的相同
// #if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    ST_START_RECORD             = 95,
    ST_STOP_RECORD              = 96,
    ST_PAUSE_RECORD             = 97,
    ST_RESUME_RECORD            = 98,
    ST_PLAY                     = 99,
    ST_PAUSE                    = 100,
// #endif
#ifdef IF_SETTING_WIZARD
    ST_PREVIOUS                 = 101,
    ST_FINISH                   = 102,
#endif
    ST_SCREENSHOT               = 103,  //截图
#if IF_BUG_27796
    ST_Z_PICKUP                 = 104,  // Zultys Pickup, label为Pickup，功能实际为Prefix
#endif

#if IF_FEATURE_EDK                      // 以下10个属于EDK custom softkey
    ST_EDK_1                    = 105,
    ST_EDK_2                    = 106,
    ST_EDK_3                    = 107,
    ST_EDK_4                    = 108,
    ST_EDK_5                    = 109,
    ST_EDK_6                    = 110,
    ST_EDK_7                    = 111,
    ST_EDK_8                    = 112,
    ST_EDK_9                    = 113,
    ST_EDK_10                   = 114,
#endif

    ST_RECORD_START             = 116,
    ST_RECORD_STOP              = 117,
    ST_RECORD_PAUSE             = 118,
    ST_RECORD_RESUME            = 119,

    ST_REDIAL                   = 120,

    ST_CALLPUSH                 = 121,
    ST_CALLINITIATION           = 122,
    ST_SILENTBARGEIN            = 123,

#if IF_BUG_23548
    ST_CALLCOMPLETIONCALLBACK   = 124,  //NFON CallCompletion
#endif

    ST_SKIP                     = 125,
    ST_START                    = 126,
    ST_IGNORE                   = 127,
    ST_DECLINE                  = 128,

    ST_DEFAULT                  = 129,
    ST_LOGIN                    = 130,
    ST_LOGOUT                   = 131,

#if IF_BUG_32779
    ST_NO_TRANS_CANCEL          = 132,  //不翻译的Cancel
#endif

#if IF_FEATURE_GBBOOK
    ST_ACCOUNT                  = 135,
#endif

    ST_DISCONNECT               = 142,
    ST_SCAN                     = 143,
    ST_REFRESH                  = 144,
    ST_WPS                      = 145,

    ST_FAR_HOLD                 = 146,
    ST_FAR_RESUME               = 147,
    ST_JOIN                     = 148,
    ST_SPLIT_ALL                = 149,
    ST_FLASH                    = 150,
    ST_BLIND_TRANSFER           = 152,  // "B Transfer" 盲转
    ST_MEMBERS                  = 154,     //Members

#if IF_FEATURE_START2START_ACD
    ST_S2SACD_LOGIN             = 500,
    ST_S2SACD_UNAVAILABLE       = 501,
    ST_S2SACD_REFRESH           = 502,
    ST_S2SACD_AVAILABLE         = 503,
#endif
#if IF_FEATURE_BROADSOFT_VOICEMAIL
    ST_VOICEMAIL_PLAY           = 504,
    ST_VOICEMAIL_PAUSE          = 505,
    ST_VOICEMAIL_RESUME         = 506,
    ST_VOICEMAIL_STOP           = 507,
    ST_VOICEMAIL_PREVIOUS       = 508,
    ST_VOICEMAIL_NEXT           = 509,
    ST_VOICEMAIL_CALLBACK       = 510,
#endif

    ST_GUEST_LOCK               = 511,
    ST_UNLOCK                   = 512,
    ST_SECURITY                 = 513,

    ST_START_SHARING            = 514,
    ST_STOP_SHARING             = 515,
    ST_VIDEO                    = 516,
    ST_AUDIO                    = 517,
    ST_UPGRADE_VIDEO            = 518,

    ST_OPENDOOR                 = 519,

    ST_HIDE                     = 520,
#if IF_FEATURE_METASWITCH
    ST_MTSW_LOGIN               = 521,
    ST_MTSW_PLAY                = 522,
    ST_MTSW_PAUSE               = 523,
    ST_MTSW_STOP                = 524,
    ST_MTSW_ACD_STATE           = 525, // ACD 切换状态
    ST_MTSW_ACD_DETAIL          = 526, // ACD queue数据
    ST_MTSW_ACD_MEMBER          = 527, // MLHG组成员
    ST_MTSW_ACD_MORE            = 528,
#endif
#if IF_FEATURE_XMLBROWSER_HOTELING
    ST_XB_HOTELING_LOGIN        = 529,
    ST_XB_HOTELING_LOGOUT       = 530,
#endif
    ST_PREV                     = 531,
    ST_RESET                    = 532,
    ST_VIEW_MORE                = 533,
};

// Option的操作类型
enum OPTION_TYPE
{
    OPTION_NONE = 0,
    OPTION_DETAIL,
    OPTION_ADD,
    OPTION_DELETE,
    OPTION_DELETE_ALL,
    OPTION_MOVE_TO_CONTACT,
    OPTION_MOVE_TO_BLACKLIST,
    OPTION_ADD_TO_PERSONAL,
    OPTION_UPDATE,
    OPTION_CALL_BACK,
    OPTION_MARK_NEW,
    OPTION_MARK_ALL_NEW,
    OPTION_MARK_HEARD,
    OPTION_MARK_ALL_HEARD,
    OPTION_ADD_TO_FAVORITE,
    OPTION_REMOVE_FROM_FAVORITE,
    OPTION_EDIT,
    OPTION_MOVE_UP,
    OPTION_MOVE_DOWN,
    OPTION_WPS,
    OPTION_WPS_PIN,
    OPTION_MANAGE_GROUP,
    OPTION_RENAME_GROUP,
    OPTION_DELETE_GROUP,
    OPTION_REFRESH,
    OPTION_CALL_VOICEMAIL,
    OPTION_SORT,
    OPTION_ADD_TO_MTSW_CONTACT,
    OPTION_SET_AS_WALLPAPER,
    OPTION_SET_AS_EXPWALLPAPER,
    OPTION_SET_AS_SCREENSAVER,
};

#ifndef _T48
struct PopupBoxInfo
{
    unsigned int            m_iType;
    int                     m_nId;
    yl::string              m_strIcon;
    yl::string              m_strTitle;
    yl::string              m_strNote;
    YLVector<SoftKey_TYPE>  m_vecSoftkey;//softkey
    int                     m_nTimes;
    PopupBoxInfo()
    {
        ReSet();
    }
    void ReSet()
    {
        m_iType = 0;
        m_nId = -1;
        m_strIcon = "";
        m_strTitle = "";
        m_strNote = "";
        m_nTimes = -1;
        m_vecSoftkey.clear();
        m_vecSoftkey.push_back(ST_EMPTY);
        m_vecSoftkey.push_back(ST_EMPTY);
        m_vecSoftkey.push_back(ST_EMPTY);
        m_vecSoftkey.push_back(ST_EMPTY);
    }
};

enum tagIdleMsgboxID
{
    MSGBOX_ID_NONE = -1,
    MSGBOX_ID_CANCEL = 0,
    IMSGBOX_ID_OK = 1,
    MSGBOX_ID_STATUS = 2,
    MSGBOX_ID_PASSWORD = 3,
    MSGBOX_ID_PASSWORD_RESET_ERROR  = 4,
    MSGBOX_ID_HOTDESK_WAITING = 5,
    MSGBOX_ID_HOTELING_LOGOUT,
    MSGBOX_ID_HOTELING_UNLOCK,
    MSGBOX_ID_HOTELING_START,
    MSGBOX_ID_HOTELING_GUESTOUT,
    MSGBOX_ID_DOWNLOAD_CONFIG,
    MSGBOX_ID_UNLOCK_PASSWORD_RESET_ERROR,
    MSGBOX_ID_BALANCE_LOADING,
    MSGBOX_ID_UPDATE_CONFIG,
#if IF_FEATURE_METASWITCH
    MSGBOX_ID_MTSW_LOGOUT,
#endif
};

enum tagIdleMsgbox
{
    IDLE_MSGBOX_TYPE_NONE = -1,
    //lcm
    IDLE_MSGBOX_TYPE_NOTE,
    IDLE_MSGBOX_TYPE_WARNING,
    IDLE_MSGBOX_TYPE_POPUPNORMAL,
    IDLE_MSGBOX_TYPE_POPUP_MIDDLE,
    IDLE_MSGBOX_TYPE_PICKUP_PAGE,
    IDLE_MSGBOX_TYPE_HOTELING_START,
    IDLE_MSGBOX_TYPE_DOWNLOAD_CONFIG,
};
#endif


typedef bool (*FuncCallBackAfterKeyPress)(int);
enum KEY_PRESS_TRIGGER_TYPE
{
    TRIGGER_TYPE_NONE = -1,
    EDK_TRIGGER_TYPE,
};
struct KeyPressCallBackData
{
    KEY_PRESS_TRIGGER_TYPE m_eType;
    int m_iId;
    FuncCallBackAfterKeyPress m_funcCallBack;

    KeyPressCallBackData()
    {
        m_eType = TRIGGER_TYPE_NONE;
        m_iId = -1;
        m_funcCallBack = NULL;
    }
};

// 由于使用void *指向各分配内存的结构体，会导致调用不到析构函数，造成内存泄漏
// 此处添加基类,将析构函数设为虚函数,各结构体继承此结构体,解决内存泄漏
struct LogicDataBase
{
    virtual ~LogicDataBase() {}
};

// 默认UIID
#define DEF_UIIID       0
#ifdef _DECT
#define  HANDSET_NUMBER  5
#else
#define  HANDSET_NUMBER  1
#endif

#endif //__COMMON_DATA_DEFINE_H__
