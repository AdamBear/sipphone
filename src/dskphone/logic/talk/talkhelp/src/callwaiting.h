#ifndef __CALL_WAITING__H_
#define __CALL_WAITING__H_

#include "feature/include/modfeature.h"
#include "singletonclassdefine.h"

class CCallWaiting
{
    DECLARE_SINGLETON_CLASS(CCallWaiting)

public:
    //更新CallWaiting状态和Code值,并根据状态发送Code
    void UpdateAndSendCode();

    //使用指定的账号发送CallWaiting Code,默认-1则使用当前账号
    void SendCallWaitingCode(int iLineId = -1);

    bool IsEnableCallWait();

private:
    //是否开启CallWait功能
    bool        m_bEnableCallwait;
    //保存CallWait Code
    CCodeInfo   m_objCallWaitCodeInfo;
};

#define _CallWaiting GET_SINGLETON_INSTANCE(CCallWaiting)
#endif
