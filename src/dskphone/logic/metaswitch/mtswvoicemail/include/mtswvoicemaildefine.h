#ifndef __MTSW_VOICEMAIL_DEFINE_H__
#define __MTSW_VOICEMAIL_DEFINE_H__

#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "metaswitch/mtswcontrol/include/mtswcommondefine.h"
#include "contacts/directory/include/directoryenumtypes.h"

#include "service_system.h"

// 语音信息状态类型.
enum ReadStatus
{
    STATUS_TYPE_NONE = 0,  // 不正确的状态.
    STATUS_UNREAD = 1,  // 未读.
    STATUS_READ = 2,  // 已读.
    STATUS_TYPE_MAX
};

// 播放状态类型.
enum PlayStatus
{
    STATUS_STOP = 0,  // stop
    STATUS_PLAY = 1,  // play
    STATUS_MTSW_PAUSE = 2  // pause
};


// MessageList.
struct VoiceMsgItemSimpData
{
    yl::string m_strName;  // 发送者或者接收者的显示名.
    time_t m_tTime;  // 发送或接收时间.
    bool m_bUrgent;         // 是否紧急邮件
    ReadStatus m_eStatus;  // 信息类型.

    VoiceMsgItemSimpData()
    {
        Reset();
    }

    void Reset()
    {
        m_strName.clear();
        m_bUrgent = false;
        m_eStatus = STATUS_TYPE_NONE;
    }
};

//语音信息Detail
struct VoiceMsgItemDetailData
{
    yl::string m_strId;     //邮件ID，唯一标识
    yl::string m_strName;  // 发送者显示名.
    yl::string m_strNumber;  // 发送者号码.
    time_t m_tTime;  // 发送或接收时间.
    yl::string m_strDuration;  // 时长.
    bool m_bUrgent;         // 是否紧急邮件
    ReadStatus m_eStatus;  // 读取状态.

    VoiceMsgItemDetailData()
    {
        Reset();
    }

    void Reset()
    {
        m_strId.clear();
        m_strName.clear();
        m_strNumber.clear();
        m_strDuration.clear();
        m_bUrgent = false;
        m_eStatus = STATUS_TYPE_NONE;
    }
};

struct VoiceMsgSimpListData
{
    YLList<VoiceMsgItemSimpData> m_listMessage;  // 信息简单信息链表.
    int m_nIndexFrom;  // 从何处下标开始获取.
    int m_nCountToGet;  // 获取的数量.
    int m_nTotalCount;  // 总数.

    VoiceMsgSimpListData()
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

// 文本信息数据结构.
struct VoicemailItemData
{
    yl::string m_strId;         //邮件ID，唯一标识
    yl::string m_strFromName;  // 发送者显示名.
    yl::string m_strFromUserName;  // 发送者用户名.
    yl::string m_strFromServer;  // 发送者服务器ip.
    yl::string m_strToName;  // 接收者显示名.
    yl::string m_strToUserName;  // 接收者用户名.
    yl::string m_strToServer;  // 接收者服务器ip.
    yl::string m_strDuration; //邮件时长
    int m_iDuration; //邮件时长
    yl::string m_strTime;  // 时间.
    bool m_bUrgent;  // 是否紧急邮件.
    ReadStatus m_eStatus;  // 读取状态.

    VoicemailItemData()
    {
        Reset();
    }

    void Reset()
    {
        m_strId.clear();
        m_strFromName.clear();
        m_strFromUserName.clear();
        m_strFromServer.clear();
        m_strToName.clear();
        m_strToServer.clear();
        m_strToUserName.clear();
        m_strTime.clear();
        m_bUrgent = false;
        m_eStatus = STATUS_TYPE_NONE;
    }
};

struct VoiceMailFileData
{
    int m_iID;
    FileDownloadStatus m_eDownloadStatus;
};

#endif
#endif
