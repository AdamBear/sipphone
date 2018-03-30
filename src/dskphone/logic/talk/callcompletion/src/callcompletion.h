#ifndef CALLCOMPLETION_CALLCOMPLETION_H_
#define CALLCOMPLETION_CALLCOMPLETION_H_

#include <yllist.h>
#include <ylstring.h>

#include <ETLLib.hpp>
#include <interfacedefine.h>
#include "singletonclassdefine.h"
#include "talk/talklogic/include/sipstructmapfunc.h"

struct CallCompletionData;
#if IF_BUG_23548
struct CompletionCallBackData;
#endif

namespace NS_CC
{
// Class CCallCompletion, 自动完成重拨功能, 它实际上是
// Call Completion的客户端, SIP服务器会监控并提示该客户端.
class CCallCompletion
{
    DECLARE_SINGLETON_CLASS(CCallCompletion)

public:
    // 订阅结构体.
    struct ItemData
    {
        enum { INVALID_CC_ID = -1, };

//         yl::string m_strCalleeDisplayName;  // 被拨打者的显示名.
//         yl::string m_strCalleeUsername;  // 被拨打者的用户名.
//         yl::string m_strCalleeServer;  // 被拨打者的服务器地址.

        BasicUserInfo   m_remoteInfo;   // 对方的信息
        BasicUserInfo m_localInfo;  // 本方的信息
        int m_nAccountId;  // 订阅账号id.
        int m_nCallId;  // 订阅时的Call Id, 用于从SIP服务器取消订阅.
        int m_nStatus;  // 状态.

        ItemData()
        {
            Reset();
        }

        VOID Reset()
        {
            m_remoteInfo.Reset();
            m_localInfo.Reset();
//             m_strCalleeDisplayName.clear();
//             m_strCalleeUsername.clear();
//             m_strCalleeServer.clear();
            m_nAccountId = 0;
            m_nCallId = 0;
            m_nStatus = -1;  // CCS_INVALID;
        }
    };

    // 订阅链表, 链表最多只有一个订阅.
    typedef YLList<ItemData *> ListItem;

    // SIP消息的处理函数.
    // | refMessage | 是待处理的消息.
    // 如果处理成功则返回TRUE, 否则返回FALSE.
    static LRESULT OnSipMessage(msgObject & refMessage);
    // 广播小的的处理函数.
    // | refMessage | 是待处理的消息.
    // 如果处理成功则返回TRUE, 否则返回FALSE.
    static LRESULT OnBCMessage(msgObject & refMessage);

    // 订阅. 订阅成功并不表示SIP订阅的成功, 而是代表
    // 以下两种步骤完成:
    //     1) 成功加入到请求列表.
    //     2) 向SIP发送订阅消息, 并处于请求订阅状态.
    // | nAccountId | 是账号id.
    // | nCallId | 是某次通话过程中的Call Id.
    // | szCalleeDisplayName | 是被拨打方的显示名.
    // | szCalleeUsername | 是被拨打方的用户名.
    // | szCalleeServer | 是被拨打方的服务器地址.
    // 如果订阅成功则返回TRUE, 否则返回FALSE.
    BOOL Subscibe(int nAccountId, int nCallId, const char * szCalleeDisplayName,
                  const char * szCalleeUsername, const char * szCalleeServer);

    // 取消订阅. 取消订阅成功并不表示从SIP取消的成功,
    // 而是代表以下步骤的完成:
    //     1) 向服务器发送取消信息.
    //     2) 成功将其从订阅列表中移除.
    // 该函数有重载形式.
    // | nCallId | 是要取消的订阅的id.
    // 如果取消成功则返回TRUE, 否则返回FALSE.
    BOOL Unsubscribe(int nCallId);
    // | nAccountId | 是账号id.
    // | szCalleeUsername | 是被拨打方的用户名.
    // | szCalleeServer | 是被拨打方的服务器地址.
    BOOL Unsubscribe(int nAccountId, const char * szCalleeUsername, const char * szCalleeServer);
    // 取消所有订阅.
    BOOL UnsubscribeAll();
    // 取消某个Account Id的所有订阅.
    // | nAccountId | 是账号id.
    BOOL UnsubscribeAllByAccountId(int nAccountId);

    // 获取订阅信息.
    // | pData | [out] 是buffer.
    // 如果获取成功则返回TRUE, 否则返回FALSE.
    BOOL GetSubscribeData(CallCompletionData * pData);

    // 获取订阅的状态.
    // | nAccountId | 是账号id.
    // | szCalleeUsername | 是被拨打方的用户名.
    // | szCalleeServer | 是被拨打方的服务器地址.
    // 如果获取到则返回订阅状态, 否则返回CCS_INVALID.
    int GetSubscribeStatus(int nAccountId, const char * szCalleeUsername, const char * szCalleeServer);
    // 获取状态. 因为目前只支持一个订阅, 所以不需要指定参数.
    int GetSubscribeStatus();
    // 更新订阅的状态.
    // | nCallId | 是要更新状态订阅的Call Id.
    // | nNewStatus | 是新的状态, 为CallCompletionStatus枚举.
    // 如果更新成功则返回TRUE, 否则返回FALSE.
    BOOL UpdateSubscribeStatus(int nCallId, int nNewStatus);
    // | nAccountId | 是账号id.
    // | szCalleeUsername | 是被拨打方的用户名.
    // | szCalleeServer | 是被拨打方的服务器地址.
    // | nNewStatus | 是新的状态, 为CallCompletionStatus枚举.
    BOOL UpdateSubscribeStatus(int nAccountId, const char * szCalleeUsername,
                               const char * szCalleeServer, int nNewStatus);
    // | nNewStatus | 是新的状态, 为CallCompletionStatus枚举.
    BOOL UpdateSubscribeStatus(int nNewStatus);

    // 拨出某个订阅.
    // Note: 同通话的拨出逻辑不同, 在拨出之前我们会取消订阅,
    //     因此, 请不要直接使用通话的拨出逻辑.
    // | nCallId | 是要拨出的订阅.
    // 如果拨出成功则返回TRUE, 否则返回FALSE.
    BOOL CallOut(int nCallId);

    // 进入等待订阅.
    // | nCallId | 是要等待的订阅.
    // 如果等待成功则返回TRUE, 否则返回FALSE.
    BOOL Wait(int nCallId);

    // 判断订阅是否存在.
    // | nAccountId | 是账号id.
    // | szCalleeUsername | 是被拨打方的用户名.
    // | szCalleeServer | 是被拨打方的服务器地址.
    // 如果已经存在返回TRUE, 否则返回FALSE.
    // 根据订阅信息查找某个订阅.
    ItemData * MyFind(int nAccountId, const char * szUsername, const char * szServer,
                      BOOL bLocalInfo = FALSE, BOOL isRemoveFromList = FALSE);


    static bool FlushIdleStatus();

#if IF_BUG_23548
    bool GetCallBackData(CompletionCallBackData & completionCallBackData);
    bool SetCallBackData(int nAccountId, bool bPopMsgBox);
#endif

private:
    // 初始化.
    BOOL MyInit();
    // 清除.
    BOOL MyFinal();

    // 订阅.
    BOOL MySubscibe(int nAccountId, int nCallId, const char * szCalleeDisplayName,
                    const char * szCalleeUsername, const char * szCalleeServer);
    // 取消订阅.
    BOOL MyUnsubscribe(int nAccountId, int nCallId, const char * szCalleeDisplayName,
                       const char * szCalleeUsername, const char * szCalleeServer);
    // 根据Call Id查找某个订阅.
    ItemData * MyFind(int nCallId, BOOL isRemoveFromList = FALSE);

    // 弹出提示框
    BOOL PopupNoteOnIdle();

private:

    ListItem m_listItem;  // 订阅列表.

    CCallCompletion(const CCallCompletion &);
    VOID operator= (const CCallCompletion &);

    static BOOL             m_bEnableCallCompletion;  // callcompletion是否被开启配置
    CallCompletionData   *  m_pWaitResponseCCData;    // 等待UI确认的数据
#if IF_BUG_23548
    CompletionCallBackData * m_pCompletionCallBackData;
#endif
};

}  // namespace NS_CC

#define _CallCompletion (NS_CC::GET_SINGLETON_INSTANCE(CCallCompletion))

#endif  // CALLCOMPLETION_CALLCOMPLETION_H_
