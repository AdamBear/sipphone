#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWifiStateScannor.cpp
//  Implementation of the Class CWifiStateScannor
//  Created on:      15-五月-2014 9:18:32
//  Original author: songk
///////////////////////////////////////////////////////////
#include "wifi_inc.h"

#define WIFI_SCAN_TIMER 10*1000

CWiFiScanState::CWiFiScanState()
{
    m_eState = SESSION_SCAN_STATE;
}

CWiFiScanState::~CWiFiScanState()
{
#if IF_SUPPORT_WIFI_CLYCLESCAN

#else
    SetTimer(false, WIFI_SCAN_TIMER);
#endif
}

bool CWiFiScanState::AfterStateCreate(long ulDeviceHandle, void * pUserData)
{
    return StartScan(0, false);
}

//退出扫描状态前的处理
bool CWiFiScanState::BeforeStateTransfer(bool bEndConnect/* = false*/)
{
    //退出扫描状态发送消息给rtServer停止扫描
#ifdef _WIFI_CLYCLESCAN
    StopScan();
#else
    if (bEndConnect)
    {
        WLAsyncHelper_WifiAsync(WIFI_TASK_SCAN_STOP);
    }
#endif
    return true;
}

//rtServer上报事件
bool CWiFiScanState::ProcessCallbackEvent(msgObject & objMsg)
{
    bool bHandle = true;
    switch (objMsg.message)
    {
    case SYS_MSG_WIFI_SCAN_RESULT:
        {
            if (0 == objMsg.lParam)   //扫描成功
            {
                _WiFiData.UpdateScanInfoList();
#if IF_SUPPORT_WIFI_CLYCLESCAN
                _WiFiManager.UpdateUI(OP_SCAN_WIFI_RESULT, WIRELESS_OP_SCAN_RESULT_SUCCESS, WIFI_NET_ID_INVAILED);

                // 进入周期扫描状态
                m_pSession->EnterState(SESSION_CYCLE_SCAN_STATE, WIFI_NET_ID_INVAILED, NULL);
            }
#else
            }
            ExitState(true);
#endif

        }
        break;
    default:
        {
            bHandle = false;
        }
        break;
    }
    return bHandle;
}

bool CWiFiScanState::ProcessTimer(int iTimerID)
{
    if (iTimerID == m_iTimer)
    {
#if IF_SUPPORT_WIFI_CLYCLESCAN
        m_pSession->EnterState(SESSION_CYCLE_SCAN_STATE, WIFI_NET_ID_INVAILED, NULL);
#else
        ExitState(true);
#endif
        return true;
    }
    return false;
}

bool CWiFiScanState::ProcessOption(const WirelessSessionData & wsData)
{
    WIRE_INFO("Receive UI message[%d], wParam[%d]", wsData.ulMessage, wsData.wParam);

    switch (wsData.ulMessage)
    {
    case WIFI_SCAN_OPTION:
        {
            if (0 == wsData.wParam)
            {
                return m_pSession->EnterState(SESSION_CYCLE_SCAN_STATE, WIFI_NET_ID_INVAILED, NULL);
            }
        }
        break;
    default:
        break;
    }
    return false;
}

//开始扫描
bool CWiFiScanState::StartScan(int iLoopTime, bool bRset)
{
    WIRE_INFO("start scan");

    WLAsyncHelper_WifiAsync(WIFI_TASK_SCAN_START);
    SetTimer(true, WIFI_SCAN_TIMER);

    return true;
}

bool CWiFiScanState::StopScan()
{
    WIRE_INFO("Stop scan");

    WLAsyncHelper_WifiAsync(WIFI_TASK_SCAN_STOP);
    SetTimer(false, WIFI_SCAN_TIMER);

    return true;
}

#endif // IF_SUPPORT_WIFI
