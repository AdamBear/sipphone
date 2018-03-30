#ifndef SETTING_TEXTMESSAGECONTROLLER_H_
#define SETTING_TEXTMESSAGECONTROLLER_H_

#include <ylstring.h>

#include "ETLLib.hpp"

#include "settinguilogic/include/common.h"

class CTextMessageModel;

// Class CTextMessageController.
class CTextMessageController
{
public:
    // 文本信息状态类型.
    enum StatusType
    {
        STATUS_TYPE_NONE = 0,  // 不正确的状态.
        STATUS_UNREAD = 1,  // 未读.
        STATUS_READ = 2,  // 已读.
        STATUS_TYPE_MAX
    };

    // 文本信息数据结构.
    struct ItemData
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
        StatusType m_eSType;  // 状态类型.

        ItemData()
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
            m_eSType = STATUS_TYPE_NONE;
        }
    };

    // Get the singleton instance.
    static CTextMessageController * GetInstance();
    // Release the singleton instance.
    static void ReleaseInstance();

    // Message handle method.
    static LRESULT OnMessage(msgObject & refObj);

    // 获取所有账号当前有的未读信息列表.
    // | pData | [out] 是buffer.
    // 如果获取成功则返回true, 否则返回false.
    bool GetAccountMsgList(AccountTextMsgListData * pData) const;

    // Add a draft message.
    // | refData | is the message data to add.
    // Return the index the new added message is at in the list, otherwise
    // return -1.
    int AddDraftMessage(const TextMsgSendItemData & refData);

    // Send a message.
    // | refData | is the message data to send.
    // Return true if sent successfully, otherwise return false.
    bool SendTheMessage(const TextMsgSendItemData & refData);

    // 设置缓存数据
    void SetTempTextMsgDetailData(const TextMsgItemDetailData & stMsgDetail);

    // 获取缓存数据
    TextMsgItemDetailData GetTempTextMsgDetailData();

    // 重置缓存数据
    void ResetTempTextMsgDetailData();

    // 设置缓存数据索引
    void SetTempDataIndex(int nIndex);

    // 重置缓存数据索引
    int GetTempDataIndex();


    // 文本信息链表.
    typedef YLList<ItemData *> ItemList;
    typedef YLList<int> IntList;

    // 各个账号对应的未查看信息数记录.
    // Key: 为账号用户名, Value: 为未查看信息数.
    typedef YLHashMap<yl::string, int> UnReadMap;

    // 获取所有账号当前有的未读信息列表.
    // | pData | [out] 是buffer.
    // 如果获取成功则返回true, 否则返回false.
    bool GetAccountMsgList(AccountTextMsgListData * pData);

    // 获取某个组的信息列表.
    // | pData | [out] 是buffer.
    // | nType | 是组类型, 为GroupType枚举值.
    // 如果获取成功则返回true, 否则返回false.
    bool GetMessageList(TextMsgSimpListData * pData, int nType) const;

    // 获取第一个未读信息的下标.
    // 如果获取成功则返回取得的下标, 否则返回-1.
    int GetFirstUnReadInMessageIndex() const;

    // 获取指定组中某下标对应的信息详情.
    // | pData | [out] 是buffer.
    // | nType | 是组类型, 为GroupType枚举值.
    // | nIndex | 是指定的下标.
    // 如果获取成功则返回true, 否则返回false.
    bool GetMessageDetail(TextMsgItemDetailData * pData,
                          int nType,
                          int nIndex);

    // 向指定组中增加一条信息.
    // | refData | 是要增加的文本数据.
    // | nGType | 是组类型.
    // | eSType | 是状态类型.
    // 如果增加成功则返回信息在链表中的序号, 否则返回-1.
    int AddMessage(const TextMsgSendItemData & refItem,
                   int nGType,
                   StatusType eSType);

    // 更新组中信息的组.
    // | nGType | 为组类型.
    // | nIndex | 为信息在组中的下标.
    // | nGNewType | 是新的组类型.
    // 如果更新成功则返回true, 否则返回false.
    bool UpdateMessageGType(int nGType,
                            int nIndex,
                            int nGNewType);

    // 删除指定组中的某个下标对应的消息.
    // | nType | 是组类型, 为GroupType枚举值.
    // | nIndex | 是指定的下标.
    // 如果删除成功则返回true, 否则返回false.
    bool RemoveMessage(int nType, int nIndex);
    // 删除一组信息.
    // | nType | 是组类型, 为GroupType枚举值.
    // | listIndex | 是要删除的下标列表.
    // 如果删除成功则返回true, 否则返回false.
    bool RemoveMessageList(int nType, const IntList & listIndex);

    // 删除指定组中搜索信息.
    // | nType | 是组类型, 为GroupType枚举值.
    // 如果删除成功则返回true, 否则返回false.
    bool RemoveAllMessage(int nType);
    // 删除所有消息
    // check容错http://10.3.5.199/Bug.php?BugID=66612;网页发SMS邮件，话机重新加载去除Idle状态导致。容错方案：现只有接收网页消息进行此相关操作，网页更新时不清除Idle状态
    bool RemoveAllMessage(bool bCheck = true);

    // 插入一条信息到链表, 不同于AddMessage, 此函数的参数里信息
    // 更加详细.
    // | refData | 是要增加的文本数据.
    // 如果增加成功则返回信息在链表中的序号, 否则返回-1.
    int AddDetailMessage(const ItemData & refItem);

    // 获取所有未读信息数.
    // 如果获取成功则返回所有未读信息数, 否则返回0.
    int GetTotalUnRead();

    // 根据账号id获取账号当前的未读信息数.
    // | nAccountId | 是账号的用户名.
    // 如果获取成功则返回当前数量, 否则返回0.
    int GetAccountUnRead(int nAccountId);

    // 处理到时的计时器消息.
    // | refMessage | 是要处理的计时器消息.
    // 如果处理成功则返回true, 否则返回false.
    bool HandleTimeoutMessage(msgObject & refMessage);

    // 立刻保存文件
    bool SaveConfigFile();

    // 重新加载配置
    // check容错http://10.3.5.199/Bug.php?BugID=66612;网页发SMS邮件，话机重新加载去除Idle状态导致。容错方案：现只有接收网页消息进行此相关操作，网页更新时不清除Idle状态
    bool ReloadConfigFile(bool bCheck = true);

    // SMS是否可用
    bool IsSMSEnable();

private:
    // Ctor and Dtor.
    CTextMessageController();
    ~CTextMessageController();

    // Do initializations.
    // Returns true if initilized successfully, otherwise return false.
    void MyInit();
    // Do finalizations.
    // Returns true if finalizated successfully, otherwise return false.
    void MyFinal();

    // 处理TextMessage消息
    bool ProcessIntanceMessage(msgObject & objMessage);

    // 处理TextMessage未读消息
    bool ProcessTextMessageChange();

    // 处理TextMessage发送消息
    bool ProcessTextMessageSend(msgObject & objMessage);

    // 从配置文件中加载信息数据.
    // 如果加载成功则返回true, 否则返回false.
    bool MyLoadConfigFile();
    // 将信息保存到配置文件中.
    // 如果保存成功则返回true, 否则返回false.
    bool MySaveConfigFile();

    // 向链表中插入一条记录.
    // | refItem | 是要插入记录的数据.
    // 如果插入成功则返回记录在链表中的序号, 否则返回-1.
    int MyInsertItem(const ItemData & refItem);

    // 根据下标获取指定组中的信息数据.
    // | nGType | 是信息组类型.
    // | nIndex | 是信息在组中的下标.
    // 如果获取成功则返回信息数据, 否则返回NULL.
    ItemData * MyFindItem(int nGType, int nIndex);
    // 重载.
    const ItemData * MyFindItem(int nGType, int nIndex) const;

    // 根据信息组类型和状态获取图片(全路径).
    // | nGType | 是信息组类型.
    // | eSType | 是信息状态.
    // 如果获取成功则返回图片(全路径), 否则返回空.
    yl::string MyGetIconPath(int nGType, StatusType eSType) const;

    // 根据账号名获取账号显示名.
    // | strUserName | 为账号名.
    // 如果获取成功则返回账号显示名, 否则返回空.
    yl::string MyGetAccountName(const yl::string & strUserName) const;

    // 将ItemData转换为文本信息的简单数据.
    // | pData | [out] 为buffer.
    // | refItem | 为ItemData.
    // 如果转换成功则返回true, 否则返回false.
    bool MyID2TMSD(TextMsgItemSimpData * pData, const ItemData & refItem) const;

    // 将ItemData转换为信息详情.
    // | pData | [out] 为buffer.
    // | refItem | 为ItemData.
    // 如果转换成功则返回true, 否则返回false.
    bool MyID2TMIDD(TextMsgItemDetailData * pData, const ItemData & refItem) const;

    // 将发送的数据转换为ItemData.
    // | pData | [out] 为buffer.
    // | nGType | 是组类型.
    // | eSType | 是状态类型.
    // | refItem | 为TextMsgSendItemData.
    // 如果转换成功则返回true, 否则返回false.
    bool MyTMDID2ID(ItemData * pData,
                    int nGType,
                    StatusType eSType,
                    const TextMsgSendItemData & refItem) const;

    // 根据发送类型以及用户名和服务器名获取显示名.
    // | nType | 为发送类型.
    // | strUser | 为用户名.
    // | strServer | 为服务器.
    // | isSelf | 为true则计算自己方, 否则计算对方.
    // 返回显示名.
    yl::string MyGetDisplayName(const yl::string & strUser,
                                const yl::string & strServer,
                                bool isSelf) const;

    // 增加账号的信息数.
    // | strUserName | 是账号的用户名.
    // | nAdd | 是要增加的数量.
    // 如果增加成功则返回true, 否则返回false.
    bool MyAddUnReadToAccount(const yl::string & strUserName, int nAdd);

    // 根据账号名获取账号当前的未读信息数.
    // | strUserName | 是账号的用户名.
    // 如果获取成功则返回当前数量, 否则返回0.
    int MyGetAccountUnRead(const yl::string & strUserName);

    // 获取所有未读信息数.
    // 如果获取成功则返回所有未读信息数, 否则返回0.
    int MyGetTotalUnRead();

    // 检查未读信息数.
    void MyCheckUnRead();

    // 设置改动标记.
    void MySetDirty(bool isDirty);
    // 获取改动标记.
    bool MyGetDirty() const;

    // Deal with the recieved message.
    // | refObj | is the message instance.
    // Return LRESULT(TRUE OR FALSE).
    LRESULT OnReceivedMessage(msgObject & objMessage);

private:
    static CTextMessageController * m_pInst; // The singleton instance.
    TextMsgItemDetailData   m_stMsgDetail;
    int                     m_nMsgDetailIndex;
    ItemList m_listItem;  // 信息链表.
    UnReadMap m_mapUnRead;  // 未读信息map.
    bool m_isDirty;  // 是否有改动.
    bool m_bCheckFirst; //是否是第一次检测
};

#define g_pTextMessageController   CTextMessageController::GetInstance()

#endif  // SETTING_TEXTMESSAGECONTROLLER_H_
