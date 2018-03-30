#ifndef CALLCOMPLETION_MODCALLCOMPLETION_H_
#define CALLCOMPLETION_MODCALLCOMPLETION_H_

// CallCompletion接口文件.
#include "callcompletionmsgdefine.h"
#include <ylstring.h>

// 订阅状态.
enum CallCompletionStatus
{
    CCS_INVALID = -1,
    CCS_SUBSCRIBING = 0,  // 订阅中.
    CCS_SUBSCRIBE_SUCCESS = 1,  // 订阅成功.
    CCS_WAIT_COMFIMING = 2,  // 等待确认.
    CCS_WAIT_REMOTE_IDLE = 3,  // 等待被拨打方空闲.
    CCS_REMOTE_CALLAVALIABLE = 4,  // 被拨打方处于空闲.
    CCS_SUBSCRIBE_FAILED = 5  // 订阅失败.
};

// CallCompletion数据结构
struct CallCompletionData
{
    yl::string m_strCalleeDisplayName;  // 被拨打者的显示名.
    yl::string m_strCalleeUsername;  // 被拨打者的用户名.
    yl::string m_strCalleeServer;  // 被拨打者的服务器地址.
    int m_nCallId;  // 订阅Call Id, 唯一标识一次订阅.
    CallCompletionStatus m_eStatus;  // 状态.

    CallCompletionData()
    {
        Reset();
    }

    VOID Reset()
    {
        m_strCalleeDisplayName = "";
        m_strCalleeUsername = "";
        m_strCalleeServer = "";
        m_nCallId = 0;
        m_eStatus = CCS_INVALID;
    }
};

#if IF_BUG_23548
struct CompletionCallBackData
{
    int nAccountId;
    bool bPopMsgBox;

    CompletionCallBackData()
    {
        nAccountId = -1;
        bPopMsgBox = false;
    }
};
#endif

/*******************************************************************
** 函数描述:   初始化CallCompletion功能
** 参数：      无
** 返回:       TRUE:成功, FALSE:失败
********************************************************************/
BOOL CallCompletion_Init();

/*******************************************************************
** 函数描述:   获取当前订阅信息
** 参数：      [out] pData: 获取到的订阅信息
** 返回:       TRUE:数据获取成功, FALSE:数据获取失败
********************************************************************/
BOOL CallCompletion_GetSubscribeData(CallCompletionData * pData);

/*******************************************************************
** 函数描述:   订阅CallCompletion
**             订阅. 订阅成功并不表示SIP订阅的成功, 而是代表
**             以下两种步骤完成:
**             1) 成功加入到请求列表.
**             2) 向SIP发送订阅消息, 并处于请求订阅状态.
** 参数：[in] nAccountId: 账号id.
**       [in] nCallId: 某次通话过程中的Call Id.
**       [in] szCalleeDisplayName: 被拨打方的显示名.
**       [in] szCalleeUsername: 被拨打方的用户名.
**       [in] szCalleeServer: 被拨打方的服务器地址.
** 返回:       TRUE:订阅操作成功, FALSE:订阅操作失败
********************************************************************/
BOOL CallCompletion_Subscibe(int nAccountId, int nCallId, const char * szCalleeDisplayName,
                             const char * szCalleeUsername, const char * szCalleeServer);

/*******************************************************************
** 函数描述:   取消订阅. 取消订阅成功并不表示从SIP取消的成功,
**             而是代表以下步骤的完成:
**             1) 向服务器发送取消信息.
**             2) 成功将其从订阅列表中移除.
** 参数：      [in] nCallId: 要取消的订阅的Call id
** 返回:       TRUE:取消操作成功, FALSE:取消操作失败
********************************************************************/
BOOL CallCompletion_UnsubscribeByCallId(int nCallId);

/*******************************************************************
** 函数描述:   取消订阅. 取消订阅成功并不表示从SIP取消的成功,
**             而是代表以下步骤的完成:
**             1) 向服务器发送取消信息.
**             2) 成功将其从订阅列表中移除.
** 参数：      [in] nAccountId: 是账号id.
**             [in] szCalleeDisplayName: 是被拨打方的显示名.
**             [in] szCalleeUsername: 是被拨打方的用户名.
**             [in] szCalleeServer: 是被拨打方的服务器地址.
** 返回:       TRUE:取消操作成功, FALSE:取消操作失败
********************************************************************/
BOOL CallCompletion_UnsubscribeByInfo(int nAccountId, const char * szCalleeUsername,
                                      const char * szCalleeServer);

/*******************************************************************
** 函数描述:   取消所有的订阅
** 参数：      无
** 返回:       TRUE:取消操作成功, FALSE:取消操作失败
********************************************************************/
BOOL CallCompletion_UnsubscribeAll();

/*******************************************************************
** 函数描述:   呼出订阅的目标
** 参数：      订阅标识Call Id
** 返回:       TRUE:拨出成功, FALSE:拨出失败
********************************************************************/
BOOL CallCompletion_CallOut(int nCallId);

/*******************************************************************
** 函数描述:   进入等待订阅
** 参数：      [in] nCallId: 是要等待的订阅.
** 返回:       TRUE:等待成功, FALSE:待失败
********************************************************************/
BOOL CallCompletion_Wait(int nCallId);

#if IF_BUG_23548
// 设置CallBack数据
bool CallCompletion_SetCallBackData(int nAccountId, bool bPopMsgBox);

// 获取CallBack数据
bool CallCompletion_GetCallBackData(CompletionCallBackData & completionCallBackData);
#endif

#endif  // CALLCOMPLETION_MODCALLCOMPLETION_H_
