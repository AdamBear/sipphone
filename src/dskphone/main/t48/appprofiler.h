
#ifndef __APP_PROFILER_H__
#define __APP_PROFILER_H__

#include <ETLLib.hpp>
#if defined(PHONE_APP_PROFILER)

class CAppProfiler
{
public:
    CAppProfiler();
    ~CAppProfiler();

public:
    static void Start(const char * pszFile);
    static void Stop();
    static void Flush();
    static LRESULT OnMessage(msgObject & refMessage);

    void RegisterFlushMsg();
    void UnRegisterFlushMsg();
};

#endif // defined(PHONE_APP_PROFILER)

#endif
