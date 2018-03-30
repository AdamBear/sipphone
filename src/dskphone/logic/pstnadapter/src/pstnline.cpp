#include "pstnline.h"

#include "pstnadapterdatadefine.h"
#include "pstnadaptermessagedefine.h"
#include "iocommand.h"

#include "pstn_audio_api.h"
#include "talk/talkhelp/include/modtalkhelp.h"

#ifdef IF_FEATURE_PSTN

#if !WIN32
#include <configparser/modconfigparser.h>
#include <configiddefine.h>
#include <dsklog/log.h>
#include <common/common_data_define.h>
#include <etlmsghandler/modetlmsghandler.h>
#include <service_sip_define.h>
#include <voip/modvoip.h>
#endif // !WIN32

#define OPEN_RETRY_INTERVAL     1000 // IO串口文件打开重试间隔
#define SYNC_READ_TIMEOUT       5000 // 同步IO读取最大时长
#define MAX_RING_INTERVAL       7000 // 最大振铃间隔（低电平超过这个时间则视为挂机）
#define MAX_CID_INTERVAL        500 // 最大送号间隔（CID超过这个时间视为两个独立来电）
#define DIAL_DELAY              1500 // 摘机与拨号操作间隔（摘机后延迟多长时间才能拨号）
#define DIAL_DIGIT_INTERVAL     200 // 拨号时每位间隔
#define FLASH_TIMEOUT           1000 // Flash超时时间

#define MAX_COMMAND_LINE_SIZE   128 // IO命令最大长度

#if WIN32
#define MAX_SYNC_READ_RETRY     1 // 同步读最大重试次数
#else
#define MAX_SYNC_READ_RETRY     10 // 同步读最大重试次数
#endif // WIN32

#define timerSetThreadTimer0(timer, interval) do\
{\
    COMMONUNITS_INFO("timerSetThreadTimer(%lu, %d)", timer, interval);\
    msgPostMsgToThread(PSTN_MGR_THREAD_HANDLE, PSTN_MSG_SET_TIMER, timer, interval);\
} while (false)
#define timerKillThreadTimer(timer) do\
{\
    COMMONUNITS_INFO("timerKillThreadTimer(%lu)", timer);\
    msgPostMsgToThread(PSTN_MGR_THREAD_HANDLE, PSTN_MSG_KILL_TIMER, timer, 0);\
} while (false)


CPstnLine::CPstnLine(int iDeviceId)
    : m_iDeviceId(iDeviceId)
    , m_iHandle(-1)
    , m_iAudioHandle(-1)
    , m_tidSerialReadThread(0)
    , m_bLinePluggedIn(false)
    , m_iLineId(0)
    , m_iCallId(0)
    , m_bCidComplete(false)
    , m_eStatus(PLS_IDLE)
    , m_bEnable(true)
    , m_bFlashing(false)
    , m_bCidAfterRing(false)
{
    COMMONUNITS_INFO("CPstnLine::CPstnLine #%d", m_iDeviceId);
}

CPstnLine::~CPstnLine()
{
    COMMONUNITS_INFO("CPstnLine::~CPstnLine #%d", m_iDeviceId);

    // 关闭所有定时器
    timerKillThreadTimer(TIMER_ID(m_iHandle));
    timerKillThreadTimer((UINT)this);
    timerKillThreadTimer(TIMER_ID(m_iCallId));
    timerKillThreadTimer(TIMER_ID(m_strCid));
    timerKillThreadTimer(TIMER_ID(m_strDial));

    // PSTN Box设备拔出时，PSTN线路同样拔出
    if (m_bLinePluggedIn)
    {
        COMMONUNITS_INFO("PSTN line pull out with PSTN Box!");
        m_bLinePluggedIn = false;
        // 广播消息
        etl_NotifyAppEx(false, PSTN_MSG_LINE_CHANGED, m_iDeviceId, PSTN_LINE_DISCONNECTED, 0, NULL);
    }

    // 关闭设备
    if (m_iHandle > 0)
    {
        //Send(IO_CMD_OPEN, IO_VALUE_OFF); // 在设备移除后向串口发送命令必然失败
        close(m_iHandle);
        m_iHandle = -1;
    }
    // 关闭语音句柄
    if (m_iAudioHandle > 0)
    {
        close(m_iAudioHandle);
        m_iAudioHandle = -1;
    }
}

bool CPstnLine::OpenSerial()
{
#if WIN32
    char pDevNode[MAX_PATH] = "";
    sprintf(pDevNode, "COM%d", m_iDeviceId);
    COMMONUNITS_INFO("Opening PSTN Box device: %s", pDevNode);
    m_iHandle = (int)CreateFile(pDevNode,   //设备名
                                GENERIC_READ | GENERIC_WRITE,       //访问模式，可同时读写
                                0,                                  //共享模式，0表示不共享
                                NULL,                               //安全性设置，一般使用NULL
                                OPEN_EXISTING,                      //该参数表示该设备必须存在否则创建失败，串口通讯需此设置
                                FILE_FLAG_OVERLAPPED,               // 异步
                                NULL);
#else
    char pDevNode[PATH_MAX] = "";
    sprintf(pDevNode, "/dev/ttyACM%d", m_iDeviceId);
    m_iHandle = open(pDevNode, O_RDWR | O_NOCTTY);
#endif // WIN32
    if (m_iHandle < 0)
    {
#if WIN32
        COMMONUNITS_WIN32_LOG("Cannot open PSTN Box device");
#else
        COMMONUNITS_ERR("Cannot open PSTN Box device [%s] ! Error:[%d]%s", pDevNode, errno,
                        strerror(errno));
        timerSetThreadTimer0(TIMER_ID(m_iHandle), OPEN_RETRY_INTERVAL);
#endif // WIN32
        return false;
    }
    timerKillThreadTimer(TIMER_ID(m_iHandle));
    COMMONUNITS_INFO("PSTN Box device [%s] opened!", pDevNode);

#if WIN32
    // 设置波特率等参数
    DCB dcb;
    ZeroMemory(&dcb, sizeof(dcb));
    if (GetCommState((HANDLE)m_iHandle, &dcb))
    {
        dcb.BaudRate = 115200;
        dcb.fDtrControl = 1;
        dcb.fRtsControl = 0;
        dcb.XonLim = dcb.XoffLim = 256;
        dcb.ByteSize = 8;
        dcb.XonChar = 17;
        dcb.XoffChar = 19;
        if (!SetCommState((HANDLE)m_iHandle, &dcb))
        {
            COMMONUNITS_WARN("Cannot set common state!");
        }
    }

    //设置串口事件
    SetCommMask((HANDLE)m_iHandle, EV_RXCHAR); //在缓存中有字符时产生事件
    SetupComm((HANDLE)m_iHandle, 16384, 16384);

    //设置串口读写时间
    COMMTIMEOUTS CommTimeOuts;
    GetCommTimeouts((HANDLE)m_iHandle, &CommTimeOuts);
    CommTimeOuts.ReadIntervalTimeout = MAXDWORD;
    CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
    CommTimeOuts.ReadTotalTimeoutConstant = 0;
    CommTimeOuts.WriteTotalTimeoutMultiplier = 10;
    CommTimeOuts.WriteTotalTimeoutConstant = 1000;

    if (!SetCommTimeouts((HANDLE)m_iHandle, &CommTimeOuts))
    {
        COMMONUNITS_WARN("Cannot set timeout!");
    }

    //清空串口
    PurgeComm((HANDLE)m_iHandle, PURGE_RXCLEAR | PURGE_TXCLEAR);

#else
    struct termios options;
    //tcgetattr函数用于获取与终端相关的参数。参数fd为终端的文件描述符
    if (tcgetattr(m_iHandle, &options) != 0)
    {
        COMMONUNITS_ERR("Failed when invoke 'tcgetattr' for PSTN Box device! Error:[%d]%s", errno,
                        strerror(errno));
    }

    //设置串口波特率
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    /*忽略调制解调器线路状态,使用接收器*/
    options.c_cflag |= CLOCAL | CREAD;
    /*不使用流控*/
    options.c_cflag &= ~CRTSCTS;
    /*8个数据位*/
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    /*无奇偶校验位*/
    options.c_cflag &= ~PARENB;
    options.c_iflag &= ~INPCK;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    /*1个停止位*/
    options.c_cflag &= ~CSTOPB;
    options.c_oflag &= ~OPOST;
    options.c_iflag &= ~(INLCR | ICRNL | IGNCR | IXON);
    options.c_oflag &= ~(ONLCR | OCRNL);
    options.c_cc[VTIME] = 1;
    options.c_cc[VMIN] = 1;

    tcflush(m_iHandle, TCIFLUSH);

    /*激活配置*/
    if (tcsetattr(m_iHandle, TCSANOW, &options) != 0)
    {
        COMMONUNITS_ERR("Failed when invoke 'tcsetattr' for PSTN Box device! Error:[%d]%s", errno,
                        strerror(errno));
    }
#endif // WIN32
    if (StartReadThread())
    {
        OpenAudioHandle();
        LineInit();
        return true;
    }
    return false;
}

bool CPstnLine::CloseSerial()
{
    return KillReadThread();
}

BOOL CPstnLine::OnTimer(UINT nTimerID)
{
    // 处理定时器
    if (TIMER_ID(m_iHandle) == nTimerID)
    {
        // 重试打开设备文件
        OpenSerial();
        return TRUE;
    }
    if (TIMER_ID(m_iCallId) == nTimerID)
    {
        COMMONUNITS_INFO("CPstnLine timer of ringing.");
        timerKillThreadTimer(nTimerID);
        if (IsRinging())
        {
            // 振铃中断超过MAX_RING_INTERVAL毫秒，视为对方挂机，广播消息
            etl_NotifyAppEx(false, PSTN_MSG_INCOMING_CANCEL, m_iDeviceId, m_iCallId, 0, NULL);
            // 重置状态
            ResetStatus();
        }
        return TRUE;
    }
    if (TIMER_ID(m_strCid) == nTimerID)
    {
        COMMONUNITS_INFO("CPstnLine timer of CID.");
        timerKillThreadTimer(nTimerID);
        // CID间隔时间超过MAX_CID_INTERVAL毫秒，视为接收完整，广播消息
        COMMONUNITS_INFO("Incoming PSTN call #%d number: %s[length=%d, size=%d]", m_iCallId,
                         m_strCid.c_str(), m_strCid.length(), m_strCid.size());
        if (m_bEnable)
        {
            CidMsgInfo stTemp;
            int nSize = m_strCid.length();
            if (nSize > NUM_LEN_MAX)
            {
                nSize = NUM_LEN_MAX;
            }
            memcpy(stTemp.m_szNumber, m_strCid.c_str(), nSize);
            stTemp.m_bCidAfterRing = m_bCidAfterRing;
            etl_NotifyAppEx(false, PSTN_MSG_INCOMING_CID, m_iDeviceId, m_iCallId, sizeof(CidMsgInfo), &stTemp);
            //etl_NotifyAppEx(false, PSTN_MSG_INCOMING_CID, m_iDeviceId, m_iCallId, m_strCid.length() + 1, m_strCid.c_str());
        }
        else
        {
            COMMONUNITS_INFO("Disabled, omitting incoming CID.");
        }
        m_bCidComplete = true;
        return TRUE;
    }
    if (TIMER_ID(m_strDial) == nTimerID)
    {
        COMMONUNITS_INFO("CPstnLine timer of dial.");
        timerKillThreadTimer(TIMER_ID(m_strDial));
        DialDigit();
        return TRUE;
    }
    if (TIMER_ID(m_bFlashing) == nTimerID)
    {
        m_bFlashing = false;
        timerKillThreadTimer(TIMER_ID(m_bFlashing));
        COMMONUNITS_WARN("Flash on PSTN line #%d time out!", m_iDeviceId);
        bool bResult = false;
        etl_NotifyAppEx(false, PSTN_MSG_FLASH_RESULT, m_iDeviceId, m_iCallId,
                        sizeof(bool), &bResult);
        return TRUE;
    }
    return FALSE;
}

bool CPstnLine::IsConnected()
{
    return m_bLinePluggedIn;
}

void CPstnLine::SetEnable(bool bEnable)
{
    if (!m_bEnable && bEnable)
    {
        if (IsRinging())
        {
            if (m_bCidComplete)
            {
                CidMsgInfo stTemp;
                int nSize = m_strCid.length();
                if (nSize > NUM_LEN_MAX)
                {
                    nSize = NUM_LEN_MAX;
                }
                memcpy(stTemp.m_szNumber, m_strCid.c_str(), nSize);
                etl_NotifyAppEx(false, PSTN_MSG_INCOMING_CID, m_iDeviceId, m_iCallId, sizeof(CidMsgInfo), &stTemp);
            }
            else
            {
                etl_NotifyAppEx(false, PSTN_MSG_INCOMING_CALL, m_iDeviceId, m_iCallId, 0, NULL);
            }
        }
    }
    m_bEnable = bEnable;
}

bool CPstnLine::SetHook(bool bOffHook, bool bSync /* = true */)
{
    PSTN_LINE_STATUS eStatus = GetStatus();
    if ((bOffHook && eStatus == PLS_TALKING)
            || ((!bOffHook) && eStatus != PLS_TALKING))
    {
        COMMONUNITS_WARN("PSTN line status not match! status: %d", eStatus);
        return false;
    }

    bool bResult = false;
    if (bSync)
    {
        // bSync为true时才同步读取
        char szValue[MAX_COMMAND_LINE_SIZE] = { 0 };
        bResult = SendAndRecv(IO_CMD_HOOK, (bOffHook ? IO_VALUE_OFF : IO_VALUE_ON), szValue);
        if (bResult)
        {
            COMMONUNITS_INFO("PSTN Box set hook result: %s", szValue);
            bResult = strcmp(szValue, IO_VALUE_OK) == 0;
        }
    }
    else
    {
        bResult = Send(IO_CMD_HOOK, (bOffHook ? IO_VALUE_OFF : IO_VALUE_ON));
    }
    //if (bResult) FIXME: 不管串口操作是否成功均设置对应状态，与UI逻辑保持一致，但可能与CPN10设备状态不一致。
    {
        if (bOffHook)
        {
            // 摘机则中断振铃
            timerKillThreadTimer(TIMER_ID(m_iCallId));
            SetStatus(PLS_TALKING);
            SetAudioEnable(true); // 通话时启用语音
        }
        else
        {
            // 挂机重置状态
            ResetStatus();
            SetAudioEnable(false); // 挂机后禁用语音
        }
    }
    return bResult;
}

bool CPstnLine::Flash()
{
    PSTN_LINE_STATUS eStatus = GetStatus();
    if (eStatus != PLS_TALKING)
    {
        COMMONUNITS_WARN("PSTN line status not match! status: %d", eStatus);
        return false;
    }
    // 先设置flash状态（避免Send命令还没返回就收到回复）
    m_bFlashing = true;
    // 发送flash命令
    bool bResult = Send(IO_CMD_HOOK, "flash");
    if (!bResult)
    {
        // 命令发送失败，则清除flash状态
        m_bFlashing = false;
    }
    return bResult;
}

bool CPstnLine::IsFlashing()
{
    return m_bFlashing;
}

bool CPstnLine::SetMute(bool bMute)
{
    return Send(IO_CMD_MUTE, (bMute ? IO_VALUE_ON : IO_VALUE_OFF));
}

bool CPstnLine::SetHold(bool bHold)
{
    // Hold通话时也可以禁用PSTN语音功能，优化性能
    if (bHold)
    {
        SetAudioEnable(false);
    }
    else
    {
        SetAudioEnable(true);
    }
    return Send(IO_CMD_HOLD, (bHold ? IO_VALUE_ON : IO_VALUE_OFF));
}

bool CPstnLine::SetDND(bool bDND)
{
    return Send(IO_CMD_DND, (bDND ? IO_VALUE_ON : IO_VALUE_OFF));
}

bool CPstnLine::SetCountryCode(const yl::string & strCountryCode /* = "" */,
                               bool bSync /* = true */)
{
    yl::string strCode = strCountryCode;
    if (strCountryCode.empty())
    {
        // 如果传入值为空则直接从配置中读取
        strCode = configParser_GetConfigString(kszCurrentCountry);
    }
    bool bResult = false;
    if (bSync)
    {
        // bSync为true时才同步读取
        char szValue[MAX_COMMAND_LINE_SIZE] = { 0 };
        bResult = SendAndRecv(IO_CMD_SET_COUNTRY, strCode.c_str(), szValue);
        if (bResult)
        {
            COMMONUNITS_INFO("PSTN Box set country code: %s", szValue);
            bResult = strcmp(szValue, IO_VALUE_OK) == 0;
        }
    }
    else
    {
        bResult = Send(IO_CMD_SET_COUNTRY, strCode.c_str());
    }
    if (bResult)
    {
        bResult = SetFlashDuration();
    }
    return bResult;
}

yl::string CPstnLine::GetCountryCode()
{
    char szValue[MAX_COMMAND_LINE_SIZE] = { 0 };
    if (SendAndRecv(IO_CMD_GET_COUNTRY, NULL, szValue))
    {
        COMMONUNITS_INFO("PSTN Box get country code: %s", szValue);
    }
    return szValue;
}

bool CPstnLine::SetFlashDuration(bool bForce /* = false */)
{
    yl::string strDuration = configParser_GetConfigString(kszFlashDuration);
    if (strDuration == "-1")
    {
        // Auto
        if (bForce)
        {
            // 重新设置国家代码
            return SetCountryCode();
        }
        else
        {
            return true;
        }
    }
    else
    {
        return Send(IO_CMD_SET_FLASH_DURATION, strDuration.c_str());
    }
}

bool CPstnLine::Dial(const yl::string & strNumber, int iCallId)
{
    PSTN_LINE_STATUS eStatus = GetStatus();
    switch (eStatus)
    {
    case CPstnLine::PLS_IDLE:
    {
        m_iCallId = iCallId;
        // FIXME: 对m_strDial并行读写，是否需要锁？
        m_strDial += strNumber;

        // 空闲状态先摘机（在摘机后进行拨号）
        // 此处必须用异步调用，因为后续操作需要设置定时器，此处设置的是主线程定时器，读取线程中设置的才是对应线程的定时器。
        bool bRet = SetHook(true, false);
        if (!bRet)
        {
            m_strDial.clear();
        }
        return bRet;
        break;
    }
    case CPstnLine::PLS_TALKING:
    {
        // 通话状态直接拨号
        char szValue[MAX_COMMAND_LINE_SIZE] = { 0 };
        if (SendAndRecv(IO_CMD_DIAL, strNumber.c_str(), szValue))
        {
            COMMONUNITS_INFO("PSTN Box dial(send DTMF) result: %s", szValue);
            return (strcmp(szValue, IO_VALUE_OK) == 0);
        }
        break;
    }
    case CPstnLine::PLS_RINGING:
    default:
        COMMONUNITS_WARN("Unknown status[%d] when dialing!", eStatus);
        break;
    }
    return false;
}

int CPstnLine::GetLineId()
{
    return m_iLineId;
}

bool CPstnLine::IsAvailable()
{
    PSTN_LINE_STATUS eStatus = GetStatus();
    COMMONUNITS_INFO("PSTN line status is: %d,%s available",
                     eStatus, (eStatus == PLS_IDLE ? "" : " not"));
    return eStatus == PLS_IDLE;
}

yl::string CPstnLine::GetVersion()
{
    char szValue[MAX_COMMAND_LINE_SIZE] = { 0 };
    if (SendAndRecv(IO_CMD_GET_VERSION, NULL, szValue))
    {
        COMMONUNITS_INFO("PSTN Box version: %s", szValue);
    }
    return szValue;
}

yl::string CPstnLine::GetFxoId()
{
    char szValue[MAX_COMMAND_LINE_SIZE] = { 0 };
    if (SendAndRecv(IO_CMD_GET_FXOID, NULL, szValue))
    {
        COMMONUNITS_INFO("PSTN Box FXO ID: %s", szValue);
    }
    return szValue;
}

bool CPstnLine::Upgrade(const yl::string & strRomPath, UpgradeCallback pCallback, void * pParam)
{
#if WIN32
    // 进入升级模式
    char szValue[MAX_COMMAND_LINE_SIZE] = { 0 };
    if (Send(IO_CMD_UPGRADE, "enter") && DoRead(IO_CMD_UPGRADE, szValue))
    {
        COMMONUNITS_INFO("PSTN Box enter upgrade mode: %s", szValue);
        if (strcmp(szValue, "ok") == 0)
        {
            // Xmodem升级
            return XmodemUpgrade(strRomPath, pCallback, pParam);
        }
    }

#endif // WIN32
    return false;
}

#if WIN32
uint16_t CPstnLine::CRC16(const uint8_t * pBuffer, size_t iLen)
{
    static const unsigned short crc16tab[256] =
    {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
        0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
        0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
        0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
        0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
        0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
        0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
        0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
        0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
        0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
        0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
        0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
        0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
        0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
        0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
        0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
        0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
        0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
        0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
        0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
        0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
        0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
    };

    register unsigned short crc = 0;
    for (register int counter = 0; counter < iLen; counter++)
    {
        crc = (crc << 8) ^ crc16tab[((crc >> 8) ^ * (char *)pBuffer++) & 0x00FF];
    }
    return crc;
}

#define XMODEM_SOH ((uint8_t)0x01) // Xmodem 数据头
#define XMODEM_STX ((uint8_t)0x02) // 1K-Xmodem 数据头
#define XMODEM_EOT ((uint8_t)0x04) // 发送结束
#define XMODEM_ACK ((uint8_t)0x06) // 认可响应
#define XMODEM_NAK ((uint8_t)0x15) // 不认可响应
#define XMODEM_CAN ((uint8_t)0x18) // 撤销传送
#define XMODEM_EOF ((uint8_t)0x1A) // 填充数据包
#define XMODEM_MAX_RETRY_TIMES 100 // 最大重试次数
#define XMODEM_PACKET_BUFFER_SIZE 128 // 报文缓冲区大小
bool CPstnLine::XmodemUpgrade(const yl::string & strRomPath, UpgradeCallback pCallback,
                              void * pParam)
{
#pragma pack(1)
    struct XmodemPacket
    {
        uint8_t command;
        uint8_t pos;
        uint8_t cpos;
        uint8_t data[XMODEM_PACKET_BUFFER_SIZE];
        uint8_t crc_hi;
        uint8_t crc_lo;
    };
#pragma pack()

    // 关闭并重新打开串口
    CloseHandle((HANDLE)m_iHandle);
    Sleep(3000); // 延时确保CPN10重启完毕
    char pDevNode[MAX_PATH] = "";
    sprintf(pDevNode, "COM%d", m_iDeviceId);
    COMMONUNITS_INFO("Opening PSTN Box device: %s", pDevNode);
    m_iHandle = (int)CreateFile(pDevNode,   // 设备名
                                GENERIC_READ | GENERIC_WRITE,       // 访问模式，可同时读写
                                0,                                  // 共享模式，0表示不共享
                                NULL,                               // 安全性设置，一般使用NULL
                                OPEN_EXISTING,                      // 该参数表示该设备必须存在否则创建失败，串口通讯需此设置
                                FILE_ATTRIBUTE_NORMAL,              // 升级采用同步模式
                                NULL);
    if (m_iHandle < 0)
    {
        COMMONUNITS_WIN32_LOG("Cannot open PSTN Box device");
        return false;
    }
    COMMONUNITS_INFO("PSTN Box device [%s] opened!", pDevNode);

    int nRetryTimes = 0; // 重试次数
    for (nRetryTimes = 0; nRetryTimes < XMODEM_MAX_RETRY_TIMES; ++nRetryTimes)
    {
        // 读取缓冲区
        char chCommand = '\0';
        int nRead = 0;
        nRead = read(m_iHandle, &chCommand, 1);
        if (nRead == 1 && chCommand == 'C')
        {
            break;
        }
        COMMONUNITS_WARN("Wrong char [read=%d]: [0x%02X]%c", nRead, chCommand, chCommand);
        Sleep(500);
    }

    // 等待'C'字符重试次数达到上限
    if (nRetryTimes == XMODEM_MAX_RETRY_TIMES)
    {
        COMMONUNITS_ERR("Max retry times when waiting starting 'C'!");
        return false;
    }

    HANDLE hRomFile = CreateFile(strRomPath.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL, NULL);
    if (hRomFile < 0)
    {
        COMMONUNITS_ERR("Cannot open ROM file '%s'!", strRomPath.c_str());
        return false;
    }
    DWORD nTotalSize = GetFileSize(hRomFile, NULL);
    DWORD nBytesSent = 0;
    uint8_t nPacketNo = 1;
    XmodemPacket packet;
    while (nBytesSent < nTotalSize)
    {
        DWORD nBytesRead = 0;
        if (!ReadFile(hRomFile, packet.data, XMODEM_PACKET_BUFFER_SIZE, &nBytesRead, NULL))
        {
            COMMONUNITS_WIN32_LOG("Cannot read from ROM file");
            return false;
        }
        if (nBytesRead != XMODEM_PACKET_BUFFER_SIZE)
        {
            COMMONUNITS_WARN("Cannot fill buffer! %d != %d", nBytesRead, XMODEM_PACKET_BUFFER_SIZE);
            // 此处应该继续，因为可能ROM文件不是XMODEM_PACKET_BUFFER_SIZE的整数倍，使用0x1A补足
        }

        // 准备发送报文
        packet.command = XMODEM_SOH;
        packet.pos = nPacketNo;
        packet.cpos = ~nPacketNo;
        for (size_t i = nBytesRead; i < XMODEM_PACKET_BUFFER_SIZE; ++i)
        {
            packet.data[i] = XMODEM_EOF;
        }
        uint16_t nCrc = CRC16(packet.data, XMODEM_PACKET_BUFFER_SIZE);
        packet.crc_hi = (nCrc >> 8) & 0xFF;
        packet.crc_lo = nCrc & 0xFF;

        // 发送报文
        int nBytesWrite = write(m_iHandle, (const char *)&packet, sizeof(packet));
        COMMONUNITS_INFO("Send Xmodem packet to PSTN Box: %d", nBytesWrite);
        if (nBytesWrite != sizeof(packet))
        {
            COMMONUNITS_WARN("Something wrong when sending Xmodem packet!");
        }
        for (nRetryTimes = 0; nRetryTimes < XMODEM_MAX_RETRY_TIMES; ++nRetryTimes)
        {
            // 读取缓冲区
            char chCommand = '\0';
            int nRead = 0;
            nRead = read(m_iHandle, &chCommand, 1);
            if (nRead == 1)
            {
                if (chCommand == XMODEM_ACK)
                {
                    COMMONUNITS_INFO("Receive ack command.");
                    break;
                }
                else if (chCommand == XMODEM_NAK)
                {
                    // 重发
                    int nBytesWrite = write(m_iHandle, (const char *)&packet, sizeof(packet));
                    COMMONUNITS_INFO("Send Xmodem packet to PSTN Box: %d", nBytesWrite);
                    if (nBytesWrite != sizeof(packet))
                    {
                        COMMONUNITS_WARN("Something wrong when sending Xmodem packet!");
                    }
                    continue;
                }
                else if (chCommand == XMODEM_CAN)
                {
                    COMMONUNITS_ERR("Upgrade cancelled by PSTN Box!");
                    return false;
                }
                COMMONUNITS_WARN("Unknown command: 0x%02X", chCommand);
            }
            COMMONUNITS_WARN("Cannot read command! [%d]", nRead);
        }

        // 发送报文重试次数达到上限
        if (nRetryTimes == XMODEM_MAX_RETRY_TIMES)
        {
            COMMONUNITS_ERR("Max retry times when sending Xmodem packet!");
            return false;
        }

        // 修改发送进度并回调
        nBytesSent += nBytesRead;
        ++nPacketNo;
        if (pCallback)
        {
            pCallback(m_iDeviceId, nBytesSent, nTotalSize, pParam);
        }
    }

    // 发送结束命令
    for (nRetryTimes = 0; nRetryTimes < XMODEM_MAX_RETRY_TIMES; ++nRetryTimes)
    {
        // 发送
        char chCommandEot = XMODEM_EOT;
        int nBytesWrite = write(m_iHandle, &chCommandEot, 1);
        COMMONUNITS_INFO("Send Xmodem command 'EOT' to PSTN Box: %d", nBytesWrite);
        if (nBytesWrite != 1)
        {
            COMMONUNITS_WARN("Something wrong when sending Xmodem command 'EOT'!");
        }

        // 读取缓冲区
        char chCommand = '\0';
        int nRead = 0;
        nRead = read(m_iHandle, &chCommand, 1);
        if (nRead == 1)
        {
            if (chCommand == XMODEM_ACK)
            {
                COMMONUNITS_INFO("Receive ack command. Upgrade finished!");
                return true;
            }
            COMMONUNITS_WARN("Unknown command: 0x%02X", chCommand);
        }
        COMMONUNITS_WARN("Cannot read command! [%d]", nRead);
    }

    // 发送结束命令重试次数达到上限
    if (nRetryTimes == XMODEM_MAX_RETRY_TIMES)
    {
        COMMONUNITS_ERR("Max retry times when sending Xmodem command 'EOT'!");
        return false;
    }

    return false;
}
#endif // WIN32

bool CPstnLine::StartReadThread()
{
    int iRet = pthread_create(&m_tidSerialReadThread, NULL, CPstnLine::SerialReadThread, this);
    if (iRet != 0)
    {
        COMMONUNITS_ERR("Cannot create serial read thread! [%d][%d]", iRet, m_tidSerialReadThread);
        m_tidSerialReadThread = 0;
    }
    return iRet == 0;
}

bool CPstnLine::KillReadThread()
{
    if (m_tidSerialReadThread == 0)
    {
        COMMONUNITS_INFO("Serial read thread not created.");
        return true;
    }
    int iRet = pthread_cancel(m_tidSerialReadThread);
    if (iRet == 0)
    {
        m_tidSerialReadThread = 0;
    }
    if (iRet == 3)
    {
        m_tidSerialReadThread = 0;
        COMMONUNITS_INFO("Serial read thread already exit.");
    }
    else if (iRet != 0)
    {
        COMMONUNITS_ERR("Cannot cancel serial read thread! [%d]", iRet);
    }
    return iRet == 0 || iRet == 3;
}

void * CPstnLine::SerialReadThread(void * pData)
{
    CPstnLine * self = static_cast<CPstnLine *>(pData);
    COMMONUNITS_INFO("Enter CPstnLine::SerialReadThread with %p", pData);

    // 读取缓冲区
    yl::string strBuffer;
    char szBuff[MAX_COMMAND_LINE_SIZE] = { 0 };
    int nRead = 0;
    memset(szBuff, 0, sizeof(szBuff));
#if WIN32
    nRead = read(m_iHandle, szBuff, sizeof(szBuff));
#else
    while ((nRead = read(self->m_iHandle, szBuff, sizeof(szBuff))) > 0)
#endif // WIN32
    {
        // 容错：过滤读取到的"\0"
        size_t pos = 0;
        for (size_t i = 0; i < nRead; ++i)
        {
            COMMONUNITS_DEBUG("Read from IO buffer #%d: %02X", i, szBuff[i]);
            if (szBuff[i] > 0)
            {
                szBuff[pos] = szBuff[i];
                ++pos;
            }
        }
        szBuff[pos] = '\0';

        COMMONUNITS_INFO("Read from IO #%d :[%d] %s", self->m_iDeviceId, nRead, szBuff);
        strBuffer += szBuff;
        // 获取有效命令
        yl::string strCmd, strValue;
        while (GetValidCommandFromBuffer(strBuffer, strCmd, strValue))
        {
            // 触发结果事件
            SerialCmdResult * pResult = self->GetCmdResultByCmd(strCmd.c_str());
            COMMONUNITS_INFO("GetCmdResultByCmd(%s)=%p", strCmd.c_str(), pResult);
            if (pResult)
            {
                size_t len = strValue.copy(pResult->m_szVaule, strValue.size());
                pResult->m_szVaule[len] = '\0';
                pResult->m_event.Pulse();
                COMMONUNITS_INFO("CPstnLine::SerialReadThread event set.");
            }
            // 进行其他处理（包括主动上报的消息）
            self->ProcessCmd(strCmd, strValue);
        }
        // 清空缓冲区
        memset(szBuff, 0, sizeof(szBuff));
    }

    COMMONUNITS_INFO("Exit CPstnLine::SerialReadThread");
    return NULL;
}

void CPstnLine::LineInit()
{
#if !WIN32
    char szValue[MAX_COMMAND_LINE_SIZE] = { 0 };

    // 打开通道
    if (SendAndRecv(IO_CMD_OPEN, IO_VALUE_ON, szValue))
    {
        COMMONUNITS_INFO("Open PSTN Box device returns: %s", szValue);
    }

    // 预先获取线路ID，通话建立时直接使用，加快速度
    if (SendAndRecv(IO_CMD_GET_LINEID, NULL, szValue))
    {
        COMMONUNITS_INFO("PSTN line ID: %s", szValue);
        sscanf(szValue, "0x%x", &m_iLineId);
        // 获取到LineID后，初始化PSTN语音状态为禁用
        SetAudioEnable(false);
    }

    // 获取当前PSTN线路状态
    if (SendAndRecv(IO_CMD_GET_STATE, "line", szValue))
    {
        if (strcmp(szValue, IO_VALUE_ON) == 0)
        {
            // PSTN线路插上状态则发送广播
            if (m_bLinePluggedIn)
            {
                COMMONUNITS_INFO("PSTN line already plugged in.");
            }
            else
            {
                COMMONUNITS_INFO("PSTN line plugged in when getting state.");
                m_bLinePluggedIn = true;
                // 重置通话状态
                ResetStatus();
                // 广播消息
                etl_NotifyAppEx(false, PSTN_MSG_LINE_CHANGED, m_iDeviceId, PSTN_LINE_CONNECTED, 0, NULL);
            }
        }
        else
        {
            COMMONUNITS_INFO("PSTN line is not plugged in.");
        }
    }

    // 获取当前FXO初始化状态
    if (SendAndRecv(IO_CMD_GET_STATE, "fxoinit", szValue))
    {
        if (strcmp(szValue, IO_VALUE_ON) == 0)
        {
            // FXO已初始化则设置国家代码
            COMMONUNITS_INFO("FXO chip already initialized.");
            // 设置国家代码
            SetCountryCode("", false);
        }
        else
        {
            // FXO未初始化，则在初始化完成上报事件中设置
            COMMONUNITS_INFO("FXO chip not initialized.");
        }
    }
#endif // !WIN32
}

void CPstnLine::ResetStatus()
{
    // 关闭定时器
    timerKillThreadTimer(TIMER_ID(m_iCallId));
    timerKillThreadTimer(TIMER_ID(m_strCid));
    timerKillThreadTimer(TIMER_ID(m_strDial));
    // 重置状态
    m_iCallId = 0;
    m_strCid.clear();
    m_bCidComplete = false;
    m_strDial.clear();
    m_bCidAfterRing = false;
    SetStatus(PLS_IDLE);
}

void CPstnLine::SetStatus(PSTN_LINE_STATUS eStatus)
{
    COMMONUNITS_INFO("Status of pstn line #%d changed from %d to %d",
                     m_iDeviceId, m_eStatus, eStatus);

    if (eStatus != m_eStatus)
    {
        m_eStatus = eStatus;
        etl_NotifyAppEx(false, PSTN_MSG_LINE_CHANGED, m_iDeviceId, PSTN_LINE_STATUS_CHANGED, 0, NULL);
    }
}

CPstnLine::PSTN_LINE_STATUS CPstnLine::GetStatus() const
{
    COMMONUNITS_INFO("Status of pstn line #%d is %d",
                     m_iDeviceId, m_eStatus);
    return m_eStatus;
}

bool CPstnLine::IsRinging() const
{
    return GetStatus() == PLS_RINGING;
}

bool CPstnLine::Send(const char * szCmd, const char * szValue /* = NULL */)
{
    // 设备禁用时不发送数据
    if (!m_bEnable)
    {
        return false;
    }
    // 格式化AT命令
    char szLine[MAX_COMMAND_LINE_SIZE] = { 0 };
    if (NULL == szValue)
    {
        snprintf(szLine, MAX_COMMAND_LINE_SIZE, "AT+%s\r\n", szCmd);
    }
    else
    {
        snprintf(szLine, MAX_COMMAND_LINE_SIZE, "AT+%s=%s\r\n", szCmd, szValue);
    }
    // 发送命令
    COMMONUNITS_INFO("Send to IO #%d: %s", m_iDeviceId, szLine);
    int iRet = write(m_iHandle, szLine, strlen(szLine));
    if (iRet < 0)
    {
        COMMONUNITS_WARN("Send to IO #%d fail: [%d]%s", m_iDeviceId, errno, strerror(errno));
    }
    return iRet == strlen(szLine);
}

bool CPstnLine::SendAndRecv(const char * szCmd, const char * szValue /* = NULL */,
                            char * szResult /* = NULL */)
{
    SerialCmdResult * pCmdResult = GetCmdResultByCmd(szCmd);
    COMMONUNITS_INFO("CPstnLine::SendAndRecv pCmdResult=%p", pCmdResult);
    if (pCmdResult)
    {
        pCmdResult->m_event.Reset();
    }
    if (!Send(szCmd, szValue))
    {
        return false;
    }
    if (pCmdResult)
    {
        if (pCmdResult->m_event.Wait(SYNC_READ_TIMEOUT))
        {
            if (szResult)
            {
                strcpy(szResult, pCmdResult->m_szVaule);
            }
            return true;
        }
        return false;
    }
    return true;
}

SerialCmdResult * CPstnLine::GetCmdResultByCmd(const char * szCmd)
{
    if (strcmp(szCmd, IO_CMD_OPEN) == 0)
    {
        return &m_oResultOpen;
    }
    else if (strcmp(szCmd, IO_CMD_GET_LINEID) == 0)
    {
        return &m_oResultLineID;
    }
    else if (strcmp(szCmd, IO_CMD_HOOK) == 0)
    {
        return &m_oResultHook;
    }
    else if (strcmp(szCmd, IO_CMD_MUTE) == 0)
    {
        return &m_oResultMute;
    }
    else if (strcmp(szCmd, IO_CMD_DND) == 0)
    {
        return &m_oResultDND;
    }
    else if (strcmp(szCmd, IO_CMD_HOLD) == 0)
    {
        return &m_oResultHold;
    }
    else if (strcmp(szCmd, IO_CMD_REJECT) == 0)
    {
        return &m_oResultRecjct;
    }
    else if (strcmp(szCmd, IO_CMD_SET_COUNTRY) == 0)
    {
        return &m_oResultSetCountry;
    }
    else if (strcmp(szCmd, IO_CMD_GET_COUNTRY) == 0)
    {
        return &m_oResultGetCountry;
    }
    else if (strcmp(szCmd, IO_CMD_DIAL) == 0)
    {
        return &m_oResultDial;
    }
    else if (strcmp(szCmd, IO_CMD_GET_STATE) == 0)
    {
        return &m_oResultState;
    }
    else if (strcmp(szCmd, IO_CMD_GET_VERSION) == 0)
    {
        return &m_oResultVer;
    }
    else if (strcmp(szCmd, IO_CMD_SET_LOGLEVEL) == 0)
    {
        return &m_oResultSetLogLevel;
    }
    else if (strcmp(szCmd, IO_CMD_GET_LOGLEVEL) == 0)
    {
        return &m_oResultGetLogLevel;
    }
    else if (strcmp(szCmd, IO_CMD_SET_LOGMOD) == 0)
    {
        return &m_oResultLogMod;
    }
    else if (strcmp(szCmd, IO_CMD_GET_FW_STATE) == 0)
    {
        return &m_oResultFwState;
    }
    else if (strcmp(szCmd, IO_CMD_SET_SN) == 0)
    {
        return &m_oResultSetSn;
    }
    else if (strcmp(szCmd, IO_CMD_GET_SN) == 0)
    {
        return &m_oResultGetSn;
    }
    else if (strcmp(szCmd, IO_CMD_UPGRADE) == 0)
    {
        return &m_oResultUpgrade;
    }
    else if (strcmp(szCmd, IO_CMD_UPGRADE) == 0)
    {
        return &m_oResultReboot;
    }
    else if (strcmp(szCmd, IO_CMD_GET_FXOID) == 0)
    {
        return &m_oResultFxoID;
    }
    return NULL;
}

bool CPstnLine::GetValidCommandFromBuffer(yl::string & strBuffer, yl::string & strCmd,
        yl::string & strValue)
{
    do
    {
        // 在缓冲区中查找"\n"（命令以"\n"结尾）
        unsigned int pos = strBuffer.find('\n');
        if (yl::string::npos == pos)
        {
            return false;
        }
        // 提取命令行
        yl::string strLine = strBuffer.substr(0, pos);
        strBuffer.erase(0, pos + 1);
        COMMONUNITS_INFO("IO command line: %s", strLine.c_str());
        if (strLine.empty())
        {
            continue;
        }
        // 解析命令行
        pos = strLine.find('=');
        if (yl::string::npos != pos)
        {
            strCmd = strLine.substr(3, pos - 3);
            strValue = strLine.substr(pos + 1);
#if WIN32
            strValue.erase(strValue.find_last_not_of("\r") + 1);
#else
            strValue.trim_right("\r");
#endif // WIN32
            COMMONUNITS_INFO("IO command: %s, value: %s[size=%d]", strCmd.c_str(), strValue.c_str(),
                             strValue.size());
            return true;
        }
    }
    while (true);
    return false;
}

void CPstnLine::ProcessCmd(const yl::string & strCmd, const yl::string & strValue)
{
    if (strCmd.compare(IO_CMD_GET_LINEID) == 0)
    {
        sscanf(strValue.c_str(), "0x%x", &m_iLineId);
        // 获取到LineID后，初始化PSTN语音状态为禁用
        SetAudioEnable(false);
    }
    else if (strCmd.compare(IO_CMD_FXO_INIT) == 0)
    {
        if (strValue.compare(IO_VALUE_OK) == 0)
        {
            COMMONUNITS_INFO("FXO chip initialized.");
            // FXO芯片初始化成功时设置国家代码
            // 此处必须使用异步调用，因为：1.不关心结果；2.同步调用会产生死锁。
            SetCountryCode("", false);
        }
        else if (strValue.compare(IO_VALUE_ERR) == 0)
        {
            COMMONUNITS_WARN("FXO chip init failed!");
        }
        else
        {
            COMMONUNITS_WARN("Unknown value!");
        }
    }
    else if (strCmd.compare(IO_CMD_LINE) == 0)
    {
        // 线路状态改变不应该根据m_bEnable过滤
        if (strValue.compare(IO_VALUE_ON) == 0)
        {
            if (m_bLinePluggedIn)
            {
                COMMONUNITS_INFO("PSTN line already plug in!");
            }
            else
            {
                COMMONUNITS_INFO("PSTN line plug in!");
                m_bLinePluggedIn = true;
                // 重置通话状态
                ResetStatus();
                // 广播消息
                etl_NotifyAppEx(false, PSTN_MSG_LINE_CHANGED, m_iDeviceId, PSTN_LINE_CONNECTED, 0, NULL);
            }
        }
        else if (strValue.compare(IO_VALUE_OFF) == 0)
        {
            COMMONUNITS_INFO("PSTN line pull out!");
            m_bLinePluggedIn = false;
            // 重置通话状态
            ResetStatus();
            // 广播消息
            etl_NotifyAppEx(false, PSTN_MSG_LINE_CHANGED, m_iDeviceId, PSTN_LINE_DISCONNECTED, 0, NULL);
        }
        else
        {
            COMMONUNITS_WARN("Unknown value!");
        }
    }
    else if (strCmd.compare(IO_CMD_RING) == 0)
    {
        PSTN_LINE_STATUS eStatus = GetStatus();
        if (eStatus != PLS_IDLE && eStatus != PLS_RINGING)
        {
            COMMONUNITS_WARN("Incoming PSTN call ring[%s] in wrong status[%d]!", strValue.c_str(), eStatus);
            return;
        }

        if (strValue.compare(IO_VALUE_ON) == 0)
        {
            if (m_iCallId <= 0)
            {
#if WIN32
                m_iCallId = 1;
#else
                m_iCallId = modVoip_NewCallId();
#endif // WIN32
            }

            COMMONUNITS_INFO("Incoming PSTN call ring #%d.", m_iCallId);

            // 停止振铃定时器
            timerKillThreadTimer(TIMER_ID(m_iCallId));
            if (!IsRinging())
            {
                SetStatus(PLS_RINGING);
                // 第一次开始振铃才广播消息
                if (m_bEnable)
                {
                    etl_NotifyAppEx(false, PSTN_MSG_INCOMING_CALL, m_iDeviceId, m_iCallId, 0, NULL);
                }
                else
                {
                    COMMONUNITS_INFO("Disabled, omitting incoming call.");
                }
            }
        }
        else if (strValue.compare(IO_VALUE_OFF) == 0)
        {
            COMMONUNITS_INFO("Incoming PSTN call ring stop #%d.", m_iCallId);
            if (IsRinging())
            {
                // 振铃中断超过MAX_RING_INTERVAL毫秒才视为对方挂机
                timerSetThreadTimer0(TIMER_ID(m_iCallId), MAX_RING_INTERVAL);
            }
        }
        else
        {
            COMMONUNITS_WARN("Unknown value!");
        }
    }
    else if (strCmd.compare(IO_CMD_CID) == 0)
    {
        // CID接收完毕后又收到CID，则视为下一通电话
        if (m_bCidComplete)
        {
            COMMONUNITS_INFO("Receive CID again, assume another call.");
            // 挂机
            etl_NotifyAppEx(false, PSTN_MSG_INCOMING_CANCEL, m_iDeviceId, m_iCallId, 0, NULL);
            // 重置状态
            ResetStatus();
        }

        if (m_iCallId <= 0)
        {
#if WIN32
            m_iCallId = 1;
#else
            m_iCallId = modVoip_NewCallId();
#endif // WIN32

            m_bCidAfterRing = false;
        }
        else
        {
            m_bCidAfterRing = true;
        }

        if (GetStatus() == PLS_IDLE)
        {
            SetStatus(PLS_RINGING);
        }
        m_strCid += strValue;
        // CID间隔时间超过MAX_CID_INTERVAL毫秒才视为接收完整并上报
        timerSetThreadTimer0(TIMER_ID(m_strCid), MAX_CID_INTERVAL);
    }
    else if (strCmd.compare(IO_CMD_HOOK) == 0)
    {
        if (m_bFlashing)
        {
            // 当前处于flash状态，hook的响应当成flash结果处理
            if (strValue.compare(IO_VALUE_OK) == 0)
            {
                COMMONUNITS_INFO("Flash on PSTN line #%d started.", m_iDeviceId);
                // 启动flash超时定时器
                timerSetThreadTimer0(TIMER_ID(m_bFlashing), FLASH_TIMEOUT);
            }
            else if (strValue.compare("flashfilter") == 0)
            {
                // flash过程中重复调用flash，由驱动过滤
                COMMONUNITS_INFO("Flash on PSTN line #%d omitted.", m_iDeviceId);
            }
            else
            {
                // flash完成视为成功
                bool bResult = strValue.compare("flashcomplete") == 0;
                COMMONUNITS_WARN("Flash on PSTN line #%d %s[%s].",
                                 m_iDeviceId, bResult ? "success" : "fail", strValue.c_str());
                // 设置Flash状态
                m_bFlashing = false;
                timerKillThreadTimer(TIMER_ID(m_bFlashing));
                // 发送结果消息
                etl_NotifyAppEx(false, PSTN_MSG_FLASH_RESULT, m_iDeviceId, m_iCallId,
                                sizeof(bool), &bResult);
            }
        }
        else
        {
            // 摘机
            if (strValue.compare(IO_VALUE_OK) == 0)
            {
                COMMONUNITS_INFO("Hook operation on PSTN line #%d success.", m_iDeviceId);
                if (!m_strDial.empty())
                {
                    // 摘机成功且有号码待拨，则设置定时器
                    timerSetThreadTimer0(TIMER_ID(m_strDial), DIAL_DELAY);
                }
            }
            else
            {
                COMMONUNITS_WARN("Hook operation on PSTN line #%d fail: %s",
                                 m_iDeviceId, strValue.c_str());
                // 摘机失败则停止拨号过程，清空待拨号码
                m_strDial.clear();
            }
        }
    }
    else if (strCmd.compare(IO_CMD_DIAL) == 0)
    {
        // 拨号结果
        bool bResult = false;
        if (strValue.compare(IO_VALUE_OK) == 0)
        {
            COMMONUNITS_INFO("Dial on PSTN line #%d success.", m_iDeviceId);
            bResult = true;
        }
        else
        {
            COMMONUNITS_WARN("Dial on PSTN line #%d fail: %s", m_iDeviceId, strValue.c_str());
            bResult = false;
            // 拨号失败则停止拨号过程
            timerKillThreadTimer(TIMER_ID(m_strDial));
            m_strDial.clear();
        }
        if (m_strDial.empty() || !bResult)
        {
            // 拨号完成或者拨号失败――广播消息
            TalkErrorDefine eResult = bResult ? TALK_ERROR_NONE : TALK_ERROR_CALLOUT_FN;
            etl_NotifyAppEx(false, PSTN_MSG_DIAL_RESULT, m_iDeviceId, m_iCallId,
                            sizeof(TalkErrorDefine), &eResult);
        }
        else
        {
            // 拨号成功且未拨号完成，则设置定时器
            timerSetThreadTimer0(TIMER_ID(m_strDial), DIAL_DIGIT_INTERVAL);
        }
    }
    else if (strCmd.compare(IO_CMD_LOG) == 0)
    {
        COMMONUNITS_INFO("[IO]%s", strValue.c_str());
    }
    else
    {
        COMMONUNITS_DEBUG("The command we don't care.");
    }
}

void CPstnLine::DialDigit()
{
    if (!m_strDial.empty())
    {
        // 获取第一位按键
        char szDigit[2] = { 0 };
        szDigit[0] = m_strDial.front();
        m_strDial.erase(0, 1);
        szDigit[1] = '\0';
        // 发送按键
        Send(IO_CMD_DIAL, szDigit);
    }
    if (m_strDial.empty())
    {
        timerKillThreadTimer(TIMER_ID(m_strDial));
    }
}

void CPstnLine::OpenAudioHandle()
{
    char szAudioDev[16] = { 0 };
    snprintf(szAudioDev, sizeof(szAudioDev), pstndev, m_iDeviceId);
    m_iAudioHandle = open(szAudioDev, O_RDWR);
    if (m_iAudioHandle <= 0)
    {
        COMMONUNITS_ERR("Cannot open PSTN audio handle! handle(%s)=%d", szAudioDev, m_iAudioHandle);
    }
    else
    {
        COMMONUNITS_INFO("PSTN audio handle opened. handle(%s)=%d", szAudioDev, m_iAudioHandle);
    }
}

void CPstnLine::SetAudioEnable(bool bEnable)
{
    // 校验PSTN语音句柄是否打开
    if (m_iAudioHandle <= 0)
    {
        COMMONUNITS_ERR("PSTN audio handle not opened!");
        return;
    }
    // 组装并发送指令
    struct pstn_param param;
    param.line = m_iLineId;
    param.active = bEnable ? 1 : 0;
    int result = ioctl(m_iAudioHandle, PSTNIOCS_SETAUDIO, &param);
    if (result >= 0)
    {
        COMMONUNITS_INFO("Set PSTN audio switch success[%d]. line=%d active=%d", result, param.line,
                         param.active);
    }
    else
    {
        COMMONUNITS_ERR("Set PSTN audio switch failed[%d]! line=%d active=%d", result, param.line,
                        param.active);
    }
}

#endif // IF_FEATURE_PSTN
