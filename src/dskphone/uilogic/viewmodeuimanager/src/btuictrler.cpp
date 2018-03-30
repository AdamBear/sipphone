#ifdef IF_SUPPORT_BLUETOOTH
#include "btuictrler.h"
#include "dskphone/ui/xwin/settingui/src/viewmodeuimanager.h"
#include "wireless/bluetoothNew/include/modbluetooth.h"
#include "translateiddefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "viewmodeuimanager/include/uicommon.h"

#define _CASE_MSG_TO_STRING(msg, strMsg)    case msg: strMsg = #msg; break;

#if IS_COLOR
#define FORMATE_COLOR_ICON(icon)     (icon + ".png")
#else
// T42 因屏幕较小,图标没办法放下,因此采用小一号图标
#define FORMATE_FULL_ICON_SMALL(icon)       (icon + "_small.dob")
#define FORMATE_FULL_ICON(icon)             (icon + ".dob")
#endif // IS_COLOR

#if IS_COLOR
#define BT_PHONE_AC_ICON         "Theme#icon@bluetooth_mobile_auto" //彩屏t29手机类型蓝牙连接图标
#define BT_PHONE_ICON            "Theme#icon@bluetooth_mobile_icon"
#define BT_HEADSET_ICON          "Theme#icon@bluetooth_headset"//彩屏t29耳机类型蓝牙连接图标
#define BT_HEADSET_AC_ICON       "Theme#icon@bluetooth_headset_auto"
#define BT_BLUETOOTH_ICON        "Theme#icon@bluetooth_common"
#define BT_BLUETOOTH_AC_ICON     "Theme#icon@bluetooth_connect"
#define BT_COMPUTER_ICON         "Theme#icon@bluetooth_pc"
#else
// 蓝牙图标----不包含前缀和后缀
#define BT_BLUETOOTH_ICON                   "bluetooth"                         // 未定义类型蓝牙列表图标
#define BT_BLUETOOTH_AC_ICON    BT_BLUETOOTH_ICON
#define BT_HEADSET_ICON                     "bluetooth_headset"                 // 耳机类型蓝牙列表图标
#define BT_HEADSET_AC_ICON                  "bluetooth_headset_auto"            // 耳机自动连接列表图标
#define BT_PHONE_ICON                       "bluetooth_mobile_icon"             // 手机类型蓝牙列表图标
#define BT_PHONE_AC_ICON                    "bluetooth_mobile_auto"             // 手机自动连接列表图标
#define BT_COMPUTER_ICON                    "bluetooth_pc"
#endif

// 彩屏和T27G的扫描icon
#define BT_SCAN_ANIMATION_ICON              "bluetooth_rotatecalling"
// T42S/T41S的扫描icon
#define BT_SCAN_ANIMATION_ICON_SMALL        "bluetooth_rotatecalling_small"

// 页面类型的最后一个值开始, 作为没有dev handle的ITEM标识
enum ItemDataID
{
    ITEM_DATA_ID_BEGIN = VIEW_MODE_UI_TYPE_END + 1,
    BT_ITEM_DATA_ID_MENU_SWITCH,
    BT_ITEM_DATA_ID_DETAIL_SYNC,
    BT_ITEM_DATA_ID_DONGLE_INFO_NAME,
    ITEM_DATA_ID_END
};

// MsgBox 标识
enum MsgBoxID
{
    MSGBOX_ID_BT_NONE = -1,
    MSGBOX_ID_CONNECT,
    MSGBOX_ID_DELETE_DEV,
    MSGBOX_ID_DELETE_ALL_DEV,
    MSGBOX_ID_MOBILE_PIN,
    MSGBOX_ID_MOBILE_DIR_SYNC,
};

#define MSGBOX_TIMEOUT_ALWAYS       0
#define MSGBOX_TIMEOUT_SHORT        1500
#define MSGBOX_TIMEOUT_LONG         2000

void PrintMsg(msgObject & objMsg)
{
    yl::string strMsg("");

    switch (objMsg.message)
    {
        _CASE_MSG_TO_STRING(BLUETOOTH_MSG_DEVICE_POWER, strMsg);
        _CASE_MSG_TO_STRING(BLUETOOTH_MSG_STATE_CHANGE, strMsg);
        _CASE_MSG_TO_STRING(BLUETOOTH_MSG_UPDATE_LIST, strMsg);
        _CASE_MSG_TO_STRING(BLUETOOTH_MSG_REQ_PIN, strMsg);
        _CASE_MSG_TO_STRING(BLUETOOTH_MSG_SHOW_PIN, strMsg);
        _CASE_MSG_TO_STRING(BLUETOOTH_MSG_REQ_SYNC, strMsg);
    }

    if (!strMsg.empty())
    {
        WIRE_INFO("Receive logic msg[%s], wParam[%d], lParam[%d]",
                  strMsg.c_str(), objMsg.wParam, objMsg.lParam);
    }
}

CBtUICtrler::CBtUICtrler()
    : m_iCurrentOperatingDev(BT_DEVICE_HANDLE_INVAILED)
{
    Init();
}

CBtUICtrler::~CBtUICtrler()
{
    UnInit();
}

bool CBtUICtrler::Init()
{
    etl_RegisterMsgHandle(BLUETOOTH_MSG_DEVICE_POWER, BLUETOOTH_MSG_DEVICE_POWER, CMSGSubscriber(this,
                          &CBtUICtrler::OnBluetoothtMsg));
    etl_RegisterMsgHandle(BLUETOOTH_MSG_STATE_CHANGE, BLUETOOTH_MSG_STATE_CHANGE, CMSGSubscriber(this,
                          &CBtUICtrler::OnBluetoothtMsg));
    etl_RegisterMsgHandle(BLUETOOTH_MSG_UPDATE_LIST, BLUETOOTH_MSG_UPDATE_LIST, CMSGSubscriber(this,
                          &CBtUICtrler::OnBluetoothtMsg));
    etl_RegisterMsgHandle(BLUETOOTH_MSG_REQ_PIN, BLUETOOTH_MSG_REQ_PIN, CMSGSubscriber(this,
                          &CBtUICtrler::OnBluetoothtMsg));
#ifdef IF_BT_SUPPORT_PHONE
    etl_RegisterMsgHandle(BLUETOOTH_MSG_SHOW_PIN, BLUETOOTH_MSG_SHOW_PIN, CMSGSubscriber(this,
                          &CBtUICtrler::OnBluetoothtMsg));
    etl_RegisterMsgHandle(BLUETOOTH_MSG_REQ_SYNC, BLUETOOTH_MSG_REQ_SYNC, CMSGSubscriber(this,
                          &CBtUICtrler::OnBluetoothtMsg));
#endif
    return true;
}

bool CBtUICtrler::UnInit()
{
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_DEVICE_POWER, BLUETOOTH_MSG_DEVICE_POWER, CMSGSubscriber(this,
                            &CBtUICtrler::OnBluetoothtMsg));
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_STATE_CHANGE, BLUETOOTH_MSG_STATE_CHANGE, CMSGSubscriber(this,
                            &CBtUICtrler::OnBluetoothtMsg));
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_UPDATE_LIST, BLUETOOTH_MSG_UPDATE_LIST, CMSGSubscriber(this,
                            &CBtUICtrler::OnBluetoothtMsg));
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_REQ_PIN, BLUETOOTH_MSG_REQ_PIN, CMSGSubscriber(this,
                            &CBtUICtrler::OnBluetoothtMsg));
#ifdef IF_BT_SUPPORT_PHONE
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_SHOW_PIN, BLUETOOTH_MSG_SHOW_PIN, CMSGSubscriber(this,
                            &CBtUICtrler::OnBluetoothtMsg));
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_REQ_SYNC, BLUETOOTH_MSG_REQ_SYNC, CMSGSubscriber(this,
                            &CBtUICtrler::OnBluetoothtMsg));
#endif
    return true;
}

bool CBtUICtrler::OnBluetoothtMsg(msgObject & objMsg)
{
    bool bHandle = true;

    PrintMsg(objMsg);

    yl::string strMsg("");
    int iTimeOut = MSGBOX_TIMEOUT_SHORT;
    MsgBoxStyle eStyle = MSGBOX_STYLE_CANCEL;
    MsgBoxID eID = MSGBOX_ID_BT_NONE;

    switch (objMsg.message)
    {
    // 初始化结果
    case BLUETOOTH_MSG_DEVICE_POWER:
        ProcessPowerResult(true);
        break;
    // 开关结果
    case BLUETOOTH_MSG_STATE_CHANGE:
        ProcessSwitchResult(objMsg.wParam);
        break;
    // 需要更新链表操作
    case BLUETOOTH_MSG_UPDATE_LIST:
        ProcessUpdateList(objMsg.wParam, objMsg.lParam, objMsg.GetExtraData());
        break;
    // PIN码处理
    case BLUETOOTH_MSG_REQ_PIN:
#ifdef IF_BT_SUPPORT_PHONE
    case BLUETOOTH_MSG_SHOW_PIN:
#endif
        ProcessRequestPin(objMsg.message, objMsg.wParam, objMsg.lParam, objMsg.GetExtraData());
        break;
    case BLUETOOTH_MSG_REQ_SYNC:
        {
            strMsg = TRID_WOULD_YOU_LIKE_TO_SYNC_PHONE_CONTACTS_TEMPORARILY;
            iTimeOut = MSGBOX_TIMEOUT_ALWAYS;
            eStyle = MSGBOX_STYLE_OK_CANCEL;
            eID = MSGBOX_ID_MOBILE_DIR_SYNC;
        }
        break;
    default:
        bHandle = false;
        break;
    }

    if (!strMsg.empty())
    {
        _ViewModeUIMgr.PopupMsgBox(eID, UIConmonAPI_Lang(TRID_NONE),
                                   UIConmonAPI_Lang(strMsg), eStyle, iTimeOut);
    }

    return bHandle;
}

// softkey处理
bool CBtUICtrler::OnSoftKeyEvent(int iKey, int iDataID,
                                 const yl::string & strValue,
                                 void * pData/* = NULL*/)
{
    bool bHandle = true;

    switch (iKey)
    {
    case ST_OK:
    case ST_SAVE:
        ConfirmOperation(iKey, iDataID, strValue, pData);
        break;
    case ST_SCAN:
    case ST_REFRESH:
        // 顺序不可调换, ctrler需先进入一些状态后UI才能够获取到正确的数据
        bHandle = ScanOperation(true);
        if (bHandle)
        {
            if (ST_SCAN == iKey)
            {
                EnterPage(WIRELESS_UI_TYPE_BT_SCAN, iDataID);
            }
            else
            {
                _ViewModeUIMgr.UpdateUI(true);
            }
            _ViewModeUIMgr.UpdateAnimation(true);
        }
        bHandle = true;
        break;
    case ST_ENTER:
        EnterPage(iDataID, iDataID);
        break;
    case ST_CONNECT:
    case ST_DISCONNECT:
        ConnectOperation(ST_CONNECT == iKey, iDataID);
        break;
    case ST_BACK:
        CancelOperation(iKey);
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

// funckey处理
bool CBtUICtrler::OnFunKeyEvent(int iKey, int iDataID, yl::string & strValue,
                                void * pData/* = NULL*/)
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

// digitkey处理
bool CBtUICtrler::OnDigitKeyEvent(int iKey, int iDataID, yl::string & strValue,
                                  void * pData/* = NULL*/)
{
    bool bHandle = false;
    if (_ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_BT_MENU)
    {
        if (iDataID == BT_ITEM_DATA_ID_MENU_SWITCH)
        {
            ConfirmOperation(ST_SAVE, iDataID, strValue, pData);
        }
        else
        {
            EnterPage(iDataID, iDataID);
        }
    }

    return true;
}

bool CBtUICtrler::OnOptionEvent(int iKey, int iDataID,
                                const yl::string & strValue, void * pData/* = NULL*/)
{
    bool bHandle = true;
    DeviceInfo devInfo;

    switch (iKey)
    {
    case OPTION_DETAIL:
        EnterPage(WIRELESS_UI_TYPE_BT_DEV_DETAIL, iDataID);
        break;
    case OPTION_DELETE:
        _ViewModeUIMgr.PopupMsgBox(MSGBOX_ID_DELETE_DEV, UIConmonAPI_Lang(TRID_NONE),
                                   UIConmonAPI_Lang(TRID_IF_DELETE_SELECTED_BLUETOOTH_DEVICE), MSGBOX_STYLE_OK_CANCEL, 0);
        break;
    case OPTION_DELETE_ALL:
        _ViewModeUIMgr.PopupMsgBox(MSGBOX_ID_DELETE_ALL_DEV, UIConmonAPI_Lang(TRID_NONE),
                                   UIConmonAPI_Lang(TRID_IF_DELETE_ALL_BLUETOOTH_ITEM), MSGBOX_STYLE_OK_CANCEL, 0);
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

// 弹框结果处理
bool CBtUICtrler::OnMsgBoxEvent(int iID, int iKey)
{
    bool bHandle = false;
    DeviceInfo devInfo;
    MsgBoxResult eResult = (MsgBoxResult)iKey;

    // 连接提示框响应
    if (MSGBOX_ID_CONNECT == iID)
    {
        if (MSGBOX_RESULT_CANCEL == eResult)
        {
            if (Bluetooth_GetBluetoothInfoByHandle(m_iCurrentOperatingDev, devInfo))
            {
                if (devInfo.eState == WIRE_DEVICE_PAIRING_STATE)
                {
                    Bluetooth_CancelPair(devInfo.strMac.c_str());
                }
                else
                {
                    ConnectOperation(false, m_iCurrentOperatingDev);
                }
            }
            m_iCurrentOperatingDev = BT_DEVICE_HANDLE_INVAILED;
        }

        bHandle = true;
    }
    // 删除一个设备提示弹框响应
    else if (MSGBOX_ID_DELETE_DEV == iID
             && MSGBOX_RESULT_OK == eResult)
    {
        if (Bluetooth_GetBluetoothInfoByHandle(_ViewModeUIMgr.GetFocusDataID(), devInfo))
        {
            Bluetooth_DeleteDevice(devInfo.strMac.c_str());
        }

        bHandle = true;
    }
    // 删除所有设备提示框响应
    else if (MSGBOX_ID_DELETE_ALL_DEV == iID
             && MSGBOX_RESULT_OK == eResult)
    {
        Bluetooth_DeleteDevice();

        bHandle = true;
    }
#ifdef IF_BT_SUPPORT_PHONE
    // 手机pin码确认提示框响应
    else if (MSGBOX_ID_MOBILE_PIN == iID)
    {
        Bluetooth_GetBluetoothInfoByHandle(m_iCurrentOperatingDev, devInfo);
        if (MSGBOX_RESULT_OK == eResult)
        {
            Bluetooth_AnswerMobilePin(devInfo.strMac.c_str());
        }
        else
        {
            Bluetooth_CancelPair(devInfo.strMac.c_str());
        }

        bHandle = true;
    }
#ifdef IF_SUPPORT_BLUETOOTH_CONTACT
    else if (MSGBOX_ID_MOBILE_DIR_SYNC == iID)
    {
        bool bSync = MSGBOX_RESULT_OK == eResult;

        if (MSGBOX_RESULT_EXTERNAL_EXIT == eResult)
        {
            // 设置idle状态
        }

        Bluetooth_GetBluetoothInfoByHandle(m_iCurrentOperatingDev, devInfo);
        Bluetooth_SetSyncContact(devInfo.strMac.c_str(), bSync);
    }
#endif // IF_SUPPORT_BLUETOOTH_CONTACT
#endif // IF_BT_SUPPORT_PHONE

    return bHandle;
}

// 获取title
yl::string CBtUICtrler::GetTitle(int iUIType)
{
    yl::string strTitle = "";
    DeviceInfo devInfo;

    switch (iUIType)
    {
    case WIRELESS_UI_TYPE_BT_MENU:
        strTitle = UIConmonAPI_Lang(TRID_BLUE_TOOTH);
        break;
    case WIRELESS_UI_TYPE_BT_SCAN:
        strTitle = UIConmonAPI_Lang(TRID_SCAN_BLUETOOTH_DEVICE);
        break;
    case WIRELESS_UI_TYPE_BT_PAIRED:
        strTitle = UIConmonAPI_Lang(TRID_PAIRED_BLUETOOTH_DEVICE);
        break;
    case WIRELESS_UI_TYPE_BT_DEV_DETAIL:
        if (Bluetooth_GetBluetoothInfoByHandle(m_iLastFocusDataID, devInfo))
        {
            strTitle = devInfo.strName;
        }
        break;
    case WIRELESS_UI_TYPE_BT_DONGLE_INFO:
        strTitle = UIConmonAPI_Lang(TRID_EDIT_MY_BLUETOOTH_DEVICE_INFORMATION);
        break;
    case WIRELESS_UI_TYPE_BT_PIN_INPUT:
        strTitle = UIConmonAPI_Lang(TRID_ENTER_PASSWORD);
        break;
    default:
        break;
    }

    return strTitle;
}

// 获取数据列表 ---- 以页为单位
bool CBtUICtrler::GetDataList(int iUIType, int iPageIndex, VecViewInfo & vecInfo,
                              int iStartPos/* = 0*/, int iNumber/* = -1*/)
{
    bool bHandle = true;
    ViewInfo viewInfo;
    DeviceInfo devInfo;

    switch (iUIType)
    {
    case WIRELESS_UI_TYPE_BT_MENU:
        {
            viewInfo.Reset();
            viewInfo.iDataID = BT_ITEM_DATA_ID_MENU_SWITCH;
            viewInfo.strDisplayName += UIConmonAPI_Lang(TRID_BLUE_TOOTH);
            viewInfo.strValue = Bluetooth_IsEnable() ? "1" : "0";
            viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_OFF));
            viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_ON));
            vecInfo.push_back(viewInfo);

            if (Bluetooth_IsEnable()
                    && Bluetooth_GetDeviceState() != DS_NO_DONGLE)
            {
                viewInfo.Reset();
                viewInfo.iDataID = WIRELESS_UI_TYPE_BT_PAIRED;
                viewInfo.strDisplayName = UIConmonAPI_Lang(TRID_PAIRED_BLUETOOTH_DEVICE);
                vecInfo.push_back(viewInfo);

                viewInfo.Reset();
                viewInfo.iDataID = WIRELESS_UI_TYPE_BT_DONGLE_INFO;
                viewInfo.strDisplayName += UIConmonAPI_Lang(TRID_EDIT_MY_BLUETOOTH_DEVICE_INFORMATION);
                vecInfo.push_back(viewInfo);
            }
        }
        break;
    case WIRELESS_UI_TYPE_BT_SCAN:
        {
            bHandle = GetDevList(vecInfo, DEVICE_LIST_SCAN, iStartPos, iNumber);

            // 扫描列表空则显示NONE
            if (!bHandle && Bluetooth_GetScanedDeviceListSize() <= 0)
            {
                viewInfo.Reset();
                viewInfo.iDataID = DATA_ID_NONE;
                viewInfo.strDisplayName = UIConmonAPI_Lang(TRID_NONE);
                vecInfo.push_back(viewInfo);
                bHandle = true;
            }
        }
        break;
    case WIRELESS_UI_TYPE_BT_PAIRED:
        {
            viewInfo.Reset();
            bHandle = GetDevList(vecInfo, DEVICE_LIST_PAIRED, iStartPos, iNumber);
            // 配对列表空则显示NONE
            if (!bHandle && vecInfo.size() <= 0)
            {
                viewInfo.iDataID = DATA_ID_NONE;
                viewInfo.strDisplayName = UIConmonAPI_Lang(TRID_NONE);
                vecInfo.push_back(viewInfo);
                bHandle = true;
            }
        }
        break;
    case WIRELESS_UI_TYPE_BT_DEV_DETAIL:
    case WIRELESS_UI_TYPE_BT_DONGLE_INFO:
        {
            viewInfo.Reset();
            if (WIRELESS_UI_TYPE_BT_DEV_DETAIL == iUIType)
            {
                bHandle = Bluetooth_GetBluetoothInfoByHandle(m_iLastFocusDataID, devInfo);
                viewInfo.iDataID = m_iLastFocusDataID;
            }
            else
            {
                bHandle = Bluetooth_GetDeviceInfo(devInfo);
                viewInfo.iDataID = BT_ITEM_DATA_ID_DONGLE_INFO_NAME;
            }
            viewInfo.strDisplayName = UIConmonAPI_Lang(TRID_DEVICE_NAME);
            viewInfo.strValue = bHandle ? devInfo.strName : "";
            vecInfo.push_back(viewInfo);

            viewInfo.Reset();
            viewInfo.iDataID = m_iLastFocusDataID;
            viewInfo.strDisplayName = UIConmonAPI_Lang(TRID_MAC_ADDRESS);
            viewInfo.strValue = bHandle ? devInfo.strMac : "";
            vecInfo.push_back(viewInfo);

            if (devInfo.eType == BT_PHONE)
            {
                viewInfo.Reset();
                viewInfo.iDataID = BT_ITEM_DATA_ID_DETAIL_SYNC;
                viewInfo.strDisplayName = UIConmonAPI_Lang(TRID_MOBILE_CONTACTS_SYNC);
                viewInfo.strValue = devInfo.bSyncContact ? "1" : "0";
                viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_OFF));
                viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_ON));
                viewInfo.strVecCustomValue.push_back("0");
                viewInfo.strVecCustomValue.push_back("1");

                vecInfo.push_back(viewInfo);
            }

            bHandle = true;
        }
        break;
    case WIRELESS_UI_TYPE_BT_PIN_INPUT:
        {
            viewInfo.Reset();
            viewInfo.strDisplayName = UIConmonAPI_Lang(TRID_PASSWORD);
            viewInfo.iDataID = DATA_ID_INVALID;
            vecInfo.push_back(viewInfo);
        }
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

// 获取softkey列表
bool CBtUICtrler::GetSoftkeyList(int iUIType, int iDataID, YLVector<int> & vecSoftkey)
{
    DeviceInfo devInfo;
    UI_LIST_BT_DEVICE listDev;
    vecSoftkey.clear();

    for (int i = 0; i < 4; ++ i)
    {
        vecSoftkey.push_back(ST_EMPTY);
    }

    switch (iUIType)
    {
    case WIRELESS_UI_TYPE_BT_MENU:
        {
            vecSoftkey[0] = ST_BACK;

            if (Bluetooth_IsEnable()
                    && Bluetooth_GetDeviceState() != DS_NO_DONGLE)
            {
                vecSoftkey[1] = ST_SCAN;
            }

            if (BT_ITEM_DATA_ID_MENU_SWITCH == iDataID)
            {
                vecSoftkey[2] = ST_SWITCH;
                vecSoftkey[3] = ST_SAVE;
            }
            else
            {
                vecSoftkey[3] = ST_ENTER;
            }
        }
        break;
    case WIRELESS_UI_TYPE_BT_SCAN:
        {
            vecSoftkey[0] = ST_BACK;

            if (Bluetooth_GetDeviceState() != DS_SCAN)
            {
                vecSoftkey[2] = ST_REFRESH;
            }

            // 判断data id对应的设备连接状态
            if (Bluetooth_GetScanedDeviceListSize() > 0)
            {
                Bluetooth_GetBluetoothInfoByHandle(iDataID, devInfo);
                vecSoftkey[3] = devInfo.eState ==
                                WIRE_DEVICE_CONNECTED_STATE ? ST_DISCONNECT : ST_CONNECT;
            }
        }
        break;
    case WIRELESS_UI_TYPE_BT_PAIRED:
        {
            vecSoftkey[0] = ST_BACK;
            vecSoftkey[1] = ST_SCAN;

            if (Bluetooth_GetBluetoothInfoByHandle(iDataID, devInfo))
            {
                vecSoftkey[2] = ST_OPTION;
                vecSoftkey[3] = devInfo.eState ==
                                WIRE_DEVICE_CONNECTED_STATE ? ST_DISCONNECT : ST_CONNECT;
            }
        }
        break;
    case WIRELESS_UI_TYPE_BT_DEV_DETAIL:
        {
            Bluetooth_GetBluetoothInfoByHandle(m_iLastFocusDataID, devInfo);
            vecSoftkey[0] = ST_BACK;
            if (iDataID != BT_ITEM_DATA_ID_DETAIL_SYNC)
            {
                vecSoftkey[3] = devInfo.eState ==
                                WIRE_DEVICE_CONNECTED_STATE ? ST_DISCONNECT : ST_CONNECT;
            }
            else
            {
                vecSoftkey[2] = ST_SWITCH;
                vecSoftkey[3] = ST_SAVE;
            }
        }
        break;
    case WIRELESS_UI_TYPE_BT_DONGLE_INFO:
        {
            vecSoftkey[0] = ST_BACK;
            if (BT_ITEM_DATA_ID_DONGLE_INFO_NAME == iDataID)
            {
                vecSoftkey[1] = ST_IMECHANGE;
                vecSoftkey[2] = ST_DELETE;
            }
            vecSoftkey[3] = ST_SAVE;
        }
        break;
    case WIRELESS_UI_TYPE_BT_PIN_INPUT:
        {
            vecSoftkey[0] = ST_BACK;
            vecSoftkey[1] = ST_IMECHANGE;
            vecSoftkey[2] = ST_DELETE;
            vecSoftkey[3] = ST_OK;
        }
        break;
    default:
        break;
    }

    return true;
}

bool CBtUICtrler::GetOptionSoftkeyList(int iUIType, int iFocusIndex,
                                       YLVector<OPTION_TYPE> & vecOptionSoftkey)
{
    if (iUIType != WIRELESS_UI_TYPE_BT_PAIRED)
    {
        return false;
    }

    vecOptionSoftkey.push_back(OPTION_DETAIL);
    vecOptionSoftkey.push_back(OPTION_DELETE);
    vecOptionSoftkey.push_back(OPTION_DELETE_ALL);

    return true;
}

int CBtUICtrler::GetListSize(int iUIType, bool bOnlyData/* = false*/)
{
    int iListSize = 0;
    DeviceInfo devInfo;

    switch (iUIType)
    {
    case WIRELESS_UI_TYPE_BT_MENU:
        iListSize = Bluetooth_GetDeviceState() != DS_NO_DONGLE && Bluetooth_IsEnable() ? 3 : 1;
        break;
    case WIRELESS_UI_TYPE_BT_SCAN:
    case WIRELESS_UI_TYPE_BT_PAIRED:
        // 列表空,则返回1
        iListSize = GetDevListSize(WIRELESS_UI_TYPE_BT_SCAN == iUIType ?
                                   DEVICE_LIST_SCAN : DEVICE_LIST_PAIRED);
        iListSize = iListSize <= 0 ? 1 : iListSize;
        break;
    case WIRELESS_UI_TYPE_BT_DONGLE_INFO:
        iListSize = 2;
        break;
    case WIRELESS_UI_TYPE_BT_DEV_DETAIL:
        if (Bluetooth_GetBluetoothInfoByHandle(m_iLastFocusDataID, devInfo))
        {
            iListSize = devInfo.eType == BT_HEADSET ? 2 : 3;
        }
        break;
    case WIRELESS_UI_TYPE_BT_PIN_INPUT:
        iListSize = 1;
        break;
    default:
        break;
    }

    return iListSize;
}

// 获取数据在链表的焦点
int CBtUICtrler::GetFocusInDataList(int iDataID)
{
    int iPos = 0;
    DEVICE_LIST_TYPE eType;

    if (_ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_BT_SCAN)
    {
        eType = DEVICE_LIST_SCAN;
    }
    else if (_ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_BT_PAIRED)
    {
        eType = DEVICE_LIST_PAIRED;
    }

    iPos = GetDevIndexInList(iDataID, eType);

    return iPos;
}

// 获取动画的信息
// [strPic]:图片名称前缀 [uNum]:图片数量 [uTime]:变换时间
bool CBtUICtrler::GetAnimationInfo(AnimationType eType, yl::string & strPic, UINT & uNum,
                                   UINT & uTime)
{
    bool bHandle = false;

    switch (eType)
    {
    case ANIMATION_CYCLE_ROTATECALL:
        //if (DS_SCAN == Bluetooth_GetDeviceState())
        {
            strPic = DEVICELIB_IS_T42(devicelib_GetPhoneType()) ?
                     BT_SCAN_ANIMATION_ICON_SMALL : BT_SCAN_ANIMATION_ICON;
            uNum = 8;
            uTime = 300;

            bHandle = true;
        }
        break;
    default:
        break;
    }

    return bHandle;
}

// 处理页面退出后的业务
bool CBtUICtrler::PageExit(int iUIType)
{
    bool bHandle = true;

    switch (iUIType)
    {
    case WIRELESS_UI_TYPE_BT_SCAN:
        // 正在扫描, 则退出扫描
        if (Bluetooth_GetDeviceState() == DS_SCAN)
        {
            ScanOperation(false);
        }
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

// 是否需要显示动画
bool CBtUICtrler::IsNeedShowAnimation(int iUIType, AnimationType eType)
{
    if (WIRELESS_UI_TYPE_BT_SCAN == iUIType
            && ANIMATION_CYCLE_ROTATECALL == eType)
    {
        return DS_SCAN == Bluetooth_GetDeviceState();
    }

    return false;
}

// 处理初始化结果
void CBtUICtrler::ProcessPowerResult(bool bEnable)
{
    if (_ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_BT_MENU)
    {
        _ViewModeUIMgr.UpdateUI();
    }
}

// 处理开关结果
void CBtUICtrler::ProcessSwitchResult(bool bEnable)
{
    _ViewModeUIMgr.UpdateUI();
}

// 处理Pin码请求
void CBtUICtrler::ProcessRequestPin(UINT uMsg, UINT wParam, UINT lParam, void * pData)
{
    if (BT_DEVICE_HANDLE_INVAILED == m_iCurrentOperatingDev)
    {
        return;
    }

    // 移除连接提示框
    _ViewModeUIMgr.RemoveMsgBox(MSGBOX_ID_CONNECT);

    if (BLUETOOTH_MSG_REQ_PIN == uMsg)
    {
        // 进入pin码输入界面
        EnterPage(WIRELESS_UI_TYPE_BT_PIN_INPUT, lParam);
    }
#ifdef IF_BT_SUPPORT_PHONE
    else if (BLUETOOTH_MSG_SHOW_PIN == uMsg)
    {
        if (!pData)
        {
            return ;
        }

        char szBuffPin[50] = { 0 };
        // Logic推送上来的格式: pin&mac
        sscanf((char *)pData, "%[^&]&*", szBuffPin);

        DeviceInfo defInfo;
        Bluetooth_GetBluetoothInfoByHandle(lParam, defInfo);

        yl::string strPinCode = commonAPI_FormatString("%s: %s\n%s: %s",
                                UIConmonAPI_Lang(TRID_CONNECT_TO).c_str(), defInfo.strName.c_str(),
                                UIConmonAPI_Lang(TRID_PASSKEY).c_str(), szBuffPin);

        _ViewModeUIMgr.PopupMsgBox(MSGBOX_ID_MOBILE_PIN,
                                   UIConmonAPI_Lang(TRID_PIN), strPinCode, MSGBOX_STYLE_OK_CANCEL, 0);
    }
#endif
}

// 处理列表更新
void CBtUICtrler::ProcessUpdateList(UINT wParam, UINT lParam, void * pData)
{
    yl::string strTipMsg("");
    yl::string strDevMac("");
    UINT uTime = 0;
    DeviceInfo devInfo;

    if (OP_CONNECT_RESULT == wParam
            || OP_DISCONNECT_RESULT == wParam)
    {
        Bluetooth_GetBluetoothInfoByMac((char *)pData, devInfo);
    }

    bool bNeedUpdateUI = true;
    bool bNeedResetFocus = false;
    switch (wParam)
    {
    // 需要更新链表
    case OP_UPDATE_LIST:
        {
            //
        }
        break;
    // 配对结果处理
    case OP_PAIR_RESULT:
        {
            _ViewModeUIMgr.RemoveMsgBox(MSGBOX_ID_CONNECT);
            if (0 == lParam)
            {
                // 弹框提示连接失败
                _ViewModeUIMgr.RemoveMsgBox(MSGBOX_ID_MOBILE_PIN);
                strTipMsg = TRID_CONNECT_FAILED;
            }
        }
        break;
    // 连接结果处理
    case OP_CONNECT_RESULT:
        {
            _ViewModeUIMgr.RemoveMsgBox(MSGBOX_ID_CONNECT);
            if (true == lParam)
            {
                // 扫描/配对列表界面, 连接上的设备会显示在第一项, 需同步焦点
                if (_ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_BT_SCAN
                        || _ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_BT_PAIRED)
                {
                    bNeedResetFocus = true;
                }
            }
            else
            {
                // 弹框提示连接失败
                strTipMsg = TRID_CONNECT_FAILED;
            }
        }
        break;
    // 扫描结果处理
    case OP_SCAN_RESULT:
        {
            // 扫描结束, 停止扫描动画
            _ViewModeUIMgr.UpdateAnimation(false);

            if (false == lParam)
            {
                strTipMsg = TRID_SCAN_FAILED;
            }
        }
        break;
    // 断开连接结果处理
    case OP_DISCONNECT_RESULT:
        {
            // 弹框提示断开xxx设备成功/失败
            strTipMsg = (yl::string)TRID_DISCONNECT_WITH + " " + devInfo.strName + " ";
            if (true == lParam)
            {
                strTipMsg += TRID_SUCCESSED;
            }
            else
            {
                strTipMsg += TRID_FAILED;
            }

            uTime = 1000;
        }
        break;
    default:
        bNeedUpdateUI = false;
        break;
    }

    // 更新UI
    if (bNeedUpdateUI)
    {
        _ViewModeUIMgr.UpdateUI(bNeedResetFocus);
    }

    // 弹出提示框!扫描过程不提示
    if (strTipMsg != "" && Bluetooth_GetDeviceState() != DS_SCAN)
    {
        _ViewModeUIMgr.PopupMsgBox(MSGBOX_ID_BT_NONE, UIConmonAPI_Lang(TRID_NOTE),
                                   UIConmonAPI_Lang(strTipMsg), MSGBOX_STYLE_CANCEL, uTime);
    }
}

// 获取设备列表
bool CBtUICtrler::GetDevList(UI_LIST_BT_DEVICE & listDevice, DEVICE_LIST_TYPE eType,
                             int iStartPos/* = 0*/, int iNumber/* = 911*/)
{
    // 获取扫描/配对列表
    if (eType == DEVICE_LIST_SCAN)
    {
        int iSize = iNumber == 911 ? Bluetooth_GetScanedDeviceListSize() : iNumber;
        Bluetooth_GetScanedDeviceList(listDevice, iStartPos, iSize);
    }
    else if (eType == DEVICE_LIST_PAIRED
             || eType == DEVICE_LIST_ALL)
    {
        Bluetooth_GetDeviceList(listDevice, eType);

        // 排序, 将连接上的设备置顶 ---- 后续和扫描列表一样, 放到logic处理
        DeviceInfo devInfo;
        for (UI_LIST_BT_DEVICE::iterator iter = listDevice.begin();
                iter != listDevice.end();
                ++ iter)
        {
            devInfo = *iter;
            if (WIRE_DEVICE_CONNECTED_STATE == devInfo.eState)
            {
                iter = listDevice.erase(iter);
                iter = listDevice.begin();
                listDevice.insert(iter, devInfo);
                break;
            }
        }
    }

    return listDevice.size() > 0;
}

// 获取设备列表
bool CBtUICtrler::GetDevList(VecViewInfo & vecInfo, DEVICE_LIST_TYPE eType,
                             int iStartPos/* = 0*/, int iNumber/* = 911*/)
{
    UI_LIST_BT_DEVICE listDevice;

    if (!GetDevList(listDevice, eType, iStartPos, iNumber))
    {
        return false;
    }

    ViewInfo viewInfo;
    for (UI_LIST_BT_DEVICE::iterator iter = listDevice.begin();
            iter != listDevice.end();
            ++ iter)
    {
        viewInfo.Reset();
        DeviceInfo & devInfo = *iter;
        // 转换数据
        LogicData2UI(devInfo, viewInfo);
        vecInfo.push_back(viewInfo);
    }

    return true;
}

// Logic数据转换为UI显示数据
void CBtUICtrler::LogicData2UI(DeviceInfo & devInfo, ViewInfo & devViewInfo)
{
    devViewInfo.iDataID = devInfo.devHandle;
    devViewInfo.strDisplayName = devInfo.strName;

    // 连接状态
    yl::string strConnectStatus("");
    switch (devInfo.eState)
    {
    case WIRE_DEVICE_PAIRING_STATE:
    case WIRE_DEVICE_CONNECTING_STATE:
        strConnectStatus = TRID_CONNECTING;
        break;
    case WIRE_DEVICE_CONNECTED_STATE:
        devViewInfo.bUseValue = false;
        strConnectStatus = TRID_CONNECTED;
        break;
    default:
        strConnectStatus = "";
        break;
    }
    devViewInfo.strValue = devInfo.strMac;
    devViewInfo.strValue1 = UIConmonAPI_Lang(strConnectStatus);

    // 设备图标
    yl::string strIcon("");
    switch (devInfo.eType)
    {
    case BT_HEADSET:
        strIcon = devInfo.eState == WIRE_DEVICE_CONNECTED_STATE && devInfo.bAutoConnect ?
                  GetFullIconName(BT_HEADSET_AC_ICON) : GetFullIconName(BT_HEADSET_ICON);
        break;
    case BT_COMPUTER:
        strIcon = GetFullIconName(BT_COMPUTER_ICON);
        break;
#ifdef IF_BT_SUPPORT_PHONE
    case BT_PHONE:
        strIcon = devInfo.eState == WIRE_DEVICE_CONNECTED_STATE && devInfo.bAutoConnect ?
                  GetFullIconName(BT_PHONE_AC_ICON) : GetFullIconName(BT_PHONE_ICON);
        break;
#endif
    default:
        strIcon = devInfo.eState == WIRE_DEVICE_CONNECTED_STATE && devInfo.bAutoConnect ?
                  GetFullIconName(BT_BLUETOOTH_AC_ICON) : GetFullIconName(BT_BLUETOOTH_ICON);
        break;
    }

    devViewInfo.strVecExtraValue.push_back(strIcon);
}

// 设备是否在列表中
int CBtUICtrler::GetDevIndexInList(int iHandle, DEVICE_LIST_TYPE eType/* = DEVICE_LIST_ALL*/)
{
    UI_LIST_BT_DEVICE listDev;

    // 获取列表
    if (DEVICE_LIST_PAIRED == eType
            || DEVICE_LIST_ALL == eType)
    {
        Bluetooth_GetDeviceList(listDev, eType);
    }
    else if (DEVICE_LIST_SCAN == eType)
    {
        Bluetooth_GetScanedDeviceList(listDev);
    }

    if (listDev.size() <= 0)
    {
        return 0;
    }

    // 查找位置
    int iPos = 0;
    for (UI_LIST_BT_DEVICE::iterator iter = listDev.begin();
            iter != listDev.end();
            ++ iter)
    {
        DeviceInfo & devInfo = *iter;
        if (iHandle == devInfo.devHandle)
        {
            break;
        }

        ++ iPos;
    }

    return iPos;
}

// 通过handle获取mac
yl::string CBtUICtrler::GetDevMacByHandle(int iHandle)
{
    DeviceInfo devInfo;

    if (Bluetooth_GetBluetoothInfoByHandle(iHandle, devInfo))
    {
        return devInfo.strMac;
    }

    return "";
}

// 获取设备列表大小
int CBtUICtrler::GetDevListSize(DEVICE_LIST_TYPE eType/* = DEVICE_LIST_ALL*/)
{
    int iSize = 0;
    UI_LIST_BT_DEVICE listDev;

    // 获取列表
    if (DEVICE_LIST_PAIRED == eType
            || DEVICE_LIST_ALL == eType)
    {
        Bluetooth_GetDeviceList(listDev, eType);
        iSize = listDev.size();
    }
    else if (DEVICE_LIST_SCAN == eType)
    {
        iSize = Bluetooth_GetScanedDeviceListSize();
    }

    return iSize;
}

// 扫描操作
bool CBtUICtrler::ScanOperation(bool bStartScan)
{
    WIRE_INFO("CBtUICtrler::ScanOperation[%s]", bStartScan ? "start" : "stop");

    bool bResult = false;
    yl::string strMsg("");

    if (bStartScan)
    {
        if (Bluetooth_GetDeviceState() == DS_NO_DONGLE)
        {
            strMsg = TRID_NO_BLUETOOTH_ADAPTER_FOUND;
        }
        else if (Bluetooth_GetDeviceState() == DS_OK)
        {
            bResult = Bluetooth_StartScanDevice();
            strMsg = bResult ? "" : TRID_SCAN_FAILED;
        }
    }
    else
    {
        bResult = Bluetooth_StopScanDevice();
    }

    // 弹框提示扫描信息
    if (!strMsg.empty())
    {
        _ViewModeUIMgr.PopupMsgBox(MSGBOX_ID_BT_NONE, UIConmonAPI_Lang(TRID_NOTE),
                                   UIConmonAPI_Lang(strMsg), MSGBOX_STYLE_CANCEL, 1500);
    }

    return bResult;
}

// 连接操作
bool CBtUICtrler::ConnectOperation(bool bConnnect, int iHandle)
{
    if (BT_DEVICE_HANDLE_INVAILED == iHandle)
    {
        return false;
    }

    // 没获取到数据
    DeviceInfo devInfo;
    if (!Bluetooth_GetBluetoothInfoByHandle(iHandle, devInfo))
    {
        return false;
    }

    WIRE_INFO("ConnectOperation. connect?[%d], handle[%d], dev state[%d]",
              bConnnect, iHandle, devInfo.eState);

    yl::string strTipMsg("");
    int iTipTime = 0;
    MsgBoxID eMsgBoxID = MSGBOX_ID_BT_NONE;
    bool bResult = false;

    if (bConnnect)
    {
        m_iCurrentOperatingDev = iHandle;
        bResult = Bluetooth_ConnectDevice(devInfo.strMac.c_str());

        eMsgBoxID = MSGBOX_ID_CONNECT;
        strTipMsg = bResult ? TRID_OLD_CONNECTING : TRID_CONNECT_FAILED;
    }
    else
    {
        bResult = Bluetooth_DisconnectDevice(devInfo.strMac.c_str());

        if (devInfo.eState == WIRE_DEVICE_CONNECTED_STATE)
        {
            iTipTime = 1000;
            strTipMsg = bResult ? TRID_DISCONNECTING : TRID_DISCONNECT_FAILED;
        }
    }

    // 弹框提示连接/断开信息
    if (strTipMsg != "")
    {
        _ViewModeUIMgr.PopupMsgBox(eMsgBoxID, UIConmonAPI_Lang(TRID_NOTE),
                                   UIConmonAPI_Lang(strTipMsg), MSGBOX_STYLE_CANCEL, iTipTime);
    }

    return bResult;
}

// 确认操作
bool CBtUICtrler::ConfirmOperation(int iKey, int iDataID,
                                   const yl::string & strValue,
                                   void * pData/* = NULL*/)
{
    WIRE_INFO("ConfirmOperation. data id [%d], value [%s]", iDataID, strValue.c_str());

    yl::string strTipMsg("");
    int iTipTime = 2000;
    MsgBoxID eMsgBoxID = MSGBOX_ID_BT_NONE;
    MsgBoxStyle eType = MSGBOX_STYLE_CANCEL;
    bool bExitPage = false;

    if (ST_OK == iKey || ST_SAVE == iKey || PHONE_KEY_OK == iKey)
    {
        // 处理menu业务
        if (_ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_BT_MENU)
        {
            if (iDataID == BT_ITEM_DATA_ID_MENU_SWITCH)
            {
                Bluetooth_SetEnable(strValue == "1" ? true : false);
                strTipMsg = strValue == "1" && Bluetooth_GetDeviceState() == DS_NO_DONGLE ?
                            TRID_NO_BLUETOOTH_ADAPTER_FOUND : TRID_SAVED;
            }
            else
            {
                EnterPage(iDataID, iDataID);
            }
        }
        // 处理dongle info业务
        else if (_ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_BT_DONGLE_INFO)
        {
            yl::string strValue("");
            if (pData
                    && GetItemValueByDataID(BT_ITEM_DATA_ID_DONGLE_INFO_NAME, *(VecItemDataID2Info *)pData, strValue)
                    && strValue != "")
            {
                Bluetooth_SetDeviceName(strValue.c_str());
                ExitPage();
            }
            if (strValue.empty())
            {
                strTipMsg = TRID_EDIT_DEVICE_NAME_CANNOT_BE_EMPTY;
                _ViewModeUIMgr.SetDataListFocus(BT_ITEM_DATA_ID_DONGLE_INFO_NAME);
            }
            else
            {
                strTipMsg = TRID_SAVING_CONFIG_FILE;
            }
        }
        // 处理pwd业务
        else if (_ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_BT_PIN_INPUT)
        {
            DeviceInfo devInfo;
            if (Bluetooth_GetBluetoothInfoByHandle(m_iLastFocusDataID, devInfo))
            {
                Bluetooth_AnswerPin(devInfo.strMac.c_str(), strValue.c_str());
            }
            ExitPage();
        }
        // 处理scan/paired业务
        else if (PHONE_KEY_OK == iKey
                 && (_ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_BT_SCAN
                     || _ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_BT_PAIRED))
        {
            // 判断data id对应的设备连接状态
            DeviceInfo devInfo;
            if (Bluetooth_GetBluetoothInfoByHandle(iDataID, devInfo))
            {
                bool bIsConnect = devInfo.eState != WIRE_DEVICE_CONNECTED_STATE;
                ConnectOperation(bIsConnect, iDataID);
            }
        }
        else if ((PHONE_KEY_OK == iKey || ST_SAVE == iKey)
                 && _ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_BT_DEV_DETAIL)
        {
            DeviceInfo devInfo;
            if (Bluetooth_GetBluetoothInfoByHandle(m_iLastFocusDataID, devInfo))
            {
                if (BT_ITEM_DATA_ID_DETAIL_SYNC == iDataID)
                {
                    Bluetooth_SetSyncContact(devInfo.strMac.c_str(), strValue == "1" ? true : false);
                    strTipMsg = TRID_SAVING_CONFIG_FILE;
                    eType = MSGBOX_STYLE_NONE;
                    iTipTime = 1000;
                    bExitPage = true;
                }
                else
                {
                    bool bIsConnect = devInfo.eState != WIRE_DEVICE_CONNECTED_STATE;
                    ConnectOperation(bIsConnect, iDataID);
                }
            }
        }
    }

    if (strTipMsg != "")
    {
        _ViewModeUIMgr.PopupMsgBox(eMsgBoxID, UIConmonAPI_Lang(TRID_NOTE),
                                   UIConmonAPI_Lang(strTipMsg), eType, iTipTime);
    }

    if (bExitPage)
    {
        ExitPage();
    }

    return true;
}

// 返回操作
bool CBtUICtrler::CancelOperation(int iKey, bool bExitPage/* = true*/)
{
    WIRE_INFO("CancelOperation. exit page[%d]", bExitPage);

    if (ST_BACK == iKey
            || PHONE_KEY_CANCEL == iKey)
    {
        // 正在扫描, 则退出扫描
        if (Bluetooth_GetDeviceState() == DS_SCAN)
        {
            ScanOperation(false);
        }

        // 正在配对, 则取消配对
        DeviceInfo devInfo;
        if (m_iCurrentOperatingDev != DATA_ID_INVALID
                && _ViewModeUIMgr.GetTopUIType() == WIRELESS_UI_TYPE_BT_PIN_INPUT)
        {
            Bluetooth_CancelPair(GetDevMacByHandle(m_iCurrentOperatingDev).c_str());
        }

        if (bExitPage)
        {
            ExitPage();
        }
    }

    return true;
}

// 通过Data ID获取item对应的value
bool CBtUICtrler::GetItemValueByDataID(int iDataID,
                                       VecItemDataID2Info & vecItemDataID2Info,
                                       yl::string & strValue)
{
    for (int iIndex = 0; iIndex < vecItemDataID2Info.size(); ++iIndex)
    {
        ItemDataID2Info & itemDataID2Info = vecItemDataID2Info[iIndex];
        if (iDataID == itemDataID2Info.iDataID)
        {
            strValue = itemDataID2Info.strValue;
            return true;
        }
    }

    return false;
}

// 获取icon全名
yl::string CBtUICtrler::GetFullIconName(yl::string strIconPrefix)
{
    yl::string strIconName;
#if IS_COLOR
    strIconName = FORMATE_COLOR_ICON(strIconPrefix);
#else
    if (DEVICELIB_IS_T42(devicelib_GetPhoneType()))
    {
        strIconName = FORMATE_FULL_ICON_SMALL(strIconPrefix);
    }
    else
    {
        strIconName = FORMATE_FULL_ICON(strIconPrefix);
    }
#endif
    return strIconName;
}

#endif // IF_SUPPORT_BLUETOOTH
