#ifndef __xMisc_H__
#define __xMisc_H__

#if !LINUX_SYSTEM
#error only linux system supported
#endif

namespace ETL_MiscHelper
{

#define xGetCPUTickCount                        etlGetCPUTickCount
#define xGetLocalTime                           etlGetLocalTime

#define xGetCurrentProcessId                    etlGetCurrentProcessId
#define xIsValidProcessID                       etlIsValidProcessID
#define xGetCurrentThreadId                     etlGetCurrentThreadId
#define xIsValidThreadID                        etlIsValidThreadID
#define xGetCurrentTaskId                       etlGetCurrentTaskId
#define xIsValidTaskID                          etlIsValidTaskID

#define xGetExeFileName                         etlGetExeFileName
#define xGetCurrentDirectory                    etlGetCurrentDirectory
#define xGetHomeDirectory                       etlGetHomeDirectory
#define xGetTempDirectory                       etlGetTempDirectory

#define xSleep                                  etlSleep
#define xDelay                                  etlDelay
#define xSwitchToThread                         etlSwitchToThread

#define xGetLastError                           etlGetLastError
#define xTransLastError                         etlTransLastError

#define xInterlockedIncrement                   etlInterlockedIncrement
#define xInterlockedDecrement                   etlInterlockedDecrement

#define xPathStat                               pathStat
#define xPathFileExist                          pathFileExist
#define xPathMkdir                              pathMkdir

#define xWideCharToMultiByte                    etlWideCharToMultiByte
#define xMultiByteToWideChar                    etlMultiByteToWideChar

#define xLoadLibrary                            etlLoadLibrary
#define xGetProcAddr                        etlGetProcAddr
#define xFreeLibrary                            etlFreeLibrary
#define xGetLoadFlag                            etlGetLoadFlag

};
using namespace ETL_MiscHelper;

#endif //__xMisc_H__
