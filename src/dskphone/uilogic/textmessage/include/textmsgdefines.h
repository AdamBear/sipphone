#ifndef __TEXTMSG_DEFINES_H
#define __TEXTMSG_DEFINES_H

#include <ylstring.h>
#include <yllist.h>
#include <cstdlib>
#include "ylhashmap.h"

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
    time_t      m_tTime;  // 发送或接收时间.
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

// 文本信息状态类型.
enum TEXTMSG_STATUS_TYPE
{
    TMSG_STATUS_TYPE_NONE = 0,  // 不正确的状态.
    TMSG_STATUS_UNREAD = 1,  // 未读.
    TMSG_STATUS_READ = 2,  // 已读.
    TMSG_STATUS_TYPE_MAX
};

// 文本信息数据结构.
struct TextMsgItemData
{
    yl::string m_strFromName;  // 发送者显示名.
    yl::string m_strFromUserName;  // 发送者用户名.
    yl::string m_strFromServer;  // 发送者服务器ip.
    yl::string m_strToName;  // 接收者显示名.
    yl::string m_strToUserName;  // 接收者用户名.
    yl::string m_strToServer;  // 接收者服务器ip.
    yl::string m_strText;  // 信息文本内容.
    time_t m_tTime;  // 时间.
    int m_nGType;  // 组类型.
    TEXTMSG_STATUS_TYPE m_eSType;  // 状态类型.

    TextMsgItemData()
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
        m_tTime = 0;
        m_nGType = 0;
        m_eSType = TMSG_STATUS_TYPE_NONE;
    }
};

#endif //__TEXTMSG_DEFINES_H