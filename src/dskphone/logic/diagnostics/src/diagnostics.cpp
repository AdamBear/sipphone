#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "dsklog/log.h"

#include "diagnostics.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "commonunits/modcommonunits.h"
#include "devicelib/networkserver.h"

#define UNDEFINEFIFO -1
#define UNDEFINE_TIMERID -1
#define knReadTimer 5

CDiagnostics::CDiagnostics(): m_strResult(""), m_ififo(UNDEFINEFIFO), m_iWtiteFifo(UNDEFINEFIFO)
{
    Init();
    m_strResult.clear();
    m_strUrl.clear();
    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, OnMessage);

}

CDiagnostics::~CDiagnostics()
{
    Init();
    m_strResult.clear();
    m_strUrl.clear();
    etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, OnMessage);
}

void CDiagnostics::Init()
{
    if (m_ififo != UNDEFINEFIFO)
    {
        close(m_ififo);
    }
    if (m_iWtiteFifo != UNDEFINEFIFO)
    {
        close(m_iWtiteFifo);
    }
    m_ififo = UNDEFINEFIFO;
    m_iWtiteFifo = UNDEFINEFIFO;
    m_iReadTimer = UNDEFINE_TIMERID;
    m_eOperateType = DIAGNOSTICSTYPE_NONE;
    remove(TEMP_PATH "diagnostics");
}

CDiagnostics * CDiagnostics::GetInstance()
{
    static CDiagnostics sinstance;
    return &sinstance;
}

yl::string CDiagnostics::GetRunCmdResult() const
{
    return m_strResult;
}

yl::string CDiagnostics::GetRunUrl() const
{
    return m_strUrl;
}

void CDiagnostics::StopSystemCmd()
{
    if (m_eOperateType == DIAGNOSTICSTYPE_PING)
    {
        commonUnits_System("killall ping");
    }
    else if (m_eOperateType == DIAGNOSTICSTYPE_TRACEROUTE)
    {
        commonUnits_System("killall traceroute");
    }
    if (m_iReadTimer != UNDEFINE_TIMERID)
    {
        timerKillThreadTimer((UINT)&m_iReadTimer);
    }
    Init();
}

bool CDiagnostics::RunSystemCmd(DIAGNOSTICSTYPE eCmd, yl::string & strTargetURL)
{
    m_strUrl = strTargetURL;

    if (strTargetURL.empty() || DIAGNOSTICSTYPE_NONE == eCmd || !netIsNetworkReady())
    {
        return false;
    }

    m_strResult.clear();
    ReplceStartToPoint(strTargetURL);

    pid_t pid;
    yl::string strTemp(TEMP_PATH "diagnostics");
    if (CreatFifo(strTemp))
    {
        pid = fork();
        if (0 > pid)
        {
            DIR_INFO("creat pid error");
            return false;
        }

        m_eOperateType = eCmd;
        if (0 == pid)
        {
            m_iWtiteFifo = open(TEMP_PATH "diagnostics", O_WRONLY);
            DIR_INFO("child ififo[%d]", m_iWtiteFifo);
            if (dup2(m_iWtiteFifo, STDOUT_FILENO) < 0)  //重定向输出到所创建的管道
            {
                DIR_INFO("dup2  STDOUT_FILENO error");
            }
            if (dup2(m_iWtiteFifo, STDERR_FILENO) < 0)
            {
                DIR_INFO("dup2  STDERR_FILENO error");
            }

            switch (eCmd)
            {
            case DIAGNOSTICSTYPE_PING:
            {
                if (execlp("ping", "ping", strTargetURL.c_str(), "-c4", NULL) < 0) //ping
                {
                    DIR_INFO("ping error");
                    return EXIT_FAILURE;
                }
            }
            break;
            case DIAGNOSTICSTYPE_TRACEROUTE:
            {
                /*
                Traceroute程序的设计是利用ICMP及IP header的TTL（Time To Live）栏位（field）。首先，traceroute送出一个TTL是1的IP datagram（其实，每次送出的为3个40字节的包，包括源地址，目的地址和包发出的时间标签）到目的地，当路径上的第一个路由器（router）收到这个datagram时，它将TTL减1。此时，TTL变为0了，所以该路由器会将此datagram丢掉，并送回一个「ICMP time exceeded」消息（包括发IP包的源地址，IP包的所有内容及路由器的IP地址），traceroute 收到这个消息后，便知道这个路由器存在于这个路径上，接着traceroute 再送出另一个TTL是2 的datagram，发现第2 个路由器...... traceroute 每次将送出的datagram的TTL 加1来发现另一个路由器，这个重复的动作一直持续到某个datagram 抵达目的地。当datagram到达目的地后，该主机并不会送回ICMP time exceeded消息，因为它已是目的地了，那么traceroute如何得知目的地到达了呢？
                Traceroute在送出UDP datagrams到目的地时，它所选择送达的port number 是一个一般应用程序都不会用的号码（30000 以上），所以当此UDP datagram 到达目的地后该主机会送回一个「ICMP port unreachable」的消息，而当traceroute 收到这个消息时，便知道目的地已经到达了。所以traceroute 在Server端也是没有所谓的Daemon 程式。
                Traceroute提取发 ICMP TTL到期消息设备的IP地址并作域名解析。每次 ，Traceroute都打印出一系列数据,包括所经过的路由设备的域名及 IP地址,三个包每次来回所花时间
                ===============================================
                traceroute命令的使用方式为：
                Usage: traceroute [-FIldnrv] [-f 1st_ttl] [-m max_ttl] [-p port#] [-q nqueries]
                [-s src_addr] [-t tos] [-w wait] [-g gateway] [-i iface]
                [-z pausemsecs] HOST [data size]

                Trace the route to HOST

                Options:
                -F      Set the don't fragment bit
                -I      Use ICMP ECHO instead of UDP datagrams
                -l      Display the ttl value of the returned packet
                -d      Set SO_DEBUG options to socket
                -n      Print hop addresses numerically rather than symbolically
                -r      Bypass the normal routing tables and send directly to a host
                -v      Verbose
                -m max_ttl      Max time-to-live (max number of hops)
                -p port#        Base UDP port number used in probes
                (default is 33434)
                -q nqueries     Number of probes per 'ttl' (default 3)
                -s src_addr     IP address to use as the source address
                -t tos          Type-of-service in probe packets (default 0)
                -w wait         Time in seconds to wait for a response
                (default 3 sec)
                -g              Loose source route gateway (8 max)
                ===========================================================

                -n    不去查询每个地址对应的域名
                -q2   每增加一跳仅发送两个探测报文，没必要发送三个
                -w2   如果发送出去的报文没有响应，最多等待2秒
                -I    使用ICMP报文而不是UDP报文
                -m30  指定TTL为30的方式（默认ttl就是30）
                */
                if (execlp("traceroute", "traceroute", strTargetURL.c_str(), "-n", "-q2", "-I", "-w2", "-m30",
                           NULL) < 0)
                {
                    DIR_INFO("traceroute error");
                    return EXIT_FAILURE;
                }
            }
            break;
            default:
                break;
            }

        }
        else
        {
            DIR_INFO("diagnostics running [%d]", m_eOperateType);
            signal(SIGCHLD,
                   SIG_IGN); //忽略SIGCHLD信号，否则子进程执行完命令会变成僵尸进程
            ReadFifo();
        }
        return true;
    }

    return false;
}


bool CDiagnostics::CreatFifo(yl::string & strFifoPath)
{
    if ((mkfifo(strFifoPath.c_str(), O_CREAT | O_RDWR | 0666)) < 0)
    {
        return false;
    }

    return true;
}

bool CDiagnostics::ReadFifo()
{
    m_strResult.clear();

    m_ififo = open(TEMP_PATH "diagnostics", O_RDONLY | O_NONBLOCK); //O_NONBLOCK 设置为非阻塞
    if (m_ififo < 0)
    {
        perror("open false\n");
        m_ififo = UNDEFINEFIFO;
        return false;
    }
    if (m_ififo != UNDEFINEFIFO)
    {
        timerKillThreadTimer((UINT)&m_iReadTimer);
    }
    if (m_iReadTimer == UNDEFINE_TIMERID)
    {
        m_iReadTimer = knReadTimer;
        timerSetThreadTimer((UINT)&m_iReadTimer, m_iReadTimer);
    }

    return true;
}

void CDiagnostics::ReplceStartToPoint(yl::string & strTargetURL)
{
    if (strTargetURL.empty())
    {
        return;
    }

    strTargetURL.replace("*", ".");
}

void CDiagnostics::NotifyResult(DIAGNOSTICSTYPE eCmd)
{
    DIR_INFO("NotifyResult: type[%d]", eCmd);
    etl_NotifyAppEx(false, DIAGNOISTICMSG_FINISH, true, eCmd, 0, NULL);
}

void CDiagnostics::OnTimer(msgObject & refObj)
{
    char pbuf[128] = {0};
    if ((UINT)&m_iReadTimer == refObj.wParam)
    {
        timerKillThreadTimer((UINT)&m_iReadTimer);
        m_iReadTimer = UNDEFINE_TIMERID;

        if (read(m_ififo, pbuf, 64) != '\0')
        {
            int length = strlen(pbuf);
            if (pbuf[length - 1] == '\032')
            {
                m_strResult.append(pbuf, length - 1);
            }
            else
            {
                m_strResult.append(pbuf);
            }
            memset(pbuf, 0, sizeof(pbuf));
            {
                m_iReadTimer = knReadTimer;
                timerSetThreadTimer((UINT)&m_iReadTimer, m_iReadTimer);
            }
        }
        else
        {
            if (m_ififo != UNDEFINEFIFO)
            {
                close(m_ififo);
            }
            if (m_iWtiteFifo != UNDEFINEFIFO)
            {
                close(m_iWtiteFifo);
            }
            m_ififo = UNDEFINEFIFO;
            m_iWtiteFifo = UNDEFINEFIFO;
            remove(TEMP_PATH "diagnostics");

            if (m_eOperateType == DIAGNOSTICSTYPE_PING)
            {
                NotifyResult(DIAGNOSTICSTYPE_PING);
            }
            else if (m_eOperateType == DIAGNOSTICSTYPE_TRACEROUTE)
            {
                NotifyResult(DIAGNOSTICSTYPE_TRACEROUTE);
            }
            m_eOperateType = DIAGNOSTICSTYPE_NONE;
        }
    }
}

LRESULT CDiagnostics::OnMessage(msgObject & refObj)
{
    bool bHandled = true;
    switch (refObj.message)
    {
    case TM_TIMER:
    {
        g_pDiagnostics->OnTimer(refObj);
    }
    break;
    default:
    {
        bHandled = false;
    }
    break;
    }

    return bHandled;
}

bool CDiagnostics::IsSystemCmdRunning()
{
    return m_eOperateType == DIAGNOSTICSTYPE_NONE ? false : true;
}

