#include "common.h"
#include <dbghelp.h>

#if 0 //_MSC_VER <= 1500
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

typedef boost::shared_ptr<int> SmartProcessPtr;
#elif __cplusplus >= 201103L
typedef std::shared_ptr<int>   SmartProcessPtr;
#elif 0
#include <ylsmartptr.h>
typedef YLSmartPtr<int>       SmartProcessPtr;
#endif

#pragma comment(lib, "Dbghelp.lib")

PROCESS_INFORMATION * WAPICreateProcess(LPCSTR szCmdline, LPCSTR szCrtDir /*= NULL*/,
                                        bool bShowWnd /*= false*/, HANDLE hStdoutHandle /*= INVALID_HANDLE_VALUE*/,
                                        HANDLE hStderrHandle /*= INVALID_HANDLE_VALUE*/)
{
    PROCESS_INFORMATION * pi = new PROCESS_INFORMATION;
    ZeroMemory(pi, sizeof(PROCESS_INFORMATION));

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = bShowWnd ? SW_SHOWNORMAL : SW_HIDE;

    if (hStdoutHandle != INVALID_HANDLE_VALUE)
    {
        si.hStdOutput = hStdoutHandle;
    }
    if (hStderrHandle != INVALID_HANDLE_VALUE)
    {
        si.hStdError = hStderrHandle;
    }

    if (CreateProcess(0, (LPSTR)szCmdline, 0, 0, TRUE, 0, 0, szCrtDir, &si, pi))
    {
        WIN32_INFO("CreateProcess %s \n", szCmdline);
        return pi;
    }

    delete pi;
    return NULL;
}

bool WAPICloseProcessByInfo(PROCESS_INFORMATION * pi)
{
    if (pi != NULL)
    {
        TerminateProcess(pi->hProcess, 0);
        WaitForSingleObject(pi->hProcess,  2048);
        // Close process and thread handles.
        CloseHandle(pi->hProcess);
        CloseHandle(pi->hThread);
        return true;
    }

    return false;
}

void WAPIWaitProcess(PROCESS_INFORMATION * pi)
{
    if (pi != NULL)
        // Wait until child process exits.
    {
        WaitForSingleObject(pi->hProcess, INFINITE);
    }
}

void WAPIProcessSync(unsigned uTimer)
{
    int istart = ::GetTickCount();
    for (int i = 0; i < uTimer; ++i)
    {
        Sleep(1);
    }

    int iend = ::GetTickCount();
    WIN32_INFO("YLProcessSync:%d %d", uTimer, iend - istart);
}

std::string WAPIGetSystemInfo()
{
    std::string strOut;
    const static int BUFSIZE = 1024;
    char chBuf[BUFSIZE] = {0};
    OSVERSIONINFOEX osvi;
    SYSTEM_INFO si;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx((OSVERSIONINFO *) &osvi);

    _snprintf(chBuf, BUFSIZE,
              "OSVERSIONINFOEX=[%u.%u][%u][%u] [%s] [%u][%u] [%u][%u][%u]\n",
              osvi.dwMajorVersion,
              osvi.dwMinorVersion,
              osvi.dwBuildNumber,
              osvi.dwPlatformId,
              osvi.szCSDVersion,
              osvi.wServicePackMajor,
              osvi.wServicePackMinor,
              osvi.wSuiteMask,
              osvi.wProductType,
              osvi.wReserved);
    strOut += chBuf;

    ZeroMemory(&si, sizeof(SYSTEM_INFO));
    GetSystemInfo(&si);
    _snprintf(chBuf, BUFSIZE,
              "SYSTEM_INFO=[%u] [%p][%p][%p] procesor=[%u][%u][%u] [%u][%u]\n",
              si.dwPageSize,
              si.lpMinimumApplicationAddress,
              si.lpMaximumApplicationAddress,
              si.dwActiveProcessorMask,
              si.dwNumberOfProcessors,
              si.dwProcessorType,
              si.dwAllocationGranularity,
              si.wProcessorLevel,
              si.wProcessorRevision);
    strOut += chBuf;

    LCID language = GetSystemDefaultLCID();
    int iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int iScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    _snprintf(chBuf, BUFSIZE,
              "lang=[%u] ScreenWidth=[%u] ScreenHeight=[%u]  [%u][%u][%u][%u]\n",
              language, iScreenWidth, iScreenHeight,
              GetSystemMetrics(SM_SERVERR2), GetSystemMetrics(SM_STARTER),
              GetSystemMetrics(SM_MEDIACENTER), GetSystemMetrics(SM_TABLETPC));
    strOut += chBuf;

    MEMORYSTATUSEX Mem;
    ZeroMemory(&Mem, sizeof(MEMORYSTATUSEX));
    Mem.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&Mem);

    DWORDLONG iMetricsMB = (1024 * 1024);
    _snprintf(chBuf, BUFSIZE,
              "MEMORYSTATUSEX=[%lu][%lu][%I64u][%I64u][%I64u][%I64u][%I64u][%I64u][%I64u]\n",
              Mem.dwLength,
              Mem.dwMemoryLoad,
              Mem.ullTotalPhys / iMetricsMB,
              Mem.ullAvailPhys / iMetricsMB,
              Mem.ullTotalPageFile / iMetricsMB,
              Mem.ullAvailPageFile / iMetricsMB,
              Mem.ullTotalVirtual / iMetricsMB,
              Mem.ullAvailVirtual / iMetricsMB,
              Mem.ullAvailExtendedVirtual / iMetricsMB);
    strOut += chBuf;

    _snprintf(chBuf, BUFSIZE,
              "build vc=[%u] cplus=[%u] time=[%s]\n", _MSC_VER, __cplusplus, __TIMESTAMP__);
    strOut += chBuf;

    _snprintf(chBuf, MAX_PATH,
              "name=[%s] directory=[%s]\n", WAPIGetCurrentFileName().c_str(), WAPIGetCurrentDir().c_str());
    strOut += chBuf;

    return strOut;
}

static BOOL IsDataSectionNeeded(const WCHAR * pModuleName)
{
    if (pModuleName == NULL)
    {
        return FALSE;
    }

    WCHAR szFileName[_MAX_FNAME] = L"";
    _wsplitpath(pModuleName, NULL, NULL, szFileName, NULL);

    if (wcsicmp(szFileName, L"ntdll") == 0)
    {
        return TRUE;
    }

    return FALSE;
}

static BOOL CALLBACK MiniDumpCallback(PVOID                            pParam,
                                      const PMINIDUMP_CALLBACK_INPUT   pInput,
                                      PMINIDUMP_CALLBACK_OUTPUT        pOutput)
{
    if (pInput == 0 || pOutput == 0)
    {
        return FALSE;
    }

    switch (pInput->CallbackType)
    {
    case ModuleCallback:
        if (pOutput->ModuleWriteFlags & ModuleWriteDataSeg)
            if (!IsDataSectionNeeded(pInput->Module.FullPath))
            {
                pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);
            }
    case IncludeModuleCallback:
    case IncludeThreadCallback:
    case ThreadCallback:
    case ThreadExCallback:
        return TRUE;
    default:
        ;
    }

    return FALSE;
}

static LONG WINAPI TopLevelUnhandledExceptionFilter(LPCTSTR lpstrDumpFilePathName,
        PEXCEPTION_POINTERS pExInfo)
{
    HANDLE hFile = ::CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL,
                                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        MINIDUMP_EXCEPTION_INFORMATION einfo;
        einfo.ThreadId = ::GetCurrentThreadId();
        einfo.ExceptionPointers = pExInfo;
        einfo.ClientPointers = FALSE;

        MINIDUMP_CALLBACK_INFORMATION mci;
        mci.CallbackRoutine     = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;
        mci.CallbackParam       = NULL;

        MINIDUMP_TYPE mdt = (MINIDUMP_TYPE)(MiniDumpWithPrivateReadWriteMemory |
                                            MiniDumpWithDataSegs |
                                            MiniDumpWithHandleData |
                                            0x00000800 |
                                            0x00001000 |
                                            MiniDumpWithUnloadedModules);

        // 写入Dump文件内容
        BOOL bOK = ::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, mdt, &einfo,
                                       NULL, &mci);

        WIN32_INFO("Create Dump File [%d]", bOK);
        ::CloseHandle(hFile);
    }
    else
    {
        WIN32_WARR("Create File %s Failed %d", lpstrDumpFilePathName, GetLastError());
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

static LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(
    LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
    return NULL;
}

static BOOL PreventSetUnhandledExceptionFilter()
{
    HMODULE hKernel32 = ::LoadLibrary(_T("kernel32.dll"));
    if (hKernel32 == NULL)
    {
        return FALSE;
    }

    void * pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
    if (pOrgEntry == NULL)
    {
        ::FreeLibrary(hKernel32);
        return FALSE;
    }

    unsigned char newJump[ 100 ];
    DWORD dwOrgEntryAddr = (DWORD) pOrgEntry;
    dwOrgEntryAddr += 5; // add 5 for 5 op-codes for jmp far

    void * pNewFunc = &MyDummySetUnhandledExceptionFilter;
    DWORD dwNewEntryAddr = (DWORD) pNewFunc;
    DWORD dwRelativeAddr = dwNewEntryAddr -  dwOrgEntryAddr;

    newJump[ 0 ] = 0xE9;  // JMP absolute
    memcpy(&newJump[ 1 ], &dwRelativeAddr, sizeof(pNewFunc));
    SIZE_T bytesWritten;
    BOOL bRet = WriteProcessMemory(GetCurrentProcess(), pOrgEntry, newJump, sizeof(pNewFunc) + 1,
                                   &bytesWritten);
    //::FreeLibrary(hKernel32);
    return bRet;
}

static LONG WINAPI UnhandledExceptionFilterEx(struct _EXCEPTION_POINTERS * pException)
{
    TCHAR szMbsFile[MAX_PATH] = { 0 };
    SYSTEMTIME st;
    ::GetLocalTime(&st);

    std::string strPath = WAPIGetCurrentDir();
    std::string strFileName = WAPIGetCurrentFileNameWithoutExtension();

    /* 不同模块路径不同，优先找tmp目录 */
    if (PathIsDirectory(".\\tmp"))
    {
        strPath = ".\\tmp\\";
    }
    else if (PathIsDirectory(".\\resource\\tmp"))
    {
        strPath = ".\\resource\\tmp\\";
    }
    else if (PathIsDirectory("..\\resource\\tmp"))
    {
        strPath = "..\\resource\\tmp\\";
    }
    else
    {
        strPath = "";
    }

    wsprintf(szMbsFile, TEXT("%s%s-%u.%u-%04d.%02d.%02d-%02d.%02d.%02d.%03d.dmp"),
             strPath.c_str(), strFileName.c_str(), ::GetCurrentProcessId(), ::GetCurrentThreadId(),
             st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    TopLevelUnhandledExceptionFilter(szMbsFile, pException);

    // TODO: MiniDumpWriteDump
    FatalAppExit(-1,  _T("Fatal Error"));
    return EXCEPTION_CONTINUE_SEARCH;
}

bool WAPIRunCrashHandler()
{
    SetUnhandledExceptionFilter(UnhandledExceptionFilterEx);
    return PreventSetUnhandledExceptionFilter();
}

std::string WAPIGetIPV4AsString(DWORD dwSocketIP)
{
    struct  in_addr    inadAddr;
    inadAddr.s_addr = dwSocketIP;
    return inet_ntoa(inadAddr);
}

bool WAPICheckPortByTCPTable(DWORD dwPort, std::string & strApplication, DWORD & dwProcessID)
{
    return WAPIGetProcessIDByLocalPort(dwPort, dwProcessID)
           && WAPIGetProcessName(dwProcessID, strApplication);
}

bool WAPIIsExeFileMatched(LPCTSTR strName, LPCTSTR strRefName)
{
    char chPath[MAX_PATH] = {0};
    strcpy(chPath, strName);
    std::transform(chPath, chPath + strlen(chPath), chPath, ::tolower);
    PathRemoveExtension(chPath);

    char chRefPath[MAX_PATH] = {0};
    strcpy(chRefPath, strRefName);
    std::transform(chRefPath, chRefPath + strlen(chRefPath), chRefPath, ::tolower);
    PathRemoveExtension(chRefPath);

    return strcmp(chPath, chRefPath) == 0;
}

void WAPIMainProcessInit()
{
    WAPIRunCrashHandler();
    ImmDisableIME(0);

    HANDLE handle = CreateMutex(NULL, false, "YealinkDskPhone");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        LPCSTR lpTips = "Another instance of simulator is running.";
        WAPICreateSystemMsgbox(5000, lpTips);
        CloseHandle(handle);
        throw std::exception(lpTips);
    }

    HANDLE constdout = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE constderr = GetStdHandle(STD_ERROR_HANDLE);
    int level = DBG_LOG_LEVEL_INFO;

    if (level >= DBG_LOG_LEVEL_DEBUG)
    {
        SetConsoleTextAttribute(constdout,
                                FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
        SetConsoleTextAttribute(constderr,
                                FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
    }
    if (level >= DBG_LOG_LEVEL_INFO)
    {
        SetConsoleTextAttribute(constdout, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
        SetConsoleTextAttribute(constderr, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
    else if (level >= DBG_LOG_LEVEL_WARN)
    {
        SetConsoleTextAttribute(constdout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
        SetConsoleTextAttribute(constderr, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
    }
    else
    {
        SetConsoleTextAttribute(constdout, FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_RED);
        SetConsoleTextAttribute(constderr, FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_RED);
    }
}
