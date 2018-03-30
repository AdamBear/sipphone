#ifndef SETTING_TEXTMESSAGECONTROLLER_H_
#define SETTING_TEXTMESSAGECONTROLLER_H_

#include <list>
#include <ylstring.h>

#include "ETLLib.hpp"

struct AccountTextMsgListData;
struct TextMsgItemDetailData;
struct TextMsgSendItemData;
struct TextMsgSimpListData;

namespace NS_SETTING
{
class CTextMessageModel;

// Class CTextMessageController.
class CTextMessageController
{
public:
    // Get the singleton instance.
    static CTextMessageController * GetInstance();
    // Release the singleton instance.
    static void ReleaseInstance();

    // Message handle method.
    static LRESULT OnMessage(msgObject & refObj);

    // Message Time handle method.
    static LRESULT OnTimeMessage(msgObject & refObj);

    // 获取所有账号当前有的未读信息列表.
    // | pData | [out] 是buffer.
    // 如果获取成功则返回true, 否则返回false.
    bool GetAccountMsgList(AccountTextMsgListData * pData) const;

    // 获取第一个未读信息的下标.
    // 如果获取成功则返回取得的下标, 否则返回-1.
    int GetFirstUnReadInMessageIndex() const;

    // Get message general list by click action.
    // | pData | [out] is the buffer.
    // | strClickAction | is the click action.
    // Return true if retrieved successfully, otherwise return false.
    bool GetMessageList(TextMsgSimpListData * pData,
                        const yl::string & strClickAction) const;

    // Get message detail info by click action and index.
    // | pData | [out] is the buffer.
    // | strClickAction | is the click action.
    // | nIndex | is the index the message in the group.
    // Return true if Retrieved successfully, otherwise return false.
    bool GetMessageDetail(TextMsgItemDetailData * pData,
                          const yl::string & strClickAction, int nIndex) const;

    // Add a draft message.
    // | refData | is the message data to add.
    // Return the index the new added message is at in the list, otherwise
    // return -1.
    int AddDraftMessage(const TextMsgSendItemData & refData);

    // Send a message.
    // | refData | is the message data to send.
    // Return true if sent successfully, otherwise return false.
    bool SendTheMessage(const TextMsgSendItemData & refData);

    // Remove the message at some index in the list.
    // | strClickAction | is the click action.
    // | nIndex | is the index message at in the list.
    // Return true if removed successfully, otherwise return false.
    bool RemoveMessage(const yl::string & strClickAction, int nIndex);
    // 删除一组信息.
    // | strClickAction | 是click action.
    // | listIndex | 是要删除的下标列表.
    // 如果删除成功则返回true, 否则返回false.
    bool RemoveMessageList(const yl::string & strClickAction,
                           const std::list<int> & listIndex);

    // Remove all the message in the list.
    // | strClickAction | is the click action.
    // Return true if removed successfully, otherwise return false.
    bool RemoveAllMessage(const yl::string & strClickAction);

    // 获取所有未读信息数.
    // 如果获取成功则返回所有未读信息数, 否则返回0.
    int GetTotalUnRead() const;

    // 根据账号id获取账号当前的未读信息数.
    // | nAccountId | 是账号的用户名.
    // 如果获取成功则返回当前数量, 否则返回0.
    int GetAccountUnRead(int nAccountId) const;

private:
    // Ctor and Dtor.
    CTextMessageController();
    ~CTextMessageController();

    // Do initializations.
    // Returns true if initilized successfully, otherwise return false.
    bool MyInit();
    // Do finalizations.
    // Returns true if finalizated successfully, otherwise return false.
    bool MyFinal();

    // Convert click action to message group type.
    // | strClickAction | is the click action.
    // Return the converted group type.
    int MyClickAction2GroupType(const yl::string & strClickAction) const;

    // Deal with the recieved message.
    // | refObj | is the message instance.
    // Return LRESULT(TRUE OR FALSE).
    LRESULT OnReceivedMessage(msgObject & objMessage);

    // 处理消息发送返回的结果.
    // | refObj | 是消息实例.
    // 返回 LRESULT(TRUE 或者 FALSE).
    LRESULT OnMessageSendResult(msgObject & objMessage);

    // 处理计时器消息.
    // | refObj | 是消息实例.
    // 返回 LRESULT(TRUE 或者 FALSE).
    LRESULT OnMessageTimer(msgObject & objMessage);

private:
    static CTextMessageController * m_pInst; // The singleton instance.
    CTextMessageModel * m_pModel; // The model.
};

}  // namespace NS_SETTING

#define g_pTextMessageController  \
    (NS_SETTING::CTextMessageController::GetInstance())

#endif  // SETTING_TEXTMESSAGECONTROLLER_H_
