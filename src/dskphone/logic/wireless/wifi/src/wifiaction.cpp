#ifdef IF_SUPPORT_WIFI

#include "wifiaction.h"
#include "taskaction/common.h"
#include "wireless/wifi/src/wifidriver.h"

#define TO_STRING(eType, str)    case eType: str = #eType; break;

CWifiAction::CWifiAction()
    : m_pWifiExecParam(NULL),
      m_eTaskType(WIFI_TASK_NONE)
{
    m_eTARequestType = NS_TA::TA_REQUEST_WIFI;
}

CWifiAction::~CWifiAction()
{
}

bool CWifiAction::Exec()
{
    if (NULL == m_pWifiExecParam)
    {
        WIRE_WARN("[Wifi task][Exec][Exec param null]");
        return false;
    }

    WIRE_INFO("[Wifi task][Exec][Task:%s][wParam:%d][lParam:%d]",
              GetTaskInfo(m_eTaskType).c_str(),
              m_pWifiExecParam->wParam,
              m_pWifiExecParam->lParam);

    switch (m_eTaskType)
    {
    case WIFI_TASK_CONFIG_CHANGE:
        m_pWifiExecParam->bResult = _WifiDriver.SaveConfig(
                                        m_pWifiExecParam->wParam,
                                        m_pWifiExecParam->lParam);
        break;
    case WIFI_TASK_OPEN:
        m_pWifiExecParam->bResult = _WifiDriver.OpenWifi();
        break;
    case WIFI_TASK_CLOSE:
        m_pWifiExecParam->bResult = _WifiDriver.CloseWifi();
        break;
    case WIFI_TASK_SCAN_START:
        m_pWifiExecParam->bResult = _WifiDriver.StartScan(0, false);
        break;
    case WIFI_TASK_SCAN_STOP:
        m_pWifiExecParam->bResult = _WifiDriver.StopScan();
        break;
    case WIFI_TASK_CYCLE_SCAN_START:
        m_pWifiExecParam->bResult = _WifiDriver.StartCycleScan();
        break;
    case WIFI_TASK_CYCLE_SCAN_STOP:
        m_pWifiExecParam->bResult = _WifiDriver.StopCycleScan();
        break;
    case WIFI_TASK_REFRESH:
        m_pWifiExecParam->bResult = _WifiDriver.RefreshState();
        break;
    default:
        break;
    }

    return m_pWifiExecParam->bResult;
}

bool CWifiAction::ReleaseData(bool bCallBackSucceed)
{
    SAFE_DELETE(m_pWifiExecParam);
    return true;
}

void CWifiAction::SetWifiExecParam(const WiFiTaskParam& sParam)
{
    m_pWifiExecParam = new WiFiTaskParam();
    if (NULL != m_pWifiExecParam)
    {
        *m_pWifiExecParam = sParam;
    }
}

yl::string CWifiAction::GetTaskInfo(WIFI_TASK_TYPE eType)
{
    yl::string str = "";
    switch (eType)
    {
        TO_STRING(WIFI_TASK_CONFIG_CHANGE, str);
        TO_STRING(WIFI_TASK_OPEN, str);
        TO_STRING(WIFI_TASK_CLOSE, str);
        TO_STRING(WIFI_TASK_SCAN_START, str);
        TO_STRING(WIFI_TASK_SCAN_STOP, str);
        TO_STRING(WIFI_TASK_CYCLE_SCAN_START, str);
        TO_STRING(WIFI_TASK_CYCLE_SCAN_STOP, str);
        TO_STRING(WIFI_TASK_REFRESH, str);
    default:
        break;
    }

    return str;
}

#endif // IF_SUPPORT_WIFI
