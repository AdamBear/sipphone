#ifndef _ACD_MODEL_START2START_
#define _ACD_MODEL_START2START_

#if IF_FEATURE_START2START_ACD
#include "ylstl/ylvector.h"
#include <ETLLib.hpp>
#include "acdmodelbase.h"
#include "interfacedefine.h"
#include "broadsoft/callcenter/include/cccommon.h"
#include "broadsoft/callcenter/include/callcenter_data_define.h"

typedef YLList<yl::string> vecCallParam;

class CACDModelStart2Start : public CACDModelBase
{
public:
    enum ACTION_TYPE
    {
        ACTION_LOGIN       = 0, // 通过呼出和DTMF来登陆
        ACTION_SYNC        = 1, // 同步最新的状态
        ACTION_SET_AVAIL   = 2, // Avail
        ACTION_SET_UNAVAIL = 3, // UnAvail
        ACTION_LOGOUT      = 4, // Logout, 流程和Login一致
    };

public:
    CACDModelStart2Start(void);
    ~CACDModelStart2Start(void);

    // 登录ACD
    virtual bool ReqLogin(acd_accout * pAcd = NULL);
    //请求状态改变，包括登陆，登陆时需考虑考虑初始化状态
    virtual bool ReqChangeACDStatus(ACD_ACTION eACDAction);

    // 获取下一个参数，只能获取一次，返回副本，原参数被清除
    yl::string GetNextCallOutParam();

    virtual bool IsUnavailReasonCodeOn();

    virtual void SetStatus(ACD_STATUS eStatus);

    // 设置自动刷新时间
    bool ReSetTimer(unsigned long iTimer);
    // 处理计时器到时消息.
    BOOL OnTimeoutMessage(msgObject & refObj);

    void SetCurReason(const yl::string & strReason)
    {
        m_strCurReason = strReason;
    }

    void GetCurReason(yl::string & strReasonName)
    {
        strReasonName = m_strCurReason;
    }

private:
    // 呼出参数解析
    void ParseCallOutParam(const yl::string & strSrc, const yl::string & filterStr);
    // 初始化呼出参数
    bool InitCalloutParam();
    // 所有Action处理
    bool OnAction(ACTION_TYPE eType);
    // 状态改变Action集合
    bool ProcessAction(ACTION_TYPE eType);
    // 获取URL
    yl::string GetURlByType(ACTION_TYPE eType);

private:

    // 呼出的参数
    vecCallParam        m_CallOutParam;
    // 与Reason有关的参数
    yl::string          m_strCurReason;
    // 自动刷新计时器.
    mkThreadTimer       m_objTimer;
    // 定时器时间
    unsigned long       m_iTimer;
};


#endif // IF_FEATURE_START2START_ACD

#endif // _ACD_MODEL_START2START_

