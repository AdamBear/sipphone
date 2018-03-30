#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWifiStateScannor.h
//  Implementation of the Class CWifiSycleScanState
//  Created on:      07-十二月-2015 21:36:32
//  Original author: sub
///////////////////////////////////////////////////////////
#include "wifi_inc.h"

CWiFiCycleScanState::CWiFiCycleScanState()
{
    m_bIsFirstScan = false;
    m_eState = SESSION_CYCLE_SCAN_STATE;
}

CWiFiCycleScanState::~CWiFiCycleScanState()
{
}

bool CWiFiCycleScanState::AfterStateCreate(long ulDeviceHandle, void * pUserData)
{
    WIRE_INFO("Enter state---->[SESSION_CYCLE_SCAN_STATE]");

    return StartScan(false);
}

//退出扫描状态前的处理
bool CWiFiCycleScanState::BeforeStateTransfer(bool bEndConnect/* = false*/)
{
    //退出扫描状态发送消息给rtServer停止扫描
    StopScan(false);

    return true;
}

//rtServer上报事件
bool CWiFiCycleScanState::ProcessCallbackEvent(msgObject & objMsg)
{
    bool bHandle = true;
    switch (objMsg.message)
    {
    case SYS_MSG_WIFI_SCAN_RESULT:
        {
            if (0 == objMsg.lParam)   //扫描成功
            {
                _WiFiData.UpdateScanInfoList();

                //供UI的setting界面也修改进来后使用
                if (m_bIsFirstScan)
                {
                    StopScan(true);
                    m_bIsFirstScan = false;
                    _WiFiManager.UpdateUI(OP_SCAN_WIFI_RESULT, WIRELESS_OP_SCAN_RESULT_SUCCESS, WIFI_NET_ID_INVAILED);

                    StartScan(false);
                }
                else
                {
                    _WiFiManager.UpdateUI(OP_WIFI_SIGNAL_UPDATE, true, WIFI_NET_ID_INVAILED);
                }
            }
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

bool CWiFiCycleScanState::ProcessTimer(int iTimerID)
{
    if (iTimerID == m_iTimer)
    {
        ExitState(true);
        return true;
    }
    return false;
}

//处理用户的操作
bool CWiFiCycleScanState::ProcessOption(const WirelessSessionData & wsData)
{
    WIRE_INFO("Receive UI option[%d], wParam[%d]", wsData.ulMessage, wsData.wParam);

    switch (wsData.ulMessage)
    {
    case WIFI_SCAN_OPTION:
        {
            if (1 == wsData.wParam)
            {
                StartScan(false);
            }
        }
        break;
    default:
        break;
    }
    return false;
}

//开始扫描
bool CWiFiCycleScanState::StartScan(bool bFirstScan)
{
    WIRE_INFO("start scan, is first scan[%d]", bFirstScan);

    if (bFirstScan)
    {
        WLAsyncHelper_WifiAsync(WIFI_TASK_SCAN_START);
    }
    else
    {
        WLAsyncHelper_WifiAsync(WIFI_TASK_CYCLE_SCAN_START);
    }

    return true;
}

bool CWiFiCycleScanState::StopScan(bool bFirstScan)
{
    WIRE_INFO("Stop scan, is first scan[%d]", bFirstScan);

    if (bFirstScan)
    {
        WLAsyncHelper_WifiAsync(WIFI_TASK_SCAN_STOP);
    }
    else
    {
        WLAsyncHelper_WifiAsync(WIFI_TASK_CYCLE_SCAN_STOP);
    }

    return true;
}

#endif // IF_SUPPORT_WIFI

