#ifndef __TEST_PC_SERVER_H__
#define __TEST_PC_SERVER_H__

#ifdef _CP

#include "xwindow-kernel/include/xmessagehandler.h"
#include "ETLLib/ETLSocket.h"

enum PackageType
{
    PK_PH2PC_TestModeEnd,
    PK_PH2PC_TestModeNEnd,
    PK_PH2PC_WriteSuccess,
    PK_PH2PC_WriteFail,
    PK_PC2PH_TestModeSuccess,
    PK_PC2PH_TestModeFail
};

struct TestModePackage
{
    int pkType;
};

///////////////////////////CPCServer///////////////////////////////////
class CPCServer : public xMessageHandler
{
public:
    CPCServer();
    virtual ~CPCServer();
    // 初始化
    bool Init();

    bool SendTestmodeCompleted();
private:
    // 处理TCP 客户端连接问题
    BOOL OnAcceptClient(msgObject &);
    // 处理通道数据接收任务
    BOOL OnReadChannelData(msgObject & msg);
    // 检测TCP 客户端是否有用
    bool CheckClientValid();

    bool SendPackage(const TestModePackage & stPackageInfo);
    bool OnReceivePCPackage(const TestModePackage & stPackageInfo);
private:

    chSocketTCPServer   m_socketServer;
    chSocketPacketTCP   m_sockTcpClient;
    chSocketAddr        m_sockClientAddr;
};

#endif //_CP

#endif //__TEST_PC_SERVER_H__
