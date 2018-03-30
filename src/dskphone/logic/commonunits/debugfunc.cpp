#include "debugfunc.h"
#include "commonunits_inc.h"
#include "backtrace.h"
#include <commonapi/stringhelperapi.h>
#include <commonapi/systemhelperapi.h>
#include "dsklog/log.h"
#include "dsklog/dsklog.h"
#include "devicelib/phonedevice.h"
#include "lamp/ledlamp/src/factory.h"
#include "adapterbox/include/modadapterbox.h"
#include "deletemanager.h"
#include <ETLLib.hpp>
#include "etlmsghandler/modetlmsghandler.h"
#include "service_autop.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "extended/actionuri_url/include/modactionuri_url.h"
#include "account/include/modaccount.h"
#include "ETLLib/base64.h"
#include "ETLLib/aestable.h"
#include "service_h323_interface.h"
#include "autopcontrol/include/modautopcontrol.h"

#include "commonunits_def.h"
#include <yllist.h>
#include <ylstring.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#include "keyguard/include/keyguardmessagedefine.h"
#include "contacts/directory/include/directorymessagedefine.h"


#ifdef IF_FEATURE_TOOLS

#define DEBUG_TOOLS 0

const char * MapLauncher(int nMesg);
const char * MapSystem(int nMesg);
const char * MapAutop(int nMesg);
const char * MapSip(int nMesg);
const char * MapIpvp(int nMesg);
const char * MapDsk(int nMesg);
const char * MapWui(int nMesg);
const char * MapH323(int nMesg);

#define IS_MKSRV_MSG(id,msg)     ((((msg)&0xffff0000) >> 16) == (id))

#define PRINT_MESSAGE_NAME(msg)         case msg: /*APP_ERR("%s [%08d][%08x]",  #msg, msg, msg);*/ return #msg;


void debug_PrintAll()
{

}

void debug_PrintTask(int nPID)
{
    // 运行情况
    char szCmd[128] = { 0 };
    sprintf(szCmd, "cat /proc/%d/status", mkit_get_taskid(mkGetHandle()));
    system(szCmd);

    // fd
    sprintf(szCmd, "ls /proc/%d/fd -all", mkit_get_taskid(mkGetHandle()));
    system(szCmd);

    return;
}

void debug_PrintTask(const char * pName)
{

}

void debug_PrintSpace(const char * pName)
{

}

const char * debug_MapMesgToName(unsigned nMesg)
{
    if (IS_MKSRV_MSG(MKSRV_ID_LAUNCHER, nMesg))
    {
        return MapLauncher(nMesg);
    }

    if (IS_MKSRV_MSG(MKSRV_ID_SYSTEM, nMesg))
    {
        return MapSystem(nMesg);
    }


    if (IS_MKSRV_MSG(MKSRV_ID_AUTOP, nMesg))
    {
        return MapAutop(nMesg);
    }

    if (IS_MKSRV_MSG(MKSRV_ID_SIPUA, nMesg))
    {
        return MapSip(nMesg);
    }

    if (IS_MKSRV_MSG(MKSRV_ID_IPVP, nMesg))
    {
        return MapIpvp(nMesg);
    }

    if (IS_MKSRV_MSG(MKSRV_ID_DSK, nMesg))
    {
        return MapDsk(nMesg);
    }

    if (IS_MKSRV_MSG(MKSRV_ID_WUI, nMesg))
    {
        return MapWui(nMesg);
    }

    if (IS_MKSRV_MSG(MKSRV_ID_H323, nMesg))
    {
        return MapH323(nMesg);
    }

    return "NOT DEFINE";
}

void PrintQueueConfig()
{
    // 打印消息队列
    //system("cat /proc/mkit/task_msg_i");
    printf("*********************mkit msg config*******************\n");
    printf("***************cat /proc/mkit/task_msg_i***************\n");
    printf("ST   : Pending : Depth : Peak : BuffMax : BuffPer\n");

    yl::string strInfo;
    chFileStdio fileSrc;
    if (fileSrc.Open("/proc/mkit/task_msg_i", chFileStdio::modeRead))
    {
        while (!fileSrc.IsEOF())
        {
            //ST:P=0 ,D=16,P=0   ,BM=8192,BP=8192
            fileSrc.ReadString(strInfo);
#if DEBUG_TOOLS
            printf("%s", strInfo.c_str());
#endif //#if DEBUG_TOOLS
        }
    }

    printf("*******************************************************\n");
    return;
}

void PrintQueue()
{
    printf("\n********************mkit msg queue*******************\n");
    printf("***************cat /proc/mkit/task_msg_q***************\n");

    //system("cat /proc/mkit/task_msg_q");
    printf("\n\nIndex : PID : Flag :    Message(wParam + lParam, data + size) :   SenderRet :   SenderWait :  SenderTask\n");

    yl::string strInfo;
    chFileStdio fileSrc;
    if (fileSrc.Open("/proc/mkit/task_msg_q", chFileStdio::modeRead))
    {
        while (!fileSrc.IsEOF())
        {
            //00:PID=402,FL=0x00000000,M=0x00000003(0x41ca4760+0x03a0bb4c,0x00000000+0),SR=(0x00000000),SW=0x00000000,ST=??
            fileSrc.ReadString(strInfo);
#if DEBUG_TOOLS
            printf("%s", strInfo.c_str());
#endif
            // 格式化处理：
            int nPos = strInfo.find("M=0x");
            if (nPos != yl::string::npos)
            {
                yl::string strMsg = strInfo.substr(nPos + 2, 10);
#if DEBUG_TOOLS
                printf("msg value[%s]\n", strMsg.c_str());
#endif//#if DEBUG_TOOLS

                int nValue = strtol(strMsg.c_str(), NULL, 16);
                yl::string strMsgName = debug_MapMesgToName((unsigned int)nValue);

#if DEBUG_TOOLS
                printf("msg name[%s]\n", strMsgName.c_str());
#endif //#if DEBUG_TOOLS

                if (!strMsgName.empty())
                {
                    strInfo.replace(strMsg, strMsgName);
                }
            }

            printf("%s", strInfo.c_str());
        }
    }
    printf("*******************************************************\n");

    return;
}

void PrintQueueStatus()
{
    // 消息状态：统计
    // 打印消息队列
    //system("cat /proc/mkit/task_msg_s");
    printf("\n*********************mkit msg status*******************\n");
    printf("***************cat /proc/mkit/task_msg_s***************\n");

    printf("\n\nST : Query , Send, Post, Bcast, Call, Peek, Reply, Flush, RecData\n");

    yl::string strInfo;
    chFileStdio fileSrc;
    if (fileSrc.Open("/proc/mkit/task_msg_s", chFileStdio::modeRead))
    {
        while (!fileSrc.IsEOF())
        {
            //ST:q(3), s(0), p(0), b(1), c(0), p(0), r(0), f(0), d(0)
            fileSrc.ReadString(strInfo);
#if DEBUG_TOOLS
            printf("%s", strInfo.c_str());
#endif //#if DEBUG_TOOLS
        }
    }

    return;
}

void debug_PrintQueue()
{
    //打印消息配置
    PrintQueueConfig();

    // 打印消息状态：统计
    PrintQueueStatus();

    // 打印消息队列
    PrintQueue();

    return;
}

void debug_PrintTimer()
{
    // 定时器个数
    system("cat /proc/mkit/task_timer_proxy_i");

    // 定时器状态:Deadline 期限？
    //system("cat /proc/mkit/task_timer_proxy_q");
    printf("\n\nIndex :      ID ,  Flag,      Message,     Interval(*10ms),    Deadline,     Owner\n");

    yl::string strInfo;
    chFileStdio fileSrc;
    if (fileSrc.Open("/proc/mkit/task_timer_proxy_q", chFileStdio::modeRead))
    {
        while (!fileSrc.IsEOF())
        {
            //13:ID=0x41cb41a4,FL=0x00000000,M=0x00000003,i=00718500,N=67990707,O=app_vpPhone
            fileSrc.ReadString(strInfo);
            printf("%s", strInfo.c_str());
        }
    }

    return;
}

struct TaskMemInfo
{
    yl::string strName;
    int nVmRss;
    int nVmSize;
    int nVmPeak;

    int nFdSize;

    TaskMemInfo()
    {
        strName.clear();
        nVmRss = 0;
        nVmSize = 0;
        nVmPeak = 0;
        nFdSize = 0;
    }
};

static bool CmpTask(TaskMemInfo & l, TaskMemInfo & r)
{
    return l.nVmRss - r.nVmRss > 0;
}

TaskMemInfo debug_GetMemInfo(yl::string & strPath)
{
    yl::string strInfo;
    TaskMemInfo stTask;
    chFileStdio fileSrc;
    if (fileSrc.Open(strPath.c_str(), chFileStdio::modeRead))
    {
        char szName[64] = { 0 };
        char szValue[32] = { 0 };
        int nCnt = 0;
        while (!fileSrc.IsEOF())
        {
            fileSrc.ReadString(strInfo);
            strInfo.replace(" ", "");

            //printf("%s", strInfo.c_str());

            if (strInfo.find("VmPeak") != yl::string::npos)
            {
                nCnt++;
                sscanf(strInfo.c_str(), "VmPeak:%skB", szName);

                stTask.nVmPeak = atoi(szName);
            }
            else if (strInfo.find("VmSize") != yl::string::npos)
            {
                nCnt++;
                sscanf(strInfo.c_str(), "VmSize:%skB", szName);
                stTask.nVmSize = atoi(szName);
            }
            else if (strInfo.find("VmRSS") != yl::string::npos)
            {
                nCnt++;
                sscanf(strInfo.c_str(), "VmRSS:%skB", szName);
                stTask.nVmRss = atoi(szName);
            }
            else if (strInfo.find("Name") != yl::string::npos)
            {
                nCnt++;
                sscanf(strInfo.c_str(), "Name:%s", szName);
                stTask.strName = szName;
            }

            //          printf("-------------[ %s %d %d %d ]\n", szName, nFree, nBuff,nCached);
            if (nCnt >= 4)
            {
                break;
            }
        }
    }

    return stTask;
}

void PrintBuddyinfo()
{
    // 打印内存段信息
    FILE * fd;
    int n;
    char szBuff[512];

    fd = fopen("/proc/buddyinfo", "r");
    if (fd == NULL)
    {
        printf("Open /proc/stat failed");
        return;
    }

    fgets(szBuff, sizeof(szBuff), fd);

    fclose(fd);

    //printf("buddyinfo:%s \n", szBuff);

    // 各内存区域块个数
    char szNode[48];
    char szZone[48];
    char szDma[48];
    int iTmp;

    int arrMem[11] = { 0 };

    //int i4kb, i8kb, i16kb, i32kb, i64kb, i128kb, i256kb, i512kb, i1024kb, i2048kb, i4096kb;
    sscanf(szBuff, "Node 0, zone      DMA%7d%7d%7d%7d%7d%7d%7d%7d%7d%7d%7d",
           &arrMem[0], &arrMem[1], &arrMem[2], &arrMem[3], &arrMem[4], &arrMem[5],
           &arrMem[6], &arrMem[7], &arrMem[8], &arrMem[9], &arrMem[10]);

    printf("buddyinfo:\n");
    printf("  4k   8k  16k  32k  64k  128k  256k  512k 1024k 2048k 4096k\n");
    printf("%4d %4d %4d %4d %4d  %4d  %4d  %4d  %4d  %4d  %4d\n",
           arrMem[0], arrMem[1], arrMem[2], arrMem[3], arrMem[4], arrMem[5], arrMem[6], arrMem[7], arrMem[8],
           arrMem[9], arrMem[10]);

    int iMemory = 0;
    for (int i = 0; i <= 10; i++)
    {
        iMemory += arrMem[i] * pow((double)2, (double)(i + 2));
    }

    /*  int iMemory = i4kb * 4 + i8kb * 8 + i16kb * 16 + i32kb * 32 + i64kb * 64 + i128kb * 128 + i256kb * 256 + i512kb * 512 + i1024kb * 1024 + i2048kb * 2048 + i4096kb * 4096;*/

    // 内存碎片率：1减去最大内存占总内存的比率（认为不是最大内存的块都算作碎片）
    float fMemory = 0;
    for (int i = 10; i >= 0; i--)
    {
        if (arrMem[i] > 0)
        {
            fMemory = 1.0 - ((arrMem[i] * pow((double)2, (double)(i + 2))) / iMemory);
            break;
        }
    }

    printf("total:%dkb \nFragmentation rate: %.2f \n\n", iMemory, fMemory);
}

void PrintTaskMemInfo()
{
    printf("***********************************************************\n");
    // 打印各进程物理内存、虚拟内存、虚拟内存峰值、设备打开个数，按物理内存排序
    YLList<yl::string> listFile;
    if (!commonAPI_getFilesInDirectoryByExt(&listFile, "/proc", "", true))
    {
        return;
    }

    YLList<TaskMemInfo> listTask;

    for (YLList<yl::string>::iterator it = listFile.begin(); it != listFile.end(); ++it)
    {
        yl::string strFile = (*it);

        yl::string strPid = pathGetFileNamePos(strFile.c_str());
        if (strPid.empty())
        {
            continue;
        }

        if (!commonAPI_IsDigital(strPid))
        {
            continue;
        }

        YLList<yl::string> listFd;
        if (!commonAPI_getFilesInDirectoryByExt(&listFd, strFile + "/fd", "", true))
        {
            printf("open fd fail");
            return;
        }

        strFile += "/status";
        if (!pathFileExist(strFile.c_str()))
        {
            continue;
        }

        TaskMemInfo stTask = debug_GetMemInfo(strFile);

        stTask.nFdSize = listFd.size();

        listTask.push_back(stTask);
    }

    Sort(listTask, CmpTask);

    printf("\nPName                  Vrss    Vsize    Vpeak      Fd\n");
    // 打印结果
    for (YLList<TaskMemInfo>::iterator it = listTask.begin(); it != listTask.end(); ++it)
    {
        TaskMemInfo & stData = (*it);
        if (stData.strName.empty())
        {
            continue;
        }

        if (stData.nVmPeak <= 0
                && stData.nVmRss <= 0
                && stData.nVmSize <= 0
                && stData.nFdSize <= 0)
        {
            continue;
        }

        printf("%-18.18s %8d %8d %8d%8d\n", stData.strName.c_str(), stData.nVmRss, stData.nVmSize,
               stData.nVmPeak, stData.nFdSize);
    }
}

void debug_PrintMemStatic()
{
    int nTotal, nFree = 0, nBuff = 0, nCache = 0;
    debug_GetTotalMemInfo(nTotal, nFree, nBuff, nCache);
    printf("Total: %d Free: %d Buff: %d Cache: %d\n", nTotal, nFree, nBuff, nCache);

    PrintBuddyinfo();

    PrintTaskMemInfo();

    return;
}



void debug_PrintFlashStatic()
{
    yl::string strInfo;
    chFileStdio fileSrc;

    int nMemory = 0;
    if (fileSrc.Open("/proc/kmsg", chFileStdio::modeRead))
    {
        char szName[64] = { 0 };
        char szValue[32] = { 0 };
        int nCnt = 0;
        while (!fileSrc.IsEOF())
        {
            fileSrc.ReadString(strInfo);
            strInfo.replace(" ", "");

            if (strInfo.find("Memory:") != yl::string::npos)
            {
                printf("%s\n", strInfo.c_str());
                break;
            }

        }
    }


    //
    system("df");

    return;
}

void debug_AnalysisLog(char * pLog)
{
//  // 字符规范
//  if (/*长度*/)
//  {
//
//  }
//  else if (/*连续字符*/)
//  {
//
//  }
//  //
//  else if (/* 变量格式、类格式*/)
//  {
//
//  }
//  else if (/*Log等级*/)
//  {
//      // 保存到告警文件
//  }

    //
}

void debug_Tool_Map_Msg(const char * pMsg)
{
    yl::string strValue = pMsg;
    unsigned int nMsg;
    if (strValue.find("0x") != yl::string::npos
            || strValue.find("0X") != yl::string::npos)
    {
        nMsg = strtoul(pMsg, NULL, 16);
    }
    else
    {
        nMsg = atoi(pMsg);
    }

    const char * pMsgName = debug_MapMesgToName(nMsg);

    printf("%s\n", pMsgName);
}

void debug_Tool_Upgrade_Rom(const char * pPath)
{
    if (NULL != pPath)
    {
        printf("try upgrade rom by autoServer[%s]\n", pPath);
//      autop_upgrade_rom(mkGetHandle(), 1000, pPath);

        Autop_NotifyAutoServerEx(false, ATP_MSG_REQ_UPGRADE_ROM, 0, 0, strlen(pPath) + 1, (LPCVOID)pPath);
    }

    return;
}

void debug_Tool_Get_Cfg(const char * pKey)
{
    if (NULL != pKey)
    {
        yl::string strValue = configParser_GetConfigString(pKey);

        printf("%s = %s\n", pKey, strValue.c_str());
    }

    return;
}


void debug_PrintNetInfo()
{
    system("cat /proc/net/dev");
}

#define DSKPHONE_SERVERR() msgGetThreadByName("app_vpPhone")

void debug_dump_logic_info(unsigned long handle, const char * szType)
{
    if (NULL == szType)
    {
        return;
    }

    char buf[8192] = { 0 };
    int retval = -1;
    mkit_message_t retmsg;
    int type = atoi(szType);

    retmsg.target = DSKPHONE_SERVERR();
    retmsg.flags = MKIT_MFL_CALL_RET | MKIT_MFL_CALL_DAT;
    retmsg.sync = 2000;
    retmsg.message = DSK_MSG_QUERY_INFO_DIAG;
    retmsg.wparam = (unsigned long)type;
    /*  retmsg.lparam = (unsigned long)flag;*/
    retmsg.data_ptr = (unsigned char *)buf;
    retmsg.data_len = 0;
    retmsg.data_max = sizeof(buf);

    retval = mkit_call(handle, &retmsg);
    if (retval < 0)
    {
        printf("dump info fail, ret val[%d]\n", retval);
    }
    else
    {
        if (retmsg.data_ptr && retmsg.data_len > 0)
        {
            printf("%s\n\n", retmsg.data_ptr);
        }
    }

    return;
}

const char * MapLauncher(int nMesg)
{
#if DEBUG_TOOLS
    APP_ERR("~~~launcher message~~~");
#endif //#if DEBUG_TOOLS

    switch (nMesg)
    {
        PRINT_MESSAGE_NAME(MKIT_MSG_TIMER)
        PRINT_MESSAGE_NAME(MKIT_MSG_INVALID)
        PRINT_MESSAGE_NAME(MKIT_MSG_IDLE)
        PRINT_MESSAGE_NAME(MKIT_MSG_QUIT)
        PRINT_MESSAGE_NAME(MKIT_MSG_REGISTER)
        PRINT_MESSAGE_NAME(MKIT_MSG_INIT)
        PRINT_MESSAGE_NAME(MKIT_MSG_TERM)

        //////////////////////////////////////////////////////////////////////////
        PRINT_MESSAGE_NAME(MKIT_MSG_SYSLOG)
        PRINT_MESSAGE_NAME(MKIT_MSG_STATS)
        PRINT_MESSAGE_NAME(MKIT_MSG_MEMCHECK)
        PRINT_MESSAGE_NAME(MKIT_MSG_REBOOT)
        PRINT_MESSAGE_NAME(MKIT_MSG_SRVREG)
        PRINT_MESSAGE_NAME(MKIT_MSG_SRVUNREG)
        //PRINT_MESSAGE_NAME(MKIT_MSG_SRVREADY)
        //PRINT_MESSAGE_NAME(MKIT_MSG_SRVLEAVE)
        PRINT_MESSAGE_NAME(MKIT_MSG_SRVDATA)
        PRINT_MESSAGE_NAME(MKIT_MSG_REQ_WATCH)
        PRINT_MESSAGE_NAME(MKIT_MSG_DEL_WATCH)
        PRINT_MESSAGE_NAME(MKIT_MSG_REQ_NOTIFY)
        PRINT_MESSAGE_NAME(MKIT_MSG_DEL_NOTIFY)

    default:
        break;
    }

    return "NOT DEFINE MKIT MSG";
}

const char * MapSystem(int nMesg)
{
#if DEBUG_TOOLS
    APP_ERR("~~~system message~~~");
#endif //#if DEBUG_TOOLS
    switch (nMesg)
    {
        PRINT_MESSAGE_NAME(SYS_MSG_NOTIFY_NETWORK_STATUS)
        PRINT_MESSAGE_NAME(SYS_MSG_NOTIFY_IP_ADDRESS_CHANGED)
        PRINT_MESSAGE_NAME(SYS_MSG_NOTIFY_TIME_CHANGED)

        PRINT_MESSAGE_NAME(SYS_MSG_NOTIFY_SYSENV_CHANGED)
        PRINT_MESSAGE_NAME(SYS_MSG_NOTIFY_CERT_REQUEST)
        PRINT_MESSAGE_NAME(SYS_MSG_NET_WOULD_CHANGED)
        PRINT_MESSAGE_NAME(SYS_MSG_WIFI_SCAN_RESULT)
        PRINT_MESSAGE_NAME(SYS_MSG_WIFI_CONNECT_STATUS)
        PRINT_MESSAGE_NAME(SYS_MSG_WIFI_WPS_RESULT)
        PRINT_MESSAGE_NAME(SYS_MSG_WIFI_WPS_PBC_CATCH)
        PRINT_MESSAGE_NAME(SYS_MSG_UPDATE_FIRWARE)
        PRINT_MESSAGE_NAME(SYS_MSG_DEVICE_REBOOT)
        PRINT_MESSAGE_NAME(SYS_MSG_UPDATE_CONFIG)
        PRINT_MESSAGE_NAME(SYS_MSG_RESET_FACTORY)
        PRINT_MESSAGE_NAME(SYS_MSG_OPENVPN_STATUS)
    default:
        break;
    }

    return "NOT DEFINE SYSTEM MSG";
}

const char * MapAutop(int nMesg)
{
#if DEBUG_TOOLS
    APP_ERR("~~~autopserver message~~~");
#endif //#if DEBUG_TOOLS
    switch (nMesg)
    {
        PRINT_MESSAGE_NAME(ATP_MSG_NOTIFY_AUTOP_START)
        PRINT_MESSAGE_NAME(ATP_MSG_NOTIFY_AUTOP_END)
        PRINT_MESSAGE_NAME(ATP_MSG_REQ_ZERO_OVER)
        PRINT_MESSAGE_NAME(ATP_MSG_REQ_START_RPS)
        PRINT_MESSAGE_NAME(ATP_MSG_REQ_RPS_STATUS)
        PRINT_MESSAGE_NAME(ATP_MSG_NOTIFY_AUTH_ERR)
    default:
        break;
    }

    return "NOT DEFINE AUTOP MSG";
}

const char * MapSip(int nMesg)
{
#if DEBUG_TOOLS
    APP_ERR("~~~sip message~~~");
#endif //#if DEBUG_TOOLS

    switch (nMesg)
    {

        PRINT_MESSAGE_NAME(SIP_REG_UPDATE_REGISTER)
        PRINT_MESSAGE_NAME(SIP_REG_UPDATE_REGISTER_NOTIFY)
        PRINT_MESSAGE_NAME(SIP_REG_UPDATE_SIP_SERVER)
        PRINT_MESSAGE_NAME(SIP_REG_UPDATE_RETRY_TIME)
        PRINT_MESSAGE_NAME(SIP_REG_UPDATE_DEV_PAIR_INFO)
        PRINT_MESSAGE_NAME(SIP_REG_UPDATE_ERROR_CODE)
        PRINT_MESSAGE_NAME(SIP_REG_UPDATE_EXCHANGE_URL)
        PRINT_MESSAGE_NAME(SIP_REG_TRANSPORT)
        PRINT_MESSAGE_NAME(SIP_REG_SERVER_TYPE)


        PRINT_MESSAGE_NAME(SIP_CALL_START)
        PRINT_MESSAGE_NAME(SIP_CALL_INCOMING)
        PRINT_MESSAGE_NAME(SIP_CALL_PROCEEDING)
        PRINT_MESSAGE_NAME(SIP_CALL_RINGBACK)
        PRINT_MESSAGE_NAME(SIP_CALL_REFUSE)
        PRINT_MESSAGE_NAME(SIP_CALL_ESTABLISH)
        PRINT_MESSAGE_NAME(SIP_CALL_FAIL)
        PRINT_MESSAGE_NAME(SIP_CALL_RELEASED)
        PRINT_MESSAGE_NAME(SIP_CALL_UPDATE)
        PRINT_MESSAGE_NAME(SIP_CALL_FORWARDED_BY_REMOTE)
        PRINT_MESSAGE_NAME(SIP_CALL_TRANSFERED_BY_REMOTE)
        PRINT_MESSAGE_NAME(SIP_CALL_REPLACED_BY_REMOTE)
        PRINT_MESSAGE_NAME(SIP_CALL_CONFERENCE_NOTIFY)
        PRINT_MESSAGE_NAME(SIP_CALL_UPDATE_INFO)
        PRINT_MESSAGE_NAME(SIP_CALL_RECV_FRAME_SYNC)
        PRINT_MESSAGE_NAME(SIP_CALL_INFORMATION_UPDATE)
        PRINT_MESSAGE_NAME(SIP_CALL_CODEC_LIST)
        PRINT_MESSAGE_NAME(SIP_CALL_FORWARDED_BY_REMOTE_EXT)
        PRINT_MESSAGE_NAME(SIP_CALL_TRANSFERED_BY_REMOTE_EXT)
        PRINT_MESSAGE_NAME(SIP_CALL_REPLACED_BY_REMOTE_EXT)

        PRINT_MESSAGE_NAME(SIP_CALL_BW_CALL_RECORD_STATUS)
        PRINT_MESSAGE_NAME(SIP_CALL_TRANSFERED_RESULT)
        PRINT_MESSAGE_NAME(SIP_CALL_PAUSED_RESULT)
        PRINT_MESSAGE_NAME(SIP_CALL_RECORD_ON_RESULT)
        PRINT_MESSAGE_NAME(SIP_CALL_RECORD_OFF_RESULT)
        PRINT_MESSAGE_NAME(SIP_CALL_SEND_INFO_RESULT)
        PRINT_MESSAGE_NAME(SIP_CALL_REINVITE_FAIL)
        PRINT_MESSAGE_NAME(SIP_CALL_CHANNEL_CHANGE)
        PRINT_MESSAGE_NAME(SIP_CALL_REINVITE_RECIEVE)
        PRINT_MESSAGE_NAME(SIP_CALL_WARNING_INFO)

        PRINT_MESSAGE_NAME(SIP_SUB_UPDATE_BLF_STATUS)
        PRINT_MESSAGE_NAME(SIP_SUB_UPDATE_BLF_STATUS_EXT)
        PRINT_MESSAGE_NAME(SIP_SUB_UPDATE_BLFLIST)
        PRINT_MESSAGE_NAME(SIP_SUB_UPDATE_BLA_STATUS)
        PRINT_MESSAGE_NAME(SIP_SUB_UPDATE_BROADSOFT_SCA_STATUS)
        PRINT_MESSAGE_NAME(SIP_SUB_UPDATE_BROADSOFT_SCA_SEIZE_RESULT)
        PRINT_MESSAGE_NAME(SIP_SUB_UPDATE_CC_STATUS)
        PRINT_MESSAGE_NAME(SIP_SUB_UPDATE_FEATURE_KEY_STATUS)
        PRINT_MESSAGE_NAME(SIP_SUB_UPDATE_HOTELING_STATUS)
        PRINT_MESSAGE_NAME(SIP_SUB_BSFT_CALLPARK_UPDATE)
        PRINT_MESSAGE_NAME(SIP_SUB_UPDATE_CALL_CENTER_STATUS)
        PRINT_MESSAGE_NAME(SIP_SUB_GCS_NOTIFY)
        PRINT_MESSAGE_NAME(SIP_SUB_UPDATE_ACD_STATUS)
        PRINT_MESSAGE_NAME(SIP_SUB_UPDATE_DIAL_TONE)
        PRINT_MESSAGE_NAME(SIP_SUB_HOT_DESKING_NOTIFY)
        PRINT_MESSAGE_NAME(SIP_SUB_TIPTEL_PBX_CONTROL_NOTIFY)
        PRINT_MESSAGE_NAME(SIP_SUB_UPDATE_DRAFT_BLA_STATUS)
        PRINT_MESSAGE_NAME(SIP_SUB_ADDRESS_BOOK_NOTIFY)
        PRINT_MESSAGE_NAME(SIP_SUB_BLA_SEIZE_LINE_RESULT)
        PRINT_MESSAGE_NAME(SIP_SUB_UPDATE_CC_SUBSCRIBE_RESULT)
        PRINT_MESSAGE_NAME(SIP_SUB_FEATURE_KEY_SUBSCRIBED_RESULT)
        PRINT_MESSAGE_NAME(SIP_SUB_SEND_NOTIFY_RESULT)
        PRINT_MESSAGE_NAME(SIP_SUB_FORTINET_DND_SUBSCRIBED_RESULT)
        PRINT_MESSAGE_NAME(SIP_SUB_FORTINET_DND_NOTIFY_RESULT)
        PRINT_MESSAGE_NAME(SIP_SUB_KTS_NOTIFY_RESULT)


        PRINT_MESSAGE_NAME(SIP_MSG_UPDATE_MWI_INFO)
        PRINT_MESSAGE_NAME(SIP_MSG_UPDATE_SMS)
        PRINT_MESSAGE_NAME(SIP_MSG_HUAWEI_COMM_DIV_MESSAGE)
        PRINT_MESSAGE_NAME(SIP_MSG_SIP_CFG_CHANGE)
        PRINT_MESSAGE_NAME(SIP_MSG_UACSTA_INFO)
        PRINT_MESSAGE_NAME(SIP_MSG_UPDATE_DATE)
        PRINT_MESSAGE_NAME(SIP_MSG_UPDATE_KPML_STATUS)
        PRINT_MESSAGE_NAME(SIP_MSG_UPDATE_SERVICE_STATUS)
        PRINT_MESSAGE_NAME(SIP_MSG_RECV_CUSTOM_INFO)
        PRINT_MESSAGE_NAME(SIP_MSG_UPDATE_APOLLO_CONF_FACTROY)

        //
#ifdef IF_FEATURE_CLOUD
        PRINT_MESSAGE_NAME(SIP_MSG_APOLLO_CONF_USER_STATE_UPDATE)
        PRINT_MESSAGE_NAME(SIP_MSG_APOLLO_CONF_DESCRIPTION_UPDATE)
        PRINT_MESSAGE_NAME(SIP_MSG_APOLLO_CONF_VIEW_STATE_UPDATE)
        PRINT_MESSAGE_NAME(SIP_MSG_APOLLO_CONF_APPOINTMENT_STATE_UPDATE)
        PRINT_MESSAGE_NAME(SIP_MSG_APOLLO_AUTOP_UPDATE)
        PRINT_MESSAGE_NAME(SIP_MSG_APOLLO_PHONE_BOOK_UPDATE)
        PRINT_MESSAGE_NAME(SIP_MSG_APOLLO_CONF_FOCUS_URI)
#endif //#ifdef IF_FEATURE_CLOUD

        //PRINT_MESSAGE_NAME(SIP_MSG_APOLLO_CONF_SOURCE_INFO)
        PRINT_MESSAGE_NAME(SIP_MSG_PUBLIC_CLOUD_CONTACT_UPDATE)
        PRINT_MESSAGE_NAME(SIP_MSG_STATE_PUBLISH_RETRY)
        PRINT_MESSAGE_NAME(SIP_MSG_EXT_VIDEO_STATE)
        PRINT_MESSAGE_NAME(SIP_MSG_FECC_CAMERA_START)
        PRINT_MESSAGE_NAME(SIP_MSG_FECC_CAMERA_STOP)
        PRINT_MESSAGE_NAME(SIP_MSG_FECC_STORE_AS_PRESET)
        PRINT_MESSAGE_NAME(SIP_MSG_FECC_ACTIVATE_PRESET)
        PRINT_MESSAGE_NAME(SIP_MSG_SEND_SMS_RESULT)
        PRINT_MESSAGE_NAME(SIP_MSG_SEND_INFO_RESULT)
        PRINT_MESSAGE_NAME(SIP_MSG_SEND_NOTIFY_RESULT)
        PRINT_MESSAGE_NAME(SIP_MSG_UC_REQ_INFO)
        PRINT_MESSAGE_NAME(SIP_MSG_UC_REQ_NOTIFY)
        PRINT_MESSAGE_NAME(SIP_MSG_AUTOTEST_MESS_CHECK)
        PRINT_MESSAGE_NAME(SIP_MSG_VERIZON_VVM)

        PRINT_MESSAGE_NAME(SIP_MSG_GET_SYSLOG_INFO)
        PRINT_MESSAGE_NAME(SIP_MSG_UPDATE_SYSLOG_INFO)

        // cmd 字段
        PRINT_MESSAGE_NAME(SIP_CMD_CALL_OUT)
        PRINT_MESSAGE_NAME(SIP_CMD_CALL_RESPONSE)
        PRINT_MESSAGE_NAME(SIP_CMD_CALL_REJECT)
        PRINT_MESSAGE_NAME(SIP_CMD_CALL_ACCEPT)
        PRINT_MESSAGE_NAME(SIP_CMD_CALL_HANGUP)
        PRINT_MESSAGE_NAME(SIP_CMD_CALL_HOLD)
        PRINT_MESSAGE_NAME(SIP_CMD_CALL_RESUME)
        PRINT_MESSAGE_NAME(SIP_CMD_CALL_DTMF)
        PRINT_MESSAGE_NAME(SIP_CMD_CALL_FORWARD)
        PRINT_MESSAGE_NAME(SIP_CMD_CALL_TRANSFER)
        PRINT_MESSAGE_NAME(SIP_CMD_CALL_CONFERENCE)
        PRINT_MESSAGE_NAME(SIP_CMD_RELOAD_ACCOUNT)
        PRINT_MESSAGE_NAME(SIP_CMD_CHECK_SYNC)
        PRINT_MESSAGE_NAME(SIP_CMD_REBOOT_PHONE)
        PRINT_MESSAGE_NAME(SIP_CMD_PASS_ACTION_URI)
        PRINT_MESSAGE_NAME(SIP_CMD_SET_OPTION)
        PRINT_MESSAGE_NAME(SIP_CMD_SET_PCAP_START)
        PRINT_MESSAGE_NAME(SIP_CMD_SET_PCAP_STOP)
        PRINT_MESSAGE_NAME(SIP_CMD_CONFERENCE_DEL_USER)
        PRINT_MESSAGE_NAME(SIP_CMD_CONFERENCE_MUTE_USER)
        PRINT_MESSAGE_NAME(SIP_CMD_CONFERENCE_MUTE_AUDIENCE)
        PRINT_MESSAGE_NAME(SIP_CMD_CREAT_CONFERENCE)
        PRINT_MESSAGE_NAME(SIP_CMD_CONFERENCE_ADD_USER)
        PRINT_MESSAGE_NAME(SIP_CMD_GET_UI_ELEMENTS)
        PRINT_MESSAGE_NAME(SIP_CMD_NAVIGATE_TO_SCREEN)
        PRINT_MESSAGE_NAME(SIP_CMD_DETECTE_UI_STATE)

        PRINT_MESSAGE_NAME(SIP_CMD_QUERY_CONTACT_INFORMATION)
        PRINT_MESSAGE_NAME(SIP_CMD_GET_CALENDAR_ITEM)
        PRINT_MESSAGE_NAME(SIP_CMD_CONFIG_PIN)
        PRINT_MESSAGE_NAME(SIP_CMD_CHANGE_PIN)
        PRINT_MESSAGE_NAME(SIP_CMD_LOCK_PIN)
        PRINT_MESSAGE_NAME(SIP_CMD_CHECK_DEVICE_UPDATE)
        PRINT_MESSAGE_NAME(SIP_CMD_SYSTEM_NOTICE)
        PRINT_MESSAGE_NAME(SIP_REQUIRE_GET_EXP)
        PRINT_MESSAGE_NAME(SIP_REQUIRE_GET_OPTION)


    default:
        break;
    }

    return "NOT DEFINE SIP MSG";
}

const char * MapIpvp(int nMesg)
{
#if DEBUG_TOOLS
    APP_ERR("~~~ipvp message~~~");
#endif //#if DEBUG_TOOLS
    switch (nMesg)
    {

        PRINT_MESSAGE_NAME(IPVP_MSG_MODECHANGED)
        PRINT_MESSAGE_NAME(IPVP_MSG_HPEVENT)
        PRINT_MESSAGE_NAME(IPVP_MSG_ATRAIN)
        PRINT_MESSAGE_NAME(IPVP_MSG_ATUNING)
        PRINT_MESSAGE_NAME(IPVP_MSG_VTRAIN)
        PRINT_MESSAGE_NAME(IPVP_MSG_VTUNING)

        // audio
        PRINT_MESSAGE_NAME(IPVP_MSG_AUDIO_RECVDTMF)
        PRINT_MESSAGE_NAME(IPVP_MSG_AUDIO_RTPVIOLATE)
        PRINT_MESSAGE_NAME(IPVP_MSG_AUDIO_ADDRCHANGED)
        PRINT_MESSAGE_NAME(IPVP_MSG_AUDIO_SSRCCHANGED)
        PRINT_MESSAGE_NAME(IPVP_MSG_AUDIO_RTCPBYE)
        PRINT_MESSAGE_NAME(IPVP_MSG_AUDIO_VQREPORT)
        PRINT_MESSAGE_NAME(IPVP_MSG_AUDIO_SRTPFAIL)
        PRINT_MESSAGE_NAME(IPVP_MSG_AUDIO_PKTDISCARD)
        PRINT_MESSAGE_NAME(IPVP_MSG_AUDIO_PKTLOST)
        PRINT_MESSAGE_NAME(IPVP_MSG_AUDIO_NETLOST)
        PRINT_MESSAGE_NAME(IPVP_MSG_AUDIO_TRAIN)
        PRINT_MESSAGE_NAME(IPVP_MSG_AUDIO_TUNING)
        PRINT_MESSAGE_NAME(IPVP_MSG_AUDIO_DIAGNOSE)
        PRINT_MESSAGE_NAME(IPVP_MSG_AUDIO_DEAD)
        PRINT_MESSAGE_NAME(IPVP_MSG_AUDIO_ALIVE)
        PRINT_MESSAGE_NAME(IPVP_MSG_AUDIO_SRTP_REINVITE)

#ifdef IF_SUPPORT_VIDEO
        // video
        PRINT_MESSAGE_NAME(IPVP_MSG_VIDEO_DEAD)
        PRINT_MESSAGE_NAME(IPVP_MSG_VIDEO_ALIVE)
        PRINT_MESSAGE_NAME(IPVP_MSG_VIDEO_RECV_RESIZED)
        PRINT_MESSAGE_NAME(IPVP_MSG_VIDEO_SEND_RESIZED)
        PRINT_MESSAGE_NAME(IPVP_MSG_VIDEO_CAM_PAUSED)
        PRINT_MESSAGE_NAME(IPVP_MSG_VIDEO_CAM_RESTORE)
        PRINT_MESSAGE_NAME(IPVP_MSG_VIDEO_RREQ_KFRAME)
        PRINT_MESSAGE_NAME(IPVP_MSG_VIDEO_RREQ_FCTRL)
        PRINT_MESSAGE_NAME(IPVP_MSG_VIDEO_CORE_ABNORMAL)
        PRINT_MESSAGE_NAME(IPVP_MSG_VIDEO_SNAPSHOT_DONE)
#endif //#ifdef IF_SUPPORT_VIDEO

        // media
        PRINT_MESSAGE_NAME(IPVP_MSG_RECORD_FAIL)
        PRINT_MESSAGE_NAME(IPVP_MSG_RECORD_DONE)
        PRINT_MESSAGE_NAME(IPVP_MSG_RECORD_STEP)
        PRINT_MESSAGE_NAME(IPVP_MSG_RECORD_ABORT)
        PRINT_MESSAGE_NAME(IPVP_MSG_MEDIA_FAIL)
        PRINT_MESSAGE_NAME(IPVP_MSG_MEDIA_DONE)
        PRINT_MESSAGE_NAME(IPVP_MSG_MEDIA_STEP)
        PRINT_MESSAGE_NAME(IPVP_MSG_MEDIA_ABORT)
        PRINT_MESSAGE_NAME(IPVP_MSG_MEDIA_REPEAT)

    default:
        break;
    }

    return "NOT DEFINE IPVP MSG";

}

const char * MapDsk(int nMesg)
{
#if DEBUG_TOOLS
    APP_ERR("~~~gui message~~~");
#endif //#if DEBUG_TOOLS
    switch (nMesg)
    {

        PRINT_MESSAGE_NAME(DSK_MSG_GETLINESTATUS)
        PRINT_MESSAGE_NAME(DSK_MSG_GETWORKSTATUS)
        PRINT_MESSAGE_NAME(DSK_MSG_SELECT_CHANNEL)
        PRINT_MESSAGE_NAME(DSK_MSG_ENUM_EXTENSION)
        PRINT_MESSAGE_NAME(DSK_MSG_ACTION_URI)
        PRINT_MESSAGE_NAME(DSK_MSG_PUSH_XML)
        PRINT_MESSAGE_NAME(DSK_MSG_USER_LEVEL_CHANGED)
        PRINT_MESSAGE_NAME(DSK_MSG_WEB_QUERY_BLFLIST)
        PRINT_MESSAGE_NAME(DSK_MSG_GETPHONE_INFO)
        PRINT_MESSAGE_NAME(DSK_MSG_REDIRECTCALL)
        PRINT_MESSAGE_NAME(DSK_MSG_SETPHONE_CFG)
        PRINT_MESSAGE_NAME(DSK_MSG_LISTEN_MULTICAST)
        PRINT_MESSAGE_NAME(DSK_MSG_CLOSE_POWERLIGHT)
        PRINT_MESSAGE_NAME(DSK_MSG_ACD_RESULT)
//        PRINT_MESSAGE_NAME(DSK_MSG_WEB_BLFLISTCONFIG_CHANGE)
        PRINT_MESSAGE_NAME(DSK_MSG_MAKE_CALL)
        PRINT_MESSAGE_NAME(DSK_MSG_CSTA)
        PRINT_MESSAGE_NAME(DSK_MSG_UPLOAD_CONFIG)
        PRINT_MESSAGE_NAME(DSK_MSG_ACTION_URI_CONFIRM)
        PRINT_MESSAGE_NAME(DSK_MSG_EXT_CHANGE)
        PRINT_MESSAGE_NAME(DSK_MSG_SAVE_CALLLOG)
        PRINT_MESSAGE_NAME(DSK_MSG_LIGHT_STATUS_CHANGE)
        PRINT_MESSAGE_NAME(DSK_MSG_LIGHT_TIME_CHANGE)
        PRINT_MESSAGE_NAME(DSK_MSG_CONTRAST_CHANGE)
        PRINT_MESSAGE_NAME(DSK_MSG_LIGHT_LEVEL_CHANGE)
        //PRINT_MESSAGE_NAME(DSK_MSG_GETBTPHONESTATUS)
        PRINT_MESSAGE_NAME(DSK_MSG_GET_MEDIA_STATS)
        PRINT_MESSAGE_NAME(DSK_MSG_GET_CONTACT_UPDATESTATS)
        PRINT_MESSAGE_NAME(DSK_MSG_GET_PHONE_STATUS)
        PRINT_MESSAGE_NAME(DSK_MSG_GET_DISPLAY_INFO)


        // T48 没有空间问题，支持查询更多消息
        //键盘锁
        PRINT_MESSAGE_NAME(KEYGUARD_ENABLE_CHANGED)
        PRINT_MESSAGE_NAME(KEYGUARD_LOCK_CHANGED)
        PRINT_MESSAGE_NAME(KEYGUARD_AUTOLOCK_TIME_CHANGED)

        // dir联系人
        PRINT_MESSAGE_NAME(LOCAL_DIRECTORY_RELOAD)
        PRINT_MESSAGE_NAME(DIRECTORY_SHORTCUT_RELOAD)
        PRINT_MESSAGE_NAME(LOCAL_DIRECTORY_RELOAD_CONFIG)

        PRINT_MESSAGE_NAME(LOCAL_SEARCH_RESULT)
        PRINT_MESSAGE_NAME(SUPER_SEARCH_RESULT)

        // call log
        PRINT_MESSAGE_NAME(LOCAL_CALLLOG_MESSAGE_MISSED)
        PRINT_MESSAGE_NAME(LOCAL_CALLLOG_MESSAGE_FORWARD)



    default:
        break;
    }

    return "NOT DEFINE GUI MSG";
}


const char * MapWui(int nMesg)
{
#if DEBUG_TOOLS
    APP_ERR("~~~web message~~~");
#endif //#if DEBUG_TOOLS
    switch (nMesg)
    {
    //  PRINT_MESSAGE_NAME()
    default:
        break;
    }

    return "NOT DEFINE WUI MSG";
}

const char * MapH323(int nMesg)
{

#if DEBUG_TOOLS
    APP_ERR("~~~h323 message~~~");
#endif //#if DEBUG_TOOLS
    switch (nMesg)
    {
        // 黑白屏有空间问题，彩屏才支持
        PRINT_MESSAGE_NAME(H323_REG_MSG_BEGIN)
        PRINT_MESSAGE_NAME(H323_REG_UPDATE_STATUS)
        PRINT_MESSAGE_NAME(H323_MSG_UPDATE_SERVICE_STATUS)

        // call
        PRINT_MESSAGE_NAME(H323_CALL_MSG_BEGIN)
        PRINT_MESSAGE_NAME(H323_CALL_NEW_INCOMING)
        PRINT_MESSAGE_NAME(H323_CALL_PROCEEDING)
        PRINT_MESSAGE_NAME(H323_CALL_RINGBACK)
        PRINT_MESSAGE_NAME(H323_CALL_REFUSE)
        PRINT_MESSAGE_NAME(H323_CALL_ACCEPT)
        PRINT_MESSAGE_NAME(H323_CALL_FAIL)
        PRINT_MESSAGE_NAME(H323_CALL_RELEASED)
        PRINT_MESSAGE_NAME(H323_CALL_REMOTE_HOLD)
        PRINT_MESSAGE_NAME(H323_CALL_REMOTE_RESUME)
        PRINT_MESSAGE_NAME(H323_CALL_REMOTE_MUTE)
        PRINT_MESSAGE_NAME(H323_CALL_REMOTE_UN_MUTE)
        PRINT_MESSAGE_NAME(H323_CALL_RECV_DTMF)
        PRINT_MESSAGE_NAME(H323_CALL_OPEN_LOGIC_CHANNEL)
        PRINT_MESSAGE_NAME(H323_CALL_FORWARD_BY_REMOTE)
        PRINT_MESSAGE_NAME(H323_CALL_TRANSFER_BY_REMOTE)
        PRINT_MESSAGE_NAME(H323_CALL_REPLACE_BY_REMOTE)
        PRINT_MESSAGE_NAME(H323_CALL_EXT_VIDEO_STATE)
        PRINT_MESSAGE_NAME(H323_CALL_VIDEO_FRAME_SYNC)
        PRINT_MESSAGE_NAME(H323_CALL_CAMERA_ACTION_START)
        PRINT_MESSAGE_NAME(H323_CALL_CAMERA_ACTION_STOP)
        PRINT_MESSAGE_NAME(H323_CALL_CAMERA_PRESET_STORE)
        PRINT_MESSAGE_NAME(H323_CALL_CAMERA_PRESET_ACTIVE)
        PRINT_MESSAGE_NAME(H323_CALL_UPDATE_CODEC_LIST)
        PRINT_MESSAGE_NAME(H323_CALL_UPDATE_REMOTE_USER_AGENT)
        PRINT_MESSAGE_NAME(H323_CALL_FLOW_CONTROL_RATE)
        PRINT_MESSAGE_NAME(H323_CALL_DIAGNOSE_LISTEN_PORT)
        PRINT_MESSAGE_NAME(H323_CALL_DIAGNOSE_SIGNAL_INFO)
        PRINT_MESSAGE_NAME(H323_CALL_DIAGNOSE_MEDIA_INFO)
        PRINT_MESSAGE_NAME(H323_USER_CUSTOM_INFO)
        PRINT_MESSAGE_NAME(H323_CALL_TRANSFERED_RESULT)
        PRINT_MESSAGE_NAME(H323_RESULT_OPEN_EXT_VIDEO)
    default:
        break;
    }

    return "NOT DEFINE H323 MSG";
}

#endif //#ifdef IF_FEATURE_TOOLS


yl::string debug_GetUpTime()
{
    yl::string strInfo;
    chFileStdio fileSrc;
    if (fileSrc.Open("/proc/uptime", chFileStdio::modeRead))
    {
        while (!fileSrc.IsEOF())
        {
            fileSrc.ReadString(strInfo);
            break;
        }
    }

    int nPos = strInfo.find(".");
    if (nPos != yl::string::npos)
    {
        strInfo = strInfo.substr(0, nPos);
    }

    return strInfo;
}

void debug_PrintUptime(char * szTime/* = NULL*/)
{
    yl::string strTime = debug_GetUpTime();
    time_t nTime = atoi(strTime.c_str());

    if (nTime > 86400)
    {
        if (NULL == szTime)
        {

            printf("run time: %d:%d:%d:%d\n", nTime / 86400, (nTime % 86400 / 3600),
                   (nTime % 86400 % 3600 / 60),
                   (nTime % 86400 % 3600 % 60));
        }
        else
        {
            sprintf(szTime, "%d:%d:%d:%d\n", nTime / 86400, (nTime % 86400 / 3600), (nTime % 86400 % 3600 / 60),
                    (nTime % 86400 % 3600 % 60));
        }
    }
    else
    {
        if (NULL == szTime)
        {
            printf("run time: %dd %d:%d\n", (nTime / 3600), (nTime % 3600 / 60),
                   (nTime % 3600 % 60));
        }
        else
        {
            sprintf(szTime, " %dd %d:%d\n", (nTime / 3600), (nTime % 3600 / 60),
                    (nTime % 3600 % 60));
        }
    }
    return;
}

bool debug_GetTotalMemInfo(int & nTotal, int & nFree, int & nBuff, int & nCached)
{
    yl::string strInfo;
    chFileStdio fileSrc;
    if (fileSrc.Open("/proc/meminfo", chFileStdio::modeRead))
    {
        char szName[64] = { 0 };
        char szValue[32] = { 0 };
        int nCnt = 0;
        while (!fileSrc.IsEOF())
        {
            fileSrc.ReadString(strInfo);
            strInfo.replace(" ", "");

            //          printf("%s", strInfo.c_str());

            if (strInfo.find("MemFree") != yl::string::npos)
            {
                nCnt++;
                sscanf(strInfo.c_str(), "MemFree:%skB", szName);

                nFree = atoi(szName);
            }
            else if (strInfo.find("Buffers") != yl::string::npos)
            {
                nCnt++;
                sscanf(strInfo.c_str(), "Buffers:%skB", szName);
                nBuff = atoi(szName);
            }
            else if (strInfo.find("Cached") != yl::string::npos)
            {
                nCnt++;
                sscanf(strInfo.c_str(), "Cached:%skB", szName);
                nCached = atoi(szName);
            }
            else if (strInfo.find("MemTotal") != yl::string::npos)
            {
                nCnt++;
                sscanf(strInfo.c_str(), "MemTotal:%skB", szName);
                nTotal = atoi(szName);
            }

            //          printf("-------------[ %s %d %d %d ]\n", szName, nFree, nBuff,nCached);
            if (nCnt >= 4)
            {
                break;
            }
        }
    }

    return true;
}
