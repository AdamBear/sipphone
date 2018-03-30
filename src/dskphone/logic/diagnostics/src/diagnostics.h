#ifndef _DIAGNOSTICS_H_
#define _DIAGNOSTICS_H_

#include <ylstl/ylstring.h>
#include "../include/moddiagnostics.h"
#include "etlmsghandler/modetlmsghandler.h"

class CDiagnostics
{
public:
    CDiagnostics();
    ~CDiagnostics();

    static CDiagnostics * GetInstance() DECLARE_ATTR;
    void StopSystemCmd();
    bool RunSystemCmd(DIAGNOSTICSTYPE eCmd, yl::string & strTargetURL);
    yl::string GetRunCmdResult() const;

    yl::string GetRunUrl() const;

    void NotifyResult(DIAGNOSTICSTYPE eCmd);
    bool IsSystemCmdRunning();
private:
    void Init();
    bool CreatFifo(yl::string & FifoPath);
    bool ReadFifo();
    void ReplceStartToPoint(yl::string & strTargetURL);
    //定时器处理
    void OnTimer(msgObject & refObj);
    //消息处理
    static LRESULT OnMessage(msgObject & refObj);
private:
    yl::string m_strResult;
    int m_ififo;
    int m_iWtiteFifo;

    yl::string m_strUrl;    // 运行的url

    DIAGNOSTICSTYPE m_eOperateType;
    int m_iReadTimer;
    //int m_timeout;
};

#define g_pDiagnostics (CDiagnostics::GetInstance())

#endif



