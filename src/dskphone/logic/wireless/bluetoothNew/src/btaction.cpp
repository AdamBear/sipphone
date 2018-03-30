#ifdef IF_SUPPORT_BLUETOOTH

#include "btaction.h"
#include "taskaction/common.h"
#include "wireless/bluetoothNew/src/btdriver.h"

#define TO_STRING(eType, str)    case eType: str = #eType; break;

CBTAction::CBTAction()
    : m_eTaskType(BT_TASK_NONE),
      m_iResult(-1),
      m_lHandle(BT_DEVICE_HANDLE_INVAILED),
      m_pBTExecParam(NULL)
{
    m_eTARequestType = NS_TA::TA_REQUEST_BT;
}

CBTAction::~CBTAction()
{

}

bool CBTAction::Exec()
{
    WIRE_INFO("[Bt task][Exec][Task:%s][Handle:%d]",
              GetTaskName(m_eTaskType).c_str(), GetRelateHandle());

    switch (m_eTaskType)
    {
    case BT_TASK_CONNECT:
        m_iResult = _BTDriver.ConnectPairedDevice(GetRelateHandle(), false);
        break;
    case BT_TASK_AUTO_CONNECT:
        m_iResult = _BTDriver.ConnectPairedDevice(GetRelateHandle(), true);
        break;
    case BT_TASK_PAIR:
        m_iResult = _BTDriver.PairDevice(GetRelateHandle());
        break;
    case BT_TASK_PRASECONTACT:
        if (m_pBTExecParam)
        {
            yl::string strXmlPath;
            m_iResult = CBTSync::PraseDirectory(strXmlPath, m_pBTExecParam->strParam.c_str());
            m_pBTExecParam->strParam = strXmlPath;
        }
        break;
    case BT_TASK_DELETEDEVICE:
        m_iResult = _BTDriver.DeleteDevice(GetRelateHandle());
        break;
    case BT_TASK_DISCONNECT:
        m_iResult = _BTDriver.DisconnectDevice(GetRelateHandle());
        break;
    case BT_TASK_POWER_OFF:
        m_iResult = _BTDriver.PowerSet(false);
        break;
    case BT_TASK_POWER_ON:
        m_iResult = _BTDriver.PowerSet(true);
        break;
    case BT_TASK_CANCLEPAIR:
        m_iResult = _BTDriver.CancelPairDevice(GetRelateHandle());
        break;
    case BT_TASK_DOWNLOAD_CONTACT_START:
        m_iResult = _BTDriver.MobileStartDownloadContact(m_pBTExecParam->ulDeviceHandle,
                    m_pBTExecParam->wParam, m_pBTExecParam->lParam);
        break;
    case BT_TASK_DOWNLOAD_CONTACT_STOP:
        m_iResult = _BTDriver.MobileStopDownload(m_pBTExecParam->ulDeviceHandle);
        break;
#if IF_SUPPORT_REPAIR
    case BT_TASK_UNPAIR:
        if (NULL != m_pBTExecParam)
        {
            m_iResult = _BTDriver.UnPairDevice(m_pBTExecParam->ulDeviceHandle);
        }
        break;
#endif
    case BT_TASK_HEADSET_EVENT:
        m_iResult = ExecHeadsetEvent();
        break;
    case BT_TASK_SCAN_START:
        m_iResult = _BTDriver.ScanDevice();
        break;
    case BT_TASK_SCAN_STOP:
        m_iResult = _BTDriver.StopScanDevice();
        break;
    case BT_TASK_CONNECT_PROFILES:
        if (NULL != m_pBTExecParam)
        {
            m_iResult = _BTDriver.ConnectProfiles(m_pBTExecParam->ulDeviceHandle,
                                                  m_pBTExecParam->wParam);
        }
        break;
    case BT_TASK_DISCONNECT_PROFILES:
        if (NULL != m_pBTExecParam)
        {
            m_iResult = _BTDriver.DisconnectProfiles(m_pBTExecParam->ulDeviceHandle,
                        m_pBTExecParam->wParam);
        }
        break;
    default:
        m_iResult = -1;
        break;
    }

    return true;
}

bool CBTAction::ReleaseData(bool bCallBackSucceed)
{
    SAFE_DELETE(m_pBTExecParam);
    return true;
}

void CBTAction::SetBtExecParam(const BTTaskParam& sParam)
{
    m_pBTExecParam = new BTTaskParam();
    if (m_pBTExecParam)
    {
        *m_pBTExecParam = sParam;
    }
}

bool CBTAction::ExecHeadsetEvent()
{
    if (NULL == m_pBTExecParam)
    {
        return false;
    }

    if (m_pBTExecParam->wParam == HEADSET_PLAY)
    {
        return _BTDriver.Play(m_pBTExecParam->ulDeviceHandle,
                              m_pBTExecParam->lParam == 1);
    }
    else if (m_pBTExecParam->wParam == HEADSET_CALL_OUT)
    {
        return _BTDriver.CallOut(m_pBTExecParam->ulDeviceHandle,
                                 m_pBTExecParam->strParam.c_str());
    }
    else if (m_pBTExecParam->wParam == HEADSET_CALL_IN)
    {
        return _BTDriver.CallIn(m_pBTExecParam->ulDeviceHandle,
                                m_pBTExecParam->strParam.c_str());
    }
    else if (m_pBTExecParam->wParam == HEADSET_ANSWER_CALL)
    {
        return _BTDriver.CallAnswer(m_pBTExecParam->ulDeviceHandle);
    }
    else if (m_pBTExecParam->wParam == HEADSET_END_CALL)
    {
        return _BTDriver.CallEnd(m_pBTExecParam->ulDeviceHandle);
    }

    return false;
}

yl::string CBTAction::GetTaskName(BT_TASK_TYPE eType)
{
    yl::string str = "";
    switch (eType)
    {
        TO_STRING(BT_TASK_CONNECT, str);
        TO_STRING(BT_TASK_AUTO_CONNECT, str);
        TO_STRING(BT_TASK_PAIR, str);
        TO_STRING(BT_TASK_PRASECONTACT, str);
        TO_STRING(BT_TASK_DELETEDEVICE, str);
        TO_STRING(BT_TASK_DISCONNECT, str);
        TO_STRING(BT_TASK_POWER_OFF, str);
        TO_STRING(BT_TASK_POWER_ON, str);
        TO_STRING(BT_TASK_CANCLEPAIR, str);
        TO_STRING(BT_TASK_DOWNLOAD_CONTACT_START, str);
        TO_STRING(BT_TASK_DOWNLOAD_CONTACT_STOP, str);
#if IF_SUPPORT_REPAIR
        TO_STRING(BT_TASK_UNPAIR, str);
#endif
        TO_STRING(BT_TASK_HEADSET_EVENT, str);
        TO_STRING(BT_TASK_SCAN_START, str);
        TO_STRING(BT_TASK_SCAN_STOP, str);
        TO_STRING(BT_TASK_CONNECT_PROFILES, str);
        TO_STRING(BT_TASK_DISCONNECT_PROFILES, str);
    }

    return str;
}

#endif // IF_SUPPORT_BLUETOOTH
