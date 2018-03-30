#include "common.h"

bool WAPIExecDosCmd(const std::string & cmd, bool bSync/* = true*/)
{
    if (cmd.empty())
    {
        return false;
    }

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;

    if (CreateProcess(0, (LPSTR)cmd.c_str(), 0, 0, TRUE, 0, 0, 0, &si, &pi))
    {
        WIN32_INFO("CreateProcess %s \n", cmd.c_str());
        if (bSync)
        {
            WaitForSingleObject(pi.hProcess, INFINITE);
        }
        return true;
    }

    return false;
}

bool WAPIExecDosCmd(const std::string & cmd, std::string & out)
{
    if (cmd.empty())
    {
        return false;
    }

    SECURITY_ATTRIBUTES sa;
    HANDLE hRead, hWrite;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

    if (!CreatePipe(&hRead, &hWrite, &sa, 0))
    {
        return false;
    }

    char command[BUFFER_SIZE] = "cmd.exe /c ";
    strcat(command, cmd.c_str());

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    si.cb = sizeof(STARTUPINFO);
    GetStartupInfo(&si);
    si.hStdError = hWrite;            //把创建进程的标准错误输出重定向到管道输入
    si.hStdOutput = hWrite;           //把创建进程的标准输出重定向到管道输入
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    //关键步骤,CreateProcess函数参数意义请查阅MSDN
    if (!CreateProcess(NULL, command, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
    {
        CloseHandle(hWrite);
        CloseHandle(hRead);
        return false;
    }
    CloseHandle(hWrite);

    char buffer[BLOCK_BUFFER_SIZE] = {0};
    DWORD bytesRead;
    out.clear();

    while (true)
    {
        if (!ReadFile(hRead, buffer, BLOCK_BUFFER_SIZE, &bytesRead, NULL))
        {
            break;
        }
        out.append(buffer, bytesRead);
    }

    CloseHandle(hRead);
    return true;
}

std::string WAPIReadRegSzValue(LPCSTR lpSubkey, LPCSTR lpValue)
{
    if (!lpSubkey || !lpValue)
    {
        return "";
    }

    HKEY hKEY;
    unsigned char buffer[BUFFER_SIZE] = {0};
    DWORD type = REG_SZ;
    DWORD len = sizeof(buffer);

    if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubkey, NULL, KEY_READ, &hKEY) == ERROR_SUCCESS)
    {
        ::RegQueryValueEx(hKEY, lpValue, NULL, &type, buffer, &len);
        ::RegCloseKey(hKEY);
    }

    return (char *)buffer;
}

DWORD WAPIReadRegDwValue(LPCSTR lpSubkey, LPCSTR lpValue)
{
    if (!lpSubkey || !lpValue)
    {
        return 0;
    }

    HKEY hKEY;
    unsigned char buffer[BUFFER_SIZE] = {0};
    DWORD type = REG_DWORD;
    DWORD len = sizeof(buffer);

    if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubkey, NULL, KEY_READ, &hKEY) == ERROR_SUCCESS)
    {
        ::RegQueryValueEx(hKEY, lpValue, NULL, &type, buffer, &len);
        ::RegCloseKey(hKEY);
    }

    return strtol((char *)buffer, 0, 10);
}

bool WAPIGetProcessID(const std::string & name, DWORD & process)
{
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    bool bResult = Process32First(hProcessSnap, &pe32);
    while (bResult)
    {
        if (_tcsicmp(name.c_str(), pe32.szExeFile) == 0)
        {
            process = pe32.th32ProcessID;
            CloseHandle(hProcessSnap);
            return true;
        }
        bResult = Process32Next(hProcessSnap, &pe32);
    }
    CloseHandle(hProcessSnap);
    return false;
}

bool WAPIGetProcessName(DWORD process, std::string & name)
{
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    bool bResult = Process32First(hProcessSnap, &pe32);
    while (bResult)
    {
        if (process == pe32.th32ProcessID)
        {
            name = pe32.szExeFile;
            CloseHandle(hProcessSnap);
            return true;
        }
        bResult = Process32Next(hProcessSnap, &pe32);
    }
    CloseHandle(hProcessSnap);
    return false;
}

bool WAPIGetProcessImagePath(const std::string & process, std::string & path)
{
    DWORD dwProcessID(0);
    return WAPIGetProcessID(process, dwProcessID) && WAPIGetProcessImagePath(dwProcessID, path);
}

bool WAPIGetProcessImagePath(DWORD process, std::string & path)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, process);
    char buffer[BUFFER_SIZE] = {0};
    if (GetProcessImageFileName(hProcess, buffer, BUFFER_SIZE) != 0)
    {
        path = WAPIDosDevicePath2LogicalPath(buffer);
        return true;
    }

    return false;
}

void WAPICreateSystemMsgbox(int time, LPCSTR formate, ...)
{
    if (NULL == formate)
    {
        return;
    }

    char * buffer = NULL;

    va_list arg_ptr;
    va_start(arg_ptr, formate);

    int len = vsnprintf(buffer, 0, formate, arg_ptr);
    if (len == 0)
    {
        return;
    }
    ++len;
    buffer = (char *)::GlobalAlloc(GPTR, len);
    if (buffer == NULL)
    {
        return;
    }
    vsnprintf(buffer, len, formate, arg_ptr);

    va_end(arg_ptr);

    while (time > 0)
    {
#ifdef UNICODE
#   define MessageBoxTimeout "MessageBoxTimeoutW"
#else
#   define MessageBoxTimeout "MessageBoxTimeoutA"
#endif
        typedef int (WINAPI * PMessageBoxTimeout)(
            __in_opt HWND, __in_opt LPCTSTR, __in_opt LPCTSTR, __in UINT, __in WORD, __in UINT);

        LibraryLoadGuard lib("user32.dll");
        PMessageBoxTimeout pFun = (PMessageBoxTimeout)lib.GetInstance(MessageBoxTimeout);
        if (!pFun)
        {
            break;
        }

        (*pFun)(NULL, buffer, _T("Info:"), MB_OK, 0, time);
        ::GlobalFree(buffer);
        return;
    }

    ::MessageBox(NULL, buffer, _T("Info:"), MB_OK);
    ::GlobalFree(buffer);
}

bool WAPICloseProcess(const std::string & process)
{
    DWORD dwProcessID(0);
    return WAPIGetProcessID(process, dwProcessID) && WAPICloseProcess(dwProcessID);
}

bool WAPICloseProcess(DWORD process)
{
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, process);
    if (hProcess)
    {
        TerminateProcess(hProcess, 0);
        WaitForSingleObject(hProcess, 2048);
        CloseHandle(hProcess);
        return true;
    }

    return false;
}

#define GET_PORT(port) (ntohs((unsigned short)(port & 0x0000FFFF)))

bool WAPIGetProcessIDByLocalPort(DWORD port, DWORD & process)
{
    return WAPIGetProcessIDByTCPPort(port, process) || WAPIGetProcessIDByUDPPort(port, process);
}

bool WAPIGetProcessIDByTCPPort(DWORD port, DWORD & process)
{
    typedef enum
    {
        TCP_TABLE_BASIC_LISTENER,
        TCP_TABLE_BASIC_CONNECTIONS,
        TCP_TABLE_BASIC_ALL,
        TCP_TABLE_OWNER_PID_LISTENER,
        TCP_TABLE_OWNER_PID_CONNECTIONS,
        TCP_TABLE_OWNER_PID_ALL,
        TCP_TABLE_OWNER_MODULE_LISTENER,
        TCP_TABLE_OWNER_MODULE_CONNECTIONS,
        TCP_TABLE_OWNER_MODULE_ALL
    } TCP_TABLE_CLASS,     *PTCP_TABLE_CLASS;

    typedef DWORD (WINAPI * PFN_GET_EXTENDED_TCP_TABLE)(
        PMIB_TCPTABLE_OWNER_PID, PDWORD, BOOL, ULONG, TCP_TABLE_CLASS, ULONG);

    LibraryLoadGuard lib("Iphlpapi.dll");
    PFN_GET_EXTENDED_TCP_TABLE GetExtendedTcpTable = (PFN_GET_EXTENDED_TCP_TABLE)
            lib.GetInstance("GetExtendedTcpTable");
    if (GetExtendedTcpTable == NULL)
    {
        return false;
    }

    DWORD dwSize = sizeof(MIB_TCPTABLE_OWNER_PID);
    if (GetExtendedTcpTable(NULL, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL,
                            0) == ERROR_INSUFFICIENT_BUFFER)
    {
        PMIB_TCPTABLE_OWNER_PID pTcpTableEx = (PMIB_TCPTABLE_OWNER_PID)::GlobalAlloc(GPTR, dwSize);
        if (pTcpTableEx == NULL)
        {
            return false;
        }

        bool bRet = false;
        if (GetExtendedTcpTable(pTcpTableEx, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL,
                                0) == NO_ERROR)
        {
            for (int i = 0; i < pTcpTableEx->dwNumEntries; i++)
            {
                MIB_TCPROW_OWNER_PID & tcp = pTcpTableEx->table[i];
                if (GET_PORT(tcp.dwLocalPort) == port)
                {
                    process = tcp.dwOwningPid;
                    bRet = true;
                    break;
                }
            }
        }

        ::GlobalFree(pTcpTableEx);
        return bRet;
    }

    return false;
}

bool WAPIGetProcessIDByUDPPort(DWORD port, DWORD & process)
{
    typedef enum
    {
        UDP_TABLE_BASIC,
        UDP_TABLE_OWNER_PID,
        UDP_TABLE_OWNER_MODULE,
    } UDP_TABLE_CLASS, * PUDP_TABLE_CLASS;

    typedef DWORD (WINAPI * PFN_GET_EXTENDED_UDP_TABLE)(
        PVOID, PDWORD, BOOL, ULONG, UDP_TABLE_CLASS, ULONG);

    LibraryLoadGuard lib("Iphlpapi.dll");
    PFN_GET_EXTENDED_UDP_TABLE GetExtendedUdpTable = (PFN_GET_EXTENDED_UDP_TABLE)
            lib.GetInstance("GetExtendedUdpTable");
    if (GetExtendedUdpTable == NULL)
    {
        return false;
    }

    DWORD dwSize = sizeof(MIB_UDPTABLE_OWNER_PID);
    if (GetExtendedUdpTable(NULL, &dwSize, TRUE, AF_INET, UDP_TABLE_OWNER_PID,
                            0) == ERROR_INSUFFICIENT_BUFFER)
    {
        PMIB_UDPTABLE_OWNER_PID pUdpTableEx = (PMIB_UDPTABLE_OWNER_PID)::GlobalAlloc(GPTR, dwSize);
        if (pUdpTableEx == NULL)
        {
            return false;
        }

        bool bRet = false;
        if (GetExtendedUdpTable(pUdpTableEx, &dwSize, TRUE, AF_INET, UDP_TABLE_OWNER_PID, 0) == NO_ERROR)
        {
            for (int i = 0; i < pUdpTableEx->dwNumEntries; i++)
            {
                MIB_UDPROW_OWNER_PID & udp = pUdpTableEx->table[i];
                if (GET_PORT(udp.dwLocalPort) == port)
                {
                    process = udp.dwOwningPid;
                    bRet = true;
                    break;
                }
            }
        }

        ::GlobalFree(pUdpTableEx);
        return bRet;
    }

    return false;
}

bool WAPICheckPortByCMD(DWORD dwPort, std::string & strApplication, DWORD & dwProcessID)
{
#define IP_PORT "0.0.0.0:%d"
#define CMD_FIND_PORT "netstat -aon | findstr " IP_PORT
#define CMD_FIND_TASK "tasklist | findstr \"%s\" "

    if (dwPort == 0)
    {
        return false;
    }

    std::string strCmdLine = formatString(CMD_FIND_PORT, dwPort);
    std::string strCmdOut;

    if (!WAPIExecDosCmd(strCmdLine, strCmdOut))
    {
        return false;
    }

    WIN32_INFO("%s", strCmdOut.c_str());

    std::vector<std::string> vecLine;
    if (!parseString(vecLine, strCmdOut, "\r\n"))
    {
        return false;
    }

    for (std::vector<std::string>::iterator it = vecLine.begin(); it != vecLine.end(); ++it)
    {
        std::vector<std::string> vecPart;
        if (!parseString(vecPart, *it, " \t") || vecPart.size() != 5)
        {
            continue;
        }

        std::string & strIPPort = vecPart[1];
        std::string & strProcessID = vecPart[4];
        std::string strSTDPort = formatString(IP_PORT, dwPort);

        trim_both(strIPPort);
        trim_both(strProcessID);
        trim_both(strSTDPort);

        if (strSTDPort != strIPPort || strProcessID.empty())
        {
            continue;
        }

        WIN32_INFO("process id=[%s]", strProcessID.c_str());
        std::string strCmdLine = formatString(CMD_FIND_TASK, strProcessID.c_str());
        std::string strCmdOut;

        if (!WAPIExecDosCmd(strCmdLine, strCmdOut))
        {
            continue;
        }

        std::vector<std::string> vecProcess;
        if (!parseString(vecProcess, strCmdOut, " \t\r\n"))
        {
            continue;
        }

        std::string & strPcoessName = vecProcess[0];
        if (strPcoessName.empty())
        {
            continue;
        }

        strApplication = strApplication;
        dwProcessID = atoi(strProcessID.c_str());
        return true;
    }

    return false;
}
