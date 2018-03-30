#ifdef IF_SUPPORT_WIFI
#include "wifiuictrler.h"
#include "dskphone/ui/xwin/settingui/src/viewmodeuimanager.h"
#include "keymap.h"
//#include "viewmodeuimanager.h"
#include "translateiddefine.h"
#include "wireless/wifi/include/modwifi.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "viewmodeuimanager/include/uicommon.h"

#define _CASE_MSG_TO_STRING(wParam, strwParam) case wParam: strwParam = #wParam; break;

#define MSGBOX_TIMEOUT_ALWAYS       0
#define MSGBOX_TIMEOUT_SHORT        1500
#define MSGBOX_TIMEOUT_LONG         2000
#define MSGBOX_TIMEOUT_WPS          120000

// MsgBox 标识
enum MsgBoxID
{
    MSGBOX_ID_BT_NONE = -1,
    MSGBOX_ID_SCAN,
    MSGBOX_ID_WPS,
    MSGBOX_ID_WPS_PIN,
    MSGBOX_ID_PBC_OPEN_SWITCH
};

void PrintWifiMsg(msgObject & objMsg)
{
    yl::string strwParam("");

    switch (objMsg.wParam)
    {
        _CASE_MSG_TO_STRING(OP_UPDATE_WIFI_DATA, strwParam);
        _CASE_MSG_TO_STRING(OP_WIFI_OPEN_RESULT, strwParam);
        _CASE_MSG_TO_STRING(OP_WIFI_CLOSE_RESULT, strwParam);
        _CASE_MSG_TO_STRING(OP_SCAN_WIFI_RESULT, strwParam);
        _CASE_MSG_TO_STRING(OP_ENTER_WIFI_PWD, strwParam);
        _CASE_MSG_TO_STRING(OP_CONNECT_WIFI_RESULT, strwParam);
        _CASE_MSG_TO_STRING(OP_DISCONNECT_WIFI_RESULT, strwParam);
        _CASE_MSG_TO_STRING(OP_WPS_OPEN, strwParam);
        _CASE_MSG_TO_STRING(OP_WPSCONNECT_RESULT, strwParam);
        _CASE_MSG_TO_STRING(OP_WPSPINCONNECT_RESULT, strwParam);
        _CASE_MSG_TO_STRING(OP_WIFI_ADD_RESULT, strwParam);
        _CASE_MSG_TO_STRING(OP_WIFI_DONGLE_STATUS_MODIFY, strwParam)
    }

    WIRE_INFO("Receive logic update msg..wParam[%s], lParam[%d]", strwParam.c_str(), objMsg.lParam);
}

CWifiUICtrler::CWifiUICtrler()
    : m_iCurrentOperatingNet(WIFI_NET_ID_INVAILED),
      m_bUserCancelScan(false)
{
    Init();
}

CWifiUICtrler::~CWifiUICtrler()
{
    UnInit();
}

bool CWifiUICtrler::Init()
{
    etl_RegisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, CMSGSubscriber(this,
                          &CWifiUICtrler::OnWifiMsg));

    return true;
}

bool CWifiUICtrler::UnInit()
{
    etl_UnregisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, CMSGSubscriber(this,
                            &CWifiUICtrler::OnWifiMsg));

    return true;
}

bool CWifiUICtrler::OnWifiMsg(msgObject & objMsg)
{
    WirelessOperationMsg * pData = (WirelessOperationMsg *)objMsg.GetExtraData();

    if (objMsg.message != WIFI_MSG_UPDATE_UI
            || !pData)
    {
        return false;
    }

    PrintWifiMsg(objMsg);

    yl::string strMsg("");
    int iTimeOut = MSGBOX_TIMEOUT_SHORT;
    MsgBoxStyle eStyle = MSGBOX_STYLE_CANCEL;
    MsgBoxID eID = MSGBOX_ID_BT_NONE;

    bool bNeedUpdateUI = true;
    bool bNeedResetFocus = false;

    switch (objMsg.wParam)
    {
    case OP_UPDATE_WIFI_DATA:
        break;
    // 处理开关结果
    case OP_WIFI_OPEN_RESULT:
    case OP_WIFI_CLOSE_RESULT:
        ProcessSwitchResult(objMsg.wParam, objMsg.lParam);
        break;
    // 处理wifi扫描结束操作
    case OP_SCAN_WIFI_RESULT:
        _ViewModeUIMgr.RemoveMsgBox(MSGBOX_ID_SCAN);
        m_WifiUIData.SetUIScanState(false);
        bNeedResetFocus = _ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_SCAN;
        break;
    // 处理wifi鉴权操作
    case OP_ENTER_WIFI_PWD:
        ProcessEnterPsk(pData->lSessionID, objMsg.lParam == 1);
        break;
    // 处理wifi连接操作
    case OP_CONNECT_WIFI_RESULT:
        {
            // 扫描界面连接成功需重置焦点
            bNeedResetFocus = WIRELESS_OP_CONNECT_SUCCESS == objMsg.lParam
                              && _ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_SCAN;

            if (WIRELESS_OP_CONNECT_WRONGPWD == objMsg.lParam
                    || WIRELESS_OP_CONNECT_FAILED == objMsg.lParam)
            {
                strMsg = TRID_CONNECT_FAILED;
            }
        }
        break;
    // 处理wifi断开连接操作
    case OP_DISCONNECT_WIFI_RESULT:
        break;
    // 处理手动按下PBC按钮
    case OP_WPS_OPEN:
        {
            bNeedUpdateUI = false;
            if (!WIFI_IsEnable())
            {
                iTimeOut = MSGBOX_TIMEOUT_ALWAYS;
                eStyle = MSGBOX_STYLE_OK_CANCEL;
                strMsg = TRID_WIFI_SWITCH_IS_CLOSED_OPEN_IT_OR_NOT;
                eID = MSGBOX_ID_PBC_OPEN_SWITCH;
            }
            else
            {
                WpsOperation(true);
            }
        }
        break;
    // 处理wps连接操作
    case OP_WPSCONNECT_RESULT:
    case OP_WPSPINCONNECT_RESULT:
        ProcessWpsConnectResult(objMsg.wParam, objMsg.lParam);
        break;
    // 处理设备添加结果
    case OP_WIFI_ADD_RESULT:
        bNeedUpdateUI = false;
        if (WIRELESS_OP_ADD_LISTFULL == objMsg.lParam)
        {
            strMsg = TRID_STORAGE_LIST_IS_FULL_PLEASE_DELETE_OLD_ONE_IF_WANTING_TO_ADD_IN;
            iTimeOut = MSGBOX_TIMEOUT_ALWAYS;
        }
        break;
    // 处理dongle状态变化
    case OP_WIFI_DONGLE_STATUS_MODIFY:
        bNeedUpdateUI = WIRELESS_OP_DONGLE_REMOVE == objMsg.lParam;
        if (WIRELESS_OP_DONGLE_REMOVE == objMsg.lParam)
        {
            m_WifiUIData.SetUISwitchEnable(false);
            bNeedResetFocus = true;
        }
        break;
    default:
        bNeedUpdateUI = false;
        break;
    }

    if (bNeedUpdateUI && IsNeedUpdateUI(objMsg.wParam))
    {
        _ViewModeUIMgr.UpdateUI(bNeedResetFocus);
    }

    if (!strMsg.empty() && !WIFI_IsScan())
    {
        _ViewModeUIMgr.PopupMsgBox(eID, UIConmonAPI_Lang(TRID_NOTE),
                                   UIConmonAPI_Lang(strMsg), eStyle, iTimeOut);
    }

    return true;
}

bool CWifiUICtrler::OnSoftKeyEvent(int iKey, int iDataID,
                                   const yl::string & strValue,
                                   void * pData /*= NULL*/)
{
    bool bHandle = true;

    switch (iKey)
    {
    case ST_BACK:
        CancelOperation(iKey, true);
        break;
    case ST_WPS:
        _ViewModeUIMgr.ShowOption();
        break;
    case ST_SAVE:
    case ST_OK:
        ConfirmOperation(iKey, iDataID, strValue, pData);
        break;
    case ST_ADD:
        // 设备数量超过
        if (WIFI_GetSavedNetworkNum() >= WIFI_SAVED_MAX)
        {
            _ViewModeUIMgr.PopupMsgBox(MSGBOX_ID_BT_NONE, UIConmonAPI_Lang(TRID_NOTE),
                                       UIConmonAPI_Lang(TRID_LIST_IS_FULL_PLEASE_DELETE_BEFORE_ADD),
                                       MSGBOX_STYLE_CANCEL, MSGBOX_TIMEOUT_SHORT);
            break;
        }

        if (_ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_NET_DETAIL)
        {
            EnterPage(WIRELESS_UI_TYPE_WIFI_NET_DETAIL_2_ADD);
        }
        else
        {
            EnterPage(WIRELESS_UI_TYPE_WIFI_NET_ADD);
        }
        break;
    case ST_DETAIL:
        EnterPage(WIRELESS_UI_TYPE_WIFI_NET_DETAIL, iDataID);
        break;
    case ST_CONNECT:
    case ST_DISCONNECT:
        if (_ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_NET_DETAIL)
        {
            ConnectOperation(ST_CONNECT == iKey, m_iLastFocusDataID);
            if (ST_CONNECT == iKey)
            {
                ExitPage();
            }
            else
            {
                _ViewModeUIMgr.UpdateSoftKet();
            }
        }
        else
        {
            ConnectOperation(ST_CONNECT == iKey, iDataID);
        }
        break;
    case ST_CANCEL:
        CancelOperation(iKey, false);
        break;
    case ST_SCAN:
        ScanOperation(true);
        break;
    case ST_ENTER:
        EnterPage(iDataID, iDataID);
        break;
    case ST_OPTION:
        _ViewModeUIMgr.ShowOption();
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

bool CWifiUICtrler::OnFunKeyEvent(int iKey, int iDataID,
                                  yl::string & strValue, void * pData /*= NULL*/)
{
    bool bHandle = true;

    switch (iKey)
    {
    case PHONE_KEY_OK:
        ConfirmOperation(iKey, iDataID, strValue, pData);
        break;
    case PHONE_KEY_CANCEL:
        CancelOperation(iKey);
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

bool CWifiUICtrler::OnDigitKeyEvent(int iKey, int iDataID,
                                    yl::string & strValue, void * pData /*= NULL*/)
{
    bool bHandle = false;
    if (_ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_MENU)
    {
        if (iDataID == WIFI_ITEM_DATA_ID_MENU_SWITCH)
        {
            SwitchOperation(strValue == "1");
        }
        else
        {
            EnterPage(iDataID, iDataID);
        }
    }

    return true;
}

bool CWifiUICtrler::OnOptionEvent(int iKey, int iDataID,
                                  const yl::string & strValue, void * pData /*= NULL*/)
{
    WIRE_INFO("Option event. iKey[%d], data id[%d], value[%s]", iKey, iDataID, strValue.c_str());

    bool bHandle = true;
    switch (iKey)
    {
    case OPTION_EDIT:
        EnterPage(WIRELESS_UI_TYPE_WIFI_NET_EDIT, iDataID);
        break;
    case OPTION_MOVE_UP:
    case OPTION_MOVE_DOWN:
        if (MovePosOperation(OPTION_MOVE_UP == iKey, iDataID))
        {
            _ViewModeUIMgr.UpdateListInfo();
        }
        break;
    case OPTION_DELETE:
    case OPTION_DELETE_ALL:
        OPTION_DELETE == iKey ? WIFI_DeleteNetwork(iDataID) : WIFI_DeleteAllNetwork();
        _ViewModeUIMgr.UpdateListInfo();
        break;
    case OPTION_WPS:
    case OPTION_WPS_PIN:
        WpsOperation(OPTION_WPS == iKey);
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

bool CWifiUICtrler::OnMsgBoxEvent(int iID, int iKey)
{
    bool bHandle = true;
    MsgBoxResult eResult = (MsgBoxResult)iKey;

    WIRE_INFO("Msgbox event. ID[%d], result[%d]", iID, eResult);

    // 处理扫描提示框
    if (MSGBOX_ID_SCAN == iID)
    {
        // 用户停止扫描
        if (MSGBOX_RESULT_CANCEL == eResult
                || MSGBOX_RESULT_TIMER_EXIT == eResult)
        {
            m_bUserCancelScan = true;
            ScanOperation(false);
            _ViewModeUIMgr.UpdateListInfo();
        }
    }
    else if (MSGBOX_ID_PBC_OPEN_SWITCH == iID)
    {
        if (MSGBOX_RESULT_OK == eResult && WIFI_SetEnable(true))
        {
            WpsOperation(true);
        }
    }
    else if (MSGBOX_ID_WPS == iID)
    {
        // wps弹框退出则需要停止wps
        if (MSGBOX_RESULT_CANCEL == eResult
                || MSGBOX_RESULT_TIMER_EXIT == eResult
                || MSGBOX_RESULT_EXTERNAL_EXIT == eResult)
        {
            WIRE_INFO("Exit wps state");
            WIFI_CancelWps();
        }
    }
    else if (MSGBOX_ID_WPS_PIN == iID)
    {
        if (MSGBOX_RESULT_OK == eResult)
        {
            WpsOperation(false, true);
        }
        else
        {
            WIRE_INFO("Exit wps-pin state");
        }
    }
    else
    {
        bHandle = false;
    }

    return bHandle;
}

yl::string CWifiUICtrler::GetTitle(int iUIType)
{
    return m_WifiUIData.GetTitle(iUIType);
}

//
bool CWifiUICtrler::GetDataList(int iUIType, int iPageIndex,
                                VecViewInfo & vecInfo,
                                int iStartPos /*= 0*/, int iNumber /*= -1*/)
{
    return m_WifiUIData.GetDataList(vecInfo, iUIType, iPageIndex,
                                    m_iLastFocusDataID, iStartPos, iNumber);
}

// 获取softkey列表
bool CWifiUICtrler::GetSoftkeyList(int iUIType, int iDataID, YLVector<int> & vecSoftkey)
{
    return m_WifiUIData.GetSoftkeyList(iUIType, iDataID, m_iLastFocusDataID, vecSoftkey);
}

//
bool CWifiUICtrler::GetOptionSoftkeyList(int iUIType, int iFocusIndex,
        YLVector<OPTION_TYPE> & vecOptionSoftkey)
{
    return m_WifiUIData.GetOptionSoftkeyList(iUIType, iFocusIndex, vecOptionSoftkey);
}

int CWifiUICtrler::GetListSize(int iUIType, bool bOnlyData /*= false*/)
{
    return m_WifiUIData.GetListSize(iUIType);
}

int CWifiUICtrler::GetFocusInDataList(int iDataID)
{
    return m_WifiUIData.GetFocusInDataList(iDataID);
}

bool CWifiUICtrler::PageExit(int iUIType)
{
    return true;
}

bool CWifiUICtrler::CorrectViewInfo(int iUIType,
                                    const VecItemDataID2Info & vecItemDataInfo,
                                    VecViewInfo & vecInfo)
{
    return m_WifiUIData.CorrectViewInfo(iUIType, vecItemDataInfo, vecInfo);
}

// 开关结果处理
bool CWifiUICtrler::ProcessSwitchResult(UINT wParam, UINT lParam)
{
    yl::string strMsg("");
    MsgBoxID eID = MSGBOX_ID_BT_NONE;
    int iTime = MSGBOX_TIMEOUT_SHORT;

    if (OP_WIFI_OPEN_RESULT == wParam)
    {
        if (WIRELESS_OP_OPEN_RESULT_NODONGLE == lParam
                || WIRELESS_OP_OPEN_RESULT_FAILED == lParam)
        {
            strMsg = WIRELESS_OP_OPEN_RESULT_NODONGLE == lParam ?
                     TRID_NO_FIND_WIFI_ADAPTER : TRID_OPEN_FAIL;
            m_WifiUIData.SetUISwitchEnable(false);
            // 开启失败, UI退出扫描状态
            m_WifiUIData.SetUIScanState(false);
            _ViewModeUIMgr.RemoveMsgBox(MSGBOX_ID_SCAN);
        }
        else if (WIRELESS_OP_OPEN_RESULT_SUCCESS == lParam)
        {
            m_WifiUIData.SetUISwitchEnable(true);
            // 开启成功, 执行扫描, 需判断用户是否点击scaning弹框的cancel
            if (!m_bUserCancelScan)
            {
                WIFI_StartScan();
            }
            m_bUserCancelScan = false;
        }
        else if (WIRELESS_OP_OPEN_RESULT_ING == lParam)
        {
            // 开启中则设置为开启成功并提示扫描中, 先刷新出开启的界面
            m_WifiUIData.SetUISwitchEnable(true);
            m_WifiUIData.SetUIScanState(true);
            _ViewModeUIMgr.RemoveMsgBox(MSGBOX_ID_SCAN);
            strMsg = TRID_SCANING;
            eID = MSGBOX_ID_SCAN;
            iTime = MSGBOX_TIMEOUT_ALWAYS;
        }
    }
    else
    {
        m_WifiUIData.SetUISwitchEnable(false);
        m_bUserCancelScan = false;
    }

    if (!strMsg.empty())
    {
        _ViewModeUIMgr.PopupMsgBox(eID, UIConmonAPI_Lang(TRID_NOTE),
                                   UIConmonAPI_Lang(strMsg), MSGBOX_STYLE_CANCEL, iTime);
    }

    return true;
}

// 鉴权消息处理
bool CWifiUICtrler::ProcessEnterPsk(int iNetID, bool bEapNetwork)
{
    if (WIFI_NET_ID_INVAILED == iNetID
            || iNetID != m_iCurrentOperatingNet)
    {
        return false;
    }

    if (bEapNetwork)
    {
        return EnterPage(WIRELESS_UI_TYPE_WIFI_AUTH_NETWORK, iNetID);
    }
    else
    {
        return EnterPage(WIRELESS_UI_TYPE_WIFI_PIN_INPUT, iNetID);
    }

    return false;
}

// wps连接结果处理
bool CWifiUICtrler::ProcessWpsConnectResult(UINT wParam, UINT lParam)
{
    // 移除wps提示框
    _ViewModeUIMgr.RemoveMsgBox(MSGBOX_ID_WPS);

    if (OP_WPSCONNECT_RESULT == wParam
            || OP_WPSPINCONNECT_RESULT == wParam)
    {
        yl::string strMsg("");
        if (WIRELESS_OP_WPSCONNECT_SUCCESS == lParam)
        {
            strMsg = TRID_CONNECT_SUCCESS;
        }
        else
        {
            strMsg = TRID_CONNECT_FAILED;
        }

        _ViewModeUIMgr.PopupMsgBox(MSGBOX_ID_BT_NONE, UIConmonAPI_Lang(TRID_NONE),
                                   UIConmonAPI_Lang(strMsg), MSGBOX_STYLE_CANCEL, MSGBOX_TIMEOUT_LONG);
    }

    return true;
}

// 是否需要更新UI
bool CWifiUICtrler::IsNeedUpdateUI(UINT wParam)
{
    bool bNeed = true;

    switch (wParam)
    {
    case OP_CONNECT_WIFI_RESULT:
    case OP_DISCONNECT_WIFI_RESULT:
        bNeed = _ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_SCAN
                || _ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_PROFILE
                || _ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_NET_DETAIL
                || _ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_MENU;
        break;
    default:
        break;
    }

    return bNeed;
}

// 开关操作
bool CWifiUICtrler::SwitchOperation(bool bOpen)
{
    WIRE_INFO("Switch operation. open?[%d]", bOpen);

    yl::string strMsg("");
    MsgBoxID eID = MSGBOX_ID_BT_NONE;
    int iTimeOut = MSGBOX_TIMEOUT_SHORT;

    WIFI_SetEnable(bOpen);
    if (bOpen && !WIFI_IsEnable())
    {
        bool bDongleVaild = WIFI_IsDongleVaild();
        strMsg = bDongleVaild ? TRID_SCANING : TRID_NO_FIND_WIFI_ADAPTER;
        eID = bDongleVaild ? eID = MSGBOX_ID_SCAN : eID;
        iTimeOut = bDongleVaild ? MSGBOX_TIMEOUT_ALWAYS : iTimeOut;

        m_WifiUIData.SetUIScanState(bDongleVaild);
    }
    else if (!bOpen)
    {
        strMsg = TRID_SAVED;
        m_WifiUIData.SetUISwitchEnable(false);
        m_WifiUIData.SetUIScanState(false);
        m_iCurrentOperatingNet = WIFI_NET_ID_INVAILED;
        m_bUserCancelScan = false;
    }

    if (!strMsg.empty())
    {
        _ViewModeUIMgr.PopupMsgBox(eID, UIConmonAPI_Lang(TRID_NOTE),
                                   UIConmonAPI_Lang(strMsg), MSGBOX_STYLE_CANCEL, iTimeOut);
    }

    return true;
}

// 扫描操作
bool CWifiUICtrler::ScanOperation(bool bStartScan)
{
    WIRE_INFO("Scan operation. start?[%d]", bStartScan);

    bool bResult = false;
    yl::string strMsg("");
    MsgBoxID eID = MSGBOX_ID_BT_NONE;

    m_WifiUIData.SetUIScanState(bStartScan);

    if (bStartScan)
    {
        if (WIFI_IsDongleVaild() && !WIFI_IsScan())
        {
            bResult = WIFI_StartScan();
            strMsg = bResult ? TRID_SCANING : TRID_SCAN_FAILED;
            eID = MSGBOX_ID_SCAN;
        }
    }
    else if (WIFI_IsScan())
    {
        bResult = WIFI_StopScan();
    }

    if (!strMsg.empty())
    {
        _ViewModeUIMgr.PopupMsgBox(MSGBOX_ID_SCAN, UIConmonAPI_Lang(TRID_NOTE),
                                   UIConmonAPI_Lang(strMsg), MSGBOX_STYLE_CANCEL, MSGBOX_TIMEOUT_ALWAYS);
    }

    return bResult;
}

// 连接操作
bool CWifiUICtrler::ConnectOperation(bool bConnect, int iNetID)
{
    WIRE_INFO("[%s] devce. net id[%d]", bConnect ? "Connect" : "Disonnect", iNetID);

    WiFiNetInfo netInfo;
    if (WIFI_NET_ID_INVAILED == iNetID
            || !WIFI_GetNetworkInfoByID(iNetID, netInfo))
    {
        return false;
    }

    yl::string strMsg("");
    bool bResult = false;

    if (bConnect)
    {
        m_iCurrentOperatingNet = iNetID;
        bResult = WIFI_StartConnect(iNetID);
    }
    else
    {
        bResult = WIFI_StartDisconnect(iNetID);
    }

    return bResult;
}

// wps操作
bool CWifiUICtrler::WpsOperation(bool bStartWps, bool bStartWpsPin /*= false*/)
{
    yl::string strMsg("");
    MsgBoxID eID;
    int iTimeOut = MSGBOX_TIMEOUT_ALWAYS;
    MsgBoxStyle eStyle;

    bool bWpsStart = false;
    if (bStartWps)
    {
        // 开启wps
        WIRE_INFO("Start wps...");
        WIFI_StartWps();
        bWpsStart = true;
    }
    else
    {
        if (bStartWpsPin)
        {
            // 开启wps-pin
            WIRE_INFO("Start wps pin...");
            WIFI_StartWpspin();
            bWpsStart = true;
        }
        else
        {
            // 弹出输入pin提示框, 确认之后则才开启wps-pin
            yl::string strWpsPin("");
            WIFI_GetWpsPin(strWpsPin);
            strMsg = commonAPI_FormatString("%s%s",
                                            UIConmonAPI_Lang(
                                                TRID_PLEASE_INPUT_BELOW_PIN_CODE_ON_YOU_WIFI_ROUTER_AND_PRESS_OK_TO_START_CONNECTING).c_str(),
                                            strWpsPin.c_str());
            eID = MSGBOX_ID_WPS_PIN;
            eStyle = MSGBOX_STYLE_OK_CANCEL;
        }
    }

    if (bWpsStart)
    {
        strMsg = UIConmonAPI_Lang(TRID_WPS_HAS_OPENED_PLEASE_CONNECT_IN);
        eID = MSGBOX_ID_WPS;
        iTimeOut = MSGBOX_TIMEOUT_WPS;
        eStyle = MSGBOX_STYLE_CANCEL;
    }

    if (!strMsg.empty())
    {
        _ViewModeUIMgr.PopupMsgBox(eID, UIConmonAPI_Lang(TRID_NOTE),
                                   strMsg, eStyle, iTimeOut, bWpsStart, !bWpsStart);
    }

    return true;
}

// 确认操作
bool CWifiUICtrler::ConfirmOperation(int iKey, int iDataID,
                                     const yl::string & strValue,
                                     void * pData /*= NULL*/)
{
    if (ST_OK != iKey && ST_SAVE != iKey && PHONE_KEY_OK != iKey)
    {
        return false;
    }

    WIRE_INFO("ConfirmOperation. UI[%d], data id [%d], value [%s]",
              _ViewModeUIMgr.GetTopUIType(), iDataID, strValue.c_str());

    // 处理menu业务
    if (_ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_MENU)
    {
        if (WIFI_ITEM_DATA_ID_MENU_SWITCH == iDataID)
        {
            SwitchOperation(strValue == "1");
        }
        else
        {
            EnterPage(iDataID, iDataID);
        }
    }
    else if (_ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_PROFILE
             || _ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_SCAN)
    {
        WiFiNetInfo netInfo;
        if (WIFI_GetNetworkInfoByID(iDataID, netInfo))
        {
            ConnectOperation(WIRE_DEVICE_INIT_STATE == netInfo.eNetState, netInfo.iNetID);
        }
    }
    else if (_ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_NET_ADD
             || _ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_NET_DETAIL_2_ADD
             || _ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_NET_EDIT)
    {
        bool bAdd = _ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_NET_ADD
                    || _ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_NET_DETAIL_2_ADD;
        if (pData)
        {
            EditNetInfoOperation(bAdd, *(VecItemDataID2Info *)pData);
        }
    }
    else if (_ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_PIN_INPUT
             || _ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_AUTH_NETWORK)
    {
        {
            AuthNetWork(m_iCurrentOperatingNet,
                        _ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_WIFI_PIN_INPUT, strValue);
        }
    }

    return true;
}

// 返回操作
bool CWifiUICtrler::CancelOperation(int iKey, bool bExitPage /*= true*/)
{
    if (ST_BACK != iKey && ST_CANCEL != iKey
            && PHONE_KEY_CANCEL != iKey)
    {
        return false;
    }

    WIRE_INFO("Cancel operation. Exit page[%d]", bExitPage);

    // psk输入界面退出或者scan/profile界面的cancel按键, 则取消连接
    if ((WIFI_NET_ID_INVAILED != m_iCurrentOperatingNet
            && (WIRELESS_UI_TYPE_WIFI_PIN_INPUT == _ViewModeUIMgr.GetTopUIType()
                || WIRELESS_UI_TYPE_WIFI_AUTH_NETWORK == _ViewModeUIMgr.GetTopUIType()))
            || (iKey == ST_CANCEL && WIFI_NET_ID_INVAILED != m_iCurrentOperatingNet
                && (WIRELESS_UI_TYPE_WIFI_SCAN == _ViewModeUIMgr.GetTopUIType()
                    || WIRELESS_UI_TYPE_WIFI_PROFILE == _ViewModeUIMgr.GetTopUIType())))
    {
        WIFI_CancelConnect(m_iCurrentOperatingNet);
        m_iCurrentOperatingNet = WIFI_NET_ID_INVAILED;
    }

    if (bExitPage)
    {
        ExitPage();
    }

    return true;
}

// 设备位置调整操作
bool CWifiUICtrler::MovePosOperation(bool bUp, int iNetID)
{
    WIRE_INFO("Move operation. Up[%d], net[%d]", bUp, iNetID);

    bool bResult = false;
    if (WIFI_AdjustPosition(iNetID, bUp))
    {
        // update ui
        bResult = true;
    }
    else
    {
        _ViewModeUIMgr.PopupMsgBox(MSGBOX_ID_BT_NONE, UIConmonAPI_Lang(TRID_NOTE),
                                   UIConmonAPI_Lang(TRID_WIFI_OPERATOR_FAIL), MSGBOX_STYLE_CANCEL, MSGBOX_TIMEOUT_SHORT);
    }

    return bResult;
}

// 编辑网络信息
bool CWifiUICtrler::EditNetInfoOperation(bool bAdd, VecItemDataID2Info & vecItemData)
{
    int iErrorDataID = DATA_ID_INVALID;
    int iResule = m_WifiUIData.EditNetInfo(iErrorDataID, bAdd, vecItemData, m_iLastFocusDataID);

    yl::string strMsg("");

    switch (iResule)
    {
    case -1:
        strMsg = TRID_SAVE_FAILED_THE_SAME_PROFILE_NAME_OR_SSID_EXIST;
        break;
    case 0:
        strMsg = TRID_SAVING_CONFIG_FILE;
        break;
    case 1:
        strMsg = TRID_LIST_IS_FULL_PLEASE_DELETE_BEFORE_ADD;
        break;
    case 2:
        strMsg = TRID_SSID_CANNOT_BE_EMPTY;
        break;
    case 3:
        strMsg = TRID_WPA_SHARED_KEY_CANNOT_BE_EMPTY;
        break;
    case 4:
        strMsg = TRID_USER_NAME_COULD_NOT_BE_EMPTY;
        break;
    default:
        break;
    }

    if (!strMsg.empty())
    {
        _ViewModeUIMgr.PopupMsgBox(MSGBOX_ID_NONE, UIConmonAPI_Lang(TRID_NOTE),
                                   UIConmonAPI_Lang(strMsg), MSGBOX_STYLE_CANCEL, MSGBOX_TIMEOUT_SHORT);
    }

    if (iErrorDataID != DATA_ID_INVALID)
    {
        _ViewModeUIMgr.SetDataListFocus(iErrorDataID);
    }

    // 保存成功, 退出页面
    if (0 == iResule)
    {
        ExitPage();
    }

    return true;
}

bool CWifiUICtrler::AuthNetWork(int iNetID, bool bEAPNetwork, const yl::string & strValue)
{
    if (WIFI_NET_ID_INVAILED == m_iCurrentOperatingNet)
    {
        ExitPage();
        return false;
    }

    bool bResult = false, bExitPage = true;;
    yl::string strMsg = "";

    if (bEAPNetwork)
    {
        bResult = WIFI_AnswerPassword(iNetID, strValue.c_str());
    }
    else
    {
        bResult = true;
        yl::string strEAPName = "", strEAPPassword = "";
        VecItemDataID2Info vecItemDataID2Info;
        _ViewModeUIMgr.GetItemDataID2Info(vecItemDataID2Info);
        for (int iIndex = 0; iIndex < vecItemDataID2Info.size(); ++ iIndex)
        {
            ItemDataID2Info & itemDataInfo = vecItemDataID2Info[iIndex];
            if (WIFI_ITEM_DATA_ID_EDIT_EAP_NAME == itemDataInfo.iDataID)
            {
                bResult = itemDataInfo.strValue.empty() ? false : true;
                strEAPName = bResult ? itemDataInfo.strValue : "";
                strMsg = bResult ? "" : TRID_USER_NAME_COULD_NOT_BE_EMPTY;
                bExitPage = bResult ? true : false;
            }
            else if (WIFI_ITEM_DATA_ID_EDIT_PSK == itemDataInfo.iDataID)
            {
                strEAPPassword = itemDataInfo.strValue;
            }
        }

        if (bResult)
        {
            bResult = WIFI_AuthNetwork(iNetID, strEAPName, strEAPPassword);
        }
    }

    if (!strMsg.empty())
    {
        _ViewModeUIMgr.PopupMsgBox(MSGBOX_ID_BT_NONE, UIConmonAPI_Lang(TRID_NOTE),
                                   UIConmonAPI_Lang(strMsg), MSGBOX_STYLE_CANCEL, MSGBOX_TIMEOUT_SHORT);
    }

    if (bExitPage)
    {
        ExitPage();
    }

    return bResult;
}

#endif // IF_SUPPORT_WIFI
