#ifndef __SCA_LISTENER_H__
#define __SCA_LISTENER_H__

#include "basetalklistener.h"

class CSCAListener : public CBaseTalkListener
{
public:

    CSCAListener();

    /* ETLmsg消息回调函数 */
    virtual bool OnMessage(msgObject& msg);

    bool OnBLASeizeResult(CCallInfo* pCallInfo, CListenInfo& data, msgObject& msg, int iResult);

    bool OnSCASeizeResult(CCallInfo* pCallInfo, CListenInfo& data, msgObject& msg, int iResult);

    bool OnSCASeize2Result(CCallInfo* pCallInfo, CListenInfo& data, msgObject& msg, int iResult);

    /* 通过账号查找 */
    struct FindByAccount
        : public CBaseInfoTraver
    {
        FindByAccount(int i) : iAccountID(i)
        {}

        virtual bool operator()(CCallInfo* p);

        int iAccountID;
    };

    /* 通过账号查找 */
    struct SCATraver
        : public CBaseInfoTraver
    {
        SCATraver(msgObject& m, CSCAListener& l) : msg(m), lsn(l)
        {}

        virtual bool operator()(CCallInfo* p);

        msgObject&  msg;
        CSCAListener&                   lsn;
    };

private:

    DECL_SIGNAL_HANDLER(OnRoutineChanged);

    DECL_SIGNAL_HANDLER(OnAccountChange);

    DECL_SIGNAL_HANDLER(BeforSessionDestory);

    DECL_SIGNAL_HANDLER(OnGetDataToUI);

    DECL_SIGNAL_HANDLER(OnCallOut);

    DECL_SIGNAL_HANDLER(BeforeCallout);

protected:
    // 检查是否合法的BLA线路
    bool CheckBLA(CCallInfo* pCallInfo);

    // BLA占线失败处理
    bool OnBLASeizeFinished(CCallInfo* pCallInfo, bool bFailed);

    bool ProcessSeizeLineResult(CCallInfo* pCallInfo, int iResult);

    bool SeizeLine(CCallInfo* pCallInfo, bool bSeize = true, int iAccountID = AUTO_SELECT_LINE_ID);

    bool ReStartTalk(CCallInfo* pCallInfo);
};

#endif //__SCA_LISTENER_H__
