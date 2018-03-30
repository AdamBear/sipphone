#include <winimp/winimp.h>
#include <dsklog/log.h>
#include <dsklog/dsklog.h>
#include <xmlparser/pugixml.hpp>
#include <devicelib/phonedevice.h>
#include <shlobj.h>
#include <service_system.h>

#ifdef LAUNCHER_ROOT_PATH
#error "have defined root path, please check the path"
#endif

#define LAUNCHER_ROOT_PATH         ".\\"
#define LAUNCHER_RESOURCE_PATH      "resource\\"
#define LAUNCHER_DATA_PATH         LAUNCHER_RESOURCE_PATH "data\\"
#define LAUNCHER_TEMP_PATH         LAUNCHER_ROOT_PATH LAUNCHER_RESOURCE_PATH "tmp\\"
#define LOCAL_LOG_PATH    LAUNCHER_TEMP_PATH "log\\"
#define PROCESS_INFO_FILE LAUNCHER_ROOT_PATH "res\\launcher.xml"
#define LOG_FILE_NAME     LOCAL_LOG_PATH "launcher.log"
#define SYS_LOG_FILE_NAME LOCAL_LOG_PATH "system.log"
#define STDOUT_LOG_FILE   LOCAL_LOG_PATH "stdout.log"

#define FACTORY_PASSWD    "YealinkConfig"
#define PHONE_APP_PREFIX  "dskPhone"

#define VAR_PHONE_TYPE "$phonetype$"
#define VAR_HTTP_PORT  "$httpport$"
#define VAR_HTTPS_PORT "$httpsport$"
#define VAR_MAC        "$mac$"
#define VAR_ZOOM       "$zoom$"
#define VAR_RESMAPPATH "$resmappath$"

#define PATH_SUB_ROOT_NAME  "Yealink\\Simulator"
#define REG_KEY_RES_PATH    "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Yealink Simulator"
#define REG_KEY_RES_NAME    "ResourcePath"

#define MAX_PATH_ENV      (MAX_PATH * 25)

typedef std::map<std::string, std::string> mapDefValue;

struct CDskPhoneInfo
{
    std::string strFileName;  // 进程名
    std::string strVersion;   // 版本号
    std::string strHwVersion; // 硬件版本号
    bool        bColor;       // 彩屏黑白屏
    std::string strResourceDir; //资源目录
};

enum LAUNCH_PROCESS_ATTR
{
    LPA_SHOW        = 0x01 << 0,
    LPA_SAFE_EXIT   = 0x01 << 1,
    LPA_SYNC        = 0x01 << 2,
    LPA_SYNC_TEST   = 0x01 << 10,
};

struct CProcessInfo
{
    std::string strFileName;  // 进程名
    std::string strMsgID;     // 用于通信的参数
    std::string strParam;     // 启动参数
    std::string strWorkPath;  // 工作目录
    unsigned    uSyncTimer;   // 等待时间
    int         iPriority;    // 启动顺序
    int         iAttr;        // 进程属性
    PROCESS_INFORMATION  *  pProcessInfo;  // 进程属性
};

bool operator< (const CProcessInfo & left, const CProcessInfo & right)
{
    return left.iPriority < right.iPriority;
}

bool operator< (const CDskPhoneInfo & left, const CDskPhoneInfo & right)
{
    return GetPhoneHwVersion(left.strHwVersion) < GetPhoneHwVersion(right.strHwVersion);
}

typedef std::vector<CProcessInfo> vecProcessInfo;
typedef std::vector<CDskPhoneInfo> vecDskProcessInfo;

static void GetProcessInfo(vecProcessInfo & vecInfo, vecDskProcessInfo & vecDskInfo,
                           mapDefValue & mapValue)
{
    using namespace pugi;

    vecInfo.clear();
    vecDskInfo.clear();

    AUTO_LOG_TIMER
    xml_document doc;
    if (doc.load_file(PROCESS_INFO_FILE))
    {
        xml_node nodeRoot = doc.child("process");
        if (nodeRoot)
        {
            xml_node nodeChild = nodeRoot.child("item");
            while (nodeChild)
            {
                CProcessInfo info;
                info.strFileName = nodeChild.attribute("name").as_string();
                if (!iend_with(info.strFileName, ".exe"))
                {
                    info.strFileName += ".exe";
                }

                info.strParam = nodeChild.attribute("arg").as_string();
                info.strMsgID = nodeChild.attribute("msg").as_string();
                info.strWorkPath = nodeChild.attribute("dir").as_string();
                info.uSyncTimer = nodeChild.attribute("period").as_uint(100);
                info.pProcessInfo = NULL;
                info.iAttr = nodeChild.attribute("attr").as_uint();

                if (nodeChild.attribute("enable").as_bool())
                {
                    vecInfo.push_back(info);
                }

                nodeChild = nodeChild.next_sibling();
            }
        }

        nodeRoot = doc.child("dskphone");
        if (nodeRoot)
        {
            xml_node nodeChild = nodeRoot.child("item");
            while (nodeChild)
            {
                CDskPhoneInfo info;
                info.strFileName = nodeChild.attribute("name").as_string();
                if (!iend_with(info.strFileName, ".exe"))
                {
                    info.strFileName += ".exe";
                }

                info.strVersion = nodeChild.attribute("ver").as_string();
                info.strHwVersion = nodeChild.attribute("hwver").as_string();
                info.bColor = nodeChild.attribute("color").as_bool();
                info.strResourceDir = nodeChild.attribute("res").as_string();

                if (nodeChild.attribute("enable").as_bool())
                {
                    vecDskInfo.push_back(info);
                }

                nodeChild = nodeChild.next_sibling();
            }
        }

        nodeRoot = doc.child("defvalue");
        if (nodeRoot)
        {
            xml_node nodeChild = nodeRoot.child("item");
            while (nodeChild)
            {
                if (nodeChild.attribute("enable").as_bool())
                {
                    std::string strName = nodeChild.attribute("name").as_string();
                    std::string strValue = nodeChild.attribute("value").as_string();

                    if (mapValue.find(strName) == mapValue.end())
                    {
                        mapValue[strName] = strValue;
                    }
                }

                nodeChild = nodeChild.next_sibling();
            }
        }
    }

    if (vecInfo.empty() || vecDskInfo.empty())
    {
        throw std::exception("invalid res process file");
    }

    std::stable_sort(vecInfo.begin(), vecInfo.end(), std::less<CProcessInfo>());
    std::stable_sort(vecDskInfo.begin(), vecDskInfo.end(), std::less<CDskPhoneInfo>());
}

// 命令行参数
static char * const short_options = "o:d:s:p:m:x:g:z:rheck";
static const struct option long_options[] =
{
    {  "checkport",     1,   NULL,   'p'  },
    {  "reset",         0,   NULL,   'r'  },
    {  "help",          0,   NULL,   'h'  },
    {  "start",         1,   NULL,   's'  },
    {  "httpport",      1,   NULL,   'o'  },
    {  "httpsport",     1,   NULL,   'd'  },
    {  "mac",           1,   NULL,   'm'  },
    {  "systeminfo",    0,   NULL,   'e'  },
    {  "clean",         0,   NULL,   'c'  },
    {  "killall",       0,   NULL,   'k'  },
    {  "export",        1,   NULL,   'x'  },
    {  "message",       1,   NULL,   'g'  },
    {  "zoom",          1,   NULL,   'z'  },
    {  0,               0,   0,       0   }
};

static std::string GetRealPath(LPCSTR lpSubPath)
{
    // 1. 当前res文件夹 > 2. 注册表res文件夹 > 3. 直接读%appdata%
    static std::string strRootResPath;

    if (strRootResPath.empty())
    {
        strRootResPath = WAPIGetCurrentDir();
        if (!pathFileExist(LAUNCHER_TEMP_PATH))
        {
            strRootResPath = WAPIReadRegSzValue(REG_KEY_RES_PATH, REG_KEY_RES_NAME);
            WIN_INFO("reg path [%s]", strRootResPath.c_str());
            if (!path_rw_access(strRootResPath))
            {
                char szBuffer[MAX_PATH] = {0};
                if (SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, FALSE))
                {
                    PathAppend(szBuffer, PATH_SUB_ROOT_NAME);
                    if (path_rw_access(szBuffer))
                    {
                        strRootResPath = szBuffer;
                        WIN_INFO("appdata path [%s]", strRootResPath.c_str());
                    }
                }

                if (!path_rw_access(strRootResPath))
                {
                    if (SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_COMMON_APPDATA, FALSE))
                    {
                        PathAppend(szBuffer, PATH_SUB_ROOT_NAME);
                        if (path_rw_access(szBuffer))
                        {
                            strRootResPath = szBuffer;
                            WIN_INFO("appdata path [%s]", strRootResPath.c_str());
                        }
                    }
                }
            }
        }

        if (!path_rw_access(strRootResPath))
        {
            throw std::exception("no res path find in tihs system");
        }
    }

    char szBuffer[MAX_PATH] = {0};
    strcpy(szBuffer, strRootResPath.c_str());
    PathAppend(szBuffer, lpSubPath);
    PathRemoveBackslash(szBuffer);
    return szBuffer;
}

static void YLCallWaitExe(const std::string & strCMDLine)
{
    if (strCMDLine.empty())
    {
        return;
    }
    std::string strOut;
    WAPIExecDosCmd(strCMDLine, strOut);
    WIN_INFO("YLCallWaitExe:\n%s", strOut.c_str());
}

static void YLInitLocalFileLog()
{
    std::string strLogPath = GetRealPath(LOG_FILE_NAME);
    // 日志处理
    run_cmd_t  run_cmd;
    memset(&run_cmd, 0, sizeof(run_cmd_t));
    run_cmd.logmode_str = "-m file,time";
    run_cmd.loglevel_str = "any=7";
    run_cmd.logfile_str = strLogPath.c_str();
    run_cmd_log_apply(&run_cmd);
}

// 恢复出厂设置接口
static int YLFullReset()
{
#define FACTORY_CFG_PATH        LAUNCHER_ROOT_PATH  LAUNCHER_RESOURCE_PATH "\\phone\\factory"
#define CONFIG_CFG_PATH         LAUNCHER_ROOT_PATH  LAUNCHER_RESOURCE_PATH "\\config"
#define FACTORY_PRIVATE_PATH    LAUNCHER_ROOT_PATH  LAUNCHER_RESOURCE_PATH "\\phone\\private"
#define CONFIG_PRIVATE_PATH     LAUNCHER_ROOT_PATH  LAUNCHER_RESOURCE_PATH "\\private"

    run_cmd_log_stop();

    std::string strFacPath = GetRealPath(FACTORY_CFG_PATH);
    std::string strCfgPath = GetRealPath(CONFIG_CFG_PATH);

    // 清空并删除目录
    delete_files_in_directory(strCfgPath.c_str(), true, true);
    // 拷贝目录
    copy_directory(strFacPath.c_str(), strCfgPath.c_str(), true, true);

    strFacPath = GetRealPath(FACTORY_PRIVATE_PATH);
    strCfgPath = GetRealPath(CONFIG_PRIVATE_PATH);

    // 清空并删除目录
    delete_files_in_directory(strCfgPath.c_str(), true, true);
    // 拷贝目录
    copy_directory(strFacPath.c_str(), strCfgPath.c_str(), true, true);

    // 删除用户上传资源文件
    std::string strCMDLine = GetRealPath(LAUNCHER_DATA_PATH);
    delete_files_in_directory(strCMDLine.c_str(), true);

    // 清空TEMP
    strCMDLine = GetRealPath(LAUNCHER_TEMP_PATH);
    delete_files_in_directory(strCMDLine.c_str(), true);

    return 0;
}

static int YLExportLog(LPCSTR lpPath)
{
#define CMD_TAR_LOG  "7z.exe a -mhe %s \"%s\\*\" \"%s\" -t7z  1>nul 2>nul"

    if (lpPath == NULL)
    {
        return -1;
    }

    run_cmd_log_stop();

    std::string strLogPath = GetRealPath(LAUNCHER_TEMP_PATH);
    std::string strPrivatePath = GetRealPath(CONFIG_PRIVATE_PATH);
    std::string strCMDLine = formatString(CMD_TAR_LOG, lpPath, strLogPath.c_str(),
                                          strPrivatePath.c_str());
    YLCallWaitExe(strCMDLine);
    return 0;
}

static void YLKillProcess(const CProcessInfo & info)
{
    WAPICloseProcess(info.strFileName);
}

static void YLKillDskPhoneProcess(const CDskPhoneInfo & info)
{
    WAPICloseProcess(info.strFileName);
}

static const CDskPhoneInfo & YLGetDskPhoneInfo(const vecDskProcessInfo & vecDskInfo, int iPhoneType)
{
    for (vecDskProcessInfo::const_iterator it = vecDskInfo.begin(); it != vecDskInfo.end(); ++it)
        if (GetPhoneHwVersion(it->strHwVersion) == (unsigned)iPhoneType)
        {
            return *it;
        }
    throw std::exception("invalid phone type");
}

static unsigned YLGetDskPhoneType(const vecDskProcessInfo & vecDskInfo,
                                  const std::string & strFileName)
{
    for (vecDskProcessInfo::const_iterator it = vecDskInfo.begin(); it != vecDskInfo.end(); ++it)
        if (WAPIIsExeFileMatched(strFileName.c_str(), it->strFileName.c_str()))
        {
            return GetPhoneHwVersion(it->strHwVersion);
        }
    throw std::exception("invalid phone name");
}

static void YLAppHelp()
{
    printf("for more help please visit our web site http://www.yealink.com \n");
}

static void YLSendMsg(const std::string & strMsgID)
{
    AUTO_LOG_TIMER

    std::vector<std::string> vecMsg;
    parseString(vecMsg, strMsgID, ";");
    for (std::vector<std::string>::iterator it = vecMsg.begin(); it != vecMsg.end(); ++it)
    {
        std::vector<std::string> vecParam;
        if (parseString(vecParam, *it, ","))
        {
            int iIndex = 0;
            std::string strTarget = vecParam[iIndex++];
            trim_both(strTarget);

            if (strTarget.empty())
            {
                continue;
            }

            int iMsgID = vecParam.size() > iIndex ? strtol(vecParam[iIndex++].c_str(), NULL, 0) : 0;
            int iMsgWparam = vecParam.size() > iIndex ? strtol(vecParam[iIndex++].c_str(), NULL, 0) : 0;
            int iMsgLparam = vecParam.size() > iIndex ? strtol(vecParam[iIndex++].c_str(), NULL, 0) : 0;

            WIN_INFO("YLSendMsg :%s-%d-%d-%d", strTarget.c_str(), iMsgID, iMsgWparam, iMsgLparam);
            msgSendMsgToThreadTimeoutEx(msgGetThreadByName(strTarget.c_str()), iMsgID, iMsgWparam, iMsgLparam,
                                        0, 0, 500);
        }
    }
}

static void YLSafeKillProcess(vecProcessInfo & vecInfo)
{
    for (vecProcessInfo::reverse_iterator it = vecInfo.rbegin(); it != vecInfo.rend(); ++it)
    {
        if (it->iAttr & (LPA_SYNC_TEST | LPA_SYNC))
        {
            continue;
        }
        else if (it->iAttr & LPA_SAFE_EXIT)
        {
            YLSendMsg(it->strMsgID);
            WAPIProcessSync(it->uSyncTimer);
        }

        WIN_INFO("stop process [%s]", it->strFileName.c_str());
        WAPICloseProcessByInfo(it->pProcessInfo);

        delete it->pProcessInfo;
        it->pProcessInfo = NULL;
    }
}

// 启动模拟器个进程
static int YLStartSimulator(unsigned uPhoneType, vecProcessInfo & vecInfo,
                            vecDskProcessInfo & vecDskInfo, mapDefValue & mapValue)
{
    try
    {
        // 杀进程
        std::for_each(vecInfo.rbegin(), vecInfo.rend(), YLKillProcess);
        std::for_each(vecDskInfo.rbegin(), vecDskInfo.rend(), YLKillDskPhoneProcess);

        std::string strStdoutPath = GetRealPath(STDOUT_LOG_FILE);
        SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};

        HANDLE hStdoutHandle = CreateFile(strStdoutPath.c_str(),
                                          GENERIC_WRITE,
                                          FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                          &sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        HANDLE hStderrHandle = INVALID_HANDLE_VALUE;
        DuplicateHandle(GetCurrentProcess(), hStdoutHandle, GetCurrentProcess(),
                        &hStderrHandle, 0, TRUE, DUPLICATE_SAME_ACCESS);

        if (hStdoutHandle == INVALID_HANDLE_VALUE)
        {
            WIN_INFO("CreateFile stdout [%d] failed", GetLastError());
        }
        if (hStderrHandle == INVALID_HANDLE_VALUE)
        {
            WIN_INFO("CreateFile stdout [%d] failed", GetLastError());
        }


        //遍历获取话机资源路径
        std::string resourceDir = "";
        for (vecProcessInfo::iterator processInfo_it = vecInfo.begin(); processInfo_it != vecInfo.end();
                ++processInfo_it)
        {
            std::string strParam = processInfo_it->strParam;
            std::string strCmd = processInfo_it->strFileName;
            bool isFind = false;
            if (istart_with(strCmd, PHONE_APP_PREFIX))
            {
                for (vecDskProcessInfo::const_iterator it = vecDskInfo.begin(); it != vecDskInfo.end(); ++it)
                {
                    if (GetPhoneHwVersion(it->strHwVersion) == uPhoneType)
                    {
                        resourceDir = it->strResourceDir;
                        isFind = true;
                        break;
                    }
                }
            }
            if (isFind)
            {
                break;
            }
        }

        // 启动进程
        for (vecProcessInfo::iterator it = vecInfo.begin(); it != vecInfo.end(); ++it)
        {
            std::string strParam = it->strParam;
            std::string strCmd = it->strFileName;

            if (istart_with(strCmd, PHONE_APP_PREFIX))
            {
                strCmd = YLGetDskPhoneInfo(vecDskInfo, uPhoneType).strFileName;
            }

            // cpp11 as test flag
            if (it->iAttr & LPA_SYNC_TEST)
            {
                while (1)
                {
                    DWORD dwProcssID(0);
                    if (!WAPIGetProcessID(strCmd, dwProcssID))
                    {
                        Sleep(1500);
                    }
                    else
                    {
                        break;
                    }
                    fprintf(stderr, "wait dskphone to start [%s][%u]\n", strCmd.c_str(), dwProcssID);
                }

                while (1)
                {
                    DWORD dwProcssID(0);
                    if (!WAPIGetProcessID(strCmd, dwProcssID))
                    {
                        break;
                    }
                    else
                    {
                        Sleep(1500);
                    }
                    fprintf(stderr, "wait dskphone to stop [%s][%u]\n", strCmd.c_str(), dwProcssID);
                }
                break;
            }

            // 替换变量
            if (!strParam.empty())
            {
                mapDefValue::iterator it = mapValue.begin();
                for (; it != mapValue.end(); ++it)
                {
                    const std::string & strVar = it->first;
                    std::string::size_type nPos = strParam.find(strVar);
                    if (nPos != std::string::npos)
                    {
                        strParam.replace(nPos, strVar.length(), it->second);
                    }
                }

                if (!strParam.empty())
                {
                    strCmd.append(" ").append(strParam);
                }
            }

            //进程的工作路径同话机配置参数一致
            it->strWorkPath = resourceDir;

            std::string strResPath = GetRealPath(it->strWorkPath.c_str());
            WIN_INFO("resource path [%s]", strResPath.c_str());

            PROCESS_INFORMATION * pi = WAPICreateProcess(strCmd.c_str(), strResPath.c_str(),
                                       it->iAttr & LPA_SHOW, hStdoutHandle, hStderrHandle);
            if (pi == NULL)
            {
                continue;
            }

            if (it->iAttr & LPA_SYNC)
            {
                // 到当前进程之前的进程检测一遍
                for (vecProcessInfo::iterator iter = vecInfo.begin(); iter != it; ++iter)
                {
                    const std::string & strProcessName = iter->strFileName;
                    DWORD dwProcssID;
                    if (WAPIGetProcessID(strProcessName, dwProcssID))
                    {
                        WIN_INFO("detect process name=[%s] id=[%lu]", strProcessName.c_str(), dwProcssID);
                    }
                    else
                    {
                        WIN_INFO("detect process [%s] failed, ", strProcessName.c_str());
                    }
                }

                WAPIWaitProcess(pi);
            }
            else
            {
                it->pProcessInfo = pi;
                WAPIProcessSync(it->uSyncTimer);
            }
        }

        // 清理其它进程
        YLSafeKillProcess(vecInfo);

        ::CloseHandle(hStderrHandle);
        ::CloseHandle(hStdoutHandle);

        WIN_INFO("dskPhone[%d] finished", uPhoneType);
        return 0;
    }
    catch (std::exception & e)
    {
        WIN_INFO("StartSimulator failed [%s]", e.what());
    }
    catch (...)
    {
        WIN_INFO("StartSimulator failed");
    }
}

// 设置环境变量
static bool YLSetEnv()
{
    char szPathBuffer[MAX_PATH_ENV] = { 0 };
    ::GetEnvironmentVariable("Path", szPathBuffer, sizeof(szPathBuffer));
    std::string strOldEnv = szPathBuffer;

    std::string strCurrentDir = WAPIGetCurrentDir();
    LPCSTR lpRootDir = strCurrentDir.c_str();
    _snprintf(szPathBuffer, MAX_PATH_ENV, "%s;%s\\gnuwin32;%s", lpRootDir, lpRootDir,
              strOldEnv.c_str());

    ::SetDllDirectory(szPathBuffer);
    return ::SetEnvironmentVariable("Path", szPathBuffer);
}

static void YLCheckPort(DWORD dwPort)
{
    DWORD dwProcessID = 0;
    std::string strPcoessName;
    if (WAPICheckPortByTCPTable(dwPort, strPcoessName, dwProcessID))
    {
        APP_ERR("Process name=%s id=%u take the port %d", strPcoessName.c_str(), dwProcessID, dwPort);
    }
}

static std::string YLGetMacByLibSys(char ch = 0)
{
    const int iMacLen = 6;
    unsigned char bufMac[iMacLen] = {0};
    if (sys_get_MAC(SYS_PHY_TYPE_WAN, bufMac, iMacLen) != 0)
    {
        WIN_ERR("system get mac address failed");
    }

    std::string strMac;
    for (UINT i = 0; i < iMacLen; i++)
        if (ch = 0)
        {
            strMac += formatString("%02X", bufMac[i]);
        }
        else
        {
            strMac += formatString("%02X%c", bufMac[i], i == iMacLen - 1 ? '\0' : ch);
        }

    return strMac;
}

int main(int argc, char * argv[])
{
    try
    {
        WAPIRunCrashHandler();
        YLInitLocalFileLog();
        YLSetEnv();

        int ch = 'h';
        mapDefValue mapValue;
        vecProcessInfo vecInfo;
        vecDskProcessInfo vecDskInfo;
        unsigned uPhoneType = 0;

        while ((ch = getopt_long(argc, argv, short_options, long_options, NULL)) != -1)
        {
            switch (ch)
            {
            case 'p':
            {
                DWORD dwProcessID = 0;
                std::string strPcoessName, strProcessPath;
                bool bTaken = WAPICheckPortByTCPTable(atoi(optarg), strPcoessName, dwProcessID);
                if (bTaken)
                {
                    WAPIGetProcessImagePath(dwProcessID, strProcessPath);
                    WAPICreateSystemMsgbox(8000,
                                           "Process name=\"%s\"  process id=\"%u\"  path=\"%s\"  take the port  \"%s\"",
                                           strPcoessName.c_str(), dwProcessID, strProcessPath.c_str(), optarg);
                }
                return bTaken;
            }
            case 'h':
            {
                YLAppHelp();
                return 0;
            }
            case 'o':
            {
                mapValue[VAR_HTTP_PORT] = optarg;
                YLCheckPort(atoi(optarg));
            }
            break;
            case 'd':
            {
                mapValue[VAR_HTTPS_PORT] = optarg;
                YLCheckPort(atoi(optarg));
            }
            break;
            case 'm':
            {
                mapValue[VAR_MAC] = optarg;
            }
            break;
            case 's':
            {
                GetProcessInfo(vecInfo, vecDskInfo, mapValue);

                if (is_all<char *>(optarg, optarg + strlen(optarg), isdigit))
                {
                    uPhoneType = (unsigned)(atoi(optarg));
                }
                else
                {
                    uPhoneType = YLGetDskPhoneType(vecDskInfo, optarg);
                }

                const CDskPhoneInfo & PhoneTypeInfo = YLGetDskPhoneInfo(vecDskInfo, uPhoneType);

                // 设置话机信息
                sys_init(NULL, 0);
                sys_set_hw_version(PhoneTypeInfo.strHwVersion.c_str());
                sys_set_sw_version(PhoneTypeInfo.strVersion.c_str());
                sys_set_model_name(devicelib_GetHardwareName());

                PHONE_TYPE_DEF phoneType = devicelib_GetPhoneType();
                mapValue[VAR_PHONE_TYPE] = toString(phoneType);

                setResPathMapFile(mapValue[VAR_RESMAPPATH]);
                setPhoneDeviceName(devicelib_GetPhoneName());
            }
            break;
            case 'e':
            {
                std::string strSysLogPath = GetRealPath(SYS_LOG_FILE_NAME);
                // 获取当前系统信息
                std::ofstream ofs(strSysLogPath.c_str());
                if (ofs)
                {
                    ofs << WAPIGetSystemInfo() << std::endl;
                    ofs.close();
                }
            }
            break;
            case 'c':
            {
                run_cmd_log_stop();
                // 清空tmp目录
                delete_files_in_directory(GetRealPath(LAUNCHER_TEMP_PATH).c_str(), true);
                YLInitLocalFileLog();
            }
            break;
            case 'k':
            {
                GetProcessInfo(vecInfo, vecDskInfo, mapValue);

                std::for_each(vecInfo.rbegin(), vecInfo.rend(), YLKillProcess);
                std::for_each(vecDskInfo.rbegin(), vecDskInfo.rend(), YLKillDskPhoneProcess);
            }
            break;
            case 'z':
            {
                mapValue[VAR_ZOOM] = optarg;
            }
            break;
            case 'x':
                return YLExportLog(optarg);
            case 'g':
                return YLSendMsg(optarg);
            case 'r':
                return YLFullReset();
            default:
                WIN_INFO("unkown option :%c %s", ch, optarg);
                break;
            }
        }

        if (uPhoneType)
        {
            // 命令行和文件都没有读到，直接使用默认值
            if (mapValue.find(VAR_MAC) == mapValue.end())
            {
                mapValue[VAR_MAC] = YLGetMacByLibSys();
            }
            if (mapValue.find(VAR_HTTP_PORT) == mapValue.end())
            {
                mapValue[VAR_HTTP_PORT] = "80";
            }
            if (mapValue.find(VAR_HTTPS_PORT) == mapValue.end())
            {
                mapValue[VAR_HTTPS_PORT] = "443";
            }

            return YLStartSimulator(uPhoneType, vecInfo, vecDskInfo, mapValue);
        }
        return -1;
    }
    catch (std::exception & e)
    {
        WIN_INFO("launcher failed [%s]", e.what());
    }
    catch (...)
    {
        WIN_INFO("launcher failed");
    }
}