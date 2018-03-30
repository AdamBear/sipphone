#ifndef __wMisc_H__
#define __wMisc_H__

#if !WINDOWS_SYSTEM
#error only windows system supported
#endif

namespace ETL_MiscHelper
{

#define wGetCPUTickCount                        etlGetCPUTickCount
#define wGetLocalTime                           etlGetLocalTime

#define wGetCurrentProcessId                    etlGetCurrentProcessId
#define wIsValidProcessID                       etlIsValidProcessID
#define wGetCurrentThreadId                     etlGetCurrentThreadId
#define wIsValidThreadID                        etlIsValidThreadID
#define wGetCurrentTaskId                       etlGetCurrentTaskId
#define wIsValidTaskID                          etlIsValidTaskID

#define wGetExeFileName                         etlGetExeFileName
#define wGetCurrentDirectory                    etlGetCurrentDirectory
#define wGetHomeDirectory                       etlGetHomeDirectory
#define wGetTempDirectory                       etlGetTempDirectory

#define wSleep                                  etlSleep
#define wDelay                                  etlDelay
#define wSwitchToThread                         etlSwitchToThread

#define wGetLastError                           etlGetLastError
#define wTransLastError                         etlTransLastError

#define wInterlockedIncrement                   etlInterlockedIncrement
#define wInterlockedDecrement                   etlInterlockedDecrement

#define wPathStat                               pathStat
#define wPathFileExist                          pathFileExist
#define wPathMkdir                              pathMkdir

#define wWideCharToMultiByte                    etlWideCharToMultiByte
#define wMultiByteToWideChar                    etlMultiByteToWideChar

#define wLoadLibrary                            etlLoadLibrary
#define wGetProcAddr                            etlGetProcAddr
#define wFreeLibrary                            etlFreeLibrary
#define wGetLoadFlag                            etlGetLoadFlag
};
using namespace ETL_MiscHelper;

#endif //__wMisc_H__
