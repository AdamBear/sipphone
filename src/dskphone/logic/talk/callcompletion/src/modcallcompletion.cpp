#include "callcompletion.h"
#include "modcallcompletion.h"

// 初始化CallCompletion功能
BOOL CallCompletion_Init()
{
    _CallCompletion;
    return TRUE;
}

// 获取当前订阅信息
BOOL CallCompletion_GetSubscribeData(CallCompletionData * pData)
{
    return _CallCompletion.GetSubscribeData(pData);
}

// 订阅CallCompletion
BOOL CallCompletion_Subscibe(int nAccountId, int nCallId, const char * szCalleeDisplayName,
                             const char * szCalleeUsername, const char * szCalleeServer)
{
    return _CallCompletion.Subscibe(nAccountId, nCallId,
                                    szCalleeDisplayName, szCalleeUsername, szCalleeServer);
}

//  取消订阅
BOOL CallCompletion_UnsubscribeByCallId(int nCallId)
{
    return _CallCompletion.Unsubscribe(nCallId);
}

//  取消订阅
BOOL CallCompletion_UnsubscribeByInfo(int nAccountId, const char * szCalleeUsername,
                                      const char * szCalleeServer)
{
    return _CallCompletion.Unsubscribe(nAccountId, szCalleeUsername, szCalleeServer);
}

// 取消所有的定阅
BOOL CallCompletion_UnsubscribeAll()
{
    return _CallCompletion.UnsubscribeAll();
}

// 呼出订阅的目标
BOOL CallCompletion_CallOut(int nCallId)
{
    return _CallCompletion.CallOut(nCallId);
}

// 进入等待订阅
BOOL CallCompletion_Wait(int nCallId)
{
    return _CallCompletion.Wait(nCallId);
}

#if IF_BUG_23548
bool CallCompletion_SetCallBackData(int nAccountId, bool bPopMsgBox)
{
    return _CallCompletion.SetCallBackData(nAccountId, bPopMsgBox);
}

bool CallCompletion_GetCallBackData(CompletionCallBackData & completionCallBackData)
{
    return _CallCompletion.GetCallBackData(completionCallBackData);
}
#endif


