#ifdef IF_BT_TEST_LOGIC
#include "btestdriver.h"
#include "bttestdriverthread.h"
#include "bttestdriverdata.h"
#include "talk/talklogic/src/talklogic_inc.h"
#include "contacts/directory/include/moddirinterface.h"
#include "extended/wifimessagedefine.h"

LRESULT OnBTTestDriverMsg(msgObject & objMsg)
{
    return _BTTestDriver.OnMsg(objMsg);
}

CBTTestDriver::CBTTestDriver(void)
{
    m_btHandle = 0;
    m_bPower = false;
    m_callbackFunc = NULL;
    m_iDownloadTimer = WIRE_INVAILD_TIMER;
    m_bChannelInMb = false;
}

CBTTestDriver::~CBTTestDriver(void)
{
    etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, OnBTTestDriverMsg);
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_TO_TESTDRIVER, BLUETOOTH_MSG_TO_TESTDRIVER,
                            OnBTTestDriverMsg);
}

void CBTTestDriver::Init()
{
    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, OnBTTestDriverMsg);
    etl_RegisterMsgHandle(BLUETOOTH_MSG_TO_TESTDRIVER, BLUETOOTH_MSG_TO_TESTDRIVER, OnBTTestDriverMsg);

    _ProcessHelper.LaunchLocalThread(_BTTestThread);

    _BTTestData.Init();

    WIRE_INFO("********BT TEST MSG[%d]**************", BLUETOOTH_MSG_TO_TESTDRIVER);
}

//消息处理
BOOL CBTTestDriver::OnMsg(msgObject & msgObj)
{
    BOOL bHandle = TRUE;
    switch (msgObj.message)
    {
    case TM_TIMER:
        ProcessTimer(msgObj.wParam);
        break;
    case BLUETOOTH_MSG_TO_TESTDRIVER:
        bHandle = ProcessTestThreadMsg(msgObj);
        break;
    default:
        bHandle = FALSE;
        break;
    }

    return bHandle;
}

//测试命令
BOOL CBTTestDriver::ProcessTestThreadMsg(msgObject & objMsg)
{
    if (objMsg.message != BLUETOOTH_MSG_TO_TESTDRIVER)
    {
        return FALSE;
    }

    WIRE_INFO("On TestCallBack Driver Msg[%d]Wparam:[%d]", objMsg.message, objMsg.wParam);

    switch (objMsg.wParam)
    {
    case BTTEST_ANSWERCALL:
        ProcessHSAnswerCall(objMsg.lParam);
        break;
    case BTTEST_ENDCALL:
        ProcessHSEndCall(objMsg.lParam);
        break;
    case BTTEST_REDIAL:
        {
            if (m_btHandle == 0)
            {
                break;
            }
            btkit_msg_t btkitMsg;
            btkitMsg.bt_event = BT_PF_HS_CALL_REDIAL;
            btkitMsg.bt_instance = m_btHandle;

            SendCallBackMsg(&btkitMsg, NULL);
        }
        break;
    case BTTEST_RMDONGLE:
    case BTTEST_ADDONGLE:
        {
            btkit_msg_t btkitMsg;
            btkitMsg.bt_event = (objMsg.wParam  == BTTEST_RMDONGLE) ? BT_ADAPTER_REMOVED : BT_ADAPTER_ADDED;
            btkitMsg.bt_instance = m_btHandle;

            SendCallBackMsg(&btkitMsg, NULL);
        }
        break;
    case BTTEST_MB_DISCONNECT:
    case BTTEST_DISCONNECT:
        {
            ProcessDisconnectDevice();
        }
        break;
    case BTTEST_INCOMMING_CALL:
        {
            ProcessIncommingCall();
        }
        break;
    case BTTEST_HANGUP_CALL:
        {
            ProcessHangUpCall(objMsg.lParam);
        }
        break;
    case BTTEST_HOLD_CALL:
        {
            ProcessHoldCall(objMsg.lParam, 1);
        }
        break;
    case BTTEST_UNHOLD_CALL:
        {
            ProcessHoldCall(objMsg.lParam, 0);
        }
        break;
    case BTTEST_CALL_OUT:
        {
            int iCallID;
            yl::string strNum = commonAPI_FormatString("%d", CBTTestData::GetRandNumber() % 10000000000);
            DeviceCallOut(m_btHandle, GetCurrendDevHandle(), strNum.c_str(), &iCallID);
        }
        break;
    case BTTEST_API_TEST:
        {
            OnInterfaceTest(objMsg);
        }
        break;
    case BTTEST_POWER_ON:
        {
            if (objMsg.lParam == 0)
            {
                PowerSet(m_btHandle, true);
            }
            else
            {
                bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(objMsg.lParam);
                pDevice->bPower = true;
            }
        }
        break;
    case BTTEST_POWER_OFF:
        {
            if (objMsg.lParam == 0)
            {
                PowerSet(m_btHandle, false);
            }
            else
            {
                bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(objMsg.lParam);
                if (NULL == pDevice)
                {
                    break;
                }
                pDevice->bPower = false;
                _WirelessSessionManager.DestorySessionByHandle(objMsg.lParam, BT_TEST_SESSION, true);
            }
        }
        break;
    case BTTEST_DISAPPEAR:
        {
            bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(objMsg.lParam);
            if (NULL != pDevice)
            {
                pDevice->bPower = false;
                _WirelessSessionManager.DestorySessionByHandle(objMsg.lParam, BT_TEST_SESSION, true);
            }
        }
        break;
    case BTTEST_ASKPAIR:
        {
            bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(objMsg.lParam);

            if (NULL != pDevice)
            {
                _WirelessSessionManager.InitSession(SESSION_PAIRING_STATE, BT_TEST_SESSION, pDevice->dev_hnd);
            }
        }
        break;
    //case BTTEST_PROPERTYCHANGE:
    //  break;
    default:
        break;
    }
}

//耳机接起电话
bool CBTTestDriver::ProcessHSAnswerCall(int iCallID)
{
    if (0 == m_btHandle)
    {
        return false;
    }

    CSessionBase * pSession = _WirelessSessionManager.GetSessionByState(SESSION_CONNECTED_STATE,
                              BT_TEST_SESSION);
    if (NULL == pSession)
    {
        return false;
    }

    //Get Focus call id
    if (iCallID <= 0)
    {
        iCallID = GetFoucsCallID(pSession);
    }

    WirelessSessionData wsData;
    wsData.ulMessage = WIRELESS_OPTION_BT_DEVICECALLANSWER;
    wsData.wParam = pSession->GetHandle();
    wsData.lParam = iCallID;

    return pSession->ProcessOption(wsData);
}

//耳机挂断电话
bool CBTTestDriver::ProcessHSEndCall(int iCallID)
{
    if (0 == m_btHandle)
    {
        return false;
    }

    CSessionBase * pSession = _WirelessSessionManager.GetSessionByState(SESSION_CONNECTED_STATE,
                              BT_TEST_SESSION);
    if (NULL == pSession)
    {
        return false;
    }

    //Get Focus call id
    if (iCallID <= 0)
    {
        iCallID = GetFoucsCallID(pSession);
    }

    WirelessSessionData wsData;
    wsData.ulMessage = WIRELESS_OPTION_BT_DEVICECALLEND;
    wsData.wParam = pSession->GetHandle();
    wsData.lParam = iCallID;

    return pSession->ProcessOption(wsData);
}

//断开设备
bool CBTTestDriver::ProcessDisconnectDevice()
{
    if (0 == m_btHandle)
    {
        return false;
    }

    _WirelessSessionManager.DeleteSession(_WirelessSessionManager.GetSessionByState(
            SESSION_CONNECTED_STATE, BT_TEST_SESSION), true);
}

//来电
bool CBTTestDriver::ProcessIncommingCall()
{
    if (0 == m_btHandle)
    {
        return false;
    }

    CSessionBase * pSession = _WirelessSessionManager.GetSessionByState(SESSION_CONNECTED_STATE,
                              BT_TEST_SESSION);
    if (NULL == pSession)
    {
        return false;
    }

    yl::string strNum = commonAPI_FormatString("%d", CBTTestData::GetRandNumber() % 10000000000);

    WirelessSessionData wsData;
    wsData.ulMessage = WIRELESS_OPTION_BT_DEVICECALLIN;
    wsData.wParam = pSession->GetHandle();
    wsData.pData = (void *)strNum.c_str();

    return pSession->ProcessOption(wsData);

}

//HOLD 通话
bool CBTTestDriver::ProcessHoldCall(int iCallID, int iHold)
{
    if (0 == m_btHandle)
    {
        return false;
    }

    CSessionBase * pSession = _WirelessSessionManager.GetSessionByState(SESSION_CONNECTED_STATE,
                              BT_TEST_SESSION);
    if (NULL == pSession)
    {
        return false;
    }

    WirelessSessionData wsData;
    wsData.ulMessage = WIRELESS_OPTION_BT_DEVICECALLHOLD;
    wsData.wParam = pSession->GetHandle();
    wsData.lParam = iCallID;
    wsData.pData = (void *)&iHold;
    return pSession->ProcessOption(wsData);
}

//挂断来电
bool CBTTestDriver::ProcessHangUpCall(int iCallID)
{
    if (0 == m_btHandle)
    {
        return false;
    }

    CSessionBase * pSession = _WirelessSessionManager.GetSessionByState(SESSION_CONNECTED_STATE,
                              BT_TEST_SESSION);
    if (NULL == pSession)
    {
        return false;
    }

    WirelessSessionData wsData;
    wsData.ulMessage = WIRELESS_OPTION_BT_DEVICECALLEND;
    wsData.wParam = pSession->GetHandle();
    wsData.lParam = iCallID;

    return pSession->ProcessOption(wsData);
}

//处理关闭电源
bool CBTTestDriver::ProcessPowerOff()
{
    return _WirelessSessionManager.DeleteAllSession(BT_TEST_SESSION, true);
}

//定时器消息
bool CBTTestDriver::ProcessTimer(int iTimerID)
{
    if (_WirelessSessionManager.SendTimerMsgToSession(iTimerID, BT_TEST_SESSION))
    {
        return true;
    }

    return false;
}

bool CBTTestDriver::SendCallBackMsg(btkit_msg_t * pMsg, void * pUserMag)
{
    if (NULL == pMsg
            || NULL == m_callbackFunc)
    {
        return false;
    }

    pMsg->bt_instance = m_btHandle;

    m_callbackFunc(pMsg, pUserMag);
    return true;
}

//初始化驱动
int CBTTestDriver::InitDriver(btkit_handle_t * dongle_hnd, btkit_create_t * create_t)
{
    if (NULL == create_t)
    {
        return -1;
    }

    m_btHandle = CBTTestData::GetRandNumber();
    *dongle_hnd = m_btHandle;

    m_callbackFunc = create_t->event_cb;

    return 0;
}

//销毁驱动
int CBTTestDriver::DestoryDriver(btkit_handle_t dongle_hnd)
{
    if (dongle_hnd != m_btHandle)
    {
        return -1;
    }

    if (m_bPower)
    {
        PowerSet(m_btHandle, false);
    }

    m_btHandle = 0;
    m_bPower = false;

    return 0;
}

//Power
int CBTTestDriver::PowerSet(btkit_handle_t dongle_hnd, bool bEnable)
{
    if (dongle_hnd != m_btHandle
            || NULL == m_callbackFunc)
    {
        return -1;
    }

    m_bPower = bEnable;

    if (!m_bPower && IsInScan())
    {
        StopScan(m_btHandle);
    }

    if (!m_bPower)
    {
        ProcessPowerOff();
    }

    btkit_msg_t btkitMsg;
    btkitMsg.bt_instance = m_btHandle;
    btkitMsg.bt_event = m_bPower ? BT_POWER_ON : BT_POWER_OFF;

    SendCallBackMsg(&btkitMsg, NULL);
    return 0;
}

//获取适配器属性
int CBTTestDriver::GetDongleProperty(btkit_handle_t dongle_hnd, btkit_property_t * dongleProperty)
{
    if (NULL == dongleProperty
            || m_btHandle != dongle_hnd)
    {
        return -1;
    }

    return _BTTestData.GetDongleProperty(dongleProperty) ? 0 : -1;
}

//设置适配器名称
int CBTTestDriver::SetDongleName(btkit_handle_t dongle_hnd, const char * szName)
{
    if (m_btHandle != dongle_hnd)
    {
        return -1;
    }

    return _BTTestData.SetDongleName(szName) ? 0 : -1;
}

//获取设备类型
btkit_dev_type_t CBTTestDriver::GetDeviceType(btkit_handle_t dongle_hnd,
        btkit_dev_instance_t dev_hnd)
{
    if (m_btHandle != dongle_hnd)
    {
        return BT_UNKNOWN_TYPE;
    }

    return _BTTestData.GetDeviceType(dev_hnd);
}

//获取设备属性
int CBTTestDriver::GetDeviceProperty(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd,
                                     btkit_dev_t * devProperty)
{
    if (m_btHandle != dongle_hnd)
    {
        return -1;
    }

    bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(dev_hnd);
    if (NULL == pDevice)
    {
        return -1;
    }

    memcpy(devProperty, &pDevice->dev_property, sizeof(btkit_dev_t));
    return 0;
}

//获取手机属性
int CBTTestDriver::GetMbProperty(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd,
                                 btkit_mb_property_t * property)
{
    if (m_btHandle != dongle_hnd)
    {
        return -1;
    }

    bttest_mb_t * pMb = _BTTestData.GetMbInfo(dev_hnd);
    if (NULL == pMb)
    {
        return -1;
    }

    memcpy(property, &pMb->mb_property, sizeof(btkit_mb_property_t));
    return 0;
}

//获取设备地址
int CBTTestDriver::GetDeviceMac(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd,
                                char * szMac)
{
    if (m_btHandle != dongle_hnd)
    {
        return -1;
    }

    bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(dev_hnd);
    if (NULL == pDevice)
    {
        return -1;
    }

    strcpy(szMac, pDevice->dev_property.bt_dev_addr);

    return 0;
}

//设备是否匹配
bool CBTTestDriver::IsDevicePaired(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd)
{
    if (dongle_hnd != m_btHandle)
    {
        return false;
    }

    bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(dev_hnd);

    if (NULL == pDevice)
    {
        return false;
    }

    return (pDevice->dev_property.paired != 0);
}

//设备是否已连接
bool CBTTestDriver::IsDeviceConnected(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd)
{
    if (dongle_hnd != m_btHandle)
    {
        return false;
    }

    bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(dev_hnd);

    if (NULL == pDevice)
    {
        return false;
    }

    return (pDevice->dev_property.connected != 0);
}

//获取保存设备数量
int CBTTestDriver::GetDeviceSum(btkit_handle_t dongle_hnd)
{
    if (dongle_hnd != m_btHandle)
    {
        return 0;
    }

    return _BTTestData.GetDeviceSum();
}

//获取设备句柄列表
int CBTTestDriver::GetDeviceHandle(btkit_handle_t dongle_hnd, btkit_dev_instance_t * devList)
{
    if (dongle_hnd != m_btHandle
            || NULL == devList)
    {
        return -1;
    }

    return (_BTTestData.GetDeviceHandleList(devList) > 0) ? 0 : -1;
}

//移除设备
int CBTTestDriver::RemoveDevice(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd)
{
    if (m_btHandle != dongle_hnd
            || IsInScan())
    {
        return -1;
    }

    _WirelessSessionManager.DestorySessionByHandle(dev_hnd, BT_TEST_SESSION, true);

    bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(dev_hnd);

    if (NULL == pDevice)
    {
        return -1;
    }

    pDevice->dev_property.paired = 0;
    pDevice->dev_property.connected = 0;
    pDevice->state = WIRE_DEVICE_INIT_STATE;

    return 0;
}

//开始扫描
int CBTTestDriver::StartScan(btkit_handle_t dongle_hnd)
{
    if (m_btHandle != dongle_hnd
            || _WirelessSessionManager.IsInState(BT_TEST_SESSION, SESSION_SCAN_STATE, BT_DEVICE_HANDLE_INVAILED,
                    NULL))
    {
        return -1;
    }

    return _WirelessSessionManager.InitSession(SESSION_SCAN_STATE, BT_TEST_SESSION) ? 0 : -1;
}

//停止扫描
int CBTTestDriver::StopScan(btkit_handle_t dongle_hnd)
{
    if (m_btHandle != dongle_hnd)
    {
        return -1;
    }

    return _WirelessSessionManager.DeleteSession(_WirelessSessionManager.GetSessionByState(
                SESSION_SCAN_STATE, BT_TEST_SESSION), true) ? 0 : -1;
}

//连接设备
int CBTTestDriver::ConnectDevice(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd)
{
    if (m_btHandle != dongle_hnd
            || IsInScan())
    {
        return -1;
    }

    WirelessSessionData wsData;
    wsData.ulMessage = WIRELESS_OPTION_BTTEST_CONNECT;
    wsData.wParam = dev_hnd;

    if (!_WirelessSessionManager.SendOptionToSessionByHandle(dev_hnd, wsData, BT_TEST_SESSION))
    {
        return _WirelessSessionManager.InitSession(SESSION_CONNECTING_STATE, BT_TEST_SESSION,
                dev_hnd) ? 0 : -1;
    }

    return 0;
}

//取消连接设备
int CBTTestDriver::DisconnectDevice(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd)
{
    if (m_btHandle != dongle_hnd
            || IsInScan())
    {
        return -1;
    }

    return _WirelessSessionManager.DestorySessionByHandle(dev_hnd, BT_TEST_SESSION, true) ? 0 : -1;
}

//匹配设备
int CBTTestDriver::PairDevice(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd)
{
    if (m_btHandle != dongle_hnd
            || IsInScan())
    {
        return -1;
    }

    int iNum = _BTTestData.GetRandNumber();

    //特殊处理,验证设备已经匹配的逻辑
    //if (iNum % 2)
    //{
    //  WIRE_INFO("bttest: device[%d] already paired", dev_hnd);
    //  bttest_dev_t* pDevice = _BTTestData.GetDeviceInfo(dev_hnd);
    //  if (NULL == pDevice)
    //  {
    //      return -1;
    //  }

    //  pDevice->dev_property.paired = 1;
    //  pDevice->state = WIRE_DEVICE_PAIRED_STATE;

    //  return 1;
    //}

    return _WirelessSessionManager.InitSession(SESSION_PAIRING_STATE, BT_TEST_SESSION,
            dev_hnd) ? 0 : -1;
}

//应答PIN码
int CBTTestDriver::AnswerPin(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd,
                             const char * szPin)
{
    if (m_btHandle != dongle_hnd
            || NULL == szPin
            || IsInScan())
    {
        return -1;
    }

    WirelessSessionData wsData;
    wsData.ulMessage = WIRELESS_OPTION_BTTEST_ANSWERPIN;
    wsData.wParam = dev_hnd;
    wsData.pData = (void *)szPin;

    return _WirelessSessionManager.SendOptionToSessionByHandle(dev_hnd, wsData,
            BT_TEST_SESSION) ? 0 : -1;
}

int CBTTestDriver::PinConfirm(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd, int iConfirm)
{
    if (m_btHandle != dongle_hnd
            || IsInScan())
    {
        return -1;
    }

    WirelessSessionData wsData;
    wsData.ulMessage = WIRELESS_OPTION_BTTEST_PINCONFIRM;
    wsData.wParam = dev_hnd;
    wsData.lParam = iConfirm;

    return _WirelessSessionManager.SendOptionToSessionByHandle(dev_hnd, wsData,
            BT_TEST_SESSION) ? 0 : -1;
}

//取消配对
int CBTTestDriver::CancelPairDevice(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd)
{
    if (m_btHandle != dongle_hnd
            || IsInScan())
    {
        return -1;
    }

    return _WirelessSessionManager.DestorySessionByHandle(dev_hnd, BT_TEST_SESSION, true) ? 0 : -1;
}

//设备呼入
int CBTTestDriver::DeviceCallIn(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd,
                                const char * szNum)
{
    if (m_btHandle != dongle_hnd
            || NULL == szNum)
    {
        return -1;
    }

    if (!_WirelessSessionManager.IsInState(BT_TEST_SESSION, SESSION_CONNECTED_STATE, dev_hnd))
    {
        return -1;
    }

    return SendOptionToSession(WIRELESS_OPTION_BT_DEVICECALLIN, dev_hnd, 0, (void *)szNum) ? 0 : -1;
}

int CBTTestDriver::DeviceCallInvite(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd,
                                    const char * szNum, int * iCallID)
{
    return DeviceCallOut(dongle_hnd, dev_hnd, szNum, iCallID);
}

//设备呼出
int CBTTestDriver::DeviceCallOut(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd,
                                 const char * szNum, int * callid)
{
    if (m_btHandle != dongle_hnd
            || NULL == szNum)
    {
        return -1;
    }

    if (!_WirelessSessionManager.IsInState(BT_TEST_SESSION, SESSION_CONNECTED_STATE, dev_hnd))
    {
        return -1;
    }

    SendOptionToSession(WIRELESS_OPTION_BT_DEVICECALLOUT, dev_hnd, 0, (void *)szNum);

    //获取call id
    *callid = GetFoucsCallID();

    return 0;
}

//设备应答
int CBTTestDriver::DeviceCallAnswer(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd,
                                    int iCallID)
{
    if (m_btHandle != dongle_hnd)
    {
        return -1;
    }

    if (!_WirelessSessionManager.IsInState(BT_TEST_SESSION, SESSION_CONNECTED_STATE, dev_hnd))
    {
        return -1;
    }

    return SendOptionToSession(WIRELESS_OPTION_BT_DEVICECALLANSWER, dev_hnd, iCallID, NULL) ? 0 : -1;
}

//设备结速通话
int CBTTestDriver::DeviceCallEnd(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd,
                                 int iCallID)
{
    if (m_btHandle != dongle_hnd)
    {
        return -1;
    }

    if (!_WirelessSessionManager.IsInState(BT_TEST_SESSION, SESSION_CONNECTED_STATE, dev_hnd))
    {
        return -1;
    }

    return SendOptionToSession(WIRELESS_OPTION_BT_DEVICECALLEND, dev_hnd, iCallID, NULL) ? 0 : -1;
}

//获取焦点Call ID
int CBTTestDriver::GetFoucsCallID(CSessionBase * pSession /*= NULL*/)
{
#ifdef IF_BT_SIMULATION_DRIVER
    CBTTestSession * pConnectedSession = dynamic_cast<CBTTestSession *>(pSession);

    if (NULL == pConnectedSession)
    {
        pConnectedSession = dynamic_cast<CBTTestSession *>(_WirelessSessionManager.GetSessionByState(
                                SESSION_CONNECTED_STATE, BT_TEST_SESSION));
    }

    if (NULL == pConnectedSession)
    {
        return -1;
    }

    CBaseState * pState = pConnectedSession->GetStatePtr();

    CBTTestConnectedState * pConnectedState = dynamic_cast<CBTTestConnectedState *>(pState);
    if (NULL == pConnectedState)
    {
        return -1;
    }

    return pConnectedState->GetFoucsCallID();
#else
    return -1;
#endif //IF_BT_SIMULATION_DRIVER

}

//获取话路数量
int CBTTestDriver::GetCallCount()
{
#ifdef IF_BT_SIMULATION_DRIVER
    CBTTestSession * pSession = dynamic_cast<CBTTestSession *>
                                (_WirelessSessionManager.GetSessionByState(SESSION_CONNECTED_STATE, BT_TEST_SESSION));

    if (NULL == pSession)
    {
        return 0;
    }

    CBaseState * pState = pSession->GetStatePtr();

    CBTTestConnectedState * pConnectedState = dynamic_cast<CBTTestConnectedState *>(pState);
    if (NULL == pConnectedState)
    {
        return 0;
    }

    return pConnectedState->GetCallCount();
#else
    return 0;
#endif // IF_BT_SIMULATION_DRIVER
}

//获取通话状态
int CBTTestDriver::GetCallStatus(int iCallID)
{
#ifdef IF_BT_SIMULATION_DRIVER
    CBTTestSession * pSession = dynamic_cast<CBTTestSession *>
                                (_WirelessSessionManager.GetSessionByState(SESSION_CONNECTED_STATE, BT_TEST_SESSION));

    if (NULL == pSession)
    {
        return 0;
    }

    CBaseState * pState = pSession->GetStatePtr();

    CBTTestConnectedState * pConnectedState = dynamic_cast<CBTTestConnectedState *>(pState);
    if (NULL == pConnectedState)
    {
        return 0;
    }

    return pConnectedState->GetCallStatus(iCallID);
#else
    return 0;
#endif // IF_BT_SIMULATION_DRIVER
}

//获取当前设备handle
unsigned long CBTTestDriver::GetCurrendDevHandle()
{
    CSessionBase * pSession = _WirelessSessionManager.GetSessionByState(SESSION_CONNECTED_STATE,
                              BT_TEST_SESSION);

    if (NULL != pSession)
    {
        return pSession->GetHandle();
    }

    return BT_DEVICE_HANDLE_INVAILED;
}

//发送操作至Session
bool CBTTestDriver::SendOptionToSession(unsigned long ulMessage, WPARAM wParam, LPARAM lParam,
                                        void * pData)
{
    WirelessSessionData wsData;
    wsData.ulMessage = ulMessage;
    wsData.wParam = wParam;
    wsData.lParam = lParam;
    wsData.pData = pData;

    return _WirelessSessionManager.SendOptionToSessionByHandle(wParam, wsData, BT_TEST_SESSION);
}

//设备HOLD
int CBTTestDriver::DeviceCallHold(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd,
                                  int iCallID, int iHold)
{
    if (m_btHandle != dongle_hnd)
    {
        return -1;
    }

    if (!_WirelessSessionManager.IsInState(BT_TEST_SESSION, SESSION_CONNECTED_STATE, dev_hnd))
    {
        return -1;
    }

    return SendOptionToSession(WIRELESS_OPTION_BT_DEVICECALLHOLD, dev_hnd, iCallID,
                               (void *)&iHold) ? 0 : -1;
}

//获取通话数量
int CBTTestDriver::GetCallSum(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd, int * iTotal)
{
    if (m_btHandle != dongle_hnd)
    {
        return -1;
    }

    if (!_WirelessSessionManager.IsInState(BT_TEST_SESSION, SESSION_CONNECTED_STATE, dev_hnd))
    {
        *iTotal = 0;
        return 0;
    }

    *iTotal = GetCallCount();

    return 0;
}

//获取通话状态
int CBTTestDriver::GetCallStatus(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd,
                                 int iCallID, int * iCallStatus)
{
    if (m_btHandle != dongle_hnd)
    {
        return -1;
    }

    if (!_WirelessSessionManager.IsInState(BT_TEST_SESSION, SESSION_CONNECTED_STATE, dev_hnd))
    {
        return -1;
    }

    *iCallStatus = GetCallStatus(iCallID);

    return 0;
}

//声音源切换
int CBTTestDriver::SwitchChannel(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd,
                                 int iCallID, int iMode)
{
    if (m_btHandle != dongle_hnd)
    {
        return -1;
    }

    if (!_WirelessSessionManager.IsInState(BT_TEST_SESSION, SESSION_CONNECTED_STATE, dev_hnd))
    {
        return -1;
    }

    bool bChannel;
    if (BT_SND_MODE_LOC_TO_MB == iMode)
    {
        bChannel = false;
    }
    else if (BT_SND_MODE_MB_TO_LOC == iMode)
    {
        bChannel = true;
    }
    else
    {
        return -1;
    }

    if ((m_bChannelInMb && !bChannel)
            || (!m_bChannelInMb && bChannel))
    {
        return -1;
    }

    m_bChannelInMb = !bChannel;

    btkit_msg_t btkitMsg;
    btkitMsg.bt_event = bChannel ? BT_MB_CHANNEL_LINK : BT_MB_CHANNEL_UNLINK;
    btkitMsg.bt_instance = m_btHandle;

    return SendCallBackMsg(&btkitMsg, NULL) ? 0 : -1;
}

//下载联系人
int CBTTestDriver::StartDownloadContact(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd,
                                        btkit_pba_data_t * data)
{
    if (m_btHandle != hnd
            || NULL == data)
    {
        return -1;
    }

    yl::string strPath;

    bool bResult = _BTTestData.StartDownloadContact(dev_hnd, data, strPath);

    btkit_msg_t btkitMsg;
    btkitMsg.bt_event = bResult ? BT_PBA_DOWNLOAD_COMPLETE : BT_PBA_DOWNLOAD_FAIL;
    btkitMsg.bt_instance = m_btHandle;
    if (bResult)
    {
        strncpy(btkitMsg.bt_callback_param.pb_file.file, strPath.c_str(), BT_MAX_NAME_LEN);
        btkitMsg.bt_callback_param.pb_file.file[BT_MAX_NAME_LEN - 1] = '\0';
    }

    //etlSleep(500);
    return SendCallBackMsg(&btkitMsg, NULL) ? 0 : -1;
}

//取消下载联系人
int CBTTestDriver::CancelDownloadContact(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd)
{
    if (m_btHandle != hnd)
    {
        return -1;
    }

    return 0;
}

void BT_GetInt(int & iParam)
{
    int iInput;

    scanf("%d", &iInput);
    iParam = iInput;
    printf("%d\n", iParam);
}

void BT_GetString(yl::string & strParam)
{
    char szInput[1024] = {0};
    scanf("%s", szInput);
    strParam = szInput;
    printf("%s\n", strParam.c_str());
}

void BT_ShowBluetoothListInfo(UI_LIST_BT_DEVICE & listDevice)
{
    for (UI_LIST_BT_DEVICE::iterator itBegin = listDevice.begin(); itBegin != listDevice.end();
            ++itBegin)
    {
        DeviceInfo & pTempInfo = (*itBegin);

        WIRE_INFO("name[%s], handle[%d], mac [%s], state[%d], type[%d], autoconnect[%d], synccontact[%d], priority[%d]",
                  pTempInfo.strName.c_str(),
                  pTempInfo.devHandle,
                  pTempInfo.strMac.c_str(),
                  /*DeviceStateToString*/(pTempInfo.eState),
                  pTempInfo.eType,
                  pTempInfo.bAutoConnect,
                  pTempInfo.bSyncContact,
                  pTempInfo.iPriority
                 );
    }
}

//接口测试
void CBTTestDriver::OnInterfaceTest(msgObject & objMessage)
{
    int iChoise = objMessage.lParam;
    yl::string strMac;
    int iParam;
    int iSessionID;
    CBaseSession * pSession = NULL;

    switch (objMessage.wParam)
    {
    case BTTEST_TALKANSWER:
        talklogic_AcceptIncomingCall();
        break;
    case BTTEST_TALKHANGUP:
        talklogic_ExitFocusSession();
        break;
    case BTTEST_TALKREJECT:
        {
            iSessionID = talklogic_GetFocusedSessionID();
            pSession = _SessionManager.GetSession(iSessionID);
            if (NULL != pSession)
            {
                pSession->ProcessIncomingCall(false);
            }
        }
        break;
    case BTTEST_API_TEST:
        {
            if (1 == iChoise)  //开启蓝牙
            {
                Bluetooth_SetEnable(true);
                WIRE_INFO("bt state is %d\n", Bluetooth_IsEnable());
                UI_LIST_BT_DEVICE listDevice;
                Bluetooth_GetDeviceList(listDevice);
                BT_ShowBluetoothListInfo(listDevice);
            }
            else if (2 == iChoise) //关闭蓝牙
            {
                Bluetooth_SetEnable(false);
                WIRE_INFO("bt state is %d\n", Bluetooth_IsEnable());
                UI_LIST_BT_DEVICE listDevice;
                Bluetooth_GetDeviceList(listDevice);
                BT_ShowBluetoothListInfo(listDevice);
            }
            else if (3 == iChoise) //开始搜索
            {
                Bluetooth_StartScanDevice();
                WIRE_INFO("bt state is %d\n", Bluetooth_GetDeviceState());
            }
            else if (4 == iChoise) //停止搜索
            {
                Bluetooth_StopScanDevice();
                UI_LIST_BT_DEVICE listDevice;
                Bluetooth_GetDeviceList(listDevice);
                BT_ShowBluetoothListInfo(listDevice);
                WIRE_INFO("bt state is %d\n", Bluetooth_GetDeviceState());
            }
            else if (5 == iChoise) //Bluetooth_ConnectDevice
            {
                UI_LIST_BT_DEVICE listDevice;
                Bluetooth_GetDeviceList(listDevice);
                BT_ShowBluetoothListInfo(listDevice);

                WIRE_INFO("Enter Handle");
                BT_GetInt(iParam);
                DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(iParam);
                if (NULL != pDevice)
                {
                    Bluetooth_ConnectDevice(pDevice->strMac.c_str());
                }

                WIRE_INFO("bt state is %d\n", Bluetooth_GetDeviceState());
            }
            else if (6 == iChoise) //Bluetooth_AnswerPin
            {
                UI_LIST_BT_DEVICE listDevice;
                Bluetooth_GetDeviceList(listDevice);
                BT_ShowBluetoothListInfo(listDevice);

                WIRE_INFO("Enter Handle");
                BT_GetInt(iParam);
                DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(iParam);
                if (NULL != pDevice)
                {
                    yl::string strPIN = "0000";
                    WIRE_INFO("enter pin code");
                    BT_GetString(strPIN);

                    Bluetooth_AnswerPin(pDevice->strMac.c_str(), strPIN.c_str());
                }

                WIRE_INFO("bt state is %d\n", Bluetooth_GetDeviceState());
            }
            else if (7 == iChoise) //Bluetooth_DisconnectDevice
            {
                UI_LIST_BT_DEVICE listDevice;
                Bluetooth_GetDeviceList(listDevice);
                BT_ShowBluetoothListInfo(listDevice);

                WIRE_INFO("Enter Handle");
                BT_GetInt(iParam);
                DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(iParam);
                if (NULL != pDevice)
                {
                    Bluetooth_DisconnectDevice(pDevice->strMac.c_str());
                }

                WIRE_INFO("bt state is %d\n", Bluetooth_GetDeviceState());
            }
            else if (8 == iChoise) //Bluetooth_DeleteDevice
            {
                UI_LIST_BT_DEVICE listDevice;
                Bluetooth_GetDeviceList(listDevice);
                BT_ShowBluetoothListInfo(listDevice);

                WIRE_INFO("Enter Handle");
                BT_GetInt(iParam);
                DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(iParam);
                if (NULL != pDevice)
                {
                    Bluetooth_DeleteDevice(pDevice->strMac.c_str());
                }
                else
                {
                    Bluetooth_DeleteDevice();
                }

                Bluetooth_GetDeviceList(listDevice);
                BT_ShowBluetoothListInfo(listDevice);
            }
            else if (9 == iChoise) //Bluetooth_SetAutoConnect
            {
                UI_LIST_BT_DEVICE listDevice;
                Bluetooth_GetDeviceList(listDevice);
                BT_ShowBluetoothListInfo(listDevice);

                WIRE_INFO("Enter Handle");
                BT_GetInt(iParam);
                DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(iParam);
                if (NULL != pDevice)
                {
                    WIRE_INFO("Enable = 1;otherwise Disable");
                    BT_GetInt(iParam);
                    Bluetooth_SetAutoConnect(pDevice->strMac.c_str(), (iParam == 1) ? true : false);
                }

                Bluetooth_GetDeviceList(listDevice);
                BT_ShowBluetoothListInfo(listDevice);
            }
            else if (10 == iChoise) //Bluetooth_SetSyncContact
            {
                UI_LIST_BT_DEVICE listDevice;
                Bluetooth_GetDeviceList(listDevice);
                BT_ShowBluetoothListInfo(listDevice);

                WIRE_INFO("Enter Handle");
                BT_GetInt(iParam);
                DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(iParam);
                if (NULL != pDevice)
                {
                    WIRE_INFO("Enable = 1;otherwise Disable");
                    BT_GetInt(iParam);
                    Bluetooth_SetSyncContact(pDevice->strMac.c_str(), (iParam == 1) ? true : false);
                }

                Bluetooth_GetDeviceList(listDevice);
                BT_ShowBluetoothListInfo(listDevice);
            }
            else if (11 == iChoise) //Bluetooth_SetDeviceName
            {
                DeviceInfo devInfo;
                Bluetooth_GetDeviceInfo(devInfo);
                WIRE_INFO("device mac[%s]:name[%s]", devInfo.strMac.c_str(), devInfo.strName.c_str());

                WIRE_INFO("input a name\n");
                BT_GetString(strMac);
                Bluetooth_SetDeviceName(strMac.c_str());

                Bluetooth_GetDeviceInfo(devInfo);
                WIRE_INFO("device mac[%s]:name[%s]", devInfo.strMac.c_str(), devInfo.strName.c_str());
            }
            else if (12 == iChoise) //Bluetooth_AdjustDevicePos
            {
                UI_LIST_BT_DEVICE listDevice;
                Bluetooth_GetDeviceList(listDevice);
                BT_ShowBluetoothListInfo(listDevice);

                WIRE_INFO("Enter Handle");
                BT_GetInt(iParam);
                DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(iParam);
                if (NULL != pDevice)
                {
                    WIRE_INFO("up = 1;otherwise down");
                    BT_GetInt(iParam);
                    Bluetooth_AdjustDevicePos(pDevice->strMac.c_str(), (iParam == 1) ? true : false);
                }

                listDevice.clear();
                Bluetooth_GetDeviceList(listDevice);
                BT_ShowBluetoothListInfo(listDevice);
            }
            else if (13 == iChoise) //Bluetooth_MoveDeviceToPos
            {
                UI_LIST_BT_DEVICE listDevice;
                Bluetooth_GetDeviceList(listDevice);
                BT_ShowBluetoothListInfo(listDevice);

                WIRE_INFO("Enter Handle");
                BT_GetInt(iParam);
                DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(iParam);
                if (NULL != pDevice)
                {
                    WIRE_INFO("enter a position");
                    BT_GetInt(iParam);
                    Bluetooth_MoveDeviceToPos(pDevice->strMac.c_str(), iParam);
                }

                Bluetooth_GetDeviceList(listDevice);
                BT_ShowBluetoothListInfo(listDevice);
            }
            else if (14 == iChoise) //Bluetooth_CancelPair
            {
                UI_LIST_BT_DEVICE listDevice;
                Bluetooth_GetDeviceList(listDevice);
                BT_ShowBluetoothListInfo(listDevice);

                WIRE_INFO("Enter Handle");
                BT_GetInt(iParam);
                DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(iParam);
                if (NULL != pDevice)
                {
                    Bluetooth_CancelPair(pDevice->strMac.c_str());
                }

                WIRE_INFO("bt state is %d\n", Bluetooth_GetDeviceState());
            }
            else if (15 == iChoise) //check status
            {
                etl_NotifyApp(FALSE, BLUETOOTH_MSG_GETSTATUS, 0, 0);
            }
            else if (16 == iChoise) //Bluetooth_CallMobileInvite
            {
                WIRE_INFO("Enter Mobile Number");
                BT_GetString(strMac);
                Bluetooth_CallMobileInvite(strMac.c_str());
            }
            else if (17 == iChoise) //Bluetooth_AnswerMobile
            {
                WIRE_INFO("Enter CallID");
                BT_GetInt(iParam);
                Bluetooth_AnswerMobile(iParam);
            }
            else if (18 == iChoise) //Bluetooth_RefuseMobile
            {
                WIRE_INFO("Enter CallID");
                BT_GetInt(iParam);
                Bluetooth_RefuseMobile(iParam);
            }
            else if (19 == iChoise) //Bluetooth_HangupMobile
            {
                WIRE_INFO("Enter CallID");
                BT_GetInt(iParam);
                Bluetooth_HangupMobile(iParam);
            }
            else if (20 == iChoise) //Bluetooth_HoldMobile
            {
                WIRE_INFO("Enter CallID");
                BT_GetInt(iParam);

                int iHold;
                WIRE_INFO("1:hold,otherwise resume");
                BT_GetInt(iHold);
                Bluetooth_HoldMobile(iParam, iHold == 1);
            }
            else if (21 == iChoise) //Bluetooth_SendMobileDTMF
            {
                WIRE_INFO("Enter CallID");
                BT_GetInt(iParam);

                int iCode;
                WIRE_INFO("enter dtmf code");
                BT_GetInt(iCode);
                Bluetooth_SendMobileDTMF(iParam, iCode);
            }
            else if (22 == iChoise) //Bluetooth_SwitchChannel
            {
                WIRE_INFO("Enter CallID");
                BT_GetInt(iParam);

                int iChannel;
                WIRE_INFO("1:To Sip Phone,otherwise To Mobile");
                BT_GetInt(iChannel);
                Bluetooth_SwitchChannel(iChannel == 1, iParam);
            }
            else if (23 == iChoise)
            {
                ContactListData listBtDir;
                Dir_GetContactListByGroupId(&listBtDir, kszBluethoothGroupId, BLUETOOTH_DIR);

                WIRE_INFO("bluetooth list size[%d]", listBtDir.m_listId.size());

                for (YLVector<int>::iterator itBeg = listBtDir.m_listId.begin();
                        itBeg != listBtDir.m_listId.end();
                        ++itBeg)
                {
                    SmartPtr pDetail = Dir_GetDetailById(*(itBeg), BLUETOOTH_DIR);
                    if (!pDetail.IsEmpty())
                    {
                        WIRE_INFO("name[%s] number[%s]", pDetail->GetDisplayName().c_str(), "sk");
                    }
                }
            }
            else if (24 == iChoise)
            {
                etl_NotifyApp(false, WIFI_MSG_UPDATE_UI, OP_WIFI_OPEN_RESULT, 0);
            }
            else
            {
                printf("\
						1: Bluetooth_SetEnable(true)\n \
						2: Bluetooth_SetEnable(false) \n \
						3: Bluetooth_StartScanDevice() \n \
						4: Bluetooth_StopScanDevice() \n \
						5: Bluetooth_ConnectDevice() \n \
						6: Bluetooth_AnswerPin() \n \
						7: Bluetooth_DisconnectDevice() \n \
						8: Bluetooth_DeleteDevice() \n \
						9: Bluetooth_SetAutoConnect() \n \
						10:Bluetooth_SetSyncContact() \n \
						11:Bluetooth_SetDeviceName() \n \
						12:Bluetooth_AdjustDevicePos(); \n \
						13:Bluetooth_MoveDeviceToPos() \n\
						14:Bluetooth_CancelPair() \n\
						15:check status \n\
						16:Bluetooth_CallMobileInvite \n\
						17:Bluetooth_AnswerMobile \n\
						18:Bluetooth_RefuseMobile \n\
						19:Bluetooth_HangupMobile \n\
						20:Bluetooth_HoldMobile \n\
						21:Bluetooth_SendMobileDTMF \n\
						22:Bluetooth_SwitchChannel \n\
						23:Bluetooth Contact test \n \
						");
            }
            break;
        }
    default:
        break;
    }
}

#endif // IF_BT_TEST_LOGIC
