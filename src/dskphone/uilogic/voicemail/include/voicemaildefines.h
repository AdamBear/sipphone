#ifndef __VOICEMAIL_DEFINES_H__
#define __VOICEMAIL_DEFINES_H__

#include <yllist.h>
#include <ylstl/ylstring.h>

#define VOICEMAIL_INFO    ACCOUNT_INFO
#define IF_TEST_VOICEMAIL 0

// Voice Mail配置项数据.
struct VoiceMailConfigData
{
    yl::string m_strMailNumber;  // 语音信箱号码.
    int        m_iAccountID;     // 账号ID
    VoiceMailConfigData()
    {
        Reset();
    }

    void Reset()
    {
        m_strMailNumber.clear();
        m_iAccountID = -1;
    }
};

// Voice Mail配置链表.
typedef YLList<VoiceMailConfigData> VoiceMailConfigListData;

// Voice Mail信息结构体.
struct VoiceMailInfoData
{
    yl::string m_strAccountDisplayName;  // 账号显示名.
    yl::string m_strMailNumber;          // 信箱号码.
    int m_nAccountStaus;                 // 账号状态, 为LINE_STATE枚举值.
    int m_nUnReadCount;                  // 未读的Voice Mail数量.
    int m_nReadCount;                    // 已读的Voice Mail数量.
    int m_nNotifyUnreadCount;            // 通知的Voice Mail未读数量.
    int m_nNotifyReadedCount;            // 通知的Voice Mail已读数量.
    int m_iAccountID;                    // 账号ID
    bool m_bDisplayVoiceMail;

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
        m_nNotifyUnreadCount = 0;
        m_nNotifyReadedCount = 0;
        m_iAccountID = -1;
        m_bDisplayVoiceMail = true;
    }
};

// 所有账号的Voice Mail列表.
typedef YLList<VoiceMailInfoData> VoiceMailInfoListData;

struct VoiceMailUserInfo
{
    int iAccountID;
    int iReadedCount;
    int iUnReadCount;
};
#endif //__VOICEMAIL_DEFINES_H__
