#ifndef _LINEDEVICE_MANAGER_H_
#define _LINEDEVICE_MANAGER_H_

#ifdef IF_SUPPORT_LINE_DEVICE
#include <ETLLib.hpp>
#include <singletonclassdefine.h>

class CLineDeviceManager
{
    DECLARE_SINGLETON_CLASS(CLineDeviceManager)

public:
    // 初始化
    void Init();

    // 插入拔出处理
    bool ProcessDeviceEvent(int iKeyEvent);

    //设备是否连接
    //** bDevice 是否检测手机端
    bool IsConnect(bool bCheckMobile = true);

    // 消息处理
    static LRESULT OnMessage(msgObject & msg);

private:
    // 是否允许处理外界设备
    bool IsNeedShowLineDevice();

    // 定时器处理
    bool OnTimer(UINT uTimerID);

private:
    // 检查的定时器
    int m_iCheckTimer;
};

#define _LineDeviceManager GET_SINGLETON_INSTANCE(CLineDeviceManager)

#endif // IF_SUPPORT_LINE_DEVICE
#endif // _LINEDEVICE_MANAGER_H_
