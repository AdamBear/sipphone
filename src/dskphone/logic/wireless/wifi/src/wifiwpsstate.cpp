#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWifiStateWps.cpp
//  Implementation of the Class CWifiStateWps
//  Created on:      15-五月-2014 9:17:48
//  Original author: songk
///////////////////////////////////////////////////////////
#include "wifi_inc.h"

CWiFiWpsState::CWiFiWpsState()
{
    m_eState = SESSION_WPS_STATE;
}

CWiFiWpsState::~CWiFiWpsState()
{

}

//进入状态后处理函数
bool CWiFiWpsState::AfterStateCreate(long ulDeviceHandle, void * pUserData)
{
    bool bHandle = StartWps();
    //开启失败
    if (!bHandle)
    {
        ExitState();
    }
    return bHandle;
}

//退出状态后处理函数
bool CWiFiWpsState::BeforeStateTransfer(bool bEndConnect/* = false*/)
{
    if (bEndConnect)
    {
        return StopWps();
    }
    return true;
}

//处理回调事件
bool CWiFiWpsState::ProcessCallbackEvent(msgObject & objMsg)
{
    bool bHandle = true;
    switch (objMsg.message)
    {
    case SYS_MSG_WIFI_WPS_RESULT:
        {
            int iNetID = objMsg.wParam;
            WIFI_UI_OPERATOR eOption = (m_eState == SESSION_WPS_STATE) ? OP_WPSCONNECT_RESULT :
                                       OP_WPSPINCONNECT_RESULT;

            if (0 == objMsg.lParam)
            {
                if (NULL != m_pSession)
                {
                    //先更新搜索列表
                    _WiFiData.UpdateScanInfoList();

                    //连接成功
                    if (NULL != m_pSessionInfo)
                    {
                        _WiFiData.CopySavedNetInfo(m_pSessionInfo->netInfo,
                                                   (const sys_wifi_network *)objMsg.GetExtraData());

                        m_pSessionInfo->authInfo.strPsk = m_pSessionInfo->netInfo.strPsk;
                        m_pSessionInfo->bAutoAdd = (NULL == _WiFiData.GetNetInfoByID(iNetID, true));
                        m_pSessionInfo->bModifyProfile = !m_pSessionInfo->bAutoAdd;
                    }

                    m_pSession->EnterState(SESSION_CONNECTED_STATE, iNetID, m_pSessionInfo);

                    //通知UI,WPS连接成功
                    _WiFiManager.UpdateUI(eOption, WIRELESS_OP_WPSCONNECT_SUCCESS, iNetID);
                }
            }
            else
            {
                //通知UI连接失败
                _WiFiManager.UpdateUI(eOption, WIRELESS_OP_WPSCONNECT_FAILED, iNetID);
                ExitState();
            }
        }
        break;
    default:
        bHandle = false;
        break;
    }
    return bHandle;
}

//开始连接
bool CWiFiWpsState::StartWps()
{
    return  _WifiDriver.StartWps();
}

bool CWiFiWpsState::StopWps()
{
    return _WifiDriver.StopWps(0);
}
#endif // IF_SUPPORT_WIFI
