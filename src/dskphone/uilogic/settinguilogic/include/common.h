#ifndef SETTING_DEFINE_COMMON_H_
#define SETTING_DEFINE_COMMON_H_

#include <ETLLib.hpp>
#include <ylstring.h>  // use memset.
#include <yllist.h>
#include <ylvector.h>

#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"
#include "interfacedefine.h"
//#include "dsskey/include/dsskey_data_define.h"
#include "dsskey/include/dsskey_enumtypes.h"
//#include <devicelib/devicelib_struct.h>
#include <singletonclassdefine.h>
#include "feature/include/featurecommon.h"
//#include "talk/fwd_dnd/include/fwddndtypes.h"
//#include "talk/fwd_dnd/include/fwddndmsgdefine.h"
//#include "talk/fwd_dnd/include/modfwddnd.h"
#include <settinguilogic/include/settingdefine.h>
#include "commonunits/commonunits_def.h"

#define GLOAD_ID    -1
// The max code length.
#define knMaxCodeLength 99

// struct LIST_STRING, records theme list data.
typedef YLList<yl::string> LIST_STRING;

// struct VEC_TEXT, vector string data.
typedef YLVector<yl::string> VEC_STRING_DATA;

// struct VEC_TEXT, vector int data.
typedef YLVector<int> VEC_INT_DATA;

// 由于在ui层使用void *指向各分配内存的结构体，会导致调用不到析构函数，造成内存泄漏
// 此处添加基类,将析构函数设为虚函数,各结构体继承此结构体,解决内存泄漏
struct SettingDataBase
{
    virtual ~SettingDataBase() {}
};

// The code info.
struct CodeInfo
{
    // All the codes are end with null, so the actual length is 99 + 1.
    char m_szCodeOn[knMaxCodeLength + 1];  // The on code.
    char m_szCodeOff[knMaxCodeLength + 1];  // The off code.

    // Constructor.
    CodeInfo()
    {
        Reset();
    }

    void Reset()
    {
        memset(m_szCodeOn, 0, sizeof(m_szCodeOn));
        memset(m_szCodeOff, 0, sizeof(m_szCodeOff));
    }

    // Overwrite operator=.
    CodeInfo & operator= (const CodeInfo & rhs)
    {
        if (this != &rhs)
        {
            // Copy on code.
            memcpy(m_szCodeOn, rhs.m_szCodeOn, sizeof(rhs.m_szCodeOn));
            m_szCodeOn[knMaxCodeLength] = '\0';

            // Copy off code.
            memcpy(m_szCodeOff, rhs.m_szCodeOff, sizeof(rhs.m_szCodeOff));
            m_szCodeOff[knMaxCodeLength] = '\0';
        }
        return *this;
    }

    // Overwrite ==.
    bool operator == (const CodeInfo & rhs)
    {
        return strcmp(m_szCodeOn, rhs.m_szCodeOn) == 0
               && strcmp(m_szCodeOff, rhs.m_szCodeOff) == 0;
    }
};  // struct CodeInfo

// struct PhoneStatusData.
struct PhoneStatusData : public SettingDataBase
{
    int m_iIPMode; // IPAddress Mode
    yl::string m_strModel;  // phone Model.
    yl::string m_strIP;
    yl::string m_strIPv6;
    yl::string m_strHardWare;  // phone hardware.
    yl::string m_strFirmWare;  // phone firmware.
    yl::string m_strProductId;  // phone product Id.
    yl::string m_strMac;  // phone MAC.
#ifdef IF_SUPPORT_WIFI
    yl::string m_strWifiMac;
#endif
#ifdef IF_SUPPORT_BLUETOOTH
    yl::string m_strBluetoothMac;
#endif

    PhoneStatusData()
    {
        Reset();
    }

    void Reset()
    {
        m_iIPMode = 0;
        m_strModel = "";
        m_strHardWare = "";
        m_strFirmWare = "";
        m_strProductId = "";
        m_strMac = "";
#ifdef IF_SUPPORT_WIFI
        m_strWifiMac = "";
#endif
#ifdef IF_SUPPORT_BLUETOOTH
        m_strBluetoothMac = "";
#endif
    }

    bool operator == (const PhoneStatusData & rhs)
    {
        return m_iIPMode == rhs.m_iIPMode
               && m_strModel == rhs.m_strModel
               && m_strHardWare == rhs.m_strHardWare
               && m_strFirmWare == rhs.m_strFirmWare
               && m_strProductId == rhs.m_strProductId
               && m_strMac == rhs.m_strMac;
    }

    bool operator != (const PhoneStatusData & rhs)
    {
        return !(*this == rhs);
    }
};

// WAN Type.
enum WanType
{
    WAN_TYPE_NONE = -1,  // Invalid WAN type.
    WAN_DHCP = 0,  // DHCP.
    WAN_PPPOE = 1,  // PPPOE.
    WAN_STATIC = 2,  // Static IP.
    WAN_TYPE_MAX
};

enum IPv6WanType
{
    IPv6_WAN_TYPE_NONE = -1,  // Invalid IPv6WAN type.
    IPv6_WAN_DHCP = 0,  // IPv6 DHCP.
    IPv6_WAN_STATIC = 1,  // IPv6 Static IP.
    IPv6_WAN_TYPE_MAX
};

// struct PhoneNetworkStatusData.
struct PhoneNetworkStatusData : public SettingDataBase
{
    int m_iWanType;  // The WAN type.
    yl::string m_strMac;
    yl::string m_strWanIP;  // The WAN IP.
    yl::string m_strWanMask;  // The WAN mask.
    yl::string m_strLanMask;  // The LAN mask.
    yl::string m_strGateway;  // The gateway.
    yl::string m_strDnsPrimary;  // The primary DNS.
    yl::string m_strDnsSecond;  // The second DNS.
    yl::string m_strLanType;
    yl::string m_strLanIP;
    yl::string m_strPrefix;  // The Prefix of IPv6.
#ifdef IF_SUPPORT_WIFI
    yl::string m_strWifiMac; //wifimac
#endif

    PhoneNetworkStatusData()
    {
        Reset();
    }

    void Reset()
    {
        m_iWanType = -1;
        m_strWanIP = "";
        m_strWanMask = "";
        m_strLanMask = "";
        m_strGateway = "";
        m_strDnsPrimary = "";
        m_strDnsSecond = "";
        m_strLanType = "";
        m_strLanIP = "";
        m_strPrefix = "";
#ifdef IF_SUPPORT_WIFI
        m_strWifiMac = "";
#endif
    }
};

#ifdef _T42
// struct PhoneNetworkStatusData.
struct PhoneNetworkIPData : public SettingDataBase
{
    int m_iWanType;  // The WAN type.

    yl::string m_strWanIP;  // The WAN IP.
    yl::string m_strWanMask;  // The WAN mask.
    yl::string m_strGateway;  // The gateway.
    yl::string m_strDnsPrimary;  // The primary DNS.
    yl::string m_strDnsSecond;  // The second DNS.

    PhoneNetworkIPData()
    {
        Reset();
    }
    void Reset()
    {
        m_iWanType = -1;
        m_strWanIP = "";
        m_strWanMask = "";
        m_strGateway = "";
        m_strDnsPrimary = "";
        m_strDnsSecond = "";

    }
};
#endif

struct PhonePlatformData : public SettingDataBase
{
    yl::string m_strProductName;
    yl::string m_strFirwareVer;
    yl::string m_strHardwareVer;
    yl::string m_strProductID;
    yl::string m_strMac;
    yl::string m_strDeviceCert;
    PhonePlatformData()
    {
        Reset();
    }

    void Reset()
    {
        m_strProductName = "";
        m_strFirwareVer = "";
        m_strHardwareVer = "";
        m_strProductID = "";
        m_strMac = "";
        m_strDeviceCert = "";
    }
};

// struct CallWaitingData.
struct CallWaitingData : public SettingDataBase
{
    bool m_isOn;  // If or not the call waiting is on.
    bool m_isPlayTone;  // If or not the call waiting tone is played.
    yl::string m_strOnCode;
    yl::string m_strOffCode;

    CallWaitingData()
    {
        Reset();
    }

    void Reset()
    {
        m_isOn = false;
        m_isPlayTone = false;
        m_strOnCode = "";
        m_strOffCode = "";
    }

    // Override operator ==.
    bool operator == (const CallWaitingData & rhs)
    {
        return m_isOn == rhs.m_isOn && m_isPlayTone == rhs.m_isPlayTone
               && rhs.m_strOnCode == m_strOnCode && rhs.m_strOffCode == m_strOffCode;
    }
};

// struct AnonymousCallData
struct AnonymousCallData : public SettingDataBase
{
    bool m_isAnonymousSendCode;    // If or not send anonymous code
    bool m_isAnonymousCallOn;  // If or not anonymous call switch is on.
    bool m_isRejectAnonymousCallOn;  // If or not reject anonymous call switch is on.
    bool m_isAnonymousRejectSendCode;  // Send anonymous reject on code or off code.
    CodeInfo m_stCodeInfoAnonymousCall;  // Anonymous call code info.
    CodeInfo m_stCodeInfoRejectAnonymousCall;  // Reject anonymous call code info.

    AnonymousCallData()
    {
        Reset();
    }

    void Reset()
    {
        m_isAnonymousSendCode = false;
        m_isAnonymousCallOn = false;
        m_isRejectAnonymousCallOn = false;
        m_isAnonymousRejectSendCode = false;
        m_stCodeInfoAnonymousCall.Reset();
        m_stCodeInfoRejectAnonymousCall.Reset();
    }
};

// struct LanguageItemData
struct LanguageItemData
{
    yl::string m_strOrigName;  // 源翻译内容.
    yl::string m_strTranslatedName;  // 翻译后输出的内容.
    yl::string m_strLangFile;  // 语言翻译文件名.

    LanguageItemData()
    {
        Reset();
    }

    void Reset()
    {
        m_strOrigName.clear();
        m_strTranslatedName.clear();
        m_strLangFile.clear();
    }
};

typedef YLList<LanguageItemData> LIST_LANGUAGE_ITEM_DATA;
// struct LanguageListData.
struct LanguageItemListData
{
    LIST_LANGUAGE_ITEM_DATA m_listLang;    // Language item data list pointer.

    LanguageItemListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listLang.clear();
    }
};

// struct ForwardData, records forward data.
struct ForwardData : public SettingDataBase
{
    // Invalid ring times.
    enum InvalidForwardTimes
    {
        INVALID_FORWARD_TIMES = -1,
    };

    BOOL m_isOn;  // Status: true-ON, false-OFF.
    yl::string m_strTarget;  // Forward target.
    yl::string m_strOnCode;  // Forward on code.
    yl::string m_strOffCode;  // Forward off code.
    int m_nRingTimes;  // After ring times.

    ForwardData()
    {
        Reset();
    }

    void Reset()
    {
        m_isOn = FALSE;
        m_strTarget = "";
        m_strOnCode = "";
        m_strOffCode = "";
        m_nRingTimes = INVALID_FORWARD_TIMES;
    }

    // Override operator==.
    bool operator == (const ForwardData & rhs)
    {
        return m_isOn == rhs.m_isOn
               && m_strTarget == rhs.m_strTarget
               && m_strOnCode == rhs.m_strOnCode
               && m_strOffCode == rhs.m_strOffCode
               && m_nRingTimes == rhs.m_nRingTimes;
    }

#ifdef IF_FEATURE_FORWARD_OPT
    bool operator != (const ForwardData & rhs)
    {
        return !operator==(rhs);
    }

    FWDElem toLogicElem()
    {
        FWDElem elem;
        elem.bEnabled = m_isOn;
        elem.strTarget = m_strTarget;
        elem.iNoAnsTimeout = m_nRingTimes;
        elem.objCodeInfo.m_strOnCode = m_strOnCode;
        elem.objCodeInfo.m_strOffCode = m_strOffCode;

        return elem;
    }
#endif
};

// struct DndData, records DND data.
struct DndData : public SettingDataBase
{
    int m_bEnabled;                      // DND
    yl::string m_strOnCode;  // DND on code.
    yl::string m_strOffCode;  // DND off code.

    DndData()
    {
        Reset();
    }

    void Reset()
    {
        m_bEnabled = false;
        m_strOnCode = "";
        m_strOffCode = "";
    }
};

// Date and time type.
enum DateAndTimeType
{
    DATE_AND_TIME_TYPE_NONE = -1,  // Invalid date time type.
    DATE_AND_TIME_MANUAL = 0,  // Manual.
    DATE_AND_TIME_SNTP = 1,  // SNTP.
    DATE_AND_TIME_TYPE_MAX
};

// Daylight saving type.
enum DaylightSavingType
{
    DAYLIGHTING_SAVING_TYPE_NONE = -1,  // Invalid daylight saving type.
    DAYLIGHTING_SAVING_OFF = 0,  // Daylight saving off.
    DAYLIGHTING_SAVING_ON = 1,  // Daylight saving on.
    DAYLIGHTING_SAVING_AUTO = 2,  // Auto daylight saving.
    DAYLIGHTING_SAVING_TYPE_MAX
};

// struct DateTimeGeneralData. records time type, time zone, etc.
struct DateTimeGeneralData : public SettingDataBase
{
    DateAndTimeType m_eDateTimeType;  // Date and time type.
    yl::string m_strTimeZone;  // Current TimeZone data.
    yl::string m_strNtpServer1;  // NTP server 1.
    yl::string m_strNtpServer2;  // NTP server 2.
    yl::string m_strTimeZoneCity;   // The City in Current TimeZone | add by dlw 2014-11-05
    DaylightSavingType m_eDayLightType;  // Day light saving type.

    DateTimeGeneralData()
    {
        Reset();
    }

    void Reset()
    {
        m_eDateTimeType = DATE_AND_TIME_TYPE_NONE;
        m_strTimeZone = "";
        m_strNtpServer1 = "";
        m_strNtpServer2 = "";
        m_strTimeZoneCity = ""; //add by dlw 2014-11-05
        m_eDayLightType = DAYLIGHTING_SAVING_TYPE_NONE;
    }
};

// struct DateTimeDHCPData, records date time DHCP data.
struct DateTimeDHCPData : public SettingDataBase
{
    bool m_isDHCPTimeOn;  // Whether or not the DHCP time is on.

    DateTimeDHCPData()
    {
        Reset();
    }

    void Reset()
    {
        m_isDHCPTimeOn = false;
    }
};

// Date format.
enum DateFormatType
{
    DATE_FORMAT_TYPE_NONE = -1,  // Invalid date format type.
    DATE_FORMAT_WWW_MMM_DD = 0,  // WWW MMM DD.
    DATE_FORMAT_DD_MMM_YY = 1,  // DD-MM-YY.
    DATE_FORMAT_YYYY_MM_DD = 2,  // YYYY-MM-DD
    DATE_FORMAT_DD_MM_YYYY = 3,  // DD/MM/YYYY.
    DATE_FORMAT_MM_DD_YY = 4,  // MM/DD/YY.
    DATE_FORMAT_DD_MMM_YYYY = 5,  // DD MM YYYY
    DATE_FORMAT_WWW_DD_MMM = 6,  // WWW DD MMM
    // http://192.168.1.99/Bug.php?BugID=20490
    DATE_FORMAT_T20_MM_DD_YY = 7,//显示格式： MM/DD/YY
    DATE_FORMAT_T20_DD_MM_YY = 8,//显示格式： DD/MM/YY
    DATE_FORMAT_T20_YY_MM_DD = 9,//显示格式： YY/MM/DD
    DATE_FORMAT_MM_DD_WWW_SPACE = 10,  // MM DD WWW
    DATE_FORMAT_MM_DD_WW_SPACE = 11, // MM DD WW
    DATE_FORMAT_YY_MM_DD_HYPHEN = 12,  // YY-MM-DD
    DATE_FORMAT_YY_MM_DD_VIRGULE = 13,  // YY/MM/DD
    DATE_FORMAT_YYYY_MM_DD_VIRGULE = 14,  // YYYY/MM/DD
    DATE_FORMAT_YYYY_MM_DD_SPACE = 15,  // YYYY MM DD
    DATE_FORMAT_TYPE_MAX
};

// Time format.
enum TimeFormatType
{
    TIME_FORMAT_TYPE_NONE = -1,  // Invalid time format type.
    TIME_FORMAT_12HOUR = 0,  // 12 hour format.
    TIME_FORMAT_24HOUR = 1,  // 24 hour format.
    TIME_FORMAT_TYPE_MAX
};

// struct DateAndTimeFormatData.
struct DateAndTimeFormatData : public SettingDataBase
{
    DateFormatType m_eDateFormat;  // Date format.
    TimeFormatType m_eTimeFormat;  // Time format.
    yl::string     m_strDiyDateFmt;

    DateAndTimeFormatData()
    {
        Reset();
    }

    void Reset()
    {
        m_eDateFormat = DATE_FORMAT_TYPE_NONE;
        m_eTimeFormat = TIME_FORMAT_TYPE_NONE;
        m_strDiyDateFmt.clear();
    }
};

//日期时间
struct DateTime : public SettingDataBase
{
    yl::string m_strYear;
    yl::string m_strMonth;
    yl::string m_strDay;
    yl::string m_strHour;
    yl::string m_strMinute;
    yl::string m_strSecond;

    DateTime()
    {
        Reset();
    }
    void Reset()
    {
        m_strYear = "";
        m_strMonth = "";
        m_strDay = "";
        m_strHour = "";
        m_strMinute = "";
        m_strSecond = "";
    }
};

// Enum, phone lock type.
enum PhoneLockType
{
    PHONE_LOCK_TYPE_NONE = 0,  // Invalid phone lock type.
    PHONE_LOCK_MENU_KEY_LOCK = 1,  // Menu key lock.
    PHONE_LOCK_FUNC_KEY_LOCK = 2,  // Func key lock.
    PHONE_LOCK_TALL_CALL_ONLY = 3,  // Tall call only.
    PHONE_LOCK_ANSWER_CALL_ONLY = 4,  // Only answer call
    PHONE_LOCK_TYPE_MAX
};

// struct PhoneLockData, records phone lock data, such as phone lock type.
struct PhoneLockData : public SettingDataBase
{
    bool m_bPhoneLock;                  // Phone lock Enable
    PhoneLockType m_ePhoneLockType;  // Phone lock type.
    int m_nPhoneLockTimeOut;        // Phone Lock time out

    PhoneLockData()
    {
        Reset();
    }

    void Reset()
    {
        m_bPhoneLock = false;
        m_ePhoneLockType = PHONE_LOCK_TYPE_NONE;
        m_nPhoneLockTimeOut = 0;
    }
};

struct PhoneBacklightData : public SettingDataBase
{
    int m_iLevel;
    int m_iTime;

    PhoneBacklightData()
    {
        Reset();
    }

    void Reset()
    {
        m_iLevel = 8;
        m_iTime = 30;
    }
};

struct PhoneContrastData : public SettingDataBase
{
    int m_iLevel;

    PhoneContrastData()
    {
        Reset();
    }

    void Reset()
    {
        m_iLevel = 8;
    }
};

// struct PasswordData, records password data.
struct PasswordData : public SettingDataBase
{
    yl::string m_strPasswordBefore;  // Before password.
    yl::string m_strPasswordNew;  // New password to set.

    PasswordData()
    {
        Reset();
    }

    void Reset()
    {
        m_strPasswordBefore = "";
        m_strPasswordNew = "";
    }

    // Override operator=.
    PasswordData & operator= (const PasswordData & rhs)
    {
        if (this != &rhs)
        {
            m_strPasswordBefore = rhs.m_strPasswordBefore;
            m_strPasswordNew = rhs.m_strPasswordNew;
        }
        return *this;
    }

    // Override operator ==.
    bool operator == (const PasswordData & rhs)
    {
        return m_strPasswordBefore == rhs.m_strPasswordBefore
               && m_strPasswordNew == rhs.m_strPasswordNew;
    }
};

// Key as send type.
enum KeyAsSendType
{
    KEY_AS_SEND_TYPE_INVALID = -1,
    //表示未启用kes as send功能.
    KEY_AS_SEND_DISABLED_SYMBOL = 0,
    //表示启用#为key as send功能.
    KEY_AS_SEND_CROSSHATCH_SYMBOL = 1,
    //表示启用*为key as send功能.
    KEY_AS_SEND_STAR_SYMBOL = 2,
    KEY_AS_SEND_TYPE_MAX
};

enum TextMsgGroupType
{
    TEXT_MSG_GROUP_TYPE_NONE = 0,  // 不正确的组.
    TEXT_MSG_GROUP_IN = 1,  // 接收组.
    TEXT_MSG_GROUP_DRAFT = 2,  // 草稿组.
    TEXT_MSG_GROUP_SENDING = 3,  // 发送组.
    TEXT_MSG_GROUP_SENT = 4,  // 已发送组.
    TEXT_MSG_GROUP_TYPE_MAX
};

// 文本一般信息.
struct TextMsgItemSimpData
{
    yl::string m_strName;  // 发送者或者接收者的显示名.
    yl::string m_strIconPath;  // 图标(全路径).
    yl::string m_strTime;  // 发送或接收时间.
    TextMsgGroupType m_eGType;  // 信息类型.

    TextMsgItemSimpData()
    {
        Reset();
    }

    void Reset()
    {
        m_strName.clear();
        m_strIconPath.clear();
        m_strTime.clear();
        m_eGType = TEXT_MSG_GROUP_TYPE_NONE;
    }
};

typedef YLList<TextMsgItemSimpData> LIST_TEXTMSG_DATA;

// 文本一般信息链表.
struct TextMsgSimpListData
{
    LIST_TEXTMSG_DATA m_listMessage;  // 信息简单信息链表.
    int m_nIndexFrom;  // 从何处下标开始获取.
    int m_nCountToGet;  // 获取的数量.
    int m_nTotalCount;  // 总数.

    TextMsgSimpListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listMessage.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};

// 发送消息结构体.
struct TextMsgSendItemData
{
    int m_nFromAccountId;  // 发送方的账号id.
    yl::string m_strText;  // 信息文本内容.
    yl::string m_strTo;  // 发送给. 可以为名或者名@服务器.
    time_t m_tTime;  // 发送时间.

    TextMsgSendItemData()
    {
        Reset();
    }

    void Reset()
    {
        m_nFromAccountId = 0;
        m_strText.clear();
        m_strTo.clear();
        m_tTime = 0;
    }
};

// 文本详情.
struct TextMsgItemDetailData
{
    yl::string m_strFromName;  // 发送者显示名.
    yl::string m_strFromUserName;  // 发送者用户名.
    yl::string m_strFromServer;  // 发送者服务器ip.
    yl::string m_strToName;  // 接收者显示名.
    yl::string m_strToUserName;  // 接收者用户名.
    yl::string m_strToServer;  // 接收者服务器ip.
    yl::string m_strText;  // 信息文本内容.
    yl::string m_strIconPath;  // 图标(全路径).
    yl::string m_strTime;  // 发送或接收时间.
    TextMsgGroupType m_eGType;  // 信息类型.

    TextMsgItemDetailData()
    {
        Reset();
    }

    void Reset()
    {
        m_strFromName.clear();
        m_strFromUserName.clear();
        m_strFromServer.clear();
        m_strToName.clear();
        m_strToServer.clear();
        m_strToUserName.clear();
        m_strText.clear();
        m_strIconPath.clear();
        m_strTime.clear();
        m_eGType = TEXT_MSG_GROUP_TYPE_NONE;
    }
};

// 某个账号下的信息数.
struct AccountTextMsgData
{
    yl::string m_strAccountDisplayName;  // 账号显示名.
    int m_nUnReadCount;  // 未读信息数.

    AccountTextMsgData()
    {
        Reset();
    }

    void Reset()
    {
        m_strAccountDisplayName.clear();
        m_nUnReadCount = 0;
    }
};

typedef YLVector<AccountTextMsgData> VEC_ACCOUNT_TEXTMSG_DATA;

// 所有账号的信息列表.
struct AccountTextMsgListData
{
    VEC_ACCOUNT_TEXTMSG_DATA m_vecMsg;  // 信息列表.

    AccountTextMsgListData()
    {
        Reset();
    }

    void Reset()
    {
        m_vecMsg.clear();
    }
};

// struct AdvancedCallFeatureHotlineData.
struct AdvancedCallFeatureHotlineData : public SettingDataBase
{
    // Invalid hot line delay time.
    enum { INVALID_HOTLINE_DELAY_TIME = -1, };

    yl::string m_strHotlineNumber;  // Hotline number.
    int m_nHotlineDelay;  // Hotline delay.

    AdvancedCallFeatureHotlineData()
    {
        Reset();
    }

    void Reset()
    {
        m_strHotlineNumber = "";
        m_nHotlineDelay = INVALID_HOTLINE_DELAY_TIME;
    }

    // Override operator=.
    AdvancedCallFeatureHotlineData & operator= (
        const AdvancedCallFeatureHotlineData & rhs)
    {
        if (this != &rhs)
        {
            m_strHotlineNumber = rhs.m_strHotlineNumber;
            m_nHotlineDelay = rhs.m_nHotlineDelay;
        }
        return *this;
    }

    // Override operator ==.
    bool operator == (const AdvancedCallFeatureHotlineData & rhs)
    {
        return m_strHotlineNumber == rhs.m_strHotlineNumber
               && m_nHotlineDelay == rhs.m_nHotlineDelay;
    }
};

// struct AdvancedCallFeatureAutoRedialData.
struct AdvancedCallFeatureAutoRedialData : public SettingDataBase
{
    // Invalid redial interval and times.
    enum { INVALID_REDIAL_INTERVAL = -1, INVALID_REDIAL_TIMES = -1, };

    bool m_isOn;  // Whether or not auto redial is on.
    int m_nRedialInterval;  // Auto redial interval.
    int m_nRedialTimes;  // Auto redial times.

    AdvancedCallFeatureAutoRedialData()
    {
        Reset();
    }

    void Reset()
    {
        m_isOn = false;
        m_nRedialInterval = INVALID_REDIAL_INTERVAL;
        m_nRedialTimes = INVALID_REDIAL_TIMES;
    }

    // Override operator=.
    AdvancedCallFeatureAutoRedialData & operator= (
        const AdvancedCallFeatureAutoRedialData & rhs)
    {
        if (this != &rhs)
        {
            m_isOn = rhs.m_isOn;
            m_nRedialInterval = rhs.m_nRedialInterval;
            m_nRedialTimes = rhs.m_nRedialTimes;
        }
        return *this;
    }

    // Override operator ==.
    bool operator == (const AdvancedCallFeatureAutoRedialData & rhs)
    {
        return m_isOn == rhs.m_isOn
               && m_nRedialInterval == rhs.m_nRedialInterval
               && m_nRedialTimes == rhs.m_nRedialTimes;
    }
};

// struct AesKeyData, records AES key data.
struct AesKeyData : public SettingDataBase
{
    yl::string m_strAesKey;  // AES key.
    yl::string m_strAesMacKey;  // AES mac key.

    AesKeyData()
    {
        Reset();
    }

    void Reset()
    {
        m_strAesKey = "";
        m_strAesMacKey = "";
    }
};

// struct AutoProvisionData, records auto provision data.
struct AutoProvisionData : public SettingDataBase
{
    yl::string m_strUrl;  // Auto provision URL.
    yl::string m_strUsername;  // Auto provision username.
    yl::string m_strPassword;  // Auto provision password.
    AesKeyData m_AesKeydata;

    AutoProvisionData()
    {
        Reset();
    }

    void Reset()
    {
        m_strUrl = "";
        m_strUsername = "";
        m_strPassword = "";
        m_AesKeydata.Reset();
    }
};


// struct ResetToFactoryData.
struct ResetToFactoryData
{
    ResetOptionType m_eType;  // Reset option type.

    ResetToFactoryData()
    {
        Reset();
    }

    void Reset()
    {
        m_eType = RESET_OPTION_TYPE_NONE;
    }
};

// struct RingFileData.
struct RingFileData
{
    yl::string m_strRingDisplayName;
    yl::string m_strRingPath;
};

typedef YLList<RingFileData>    LIST_RING_FILE_DATA;
// struct SoundListData.
struct RingFileListData
{
    LIST_RING_FILE_DATA m_listRing;  // Sound data list pointer.

    RingFileListData()
    {
        Reset();
    }

    void Reset()
    {
        // Nothing to do.
    }
};

// struct IntercomData, records intercom data.
struct IntercomData : public SettingDataBase
{
    bool m_isAllowed;  // If or not intercom allowed.
    bool m_isMuteOn;  // If or not mute is on.
    bool m_isToneOn;  // If or not tone is on.
    bool m_isBargeOn;  // If or not barge is on.

    IntercomData()
    {
        Reset();
    }

    void Reset()
    {
        m_isAllowed = false;
        m_isMuteOn = false;
        m_isToneOn = false;
        m_isBargeOn = false;
    }
};

// Account status struct.
struct AccountStatusData
{
    yl::string m_strAccountName;  // Account registed name.
    int m_nStatus;  // Account status, is LINE_STATE enum.
    int m_nID;

    AccountStatusData()
    {
        Reset();
    }

    void Reset()
    {
        m_strAccountName.clear();
        m_nStatus = -1;  // enum LINE_STATE->LS_UNKNOW
        m_nID = -1;
    }
};

typedef YLList<AccountStatusData>    LIST_ACCOUNT_STATUS_DATA;
// Account status list data.
struct AccountStatusListData
{
    LIST_ACCOUNT_STATUS_DATA m_listStatus;  // Account status list.
};

// Account detail info.
struct AccountDetailData : public SettingDataBase
{
    yl::string m_strLabel;  // Label.
    yl::string m_strDisplayName;  // Display name.
    yl::string m_strRegisterName;  // Register name.
    yl::string m_strUserName;  // Username.
    yl::string m_strPassword;  // Password.
    yl::string m_strOutboundProxy1;  // Outbound proxy.
    yl::string m_strOutboundProxy2;
    int m_nProxyFallbackInterval;  // Outbound proxy fallback interval.
    yl::string m_strSipServer1;  // Sip server.
    yl::string m_strSipServer2;  // Sip server.
    yl::string m_strStunServer;  // Stun server.
    bool m_isActivated;  // If or not activated.
    bool m_isOutboundEnable;  // If or not outbound is enable.
    bool m_isStunEnable;  // If or not STUN enable.

    AccountDetailData()
    {
        Reset();
    }

    void Reset()
    {
        m_strLabel = "";
        m_strDisplayName = "";
        m_strRegisterName = "";
        m_strUserName = "";
        m_strPassword = "";
        m_strOutboundProxy1 = "";
        m_strOutboundProxy2 = "";
        m_nProxyFallbackInterval = 3600;
        m_strSipServer1 = "";
        m_strSipServer2 = "";
        m_isActivated = false;
        m_isOutboundEnable = false;
        m_isStunEnable = false;
    }
};

#if IF_BUG_22024_LogonWizard_AutopURL
enum LoginWizardAutopRULType
{
    LW_AUTOP_URL_NONE = 0,//未设置
    LW_AUTOP_URL_OFFICE,//办公电话 url
    LW_AUTOP_URL_CALLCENTER,//呼叫中心电话 url
};
#endif

#ifdef IF_FEATURE_HOTDESKING
// HotDesk Base info.
struct HotDeskBaseData : public SettingDataBase
{
    yl::string m_strRegisterName;  // Register name.
    yl::string m_strUserName;  // Username.
    yl::string m_strPassword;  // Password.
    yl::string m_strOutboundProxy1;  // Outbound proxy1.
    yl::string m_strOutboundProxy2;  // Outbound proxy2.
    yl::string m_strSipServer1;  // Sip server.
    yl::string m_strSipServer2;  // Sip server.
    bool m_bOutboundEnable;  // If or not outbound is enable.
#if IF_BUG_22024_LogonWizard_AutopURL
    LoginWizardAutopRULType m_eAutopURL;// for Login wizard availed autop url select.
#endif

    HotDeskBaseData()
    {
        Reset();
    }

    void Reset()
    {
        m_strRegisterName = "";
        m_strUserName = "";
        m_strPassword = "";
        m_strOutboundProxy1 = "";
        m_strOutboundProxy2 = "";
        m_strSipServer1 = "";
        m_strSipServer2 = "";
        m_bOutboundEnable = false;
#if IF_BUG_22024_LogonWizard_AutopURL
        m_eAutopURL = LW_AUTOP_URL_NONE;
#endif
    }
};
#endif  //IF_FEATURE_HOTDESKING


// Network wan Static Dns Server data.
struct NetworkWanDnsServerData
{
    bool m_bStaticDns;
    int  m_nMode;                    // ip addr mode
    yl::string m_strWanPrimaryDns;  // Primary DNS.
    yl::string m_strWanSendaryDns;  // Secondary DNS.

    NetworkWanDnsServerData()
    {
        Reset();
    }

    void Reset()
    {
        m_nMode = 0;
        m_bStaticDns = false;
        m_strWanPrimaryDns = "";
        m_strWanSendaryDns = "";
    }
};

// Network wan port type data.
struct NetworkWanTypeData
{
    WanType m_eType;  // Wan type, i.e. DHCP, STATIC IP, PPPOE, etc.

    NetworkWanTypeData()
    {
        Reset();
    }

    void Reset()
    {
        m_eType = WAN_TYPE_NONE;
    }
};

// Network wan port static ip data.
struct NetworkWanStaticIpData : public SettingDataBase
{
    yl::string m_strWanIp;  // Wan ip.
    yl::string m_strWanMask;  // Wan mask.
    yl::string m_strWanGateWay;  // Wan gateway.
    yl::string m_strWanPrimaryDns;  // Primary DNS.
    yl::string m_strWanSendaryDns;  // Secondary DNS.

    NetworkWanStaticIpData()
    {
        Reset();
    }

    void Reset()
    {
        m_strWanIp = "";
        m_strWanMask = "";
        m_strWanGateWay = "";
        m_strWanPrimaryDns = "";
        m_strWanSendaryDns = "";
    }
};

// Network wan port pppoe data.
struct NetworkWanPppoeData : public SettingDataBase
{
    yl::string m_strPppoeUser;  // Pppoe user.
    yl::string m_strPppoePin;  // Pppoe pin.

    NetworkWanPppoeData()
    {
        Reset();
    }

    void Reset()
    {
        m_strPppoeUser = "";
        m_strPppoePin = "";
    }
};

// Network wan port type data.
#ifdef _T42
struct NetworkRouterData : public SettingDataBase
{
    yl::string m_strIP; //ip
    yl::string m_strSubNetMask;//submask
    bool m_bDhcpServer;//enable dhcp server

    NetworkRouterData()
    {
        Reset();
    }

    void Reset()
    {
        m_strIP = "";
        m_strSubNetMask = "";
        m_bDhcpServer = false;
    }
};

struct NetworkDhcpData : public SettingDataBase
{
    bool m_bDhcpServer;//enable dhcp server
    yl::string m_strStartIp;
    yl::string m_strEndIp;
    NetworkDhcpData()
    {
        Reset();
    }
    void Reset()
    {
        m_bDhcpServer = false;
        m_strStartIp = "";
        m_strEndIp = "";
    }
};

#else
struct NetworkRouterData
{
    yl::string m_strIPAddress;  // Router IP
    yl::string m_strNetMask;    // Router Subnet Mask
    bool        m_bDHCPServer;   // 是否启用DHCP服务器
    yl::string m_strStartIP;
    yl::string m_strEndIP;

    NetworkRouterData()
    {
        Reset();
    }

    void Reset()
    {
        m_strIPAddress.clear();
        m_strNetMask.clear();
        m_bDHCPServer = true;
        m_strStartIP.clear();
        m_strEndIP.clear();
    }
};
#endif

// Network VLAN data.
struct NetworkVlanData : public SettingDataBase
{
    int m_nWanVid;  // WAN VID number.
    int m_nPcVid;  // PC VID number.
    int m_nWanPriority;  // WAN priority.
    int m_nPcPriority;  // PC priority.
    bool m_isWanEnable;  // If or not WAN VLAN is enable.
    bool m_isPcEnable;  // IF or not PC VLAN is enable.

    NetworkVlanData()
    {
        Reset();
    }

    void Reset()
    {
        m_nWanVid = 0;
        m_nPcVid = 0;
        m_nWanPriority = 0;
        m_nPcPriority = 0;
        m_isWanEnable = false;
        m_isPcEnable = false;
    }
};

// Network VLAN DHCP data.
#ifdef _T42
struct NetworkVlanDHCPData : public SettingDataBase
#else
struct NetworkVlanDHCPData
#endif
{
    bool m_bEnable;
    yl::string m_strOption; // Option

    NetworkVlanDHCPData()
    {
        Reset();
    }

    void Reset()
    {
        m_bEnable = false;
        m_strOption = "132";
    }
};

#ifndef _T42
// Web server type.
enum WebServerType
{
    WEB_SERVER_TYPE_NONE = -1,  // Invalid web server type.
    WEB_SERVER_OFF = 0,  // Web server is off.
    WEB_SERVER_HTTP_ONLY = 1 << 0,  // HTTP only.
    WEB_SERVER_HTTPS_ONLY = 1 << 1,  // HTTPS only.
    WEB_SERVER_ALL = WEB_SERVER_HTTP_ONLY | WEB_SERVER_HTTPS_ONLY,  // HTTP & HTTPS.
    WEB_SERVER_TYPE_MAX
};
#endif

// Network web server data.
struct NetworkWebServerData : public SettingDataBase
{
    bool m_bHTTP_On;  // Status: true-ON, false-OFF.
    bool m_bHTTPS_On;
    yl::string m_strHTTP_Port;  // Forward target.
    yl::string m_strHTTPS_Port;
#if IF_BUG_29939
    yl::string m_strLoginTimer;    //登入Web GUI配置界面前的允许时长
    yl::string m_strActivityTimer; //登入Web GUI配置界面后的无操作允许时长
    yl::string m_strMaxTimer;      //登入Web GUI配置界面后最大允许操作时长
#endif

    NetworkWebServerData()
    {
        Reset();
    }

    void Reset()
    {
        m_bHTTP_On = false;
        m_bHTTPS_On = false;
        m_strHTTP_Port = "";
        m_strHTTPS_Port = "";
#if IF_BUG_29939
        m_strLoginTimer = "";
        m_strActivityTimer = "";
        m_strMaxTimer = "";
#endif
    }
};

// network 802.1x mode type.
enum Network802DotxModeType
{
    NETWOKR_802Dotx_TYPE_NONE = -1,   // Invalid type.
    NETWOKR_802Dotx_Disable = 0,     // Disable.
    NETWOKR_802Dotx_EAP_MD5 = 1,    // EAP-MD5.
    NETWOKR_802Dotx_EAP_TLS,        // EAP-TLS.
    NETWOKR_802Dotx_PEAP_MD5,     // PEAP-MSCHAPV2.
    NETWOKR_802Dotx_EAP_TTLS_MS,    //EAP-TTLS/EAP-MSCHAPv2
    NETWOKR_802Dotx_PEAP_GTC,       //EAP-PEAP/GTC
    NETWOKR_802Dotx_TTLS_GTC,       //EAP-TTLS/EAP-GTC
    NETWORK_802Dotx_EAP_FAST,       //EAP-FAST
    NETWOKR_802Dotx_EAP_Max
};

// Network 802.1x data.
struct Network802Dot1xData : public SettingDataBase
{
    yl::string m_strId;  // 802.1x id.
    yl::string m_strPassword;  // 802.1x password(MD5).
    Network802DotxModeType m_eMode;  // 802.1x mode.

    Network802Dot1xData()
    {
        Reset();
    }

    void Reset()
    {
        m_eMode = NETWOKR_802Dotx_TYPE_NONE;
        m_strId = "";
        m_strPassword = "";
    }
};

// Network VPN data.
struct NetworkVpnData : public SettingDataBase
{
    bool m_isEnable;  // If or not VPN is enable.

    NetworkVpnData()
    {
        Reset();
    }

    void Reset()
    {
        m_isEnable = false;
    }
};

// Network LLDP data.
#ifdef _T42
struct NetworkLLDPData : public SettingDataBase
#else
struct NetworkLLDPData
#endif
{
    bool m_bEnable;  // If or not LLDP is enable.
    int  m_nPacketInterval; // Packet Interval

    NetworkLLDPData()
    {
        Reset();
    }

    void Reset()
    {
        m_bEnable = false;
        m_nPacketInterval = 60;
    }
};

// Network CDP data.
#ifdef _T42
struct NetworkCDPData : public SettingDataBase
#else
struct NetworkCDPData
#endif
{
    bool m_bEnable;  // If or not CDP is enable.
    int  m_nCDPInterval; // CDP Interval

    NetworkCDPData()
    {
        Reset();
    }

    void Reset()
    {
        m_bEnable = false;
        m_nCDPInterval = 60;
    }
};

#ifdef _T42
struct NetworkStunData : public SettingDataBase
#else
struct NetworkStunData
#endif
{
    bool m_bEnable;
    yl::string  m_strStunSeverAddr;
    int m_iPort;

    NetworkStunData()
    {
        Reset();
    }

    void Reset()
    {
        m_bEnable = false;
        m_strStunSeverAddr = "";
        m_iPort = 3478;
    }
};

#ifndef _T42
// Network VPN data.
struct FWDInternationalData
{
    bool m_isEnable;  // If or not FWDInternational is enable.

    FWDInternationalData()
    {
        Reset();
    }

    void Reset()
    {
        m_isEnable = false;
    }
};
#endif

// PagingGroupInfo继承黑白屏特有的基类
struct PagingGroupInfoEx : public SettingDataBase
{
    PagingGroupInfo data;
};

struct AutoAnswerData : public SettingDataBase
{
    bool m_isOn;  // Auto answer status.
    int m_nLineId;  // Line id.
    bool m_IsAutoAnswerMute;
    AutoAnswerData()
    {
        Reset();
    }

    void Reset()
    {
        m_isOn = false;
        m_nLineId = -1;
        m_IsAutoAnswerMute = false;
    }
};

#ifdef IF_FEATURE_AUTO_ANSWER_TIMER
// Auto Answer
struct AutoAnswerTimer : public SettingDataBase
{
    int m_iTimer;

    AutoAnswerTimer()
    {
        Reset();
    }

    void Reset()
    {
        m_iTimer = 0;
    }
};
#endif

#if IF_BUG_27045
struct AreaCodeData : public SettingDataBase
{
    yl::string m_strCode;   //code
    bool m_bEnable;//enable  AreaCode

    AreaCodeData()
    {
        Reset();
    }

    void Reset()
    {
        m_strCode = "";
        m_bEnable = false;
    }
};
#endif

// network 802.1x mode type.
enum HotDeskType
{
    HDT_BASE = 0,               // HotDeskBase.
    HDT_LOGON_WIZRAD,           // LogonWizard.
    HDT_BASE_WITH_DATA,     // HotDeskBase with data.
    HDT_LOGON_WIZRAD_WITH_DATA, // LogonWizard with data.
};

// 重启前提示的回调函数
typedef void (*FunRebootTipCallBack)(const yl::string & strTip);

enum TextMsgMember
{
    TEXT_MSG_FROM_NAME = 0,
    TEXT_MSG_FROM_USER_NAME = 1,
    TEXT_MSG_FROM_SERVER = 2,
    TEXT_MSG_TO_NAME = 3,
    TEXT_MSG_TO_USER_NAME = 4,
    TEXT_MSG_TO_SERVER = 5,
    TEXT_MSG_TEXT = 6,
    TEXT_MSG_ICON_PATH = 7,
    TEXT_MSG_TIME = 8,
    TEXT_MSG_GTYPE = 9,
};

struct TimeZoneData
{
    // Invalid time zone type.
    enum { INVALID_TIMEZONE_TYPE = -1, };

    yl::string m_strTime;  // Time, such as "+8:.
    yl::string m_strZone;  // Zone, such as "Cuba(Havana)".
    yl::string m_strBegin;  // Begin time.
    yl::string m_strEnd;  // End time.
    yl::string m_strOffset;  // Offset.
    int m_nType;  // Time zone type.

    TimeZoneData()
    {
        Reset();
    }

    void Reset()
    {
        m_strTime = "";
        m_strZone = "";
        m_strBegin = "";
        m_strEnd = "";
        m_strOffset = "";
        m_nType = INVALID_TIMEZONE_TYPE;
    }
};

enum HotelingType
{
    HOTELING_TYPE_ACDON = 0,  //开了ACD
    HOTELING_TYPE_ACDOFF = 1,  //普通的
    HOTELING_TYPE_XMLBROWSER = 2,  // 使用xmlbrowser.
};


#if IF_SUPPORT_WALLPAPER
enum WALLPAPER_Type_E
{
    WALLPAPER_Type_CURRENT = 0,
    WALLPAPER_Type_DEFAULT,
};

enum BrowseImageType_E
{
    BROWSE_TYPE_NONE = 0,
    BROWSE_TYPE_WALL_PAPER,
    BROWSE_TYPE_EXP_WALL_PAPER,
    BROWSE_TYPE_USB_PHOTO,
};

struct ST_DisPlayWallPaperData
{
    BrowseImageType_E eBrowseType;

    yl::string strCurPaper; //界面初始化要显示的图片
};

struct ST_WallPaperData
{
    yl::string m_strPaperDisplayName;
    yl::string m_strResName;
    yl::string m_strPaperPath;

    ST_WallPaperData()
    {
        Reset();
    }
    void Reset()
    {
        m_strPaperDisplayName = "";
        m_strResName = "";
        m_strPaperPath = "";
    }
};

// LIST_WALLPAPER_DATA, records Wallpaper list data.
typedef YLList<ST_WallPaperData> LIST_WALLPAPER_DATA;

// Enum screen saver type.
enum ScreenSaverType_E
{
    SCREEN_SAVER_TYPE_NONE = 0,  // Invalid screen saver type.
    SCREEN_SAVER_PHOTOS = 1,  // Photos screen saver.
    SCREEN_SAVER_VIDEOS = 2,  // Videos screen saver.
    SCREEN_SAVER_MAX
};

// struct ST_ScreenSaverData, records screen saver data.
struct ST_ScreenSaverData
{
    ScreenSaverType_E m_eType;  // Screen saver type.
    int m_nInterval;  // Screen saver interval(2-20s).
    int m_nTimeout;  // Timeout(ms), Off(0), 1min, 2min, 5min, 10min, 30min.
};

// struct ST_DisplayGeneralData, records display general data.
struct ST_DisplayGeneralData
{
    int m_nActiveLevel;  // Active level. 1-10.
    int m_nInactiveLevel;  // Inactive level. 0, 1.
    int m_nBackLightTime;  // Back light time(min). 0-Always on.

    ST_DisplayGeneralData()
    {
        Reset();
    }

    void Reset()
    {
        m_nActiveLevel = 0;
        m_nInactiveLevel = 0;
        m_nBackLightTime = 0;
    }
};

// Remove returned value.
enum RemoveResutType_E
{
    RRT_OK = 0,  // Removed successfully.
    RRT_FILE_CANNOT_REMOVED = 1,  // Cannot remove.
    RRT_FAIL_TO_REMOVE = 2,  // Fail to remove.
    RRT_OTHER_ERROR = 3,  // Other error.
    RRT_MAX,
};

//图片浏览控件信息结构体
struct ST_PicsBrowseControlInfo
{
    int x;
    int y;
    int nWidth;
    int nHigh;
    bool bVisible;
    bool bCreate;

    ST_PicsBrowseControlInfo()
    {
        Reset();
    }

    ST_PicsBrowseControlInfo(int x, int y, int nWidth, int nHeight, bool bVisible, bool bCreate)
    {
        x = x;
        y = y;
        nWidth = nWidth;
        nHigh = nHeight;
        bVisible = bVisible;
        bCreate = bCreate;
    }

    void Reset()
    {
        x = 0;
        y = 0;
        nWidth = 0;
        nHigh = 0;
        bVisible = true;
        bCreate = true;
    }

    // Overwrite operator=.
    ST_PicsBrowseControlInfo& operator= (const ST_PicsBrowseControlInfo& rhs)
    {
        if (this != &rhs)
        {
            x = rhs.x;
            y = rhs.y;
            nWidth = rhs.nWidth;
            nHigh = rhs.nHigh;
            bVisible = rhs.bVisible;
            bCreate = rhs.bCreate;
        }
        return *this;
    }
};
#endif

#endif  // SETTING_DEFINE_COMMON_H_
