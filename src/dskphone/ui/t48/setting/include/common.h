#ifndef __SETTING_COMMON_H__
#define __SETTING_COMMON_H__

#include <ylstring.h>

#include <cstdlib>  // use memset.
#include <yllist.h>
#include <ylvector.h>
#include "dsskey/include/dsskey_enumtypes.h"
#include <exp/include/modexp.h>
#include "talk/fwd_dnd/include/fwddndtypes.h"
#include "account/include/account_datadefine.h"

#ifdef __cplusplus
extern "C" {
#endif

const char * const kszSepTimeAndZone = " ";

// 由于在ui层使用void *指向各分配内存的结构体，会导致调用不到析构函数，造成内存泄漏
// 此处添加基类,将析构函数设为虚函数,各结构体继承此结构体,解决内存泄漏
struct SettingDataBase
{
    virtual ~SettingDataBase() {}
};

// The code info.
struct CodeInfo
{
    // The max code length.
    static const int knMaxCodeLength = 99;

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

struct ExternalDeviceData : public SettingDataBase
{
    yl::string m_strWifiMac;
    yl::string m_strBluetoothMac;

    ExternalDeviceData()
    {
        Reset();
    }

    void Reset()
    {
        m_strWifiMac = "";
        m_strBluetoothMac = "";
    }
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
    bool m_AnonymousRejectSendCode;  // Send anonymous reject on code or off code.
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
        m_AnonymousRejectSendCode = false;
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

// struct LanguageListData.
struct LanguageItemListData
{
    YLList<LanguageItemData> m_listLang;    // Language item data list pointer.

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

    bool m_isOn;  // Status: true-ON, false-OFF.
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
        m_isOn = false;
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

// Enum. Forward type.
enum ForwardType
{
    FORWARD_TYPE_NONE = 0,  // Has no type.
    FORWARD_TYPE_ALWAYS = 1 << 0,  // Forward always.
    FORWARD_TYPE_BUSY = 1 << 1,  // Forward busy.
    FORWARD_TYPE_NOANSWER = 1 << 2,  //  Forward no answer.
    FORWARD_TYPE_ALL = FORWARD_TYPE_ALWAYS | FORWARD_TYPE_BUSY | FORWARD_TYPE_NOANSWER,
    FORWARD_TYPE_MAX
};

// struct ForwardDataAndType, records forward data and type.
struct ForwardDataAndType
{
    ForwardType m_eType;  // Forward type.
    ForwardData m_stData;  // Forward data.
    // Invalid ring times.

    ForwardDataAndType()
    {
        Reset();
    }

    void Reset()
    {
        m_eType = FORWARD_TYPE_NONE;
        m_stData.Reset();
    }
};

// struct DndData, records DND data.
struct DndData : public SettingDataBase
{
    bool        m_bEnabled;
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
    DaylightSavingType m_eDayLightType;  // Day light saving type.
    yl::string m_strTimeZoneCity;   // Current TimeZone City| add by dlw 2014-11-06
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
        m_eDayLightType = DAYLIGHTING_SAVING_TYPE_NONE;
        m_strTimeZoneCity = "";
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
    DATE_FORMAT_MM_DD_WWW_SPACE = 7,  // MM DD WWW
    DATE_FORMAT_MM_DD_WW_SPACE = 8, // MM DD WW
    DATE_FORMAT_YY_MM_DD_HYPHEN = 9,  // YY-MM-DD
    DATE_FORMAT_YY_MM_DD_VIRGULE = 10,  // YY/MM/DD
    DATE_FORMAT_YYYY_MM_DD_VIRGULE = 11,  // YYYY/MM/DD
    DATE_FORMAT_YYYY_MM_DD_SPACE = 12,  // YYYY MM DD
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
    bool           m_bDateShowFirst;

    DateAndTimeFormatData()
    {
        Reset();
    }

    void Reset()
    {
        m_eDateFormat = DATE_FORMAT_TYPE_NONE;
        m_eTimeFormat = TIME_FORMAT_TYPE_NONE;
        m_strDiyDateFmt.clear();
        m_bDateShowFirst = false;
    }
};

#ifdef _T42
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
#endif

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
#ifdef _T42
struct PhoneLockData : public SettingDataBase
{
    bool m_bPhoneLock;                  // Phone lock Enable
    KEY_GUARD_TYPE m_ePhoneLockType;    // Phone lock type.

    PhoneLockData()
    {
        Reset();
    }

    void Reset()
    {
        m_bPhoneLock = false;
        m_ePhoneLockType = KG_ALL_NONE;
    }
};
#else
struct PhoneLockData
{
    PhoneLockType m_ePhoneLockType;  // Phone lock type.

    PhoneLockData()
    {
        Reset();
    }

    void Reset()
    {
        m_ePhoneLockType = PHONE_LOCK_TYPE_NONE;
    }
};
#endif

#ifdef _T42
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
#endif

// struct PasswordData, records password data.
struct PasswordData : public SettingDataBase
{
    yl::string m_strPasswordBefore; // Before password.
    yl::string m_strPasswordNew;        // New password to set.

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
    time_t     m_tTime;  // 发送或接收时间.
    TextMsgGroupType m_eGType;  // 信息类型.

    TextMsgItemSimpData()
    {
        Reset();
    }

    void Reset()
    {
        m_strName.clear();
        m_strIconPath.clear();
        m_tTime = 0;
        m_eGType = TEXT_MSG_GROUP_TYPE_NONE;
    }
};

// 文本一般信息链表.
struct TextMsgSimpListData
{
    YLList<TextMsgItemSimpData> m_listMessage;  // 信息简单信息链表.
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
    time_t      m_tTime;    //Time_t格式的时间
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

// 所有账号的信息列表.
struct AccountTextMsgListData
{
    YLList<AccountTextMsgData> m_listMsg;  // 信息列表.

    AccountTextMsgListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listMsg.clear();
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

// struct AdvancedCallFeatureCallCompletionData
struct AdvancedCallFeatureCallCompletionData
{
    bool m_isOn;  // Whether or not the call completion is on.

    AdvancedCallFeatureCallCompletionData()
    {
        Reset();
    }

    void Reset()
    {
        m_isOn = false;
    }

    // Override operator=.
    AdvancedCallFeatureCallCompletionData & operator= (
        const AdvancedCallFeatureCallCompletionData & rhs)
    {
        if (this != &rhs)
        {
            m_isOn = rhs.m_isOn;
        }
        return *this;
    }

    // Override operator ==.
    bool operator == (const AdvancedCallFeatureCallCompletionData & rhs)
    {
        return m_isOn == rhs.m_isOn;
    }
};

// struct AdvancedCallFeatureGeneralData.
struct AdvancedCallFeatureGeneralData
{
    KeyAsSendType m_eType;  // Key as send.
    bool m_isHistorySaved;  // Whether or not history should be saved.

    AdvancedCallFeatureGeneralData()
    {
        Reset();
    }

    void Reset()
    {
        m_eType = KEY_AS_SEND_TYPE_INVALID;
        m_isHistorySaved = false;
    }

    // Override operator=.
    AdvancedCallFeatureGeneralData & operator= (
        const AdvancedCallFeatureGeneralData & rhs)
    {
        if (this != &rhs)
        {
            m_eType = rhs.m_eType;
            m_isHistorySaved = rhs.m_isHistorySaved;
        }
        return *this;
    }

    // Override operator ==.
    bool operator == (const AdvancedCallFeatureGeneralData & rhs)
    {
        return m_eType == rhs.m_eType
               && m_isHistorySaved == rhs.m_isHistorySaved;
    }
};

// struct AdvanceCallFeatureData
struct AdvanceCallFeatureData
{
    yl::string m_strKeyAsSend;  // Key as send string.
    yl::string m_strHotlineNumber;  // Hotline number.
    int m_nHotlineDelay;  // Hotline delay time.
    int m_nAutoRedialInterval;  // Auto redial interval.
    int m_nAutoRedialTimes;  // Auto redial times.

    bool m_isHistory;  // History save switch, true-save, false-not save.
    bool m_isAutoRedial;  // Auto redial switch, false-closed, true-open.
    bool m_isCallCompletion;  // Call completion switch, false-closed, true-open.

    AdvanceCallFeatureData()
    {
        Reset();
    }

    void Reset()
    {
        m_strKeyAsSend = "";
        m_strHotlineNumber = "";
        m_nHotlineDelay = 0;
        m_nAutoRedialInterval = 0;
        m_nAutoRedialTimes = 0;
        m_isHistory = false;
        m_isAutoRedial = false;
        m_isCallCompletion = false;
    }
};

// struct AesKeyData, records AES key data.
#ifdef _T42
struct AesKeyData : public SettingDataBase
#else
struct AesKeyData
#endif
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
    yl::string m_strAesKey;    // AES key.
    yl::string m_strAesMacKey; // AES mac key.

    AutoProvisionData()
    {
        Reset();
    }

    void Reset()
    {
        m_strUrl = "";
        m_strUsername = "";
        m_strPassword = "";
        m_strAesKey = "";
        m_strAesMacKey = "";
    }

    bool operator ==(const AutoProvisionData & rhf)
    {
        return (m_strUrl == rhf.m_strUrl
                && m_strUsername == rhf.m_strUsername
                && m_strPassword == rhf.m_strPassword
                && m_strAesKey == rhf.m_strAesKey
                && m_strAesMacKey == rhf.m_strAesMacKey);
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

// struct SoundGeneralData.
struct SoundGeneralData
{
    bool m_isKeypressSoundOn;  // Whether or not the key press Sound is on.

    SoundGeneralData()
    {
        Reset();
    }

    void Reset()
    {
        m_isKeypressSoundOn = false;
    }
};

// struct RingFileData.
struct RingFileData
{
    yl::string m_strRingDisplayName;
    yl::string m_strRingPath;
};

// struct SoundListData.
struct RingFileListData
{
    YLList<RingFileData> m_listRing;  // Sound data list pointer.

    RingFileListData()
    {
        Reset();
    }

    void Reset()
    {
        // Nothing to do.
    }
};

// struct VolumeData
struct VolumeData
{
    int m_nVolumeValue;  // The value of volume.

    VolumeData()
    {
        Reset();
    }

    void Reset()
    {
        m_nVolumeValue = 0;
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
#ifndef _T42
// struct DSS key data, records DSS key data.
struct DssKeyData
{
    DssKey_Type m_eType;  // DSS key type.
    int m_iLineIndex;    // Line Index
    int m_iIndex;           // Index Id.
    DssKeyEventType m_eEventType;  // Key Event type.
    yl::string m_strValue;  // Value.
    yl::string m_strExtension;  // Extension.
    yl::string m_strLabel;      // Label
    DssKeyLockType m_eLockType;  //Lock Type

    DssKeyData()
    {
        Reset();
    }

    void Reset()
    {
        m_eType = DT_NONE;
        m_iLineIndex = -1;   // Line Index
        m_iIndex = -1;
        m_eEventType = DSS_KEY_EVENT_TYPE_CONFERENCE;
        m_strValue = "";
        m_strExtension = "";
        m_strLabel = "";
        m_eLockType = DSS_KEY_LOCK_TYPE_DEFAULT;
    }

    void operator=(const DssKeyData & refData)
    {
        if (&refData == this)
        {
            return;
        }
        m_eType = refData.m_eType;
        m_iLineIndex = refData.m_iLineIndex;     // Line Index
        m_iIndex = refData.m_iIndex;
        m_eEventType = refData.m_eEventType;
        m_strValue = refData.m_strValue;
        m_strExtension = refData.m_strExtension;
        m_strLabel = refData.m_strLabel;
        m_eLockType = refData.m_eLockType;
    }

};
#endif

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

// Account status list data.
struct AccountStatusListData
{
    YLList<AccountStatusData> m_listStatus;  // Account status list.
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
    yl::string m_strOutboundProxy2;  // Outbound proxy.
    yl::string m_strSipServer1;  // Sip server.
    yl::string m_strSipServer2;  // Sip server.
    //bool m_bIsAutoAnswer; // auto answer
    bool m_isActivated;  // If or not activated.
    bool m_isOutboundEnable;  // If or not outbound is enable.
    bool m_isNATEnable;  // If or not STUN enable.

    int        m_nProxyFallbackInterval;  // Outbound proxy fallback interval.

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
        m_strSipServer1 = "";
        m_strSipServer2 = "";
        m_isActivated = false;
        m_isOutboundEnable = false;
        //m_bIsAutoAnswer = false;
        m_isNATEnable = false;
        m_nProxyFallbackInterval = 3600;
    }
};

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

// 时区列表数据.
struct TimeZoneListData
{
    YLList<yl::string> m_listTimeZone;  // 时区列表.

    TimeZoneListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listTimeZone.clear();
    }
};

// 时区城市列表数据 | add by dlw 2014-11-05
struct TimeZoneCityListData
{
    YLList<yl::string> m_listTimeZoneCity;  // 时区城市列表.

    TimeZoneCityListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listTimeZoneCity.clear();
    }
};

// struct AutoAnswerData.
#ifdef _T42
struct AutoAnswerData : public SettingDataBase
{
    bool m_isOn;  // Auto answer status.
    int m_nLineId;  // Line id.

    AutoAnswerData()
    {
        Reset();
    }

    void Reset()
    {
        m_isOn = false;
        m_nLineId = -1;
    }
};
#endif

// struct BroadSoftDirectoryData
struct BroadSoftDirectoryData
{
    yl::string m_strDisplayName;  // Broad soft directory display name.
    yl::string m_strServer;  // Broad soft directory server.
    yl::string m_strPort;  // Broad soft directory port.
    yl::string m_strUserName;  // Broad soft directory user name.
    yl::string m_strPassword;  // Broad soft directory password.

    BroadSoftDirectoryData()
    {
        Reset();
    }

    void Reset()
    {
        m_strDisplayName = "";
        m_strServer = "";
        m_strPort = "";
        m_strUserName = "";
        m_strPassword = "";
    }
};

// Open menu string.
const char * const kszOpenXmlAction = "OpenMenu";
// Open page string.
const char * const kszOpenPageAction = "OpenPage";
// 打开联系人页面.
const char * const kszOpenDirectoryPage = "OpenDirectoryPage";
// 打开通话记录页面.
const char * const kszOpenCallLogPage = "OpenCallLogPage";
// Open sub-page string.
const char * const kszOpenSubPageAction = "OpenSubPage";
// Open sub-page string.
const char * const kszEnterDialAction = "EnterDial";
// 本地联系人click action mask.
const char * const kszCALocalDirectory = "local_contact_x";
// Remote directory click action mask
const char * const kszCARemoteDirectory = "remote_phonebook_x";
// BroadSoft directory click action mask.
const char * const kszCABroadSoftDirectory = "broadsoft_phonebook_x";
// BroadSoft calllog click action mask.
const char * const kszCABroadSoftCallLog = "broadsoft_calllog_x";
// BroadTouch UC Group click action mask.
#define kszCABroadTouchUCDirectory "btuc_phonebook_"

struct WallPaperData
{
    yl::string m_strPaperDisplayName;
    yl::string m_strResName;
    yl::string m_strPaperPath;

    WallPaperData()
    {

    }
};

// struct WallPaperListData, records Wallpaper list data.
struct WallPaperListData
{
    YLList<WallPaperData> m_listPaperData;

    WallPaperListData()
    {

    }
};

// struct ThemeListData, records theme list data.
struct ThemeListData
{
    YLList<yl::string> m_listTheme;

    ThemeListData()
    {
        Reset();
    }

    void Reset()
    {
        // Nothing to do.
    }
};

// Enum screen saver type.
enum ScreenSaverType
{
    SCREEN_SAVER_TYPE_NONE = 0,  // Invalid screen saver type.
    SCREEN_SAVER_PHOTOS = 1,  // Photos screen saver.
    SCREEN_SAVER_VIDEOS = 2,  // Videos screen saver.
    SCREEN_SAVER_MAX
};

// struct ScreenSaverData, records screen saver data.
struct ScreenSaverData
{
    ScreenSaverType m_eType;  // Screen saver type.
    int m_nInterval;  // Screen saver interval(2-20s).
    int m_nTimeout;  // Timeout(ms), Off(0), 1min, 2min, 5min, 10min, 30min.
};

// struct PhotoListData, records photo list.
struct PhotoListData
{
    YLList<yl::string> m_listPhoto;  // Photo list.

    PhotoListData()
    {
        Reset();
    }

    void Reset()
    {
        // Nothing to do.
    }
};

// struct DisplayGeneralData, records display general data.
struct DisplayGeneralData
{
    int m_nActiveLevel;  // Active level. 1-10.
    int m_nInactiveLevel;  // Inactive level. 0, 1.
    int m_nBackLightTime;  // Back light time(min). 0-Always on.

    DisplayGeneralData()
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
enum RemoveResutType
{
    RRT_OK = 0,  // Removed successfully.
    RRT_FILE_CANNOT_REMOVED = 1,  // Cannot remove.
    RRT_FAIL_TO_REMOVE = 2,  // Fail to remove.
    RRT_OTHER_ERROR = 3,  // Other error.
    RRT_MAX,
};

// 图片类型.
enum ImagePathType
{
    IMAGE_PATH_TYPE_NONE = 0,  // 不正确的类型.
    IMAGE_PATH_SYSTEM = 1,  // 系统图片.
    IMAGE_PATH_VARDATA = 2,  // 客户上传的图片.
    IMAGE_PATH_USERDATA = 3,  // 用户上传的图片.
    IMAGE_PATH_TYPE_MAX
};

// Voice Mail配置项数据.
struct VoiceMailConfigData
{
    yl::string m_strMailNumber;  // 语音信箱.

    VoiceMailConfigData()
    {
        Reset();
    }

    void Reset()
    {
        m_strMailNumber.clear();
    }
};

// Voice Mail配置链表.
struct VoiceMailConfigListData
{
    YLList<VoiceMailConfigData> m_listCfg;  // Vioce Mail配置项列表.

    VoiceMailConfigListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listCfg.clear();
    }
};

// Voice Mail信息结构体.
struct VoiceMailInfoData
{
    yl::string m_strAccountDisplayName;  // 账号显示名.
    yl::string m_strMailNumber;  // 信箱号码.
    int m_nAccountStaus;  // 账号状态, 为LINE_STATE枚举值.
    int m_nUnReadCount;  // 未读的Voice Mail数量.
    int m_nReadCount;  // 已读的Voice Mail数量.

    VoiceMailInfoData()
    {
        Reset();
    }

    void Reset()
    {
        m_strAccountDisplayName.clear();
        m_strMailNumber.clear();
        m_nAccountStaus = -1;  // LS_UNKNOW.
        m_nUnReadCount = 0;
        m_nReadCount = 0;
    }
};

// 所有账号的Voice Mail列表.
struct VoiceMailInfoListData
{
    YLList<VoiceMailInfoData> m_listInfo;  // 账号的Voice Mail列表.

    VoiceMailInfoListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listInfo.clear();
    }
};

// setting pageMode
enum PAGE_MODE
{
    PAGE_MODE_SETTING, // 普通设置界面
    PAGE_MODE_ZERO,    // Zero模式下的界面
    PAGE_MODE_LOCK,    // 键盘锁开启后的界面
};

struct SubMenuData
{
    int         iFirstSelectItem;    // 进入SubMenu的时候选中第几个Item
    void    *   pBeforeSetSubpageData;   // 设置子页面前的附加数据
    void    *   pAfterSetSubpageData;    // 设置子页面后的附加数据
    PAGE_MODE   eMode;               // SubMenu的页面类型
    yl::string strMenuAction;        // 页面的Action

    // 默认构造函数
    SubMenuData()
    {
        Reset();
    }

    // 带Action的构造函数
    SubMenuData(const yl::string & strAction)
    {
        Reset();
        strMenuAction = strAction;
    }

    // 带Action和Index的构造函数
    SubMenuData(const yl::string & strAction, int iIndex)
    {
        Reset();
        strMenuAction    = strAction;
        iFirstSelectItem = iIndex;
    }

    // 带Action和mode的构造函数
    SubMenuData(const yl::string & strAction, PAGE_MODE ePageMode)
    {
        Reset();
        strMenuAction = strAction;
        eMode         = ePageMode;
    }

    void Reset()
    {
        iFirstSelectItem = 0;
        pBeforeSetSubpageData = NULL;
        pAfterSetSubpageData = NULL;
        eMode = PAGE_MODE_SETTING;
        strMenuAction = "";
    }
};

// struct MenuItemData, it records menu item, eg. id_name, icon_name.
struct MenuItemData
{
    yl::string m_strIdName;  // Id name, one and only to identify one menu item.
    yl::string m_strDisplayName;  // Display name, can be changed.
    yl::string m_strIconName;  // Normal Icon name used to display.
    yl::string m_strSelectedIconName; // Selected Icon name used to display
    yl::string m_strClickAction;  // Click action. When user clicks one menu item,
    // do the action.
    yl::string m_strLongClickAction;  // Long click action. When user clicks one menu item for
    // a long time, do the action.
    yl::string m_strUserLevel;  // User level, eg.admin.
    bool m_bShouldBeTranslated;

    MenuItemData()
    {
        Reset();
    }

    void Reset()
    {
        m_strIdName = "";
        m_strDisplayName = "";
        m_strIconName = "";
        m_strSelectedIconName = "";
        m_strClickAction = "";
        m_strLongClickAction = "";
        m_strUserLevel = "";
        m_bShouldBeTranslated = true;
    }

    MenuItemData & operator= (const MenuItemData & rhs)
    {
        // No need to copy self.
        if (this != &rhs)
        {
            m_strIdName = rhs.m_strIdName;
            m_strDisplayName = rhs.m_strDisplayName;
            m_strIconName = rhs.m_strIconName;
            m_strSelectedIconName = rhs.m_strSelectedIconName;
            m_strClickAction = rhs.m_strClickAction;
            m_strLongClickAction = rhs.m_strLongClickAction;
            m_strUserLevel = rhs.m_strUserLevel;
            m_bShouldBeTranslated = rhs.m_bShouldBeTranslated;
        }
        return *this;
    }

    // 重载比较操作符
    bool operator== (const MenuItemData & rhs) const
    {
        return (m_strIdName == rhs.m_strIdName
                && m_strDisplayName == rhs.m_strDisplayName
                && m_strIconName == rhs.m_strIconName
                && m_strSelectedIconName == rhs.m_strSelectedIconName
                && m_strClickAction == rhs.m_strClickAction
                && m_strLongClickAction == rhs.m_strLongClickAction
                && m_strUserLevel == rhs.m_strUserLevel
                && m_bShouldBeTranslated == rhs.m_bShouldBeTranslated);
    }

    bool operator!= (const MenuItemData & rhs) const
    {
        return !(this->operator ==(rhs));
    }
};

typedef YLList<MenuItemData *> MenuItemList;
typedef MenuItemList::iterator MenuItemListIter;

// struct MenuItemDataList.
struct MenuItemListData
{
    yl::string m_strClickAction;  // Such as "OpenXml("menu_list.xml")";
    MenuItemList * m_pMenuItemList; // Menu item data list pointer.

    MenuItemListData()
    {
        Reset();
    }

    void Reset()
    {
        m_strClickAction = "";
        m_pMenuItemList = NULL;
    }

    // Whether or not the struct data is valid.
    bool IsValid() const
    {
        return m_pMenuItemList != NULL && !m_strClickAction.empty();
    }

    // Overwrite operator=.
    MenuItemListData & operator= (const MenuItemListData & rhs)
    {
        if (this != &rhs)
        {
            m_strClickAction = rhs.m_strClickAction;
            m_pMenuItemList = rhs.m_pMenuItemList;
        }
        return *this;
    }
};

// 重启前提示的回调函数
typedef void (*FunRebootTipCallBack)(const yl::string & strTip);

#ifdef __cplusplus
}  // __cplusplus
#endif

#endif  // __SETTING_COMMON_H__
