#ifndef __MOD_DIANOSTICS_H__
#define __MOD_DIANOSTICS_H__

#include <ylstl/ylstring.h>
#include "innermsgdefine.h"

enum DIAGNOSTICSTYPE
{
    DIAGNOSTICSTYPE_NONE,
    DIAGNOSTICSTYPE_PING,
    DIAGNOSTICSTYPE_TRACEROUTE
};

enum DIAGNOSTICSMSG
{
    DIAGNOSTICSMSG_NONE = INNER_MSG_DIAGNOTICS_BEGIN,
    DIAGNOISTICMSG_FINISH,
    DIAGNOSTICSMSG_END = INNER_MSG_DIAGNOTICS_END,
};

//执行系统 ping和traceroute命令, 返回执行结果
bool Diagnostics_RunSysCmd(DIAGNOSTICSTYPE eCmd, yl::string & strTargetURL);

//取消执行命令
void Diagnostics_StopSysCmd();

//获取执行结果
yl::string Diagnostics_GetDiagnositcsResult();

yl::string Diagnostics_GetDiagnositcsUrl();

bool Diagnostics_IsSystemCmdRunning();

#endif
