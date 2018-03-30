#ifndef __PSTN_LINE_H__
#define __PSTN_LINE_H__

#ifdef IF_FEATURE_PSTN

#if WIN32
#include "win32_mkthread.h"
#else
#include <ETLLib.hpp>
#endif // WIN32

#include "pstnadapterdatadefine.h"

class CPstnLine
{
public:
    enum
    {
        HPDEV_DEVICE_ID_OFFSET = 192 // hpdev上报设备编号偏移量
    };

    CPstnLine(int iDeviceId = 0);
    ~CPstnLine();

    // 打开串口
    bool OpenSerial();
    // 关闭串口
    bool CloseSerial();

    // 定时器消息处理，已处理返回TRUE，否则返回FALSE
    BOOL OnTimer(UINT nTimerID);

    //virtual void DefaultProcessMessage(msgObject& msg);

    /**
     * @brief 判断当前是否连接PSTN线路
     *
     * @return 当前是否连接PSTN线路
     */
    bool IsConnected();

    /**
     * @brief 设置是否启用
     *
     * @param bEnable true=启用, false=禁用
     */
    void SetEnable(bool bEnable);

    /**
     * @brief 摘机/挂机
     *
     * @param bOffHook true=摘机, false=挂机
     * @param bSync true=同步操作 false=异步操作（主要用于拨号接口内部调用）
     *
     * @return 操作是否成功
     */
    bool SetHook(bool bOffHook, bool bSync = true);

    /**
     * @brief 拍叉簧
     *
     * @return 操作是否成功
     */
    bool Flash();

    /**
     * @brief 当前是否处于Flash（拍叉簧）状态
     *
     * @return 当前是否处于Flash状态
     */
    bool IsFlashing();

    /**
     * @brief 设置静音/取消静音
     *
     * @param bMute true=静音, false=取消静音
     *
     * @return 操作是否成功
     */
    bool SetMute(bool bMute);

    /**
     * @brief 通话保持/取回
     *
     * @param bHold true=保持, false=取回
     *
     * @return 操作是否成功
     */
    bool SetHold(bool bHold);

    /**
     * @brief 设置免打扰状态
     *
     * @param bDND true=免打扰, false=退出免打扰
     *
     * @return 操作是否成功
     */
    bool SetDND(bool bDND);

    /**
     * @brief 设置国家代码
     * @details 把国家代码写入硬件Flash，断电后不会丢失。国家代码用于适配不同国家的
     *      AC阻抗标准。
     *
     * @param strCountryCode 国家代码，为空则直接从配置文件读取
     * @param bSync true=同步操作 false=异步操作（主要用于拨号接口内部调用）
     *
     * @return 操作是否成功
     */
    bool SetCountryCode(const yl::string & strCountryCode = "", bool bSync = true);

    /**
     * @brief 获取国家代码
     * @details 从硬件Flash中读出国家代码
     *
     * @return 国家代码
     */
    yl::string GetCountryCode();

    /**
     * @brief 设置拍叉簧时间
     * @details 接口内部读取配置并发送命令给PSTN Box。
     *      注意：由于设置国家代码时也会自动修改拍叉簧时间，因此设置国家代码后需要重新设置拍叉簧时间。
     *
     * @param bForce 拍叉簧时间设置为Auto时，是否强制重新设置国家代码
     *
     * @return 操作是否成功
     */
    bool SetFlashDuration(bool bForce = false);

    /**
     * @brief 拨号
     * @details 把要拨打的号码放入缓冲区，以一定的间隔拨出
     *
     * @param strNumber 拨打号码
     * @param iCallId 呼叫ID
     *
     * @return 操作是否成功
     */
    bool Dial(const yl::string & strNumber, int iCallId);

    /**
     * @brief 获取线路ID
     * @details 获取线路ID，用于VPM调用
     *
     * @return 线路ID
     */
    int GetLineId();

    /**
     * @brief 判断PSTN线路是否可用（可以用于呼出）
     * @details 判断PSTN线路状态是否为空闲
     *
     * @return PSTN线路是否可用
     */
    bool IsAvailable();

    /**
     * @brief 获取PSTN Box固件版本号
     *
     * @return PSTN Box固件版本号
     */
    yl::string GetVersion();

    /**
     * @brief 获取FXO芯片ID
     *
     * @return FXO芯片ID
     */
    yl::string GetFxoId();

    /**
     * @brief 升级进度回调函数
     *
     * @param iDeviceId 设备ID
     * @param iProgress 当前升级进度，即当前已成功发送字节数
     * @param iTotal 升级总过程，即固件大小字节数
     * @param pParam 自定义参数
     */
    typedef void(*UpgradeCallback)(int iDeviceId, size_t iProgress, size_t iTotal, void * pParam);

    /**
     * @brief 升级PSTN Box固件，同步调用
     *
     * @param strRomPath 升级固件ROM的路径
     * @param pCallback 升级进度回调函数
     * @param pParam 传递给升级进度回调函数的参数
     *
     * @return 升级是否成功
     */
    bool Upgrade(const yl::string & strRomPath, UpgradeCallback pCallback, void * pParam);

#if WIN32
    /**
     * @brief 计算CRC16校验和
     *
     * @param pBuffer 待计算校验和的缓冲区
     * @param iLen 缓冲区大小
     *
     * @return CRC16校验和
     */
    static uint16_t CRC16(const uint8_t * pBuffer, size_t iLen);

    /**
     * @brief Xmodem协议升级PSTN Box固件，同步调用
     *
     * @param strRomPath 升级固件ROM的路径
     * @param pCallback 升级进度回调函数
     * @param pParam 传递给升级进度回调函数的参数
     *
     * @return 升级是否成功
     */
    bool XmodemUpgrade(const yl::string & strRomPath, UpgradeCallback pCallback, void * pParam);
#endif // WIN32

protected:
    // PSTN线路状态
    enum PSTN_LINE_STATUS
    {
        PLS_IDLE, // 空闲
        PLS_RINGING, // 振铃
        PLS_TALKING // 通话
    };

    int m_iDeviceId; /// 设备ID
    int m_iHandle; /// 设备句柄
    int m_iAudioHandle; /// 语音句柄
    pthread_t m_tidSerialReadThread; /// 串口读取线程ID
    bool m_bLinePluggedIn; /// PSTN线路是否插入
    int m_iLineId; /// 线路ID
    int m_iCallId; /// 通话ID【地址作为振铃定时器ID】
    yl::string m_strCid; /// 主叫号码【地址作为来显定时器ID】
    bool m_bCidComplete; /// 主叫号码接收完成
    yl::string m_strDial; /// 拨号内容【地址作为拨号定时器ID】
    PSTN_LINE_STATUS m_eStatus; /// PSTN线路状态
    bool m_bEnable; /// 是否启用
    bool m_bFlashing; /// 是否处于Flash状态【地址作为Flash超时定时器ID】

    bool m_bCidAfterRing;

    // 串口发送结果事件
    SerialCmdResult m_oResultOpen;
    SerialCmdResult m_oResultLineID;
    SerialCmdResult m_oResultHook;
    SerialCmdResult m_oResultMute;
    SerialCmdResult m_oResultDND;
    SerialCmdResult m_oResultHold;
    SerialCmdResult m_oResultRecjct;
    SerialCmdResult m_oResultSetCountry;
    SerialCmdResult m_oResultGetCountry;
    SerialCmdResult m_oResultDial;
    SerialCmdResult m_oResultState;
    SerialCmdResult m_oResultVer;
    SerialCmdResult m_oResultSetLogLevel;
    SerialCmdResult m_oResultGetLogLevel;
    SerialCmdResult m_oResultLogMod;
    SerialCmdResult m_oResultFwState;
    SerialCmdResult m_oResultSetSn;
    SerialCmdResult m_oResultGetSn;
    SerialCmdResult m_oResultUpgrade;
    SerialCmdResult m_oResultReboot;
    SerialCmdResult m_oResultFxoID;

    // 启动串口IO读取线程
    bool StartReadThread();
    // 停止串口IO读取线程
    bool KillReadThread();

    static void * SerialReadThread(void * pData);

    // PSTN Box插入后的初始化（包括获取当前线路状态等）
    void LineInit();

    // 重置状态（挂机IDLE状态）
    void ResetStatus();

    // 设置话机当前状态
    void SetStatus(PSTN_LINE_STATUS eStatus);
    // 获取话机当前状态
    PSTN_LINE_STATUS GetStatus() const;
    // 判断话机当前是否在振铃
    bool IsRinging() const;

    // 发送IO命令
    bool Send(const char * szCmd, const char * szValue = NULL);
    // 发送IO命令并等待响应
    bool SendAndRecv(const char * szCmd, const char * szValue = NULL, char * szResult = NULL);

    // 根据命令获取Event对象
    SerialCmdResult * GetCmdResultByCmd(const char * szCmd);

    // 从缓冲区中获取有效命令
    static bool GetValidCommandFromBuffer(yl::string & strBuffer, yl::string & strCmd,
                                          yl::string & strValue);

    // 统一处理IO命令
    void ProcessCmd(const yl::string & strCmd, const yl::string & strValue);

    // 从拨号缓冲区中发送一个按键
    void DialDigit();

    // 打开语音操作句柄
    void OpenAudioHandle();
    // 设置语音是否启用
    void SetAudioEnable(bool bEnable);
};

#endif // IF_FEATURE_PSTN

#endif // !__PSTN_LINE_H__
