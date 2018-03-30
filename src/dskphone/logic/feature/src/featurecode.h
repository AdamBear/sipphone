#ifndef __FEATURE_H__
#define __FEATURE_H__

#include <singletonclassdefine.h>
#include <yllist.h>
#include <ylstring.h>
#include <interfacedefine.h>
#include "../include/featurecommon.h"
#include "talk/fwd_dnd/include/fwddndtypes.h"

typedef YLList<CodeSendFlag *> ListCodeSendFlag;

// 发送特征码开启的通话信息
struct Code_CallInfo
{
    int     iLineId;    // 帐号Id
    UINT    uCallId;    // 通话Id
};

typedef YLList<Code_CallInfo *> ListSendCodeCallID;

class CFeatureCode
{
    DECLARE_SINGLETON_CLASS(CFeatureCode)

public:
    // 发送开启特征码
    BOOL SendOnCode(int iLineId, CCodeInfo & refCode);
    // 发送关闭的特征码
    BOOL SendOffCode(int iLineId, CCodeInfo & refCode);

    // SIP消息的处理
    BOOL ProcessSIPMsg(msgObject & objMessage);
    // 定时器的响应
    BOOL OnTimer(UINT uTimerID) DECLARE_ATTR;
    // DND状态改变消息处理
    BOOL ProcessDNDMsg(int iAccountId, bool bEnable);
    // FWD状态改变消息处理
    BOOL ProcessFWDMsg(int iAccountId, FWD_TYPE eType, bool bEnable, yl::string strTarget);

    // 发送Code到指定账号的服务器, FWD中有用
    BOOL SendCodeToServer(int iLineId, const yl::string & strCode);

    // 跳过上电后的首次Code发送. 首次账号注册上时要避免发送
    // http://192.168.1.99/Bug.php?BugID=22736
    // | iType | [in] 某Code类型, 定义在CodeSendFlag里
    // | iLineId | [in] 账号iD，-1为所有账号
    bool IsSkipCodeSend(int iType, int iLineId);

private:
    BOOL SetCodeCallTimer(msgObject & objMessage);
    void AddInitSendType(int iType);

private:
    ListSendCodeCallID  m_pListCodeCallID;
    ListCodeSendFlag    m_listInitSendFlag;
    UINT                m_uInitTimer;
};

// 获取实例
#define _FeatureCode GET_SINGLETON_INSTANCE(CFeatureCode)

#endif

