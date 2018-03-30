#ifdef _CP

#include "pcserver.h"
#include <ETLLib.hpp>
#include "xwindow-kernel/include/xkernel.h"
#include "xwindow/xWindowManager.h"
#include "commonunits/modcommonunits.h"
#ifndef WIN32
#include <signal.h>
#endif
/////////////////////////////CPCServer/////////////////////////////////
CPCServer::CPCServer()
{

}

CPCServer::~CPCServer()
{
#if LINUX_SYSTEM
    g_WorkStation.DisconnectMsgReadHandler(m_socketServer.m_hSocket);
    g_WorkStation.DisconnectMsgReadHandler(m_sockTcpClient.m_hSocket);
#endif
    m_sockTcpClient.DestroySocket();
    m_socketServer.DestroySocket();
}

bool CPCServer::Init()
{
    BOOL bRet = m_socketServer.CreateSocket(chSocketAddr("0.0.0.0", 6001));
    if (!bRet)
    {
        TESTMODE_WARN("Fail to bind TCP:6001");
        return false;
    }

    m_socketServer.SocketListen(10);
#if LINUX_SYSTEM
    g_WorkStation.ConnectMsgReadHandler(m_socketServer.m_hSocket, MSG_METHOD(this,
                                        CPCServer::OnAcceptClient));
#endif
    return true;
}

BOOL CPCServer::OnAcceptClient(msgObject &)
{
    if (m_sockTcpClient.IsTCPConnected() || m_sockTcpClient.IsValidSocket())
    {
        //做抢线处理
        TESTMODE_INFO("Accept New Connection. Close Connection[%s].",
                      m_sockClientAddr.GetIPString(TRUE).c_str());
#if LINUX_SYSTEM
        g_WorkStation.DisconnectMsgReadHandler(m_sockTcpClient.m_hSocket);
#endif
        m_sockTcpClient.DestroySocket();
    }

    if (m_socketServer.AcceptOnSocket(m_sockTcpClient, &m_sockClientAddr, 1000))
    {
        TESTMODE_INFO("TCP Client connected [%s].", m_sockClientAddr.GetIPString(TRUE).c_str());
#if LINUX_SYSTEM
        g_WorkStation.ConnectMsgReadHandler(m_sockTcpClient.m_hSocket, MSG_METHOD(this,
                                            CPCServer::OnReadChannelData));
#endif
    }

    return TRUE;
}

bool CPCServer::CheckClientValid()
{
    if (!m_sockTcpClient.IsTCPConnected() && m_sockTcpClient.IsValidSocket())
    {
        TESTMODE_WARN("TCP Client disconnected [%s].", m_sockClientAddr.GetIPString(TRUE).c_str());
#if LINUX_SYSTEM
        g_WorkStation.DisconnectMsgReadHandler(m_sockTcpClient.m_hSocket);
#endif
        m_sockTcpClient.DestroySocket();
        return false;
    }
    return true;
}

BOOL CPCServer::OnReadChannelData(msgObject & msg)
{
    int readCount = 0;
    if (!CheckClientValid())
    {
        return FALSE;
    }

    // read tcp client data
    readCount = m_sockTcpClient.socketReceive(sizeof(TestModePackage));

    if (readCount != sizeof(TestModePackage))
    {
        TESTMODE_WARN("OnReadChannelData readCount is %d.", readCount);
        return FALSE;
    }

    //get tcp client data
    if (!m_sockTcpClient.m_tcpPacketBuffer.IsEmpty())
    {
        //读取数据
        TestModePackage stReceivePackage;
        memcpy(&stReceivePackage, m_sockTcpClient.m_tcpPacketBuffer.GetBuffer(), sizeof(TestModePackage));
        OnReceivePCPackage(stReceivePackage);
        m_sockTcpClient.m_tcpPacketBuffer.Clear();
    }
    return TRUE;
}

bool CPCServer::SendTestmodeCompleted()
{
    TestModePackage stPackage;
    stPackage.pkType = PK_PH2PC_TestModeEnd;
    return SendPackage(stPackage);
}

bool CPCServer::SendPackage(const TestModePackage & stPackageInfo)
{
    if (!CheckClientValid())
    {
        return false;
    }

    BOOL bSend = FALSE;

#ifndef WIN32
    TESTMODE_INFO("SendPackage : %d", stPackageInfo.pkType);
    sighandler_t  sigHandle = signal(SIGPIPE, SIG_IGN);
    bSend = m_sockTcpClient.Send((LPCSTR)&stPackageInfo, sizeof(TestModePackage));
    signal(SIGPIPE, sigHandle);
#else
    bSend = m_sockTcpClient.Send((LPCSTR)&stPackageInfo, sizeof(TestModePackage));
#endif

    if (!bSend)
    {
        return false;
    }
    return true;
}

bool CPCServer::OnReceivePCPackage(const TestModePackage & stPackageInfo)
{
    switch (stPackageInfo.pkType)
    {
    case PK_PC2PH_TestModeSuccess:
    case PK_PC2PH_TestModeFail:
        {
            bool bTestmodeFlag = false;
            if (PK_PC2PH_TestModeSuccess == stPackageInfo.pkType)
            {
                bTestmodeFlag = true;
            }

            TestModePackage stPackageInfo;

            if (commUnits_SetResetFactoryEnable(bTestmodeFlag))
            {
                TESTMODE_INFO("write testmode flag %d success!", bTestmodeFlag);
                stPackageInfo.pkType = PK_PH2PC_WriteSuccess;
            }
            else
            {
                TESTMODE_INFO("write testmode flag %d fail!", bTestmodeFlag);
                stPackageInfo.pkType = PK_PH2PC_WriteFail;
            }

            if (SendPackage(stPackageInfo))
            {
                TESTMODE_INFO("send stPackageInfo %d success!", stPackageInfo.pkType);
            }
            else
            {
                TESTMODE_INFO("send stPackageInfo %d fail!", stPackageInfo.pkType);
            }
        }
        break;
    default:
        break;
    }
    return true;
}

#endif
