
#include "appprofiler.h"

#if defined(PHONE_APP_PROFILER)
#include <profiler.h>
#include "define/toolmsgdefine.h"
#include "etlmsghandler/modetlmsghandler.h"

namespace
{
#define APP_PROF_FILE TEMP_PATH "log/dskPhone.prof"
}

CAppProfiler::CAppProfiler()
{

}

CAppProfiler::~CAppProfiler()
{

}

void CAppProfiler::Start(const char * pszFile)
{
    if (NULL == pszFile)
    {
        return;
    }

    ProfilerStart(pszFile);
}

void CAppProfiler::Stop()
{
    ProfilerStop();
}

void CAppProfiler::Flush()
{
    ProfilerFlush();
}

LRESULT CAppProfiler::OnMessage(msgObject & refMessage)
{
    switch (refMessage.message)
    {
    case TOOL_MSG_CPU_PROFILE_START:
        {
            Start(APP_PROF_FILE);
        }
        break;
    case TOOL_MSG_CPU_PROFILE_STOP:
        {
            Stop();
        }
        break;
    case TOOL_MSG_CPU_PROFILE_FLUSH:
        {
            Flush();
        }
        break;
    default:
        break;
    }
    return TRUE;
}

void CAppProfiler::RegisterFlushMsg()
{
    EtlMsgHandler_RegisterMsgHandle(TOOL_MSG_CPU_PROFILE_BEGIN, TOOL_MSG_CPU_PROFILE_END,
                                    &CAppProfiler::OnMessage);
}

void CAppProfiler::UnRegisterFlushMsg()
{
    EtlMsgHandler_UnregisterMsgHandle(TOOL_MSG_CPU_PROFILE_BEGIN, TOOL_MSG_CPU_PROFILE_END,
                                      &CAppProfiler::OnMessage);
}

#endif // defined(PHONE_APP_PROFILER)
