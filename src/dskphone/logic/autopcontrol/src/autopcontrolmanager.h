#ifndef _AUTOPCONTROLMANAGER__H
#define _AUTOPCONTROLMANAGER__H
#include "taskaction/taskactioncallbackbase.h"
#include "threadtask/include/networkfileaction.h"
#include "ylstl/ylstring.h"
#include "../include/autop_common.h"
#include "ETLLib/ETLLib.hpp"
#include "baseautopprocessor.h"

using namespace NS_TA;

class CAutopControlData;

class CAutopControlManager : public CTaskActionCallbackBase
{
public:
    static CAutopControlManager & GetInstance()
    {
        static CAutopControlManager sInstance;
        return sInstance;
    }

    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

    void Exit();

    void Init();

    bool Start(AUTOP_UPDATE_TYPE eType);

    bool ProcessStartConnect(AUTOP_UPDATE_TYPE eType, const yl::string & strUserName,
                             const yl::string & strPasswd);

    bool IsAutopCodeUsable();

    bool WaitingForNetwork();

    AUTOP_UPDATE_TYPE GetUpdateType();

    //这个函数放到这里处理，主要为了让taskaction统一处理
    bool ConnectServer(const yl::string & strUrl, AUTOP_UPDATE_TYPE eType = AUT_NONE);

    bool IsNeedShowUpdatingWnd();

    LRESULT ProcessRedirectMsg(msgObject & objMsg);

    CAutopControlData & GetAutopDataInst()
    {
        return m_autopData;
    }

    inline void SetAutopStatus(int iStatus)
    {
        m_iAutopStatus = iStatus;
    }

    inline int  GetAutopStatus()
    {
        return m_iAutopStatus;
    }

    inline void SetUserName(const yl::string & strName)
    {
        m_strUserName = strName;
    }

    inline void SetPassword(const yl::string & strPasswd)
    {
        m_strPasswd = strPasswd;
    }

    inline yl::string GetUserName()
    {
        return m_strUserName;
    }

    inline yl::string GetPasswd()
    {
        return m_strPasswd;
    }

    inline bool IsAutopNow()
    {
        return m_bIsAutopNow;
    }

    inline void SetAutopNow(bool bAutopNow)
    {
        m_bIsAutopNow = bAutopNow;
    }

    inline bool IsStartByZero()
    {
        return m_bIsStartByZero;
    }

    inline void SetStartByZero(bool bStartByZero)
    {
        m_bIsStartByZero = bStartByZero;
    }

protected:

    POWER_ON_STATUS CheckAutopStatus();

    void ProcessAutopStartup();

    bool ReadyToConnect();

    bool OnTimer(unsigned int uTimerID);

    bool IsReadToConnect();

    CBaseAutopProcessor * CreateProcessor(AUTOP_UPDATE_TYPE eType);

    CAutopControlManager();
    ~CAutopControlManager();

private:

    // autop 状态
    int m_iAutopStatus;

    bool m_bIsStartByZero;
    bool m_bIsAutopNow;

    yl::string m_strUserName;
    yl::string m_strPasswd;

    CNetworkFileAction * m_pAction;

    CBaseAutopProcessor * m_pProcessor;

    static CAutopControlData m_autopData;
};

#define _AutopControlMamager CAutopControlManager::GetInstance()
#define _AutopControlData  _AutopControlMamager.GetAutopDataInst()

//数据类
class CAutopControlData
{
public:
    CAutopControlData();

    ~CAutopControlData();
    void Init();
    // strNum是否是某个Code的前缀
    // 返回 0->部分匹配 1->全部匹配 -1->不匹配
    int MatchAutoPCodePrefix(const yl::string & strNum, char cKey = 0);

    // 获取当前的code param
    inline const LPAUTOP_PARAM GetMatchCodeInfo()
    {
        return m_lpPressing;
    }
protected:
    // 解析用户名和密码
    bool ParseUserName(const yl::string & strUrl, LPAUTOP_PARAM lpPressing);
    void ReleaseAutopCode();

private:
    // 所有配置信息
    YLList<LPAUTOP_PARAM> m_listAutopParam;
    LPAUTOP_PARAM       m_lpPressing;
};

#endif //_AUTOPCONTROLMANAGER__H