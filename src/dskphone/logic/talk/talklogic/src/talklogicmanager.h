#ifndef __TALKLOGIC_MANAGER_H__
#define __TALKLOGIC_MANAGER_H__

class CCallInfo;
class CSingleSession;

#include <singletonclassdefine.h>
#include "../include/sipstructmapfunc.h"

// Talklogic的管理类(singleton)
class CTalklogicManager
{
    DECLARE_SINGLETON_CLASS(CTalklogicManager)
public:
    // 初始化
    void Init();
    // 释放
    void Uninit();
    // 处理SIP消息
    bool OnSIPMsg(msgObject& msg);

#ifdef IF_FEATURE_PSTN
    // 响应PSTN来电
    bool ProcessPstnIncomingCall(const msgObject& msg, const yl::string& strFromNumber = yl::string());
#endif // IF_FEATURE_PSTN

    // 进入拨号界面
    CSingleSession* EnterDialUI(const yl::string& strNumber, int iAccountId = AUTO_SELECT_LINE_ID,
                                TALK_CALLOUT_TYPE eCallOutType = TCT_NORMAL, ROUTINE_STATE eRoutie = ROUTINE_DIALING);

    // 响应来电
    int ProcessSipIncomingCall(msgObject& msg);

    // 账号是否允许呼出
    bool IsAccountEnableToCallout(int iAccount);

    //////////////////////end:For TalkUI///////////////////////////////
    // 进入拨号呼出界面
    TALK_CALLOUT_RESULT EnterCallOut(const yl::string& strNumber, const yl::string& strName,
                                     int iAccountId = AUTO_SELECT_LINE_ID, bool bConferenceDirect = false,
                                     CallOut_Param* pCallParam = NULL, int nCallId = -1, bool bNeedPrefix = false,
                                     bool bForceToCallOut = false, bool bConfOnePress = false);
    // 呼出处理
    TALK_CALLOUT_RESULT ProcessCallOut(CSingleSession* pSingleSession, LPCSTR lpszNumber, const yl::string& strName);
    // 获取可用账号
    int GetUsableAccount(int iAccountId, const yl::string& strServer = "");

    // http://192.168.1.99/Bug.php?BugID=18570
    bool GetCallInIntercom()
    {
        return m_bCallInIntercom;
    }
    void SetCallInIntercom(bool bCallInIntercom)
    {
        m_bCallInIntercom = bCallInIntercom;
    }

    // 尝试进入新的Routine，否则进入Finished Routine，返回值代表是否进入目标Routine
    bool TryEnterRoutine(CSingleSession* pSession, ROUTINE_STATE eNewState,
                         LPCSTR lpNum = NULL);

    // 尝试进入新的Routine，否则进入Finished Routine，返回值代表是否进入目标Routine
    bool TryEnterPreRoutine(CSingleSession* pSession, ROUTINE_STATE eNewState,
                            const yl::string& strNum);

    // 尝试进入Finished Routine，返回值代表是否进入
    bool TryEnterFinishedRoutine(CSingleSession* pSession, TalkErrorDefine eError = TALK_ERROR_NONE);

    // 呼出失败的处理
    bool ProcessCallFailed(CBaseRoutine* pRoutine, msgObject& objMsg,
                           bool bCallFail = true, bool bPlayBusyTone = true, bool bEndCall = true);

    // 是否允许新通话
    bool IsAllowNewCall(int iAccountID);

private:

    // 处理来电
    int ProcessIncomingCall(int iAccountID, int iCallID, CallInUserInfo& userInfo);

private:
    // 是否是Intercom来电(bugid:18570)
    bool        m_bCallInIntercom;
};



// 获取全局唯一的对象
#define _TalklogicManager GET_SINGLETON_INSTANCE(CTalklogicManager)

#endif // __TALKLOGIC_MANAGER_H__
