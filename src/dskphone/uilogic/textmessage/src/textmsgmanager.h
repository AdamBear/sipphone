#ifndef __TEXTMSG_MANAGER_H
#define __TEXTMSG_MANAGER_H
#include "textmsgdefines.h"
#include <ETLLib.hpp>
#include "interfacedefine.h"
#include "etlmsghandler/modetlmsghandler.h"

class CTextMsgManager
{
public:
    static CTextMsgManager & GetInstance()
    {
        static CTextMsgManager sInstance;
        return sInstance;
    }

public:
    // 文本信息链表.
    typedef YLList<TextMsgItemData *> ItemList;

    // 各个账号对应的未查看信息数记录.
    // Key: 为账号用户名, Value: 为未查看信息数.
    typedef YLHashMap<yl::string, int, STRING_HASH> UnReadMap;

    // 获取所有账号当前有的未读信息列表.
    bool GetAccountMsgList(AccountTextMsgListData & refData);

    // 获取第一个未读信息的下标.
    // 如果获取成功则返回取得的下标, 否则返回-1.
    int GetFirstUnReadInMessageIndex() ;

    // Get message general list by click action.
    bool GetMessageList(TextMsgSimpListData & refData, TextMsgGroupType eGtype);

    // Get message detail info by click action and index.
    bool GetMessageDetail(TextMsgItemDetailData & refData, TextMsgGroupType eGtype, int nIndex);

    TEXTMSG_STATUS_TYPE GetMessageStatus(TextMsgGroupType eGtype, int nIndex);

    // Add a draft message.
    int AddDraftMessage(const TextMsgSendItemData & refData);

    // 向指定组中增加一条信息.
    int AddMessage(const TextMsgSendItemData & refItem, TextMsgGroupType eGType,
                   TEXTMSG_STATUS_TYPE eSType);

    // Send a message.
    bool SendTheMessage(const TextMsgSendItemData & refData);

    // Remove the message at some index in the list.
    bool RemoveMessage(TextMsgGroupType eGtype, int nIndex);

    // 删除一组信息.
    bool RemoveMessageList(TextMsgGroupType eGtype, YLList<int> & listIndex);

    // Remove all the message in the list.
    bool RemoveAllMessage(TextMsgGroupType eGtype);

    bool RemoveAllMessage(bool bCheck = true);

    // 处理计时器消息.
    bool OnTimer(int iTimerID) DECLARE_ATTR;

    // 消息处理
    bool ProcessMsg(msgObject & objMessage);

    int GetTotalUnRead();

    // 根据账号名获取账号当前的未读信息数.
    int GetAccountUnRead(const yl::string & strUserName);

    // http://192.168.1.99/Bug.php?BugID=22025
    // SMS是否可用
    bool IsSMSEnable();

    // 解决循环初始化
    void Init();

    void Uninit();

    // Convert click action to message group type.
    TextMsgGroupType ClickAction2GroupType(const yl::string & strClickAction) const;

private:
    // 处理TextMessage消息
    bool ProcessIntanceMessage(msgObject & objMessage);

    // 处理TextMessage未读消息
    bool ProcessTextMessageChange();

    // 增加账号的信息数.
    bool AddUnReadToAccount(const yl::string & strUserName, int nAdd);

    // Do initializations.
    bool DoInit();

    // Do finalizations.
    bool DoRelease();

    // 根据下标获取指定组中的信息数据.
    TextMsgItemData * FindItem(int nGType, int nIndex);

    // 处理消息发送返回的结果.
    LRESULT OnMessageSendResult(msgObject & objMessage);

    // 从配置文件中加载信息数据.
    bool LoadConfigFile(bool bCheck = true);

    // 将信息保存到配置文件中.
    bool SaveConfigFile();

    // 向链表中插入一条记录.
    int InsertItem(const TextMsgItemData & refItem);

    // 根据信息组类型和状态获取图片(全路径).
    yl::string GetIconPath(int nGType, TEXTMSG_STATUS_TYPE eSType);

    // 将发送的数据转换为ItemData.
    bool TMDID2ID(TextMsgItemData & refData, int nGType, TEXTMSG_STATUS_TYPE eSType,
                  const TextMsgSendItemData & refItem);

    // 设置改动标记.
    void SetDirty(bool isDirty);

    // 获取改动标记.
    bool GetDirty();

    // 检查未读信息数.
    void CheckUnRead();

    // 更新组中信息的组.
    bool UpdateMessageGType(TextMsgGroupType eGType, int nIndex, TextMsgGroupType  eGNewType);

    // 将ItemData转换为文本信息的简单数据.
    bool ID2TMSD(TextMsgItemSimpData & refData, const TextMsgItemData & refItem);

    // 根据发送类型以及用户名和服务器名获取显示名.
    // 返回显示名.
    yl::string GetDisplayName(const yl::string & strUser, const yl::string & strServer,
                              bool isSelf) const;

    // 将ItemData转换为信息详情.
    bool ID2TMIDD(TextMsgItemDetailData & refData, const TextMsgItemData & refItem);

    // 插入一条信息到链表, 不同于AddMessage, 此函数的参数里信息
    int AddDetailMessage(const TextMsgItemData & refItem);

private:
    ItemList m_listItem;  // 信息链表.
    UnReadMap m_mapUnRead;  // 未读信息map.
    bool m_isDirty;  // 是否有改动.
    bool m_bCheckFirst; //是否是第一次检测
    const int m_iTimerID;

private:
    CTextMsgManager();
    ~CTextMsgManager();
};

#define _TextMsgManager CTextMsgManager::GetInstance()

#endif //__TEXTMSG_MANAGER_H