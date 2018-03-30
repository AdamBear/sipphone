#include "wifi_inc.h"
#include "settingui/src/cdlgaskpassword.h"

Q_DECLARE_METATYPE(WifiAddData);
Q_DECLARE_METATYPE(WirelessSearchStatisticsData);

namespace
{
#define DEFAULT_WPS_WPSPIN_TIME         120000

//
//#define TEST_WIFI
// action
#define ENTER_WIFI_STORAGE_PAGE         "OpenSubPage(wifistorage)"
#define ENTER_WIFI_STATUS_PAGE          "OpenSubPage(wifistatus)"
#define ENTER_WIFI_ADD_EDIT_PAGE        "OpenSubPage(wifiaddedit)"
#define ENTER_WIFI_DETAIL_PAGE          "OpenSubPage(wifidetail)"
// item click action
#define WIFI_STATUS                     "WifiStatus"
#define WIFI_STORAGE                    "WifiStorage"
#define CONNECT_WIFI                    "Connect_Wifi"
#define DISCONNECT_WIFI                 "DisConnect_Wifi"
// softkey action
#define CANCEL_SCAN                     "Cancel_Scan"
#define CANCEL_CONNECT                  "Cancel_Connect"
#define WIFI_SCAN                       "Scan"
#define WIFI_ADD                        "Add"
#define WIFI_SOFTKEY_WPS_PIN            "WPS-PIN"
#define WIFI_SOFTKEY_WPS                "WPS"
// popup dlg id
#define PopupBoxID_WPSPIN               90
#define PopupBoxID_RemainTime           91
#define PopupBoxID_CancelWPS_WPSPIN     93
// wifi scanning waiting animation location
#ifdef _T49
const QRect RECT_WIFI_SCAN_WAITING(416, 455, 62, 62);
#else
const QRect RECT_WIFI_SCAN_WAITING(240, 217, 62, 62);
#endif
// wifi scanning waiting animation frequent
#define WIFI_SCANNING_ANIMATION_FREQUENT 270

#ifdef _T49
// menu item click bg color
#define MENU_ITEM_CLICK_BG_COLOR QColor(233, 233, 233)
#endif

#define OPEN_PASSWORD_ACTION    "OpenSubPage(enter_password)"

#define WIFI_ITEM_BEGIN_INDEX_BY_TESTMODE 2
#define WIFI_ITEM_BEGIN_INDEX_BY_MORDEL   4
}

// 处理消息.
static LRESULT OnWifiBaseUIMessage(msgObject & refMessage)
{
#ifdef IF_SUPPORT_WIFI
    SETTINGUI_INFO("[CDlgWifiBase::OnWifiBaseUIMessage] message[%d], lParam[%d], wParam[%d], UpUI[%d]",
                   refMessage.message
                   ,  refMessage.lParam, refMessage.wParam, WIFI_MSG_UPDATE_UI);


    CDlgWifiBase * pWifiBase = qobject_cast<CDlgWifiBase *>(UIManager_GetPrivateDlg(DLG_CDlgWifiBase));

    // http://10.2.1.199/Bug.php?BugID=93430
    // OP_WIFI_OPEN_RESULT 不在顶层时也许处理
    if (NULL == pWifiBase || (!pWifiBase->IsTopPage() && refMessage.wParam != OP_WIFI_OPEN_RESULT))
    {
        SETTINGUI_INFO("CDlgWifiBase not top page");
        WirelessOperationMsg * wifiMsg = (WirelessOperationMsg *)refMessage.GetExtraData();
#ifdef FRESH_WINDOW_WITH_MIN_CHANGED
        if (NULL != wifiMsg && NULL != pWifiBase
                && OP_CONNECT_WIFI_RESULT == wifiMsg->iOperationType
                && WIRELESS_OP_CONNECT_SUCCESS == wifiMsg->iOperationResult)
        {
            pWifiBase->ReSetCurOffSet();
        }
#else
        if (NULL != wifiMsg && OP_ENTER_WIFI_PWD != wifiMsg->iOperationResult)
        {
            _UIWifiManager.SetNeedUpdate(true);
        }
#endif
        if (NULL != wifiMsg
                && (OP_WIFI_CLOSE_RESULT == wifiMsg->iOperationType
                    || OP_SCAN_WIFI_RESULT == wifiMsg->iOperationType))
        {
            _UIWifiManager.SetNeedUpdate(true);
        }
        return 0;
    }
    else
    {
        return pWifiBase->OnInterWifiBaseUIMessage(refMessage);
    }
#endif
    return 0;
}

LRESULT CDlgWifiBase::OnInterWifiBaseUIMessage(msgObject & refMessage)
{
#ifdef IF_SUPPORT_WIFI
    SETTINGUI_INFO("OP_UPDATE_WIFI_DATA = %d ", OP_UPDATE_WIFI_DATA);
    SETTINGUI_INFO("OP_WIFI_OPEN_RESULT = %d ", OP_WIFI_OPEN_RESULT);
    SETTINGUI_INFO("OP_WIFI_CLOSE_RESULT = %d ", OP_WIFI_CLOSE_RESULT);
    SETTINGUI_INFO("OP_WPSPINCONNECT_RESULT = %d ", OP_WPSPINCONNECT_RESULT);
    SETTINGUI_INFO("OP_WPSCONNECT_RESULT = %d", OP_WPSCONNECT_RESULT);
    SETTINGUI_INFO("OP_SCAN_WIFI_RESULT = %d ", OP_SCAN_WIFI_RESULT);
    SETTINGUI_INFO("OP_CONNECT_WIFI_RESULT = %d ", OP_CONNECT_WIFI_RESULT);
    SETTINGUI_INFO("OP_ENTER_WIFI_PWD = %d ", OP_ENTER_WIFI_PWD);
    SETTINGUI_INFO("OP_DISCONNECT_WIFI_RESULT = %d ", OP_DISCONNECT_WIFI_RESULT);
    SETTINGUI_INFO("OP_WIFI_ADD_RESULT = %d ", OP_WIFI_ADD_RESULT);

    if (WIFI_MSG_UPDATE_UI != refMessage.message)
    {
        return TRUE;
    }

    WirelessOperationMsg * wifiMsg = (WirelessOperationMsg *)refMessage.GetExtraData();
    SETTINGUI_INFO("[SettingUI]OnWifiBaseUIMessage message[%d], type[%d], result[%d]",
                   refMessage.message
                   ,  wifiMsg->iOperationType, wifiMsg->iOperationResult);
    if (NULL != wifiMsg)
    {
#ifdef _T49
#ifdef QT_SLIP_GESTURE_SUPPORT
        // record cur off set
        m_pWifiDelegate->RecordOffSet();
#endif
#else
        m_pWifiDelegate->RecordOffSet();
#endif

        switch (wifiMsg->iOperationType)
        {
        case OP_WIFI_ADD_RESULT:
        {
            if (WIRELESS_OP_ADD_LISTFULL == wifiMsg->iOperationResult)
            {
                _UIWifiManager.Wifi_ShowCommonMsgBox(NULL
                                                     , LANG_TRANSLATE(TRID_STORAGE_LIST_IS_FULL_PLEASE_DELETE_OLD_ONE_IF_WANTING_TO_ADD_IN)
                                                     , MESSAGEBOX_Cancel);
            }
        }
        break;
        case OP_UPDATE_WIFI_DATA:
        {
            if (NULL != m_pWifiSwitch)
            {
                DoRelayout(m_pWifiSwitch->GetData());
            }
        }
        break;
        case OP_WIFI_CLOSE_RESULT:
        {
            if (WIRELESS_OP_OPEN_RESULT_ING != wifiMsg->iOperationResult)
            {
                m_pWifiSwitch->setEnabled(true);
            }

#ifdef _T49
#ifdef QT_SLIP_GESTURE_SUPPORT
            // reset off set
            m_pWifiDelegate->ClearOffSet();
#endif
#else
            m_pWifiDelegate->ClearOffSet();
#endif

            _UIWifiManager.StopWifiWPS_WPSPIN();
            MessageBox_RemoveMessageBox(this);
            _MainWND.SetTimerLock(false);

            CheckSwitchChange();
        }
        break;
        case OP_WIFI_OPEN_RESULT:
        {
            if (WIRELESS_OP_OPEN_RESULT_ING != wifiMsg->iOperationResult)
            {
                m_pWifiSwitch->setEnabled(true);
            }

#ifdef _T49
#ifdef QT_SLIP_GESTURE_SUPPORT
            // reset off set
            m_pWifiDelegate->ClearOffSet();
#endif
#else
            m_pWifiDelegate->ClearOffSet();
#endif

            //WIFI开启结果 iOperationResult 0/1/2/3 success/no dongle/cable connected/failed
#ifdef IF_SUPPORT_WIFI
            if (WIRELESS_OP_OPEN_RESULT_FAILED == wifiMsg->iOperationResult)
            {
                SetWifiOpenClose(false);
                if (!GetOpenByTestMode())
                {
                    _UIWifiManager.Wifi_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_OPEN_FAIL)
                                                         , MESSAGEBOX_Cancel, SHORT_TIME);
                }
            }
            else if (WIRELESS_OP_OPEN_RESULT_SUCCESS == wifiMsg->iOperationResult)
            {
                CheckSwitchChange();
                SETTINGUI_INFO("[wifiBase msg][start scan wifi deivces]");
                OnAction(WIFI_SCAN);
            }
#else
            if (!WIFI_IsCableConnected())
            {
                if (WIRELESS_OP_OPEN_RESULT_NODONGLE == wifiMsg->iOperationResult)
                {
                    _UIWifiManager.Wifi_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_NO_FIND_WIFI_ADAPTER)
                                                         , MESSAGEBOX_Cancel, SHORT_TIME);
                }
                else if (WIRELESS_OP_OPEN_RESULT_FAILED == wifiMsg->iOperationResult)
                {
                    SetWifiOpenClose(false);
                    if (!GetOpenByTestMode())
                    {
                        _UIWifiManager.Wifi_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_OPEN_FAIL)
                                                             , MESSAGEBOX_Cancel, SHORT_TIME);
                    }
                }
            }
#endif
        }
        break;
        case OP_WPSPINCONNECT_RESULT:
        {
            SETTINGUI_INFO("[SettingUI][OnWifiBaseUIMessage][OnWifiBaseUIMessage::OP_WPSPINCONNECT_RESULT]");
            //WPS/WPS-PIN连接结果 lparam 0/1 failed/success
            _MainWND.SetTimerLock(false);
            MessageBox_RemoveTopMessageBox();
            if (0 == wifiMsg->iOperationResult)
            {
                _UIWifiManager.Wifi_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_CONNECT_FAILED)
                                                     , MESSAGEBOX_Cancel, SHORT_TIME);
            }
            else if (1 == wifiMsg->iOperationResult)
            {
#ifdef _T49
#ifdef QT_SLIP_GESTURE_SUPPORT
                // reset off set
                m_pWifiDelegate->ClearOffSet();
#endif
#else
                m_pWifiDelegate->ClearOffSet();
#endif

                _UIWifiManager.Wifi_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_CONNECT_SUCCESS)
                                                     , MESSAGEBOX_Cancel, SHORT_TIME);
            }

            if (NULL != m_pWifiSwitch)
            {
                DoRelayout(m_pWifiSwitch->GetData());
            }
        }
        break;
        case OP_WPSCONNECT_RESULT:
        {
            SETTINGUI_INFO("[SettingUI][OnWifiBaseUIMessage][OnWifiBaseUIMessage::OP_WPSCONNECT_RESULT]");
            //WPS-PBC连接结果 iOperationResult 0/1 failed/success
            _MainWND.SetTimerLock(false);
            MessageBox_RemoveTopMessageBox();
            if (0 == wifiMsg->iOperationResult)
            {
                _UIWifiManager.Wifi_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_CONNECT_FAILED)
                                                     , MESSAGEBOX_Cancel, SHORT_TIME);
            }
            else if (1 == wifiMsg->iOperationResult)
            {
#ifdef _T49
#ifdef QT_SLIP_GESTURE_SUPPORT
                // reset off set
                m_pWifiDelegate->ClearOffSet();
#endif
#else
                m_pWifiDelegate->ClearOffSet();
#endif

                _UIWifiManager.Wifi_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_CONNECT_SUCCESS)
                                                     , MESSAGEBOX_Cancel, SHORT_TIME);
            }

            if (NULL != m_pWifiSwitch)
            {
                DoRelayout(m_pWifiSwitch->GetData());
            }
        }
        break;
        case OP_SCAN_WIFI_RESULT:
        {
#ifdef _T49
#ifdef QT_SLIP_GESTURE_SUPPORT
            // reset off set
            m_pWifiDelegate->ClearOffSet();
#endif
#else
            m_pWifiDelegate->ClearOffSet();
#endif

            //扫描结果 lparam : 0/1 failed/success
            SetOff2OnScan(false);
            if (NULL != m_pWifiSwitch)
            {
                DoRelayout(m_pWifiSwitch->GetData());
            }
        }
        break;
        case OP_CONNECT_WIFI_RESULT:
        {
            //连接结果  lparam :0/1/2 success/wrong pwd/failed
            if (-1 != wifiMsg->lSessionID)
            {
                if (WIRELESS_OP_CONNECT_SUCCESS == wifiMsg->iOperationResult)
                {
#ifdef _T49
#ifdef QT_SLIP_GESTURE_SUPPORT
                    // reset off set
                    m_pWifiDelegate->ClearOffSet();
#endif
#else
                    m_pWifiDelegate->ClearOffSet();
#endif

                    if (NULL != m_pWifiSwitch)
                    {
                        DoRelayout(m_pWifiSwitch->GetData());
                    }
                }
                else if (WIRELESS_OP_CONNECT_FAILED == wifiMsg->iOperationResult)
                {
                    SetConnectResultNote(wifiMsg->iOperationResult, wifiMsg->lSessionID);
                }
            }
        }
        break;
        case OP_ENTER_WIFI_PWD:
        {
            SETTINGUI_INFO("[SettingUI][OnWifiBaseUIMessage::OP_ENTERPWD]");

            if (wifiMsg->lSessionID != WIFI_SECTION_ID_INVAILED  && !_UIWifiManager.IsOperatering())
            {
                _UIWifiManager.SetOperatering(true);
                m_iPopWifiID = wifiMsg->lSessionID;

                bool bAuthentication = (refMessage.lParam == 1);

                PopPwdDialog(bAuthentication);
            }
        }
        break;
        case OP_DISCONNECT_WIFI_RESULT:
        {
            //断开连接结果lparam:0/1 failed/success
            if (-1 != wifiMsg->lSessionID)
            {
                SetDisConnectResultNote(wifiMsg->iOperationResult, wifiMsg->lSessionID);
            }
        }
        break;
        default:
            break;
        }
    }
#endif
    return TRUE;
}

/************************************* CDlgWifiBase ********************************************/
CDlgWifiBase::CDlgWifiBase(QWidget * parent /*= NULL*/)
    : CDlgBaseSubPage(parent)
    , m_pWifiSwitch(NULL)
    , m_iPreWifiID(-1)
    , m_bOnScan(false)
    , m_bConnecting(false)
    , m_pWifiDelegate(NULL)
    , m_pPicAnimation(NULL)
    , m_bTestModeOpen(false)
    , m_bFirstCreate(true)
    , m_iPopWifiID(WIFI_SECTION_ID_INVAILED)
#ifdef _T49
    , m_pTimer(NULL)
#endif
{
    InitData();
}

CDlgWifiBase::~CDlgWifiBase(void)
{
    _MainWND.SetTimerLock(false);

    if (NULL != m_pPicAnimation)
    {
        m_pPicAnimation->Stop();
        m_pPicAnimation->hide();
    }

    if (_UIWifiManager.IsOperatering())
    {
        OnCancelPassword();
    }

#ifdef _T49
    if (NULL != m_pTimer)
    {
        if (m_pTimer->isActive())
        {
            m_pTimer->stop();
        }
        disconnect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnTimeOut()));
    }
#endif

    if (OR_INTERFACE_SUCCESS == _UIWifiManager.IsScanning())
    {
        _UIWifiManager.CancelScanWifiDevices("CC");
    }

    m_bOnScan = false;

    m_bTestModeOpen = false;
#ifdef _T49
    if (NULL != m_pWifiDelegate)
    {
        m_pWifiDelegate->SetTestMode(false);
    }
#endif
}

void CDlgWifiBase::Init()
{
#ifdef IF_SUPPORT_WIFI
    etl_RegisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, OnWifiBaseUIMessage);
#endif
}

void CDlgWifiBase::Uninit()
{
#ifdef IF_SUPPORT_WIFI
    etl_UnregisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, OnWifiBaseUIMessage);
#endif
}

void CDlgWifiBase::ReSetCurOffSet()
{
#ifdef _T49
#ifdef QT_SLIP_GESTURE_SUPPORT
    m_pWifiDelegate->ClearOffSet();
#endif
#else
    m_pWifiDelegate->ClearOffSet();
#endif
}

void CDlgWifiBase::InitData()
{
#ifndef TEMP_SLIP_SOLUTION
    m_pWifiSwitch = new qCheckBox(this);
#endif
    m_pWifiDelegate = new WifiDelegate();
#ifdef _T49
    m_pTimer = new QTimer(this);
#endif
    m_pPicAnimation = new qMultiPicAnimation(this);
    if (
#ifndef TEMP_SLIP_SOLUTION
        NULL == m_pWifiSwitch ||
#endif
        NULL == m_pWifiDelegate
        || NULL == m_pPicAnimation
#ifdef _T49
        || NULL == m_pTimer
#endif
    )
    {
        return;
    }

    // init delegate
    m_pWifiDelegate->setParent(this);
    m_pWifiDelegate->BindFramelist(m_pGeneralFrame);
    m_pWifiDelegate->InitSettingListStyle();
    m_pListDelegate = m_pWifiDelegate;
    m_pWifiDelegate->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG_TOP,
                                    PIC_GENERAL_LIST_ITEM_CLICK_BG,
                                    PIC_GENERAL_LIST_ITEM_CLICK_BG);
    m_pWifiDelegate->SetItemParent(this);
    m_pListDelegate->SetItemParent(this);
    QObject::connect(m_pWifiDelegate, SIGNAL(ItemDetailAction(CListItem *)), this,
                     SLOT(OnItemDetailAction(CListItem *)));
    QObject::connect(m_pWifiDelegate, SIGNAL(listItemAction(int, QString)), this,
                     SLOT(OnListItemAction(int, QString)));

#ifdef _T49
    m_pListDelegate->SetItemClickBGColor(true, MENU_ITEM_CLICK_BG_COLOR);
    m_pWifiDelegate->SetItemClickBGColor(true, MENU_ITEM_CLICK_BG_COLOR);
    m_pListDelegate->SetUiActionProcessor(this);
    m_pWifiDelegate->SetUiActionProcessor(this);
#endif

    // set support slip attribute
#ifdef QT_SLIP_GESTURE_SUPPORT
    // set support different item height attribute
    CT49FrameListStyle & t49Style = m_pGeneralFrame->GetFrameListStyle();
    t49Style.EnableMultiItemType(true);
#endif

    // init animitor
    m_pPicAnimation->setGeometry(RECT_WIFI_SCAN_WAITING);
    m_pPicAnimation->AddPicture(PIC_WIFI_SCANNING_1);
    m_pPicAnimation->AddPicture(PIC_WIFI_SCANNING_2);
    m_pPicAnimation->AddPicture(PIC_WIFI_SCANNING_3);
    m_pPicAnimation->AddPicture(PIC_WIFI_SCANNING_4);
    m_pPicAnimation->AddPicture(PIC_WIFI_SCANNING_5);
    m_pPicAnimation->AddPicture(PIC_WIFI_SCANNING_6);
    m_pPicAnimation->AddPicture(PIC_WIFI_SCANNING_7);
    m_pPicAnimation->AddPicture(PIC_WIFI_SCANNING_8);
    m_pPicAnimation->SetPlaySpeed(WIFI_SCANNING_ANIMATION_FREQUENT);
    m_pPicAnimation->hide();
#ifndef TEMP_SLIP_SOLUTION
    m_pWifiSwitch->installEventFilter(this);

    connect(m_pWifiSwitch->GetRadioButtonOn(), SIGNAL(toggled(bool)), this,
            SLOT(OnSwitchDeviceChange(bool)));
#endif

    SetSoftkeyType(STYLE_TYPE_DIVIDE);

#ifdef _T49
#ifdef FRESH_WINDOW_WITH_MIN_CHANGED
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnTimeOut()));
#endif
#endif // _T49
}

/********************************** 继承自CListSubPage ***********************************/
bool CDlgWifiBase::GetSoftkeyData(CArraySoftKeyBarData & objWndData
#ifdef _T49
                                  , QObject * pSignalReceiver
#endif
                                 )
{
    for (int i = 0; i < SOFTKEY_NUM; ++i)
    {
        objWndData[i].m_strSoftkeyTitle.clear();
        objWndData[i].m_strSoftkeyAction.clear();
        objWndData[i].m_strIconPath.clear();
    }

    // 开启设置Softkey
#ifndef TEMP_SLIP_SOLUTION
    if (NULL != m_pWifiSwitch && m_pWifiSwitch->GetData())
#else
    if (1)
#endif
    {
        if (OR_INTERFACE_SUCCESS == _UIWifiManager.IsScanning() || m_bOnScan)
        {
            objWndData[0].m_strSoftkeyAction = CANCEL_SCAN;
            objWndData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_CANCEL);
            objWndData[0].m_strIconPath = PIC_SOFTKEY_CANCEL;
        }
        else if (m_bConnecting)
        {
            objWndData[0].m_strSoftkeyAction = CANCEL_CONNECT;
            objWndData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_CANCEL);
            objWndData[0].m_strIconPath = PIC_SOFTKEY_CANCEL;
        }
        else
        {
            objWndData[0].m_strSoftkeyAction = WIFI_ADD;
            objWndData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_ADD);
            objWndData[0].m_strIconPath = PIC_SOFTKEY_ADD;

            objWndData[1].m_strSoftkeyAction = WIFI_SOFTKEY_WPS;
            objWndData[1].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_WPS);
            objWndData[1].m_strIconPath = PIC_SOFTKEY_WPS;

            objWndData[2].m_strSoftkeyAction = WIFI_SOFTKEY_WPS_PIN;
            objWndData[2].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_WPS_PIN);
            objWndData[2].m_strIconPath = PIC_SOFTKEY_WPS_PIN;

            objWndData[3].m_strSoftkeyAction = WIFI_SCAN;
            objWndData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SCAN);
            objWndData[3].m_strIconPath = PIC_SOFTKEY_SCAN;
        }
    }

    return true;
}

//通过控制获取已有配置数据，通过V层在界面上显示出来
bool CDlgWifiBase::LoadPageData()
{
    SETTINGUI_INFO("[CDlgWifiBase::LoadPageData()][Wifi_IsEnable[%d]]"
                   , OR_INTERFACE_SUCCESS == _UIWifiManager.IsWifiEnable());

#ifndef TEMP_SLIP_SOLUTION
    if (NULL != m_pWifiSwitch)
    {
        m_pWifiSwitch->SetData(OR_INTERFACE_SUCCESS == _UIWifiManager.IsWifiEnable());
    }
#else
    bool bOn = OR_INTERFACE_SUCCESS == _UIWifiManager.IsWifiEnable();
#endif

#ifdef TEST_WIFI
    DoRelayout(true);
#else
#ifndef TEMP_SLIP_SOLUTION
    if (!m_pWifiSwitch->GetData())
    {
        DoRelayout(m_pWifiSwitch->GetData());
    }
    _UIWifiManager.SetWifiEnable(m_pWifiSwitch->GetData());
#else
    DoRelayout(bOn);
#endif
#endif

    return true;
}

bool CDlgWifiBase::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (NULL == pObj || NULL == pEvent)
    {
        return false;
    }
    if (QEvent::KeyPress == pEvent->type())
    {
        QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
        switch (pKeyEvent->key())
        {
        case PHONE_KEY_1:
        case PHONE_KEY_2:
        case PHONE_KEY_3:
        case PHONE_KEY_4:
        case PHONE_KEY_5:
        case PHONE_KEY_6:
        case PHONE_KEY_7:
        case PHONE_KEY_8:
        case PHONE_KEY_9:
        {
            return true;
        }
        break;
        default:
            break;
        }
    }

    return CDlgBaseSubPage::eventFilter(pObj, pEvent);
}

bool CDlgWifiBase::OnAction(const QString & strAction)
{
    SETTINGUI_INFO("[CDlgWifiBase::OnAction()][%s][preId][%d]"
                   , strAction.toUtf8().data(), m_iPreWifiID);

    if (WIFI_ADD == strAction && !GetOpenByTestMode())
    {
        m_bOnScan = false;
#ifdef IF_SUPPORT_WIFI
        if (MAX_SAVED_NUM <= WIFI_GetSavedNetworkNum())
        {
            _UIWifiManager.Wifi_ShowCommonMsgBox(NULL,
                                                 LANG_TRANSLATE(TRID_LIST_IS_FULL_PLEASE_DELETE_BEFORE_ADD)
                                                 , MESSAGEBOX_Cancel, SHORT_TIME);
        }
        else
#endif
        {
            CreatePageByString(ENTER_WIFI_ADD_EDIT_PAGE, ENUM_WIFI_ADD_PAGE_TYPE,
                               m_pGeneralFrame->GetFocusItem());
        }
    }
    else if (WIFI_STATUS == strAction && !GetOpenByTestMode())
    {
        m_bOnScan = false;
#ifdef _T49
        if (NULL != m_pTimer)
        {
            if (m_pTimer->isActive())
            {
                m_pTimer->stop();
            }
        }
#endif
        emit openpage(SubMenuData(ENTER_WIFI_STATUS_PAGE));
    }
    else if (WIFI_STORAGE == strAction && !GetOpenByTestMode())
    {
        m_bOnScan = false;
#ifdef _T49
        if (NULL != m_pTimer)
        {
            if (m_pTimer->isActive())
            {
                m_pTimer->stop();
            }
        }
#endif
        emit openpage(SubMenuData(ENTER_WIFI_STORAGE_PAGE));
    }
    else if (CANCEL_SCAN == strAction)
    {
        m_bOnScan = false;

        if (OR_INTERFACE_SUCCESS == _UIWifiManager.IsScanning())
        {
            _UIWifiManager.CancelScanWifiDevices("CC");
            emit RefreshSoftkey();
            DoRelayout(true);
        }
    }
    else if (CANCEL_CONNECT == strAction)
    {
        if (m_bConnecting)
        {
            m_bConnecting = false;
            emit RefreshSoftkey();

            // 更新Item状态
            if (-1 != m_iPreWifiID && m_mpIDIndex[m_iPreWifiID] < m_vecWidgetPair.size())
            {
                int iIdx = m_mpIDIndex[m_iPreWifiID];
                // 消除之前连接的提示
                SETTINGUI_INFO("cancel connect idx[%d], str[%s], INT_NONE", iIdx, "");
                SetWifiItemSavedDataByDataIndex(iIdx, "", INT_NONE, SETTING_ITEM_WIFI, CONNECT_WIFI);
                WifiAddData wifiData = m_vecWidgetPair[iIdx].appendVariantData.value<WifiAddData>();
                m_iPreWifiID = -1;
                _UIWifiManager.CancelWifiConnect("CC", wifiData.iNetID);
            }
        }
    }
    else if (WIFI_SCAN == strAction)
    {
        if (OR_INTERFACE_FAIL == _UIWifiManager.IsScanning())
        {
            m_pPicAnimation->show();
            m_pPicAnimation->Play();

            _UIWifiManager.ScanWifiDevices("CC");
            emit RefreshSoftkey();

            if (NULL != m_pWifiSwitch)
            {
                DoRelayout(m_pWifiSwitch->GetData());
            }
        }
    }
    else if (CONNECT_WIFI == strAction)
    {
        // 更新Item状态
        if (-1 == m_iPreWifiID || m_mpIDIndex[m_iPreWifiID] != m_pGeneralFrame->GetFocusItemDataIndex()
                || !m_bConnecting)
        {
            if (-1 != m_iPreWifiID && m_mpIDIndex[m_iPreWifiID] < m_vecWidgetPair.size())
            {
                // 消除之前连接的提示
                SETTINGUI_INFO("connect other idx[%d], str[%s], INT_NONE"
                               , m_mpIDIndex[m_iPreWifiID], "");
                SetWifiItemSavedDataByDataIndex(m_mpIDIndex[m_iPreWifiID], "", INT_NONE, SETTING_ITEM_WIFI,
                                                CONNECT_WIFI);
                WifiAddData wifiData =
                    m_vecWidgetPair[m_mpIDIndex[m_iPreWifiID]].appendVariantData.value<WifiAddData>();
                _UIWifiManager.CancelWifiConnect("CC", wifiData.iNetID);
            }

            // 更新连接提示
            m_iPreWifiID = -1;
            CListItemPtr pNowItem = m_pGeneralFrame->GetFocusItem();
            if (NULL != pNowItem && pNowItem->inherits("CSettingWifiItem"))
            {
                CSettingWifiItem * pWifiItem = static_cast<CSettingWifiItem *>(pNowItem);
                if (NULL != pWifiItem)
                {
                    m_iPreWifiID = pWifiItem->GetNetID();
                    std::string strNote = TRID_CONNECTING;
                    pWifiItem->SetNoteDisplayAndType(LANG_TRANSLATE(strNote.c_str()), INT_ANIMATION);
                    m_bConnecting = true;
                    emit RefreshSoftkey();
                    if (OR_INTERFACE_SUCCESS == _UIWifiManager.ConnectWifi("CC", pWifiItem->GetNetID()))
                    {
                        SETTINGUI_INFO("connecting idx[%d], str[%s], INT_NONE"
                                       , m_mpIDIndex[m_iPreWifiID], "");
                        SetWifiItemSavedDataByDataIndex(m_mpIDIndex[m_iPreWifiID],
                                                        LANG_TRANSLATE(TRID_CONNECTING).toUtf8().data()
                                                        , INT_ANIMATION, SETTING_ITEM_WIFI, CONNECT_WIFI);
                    }
                    else
                    {
                        SetWifiItemSavedDataByDataIndex(m_mpIDIndex[m_iPreWifiID],
                                                        LANG_TRANSLATE(TRID_CONNECT_FAILED).toUtf8().data()
                                                        , INT_DISAPPEAR, SETTING_ITEM_WIFI, CONNECT_WIFI);
                    }
                }
            } // end if (NULL != pNowItem && pNowItem->inherits("CSettingWifiItem"))
        }

    }
    else if (DISCONNECT_WIFI == strAction)
    {
        // 更新Item状态
        CListItemPtr pNowItem = m_pGeneralFrame->GetFocusItem();
        if (NULL != pNowItem && pNowItem->inherits("CSettingWifiItem"))
        {
            CSettingWifiItem * pWifiItem = static_cast<CSettingWifiItem *>(pNowItem);
            if (NULL != pWifiItem
                    && OR_INTERFACE_SUCCESS == _UIWifiManager.DisconnectWifi("CC", pWifiItem->GetNetID())) //连接ID
            {
                std::string strNote = TRID_DISCONNECTING;
                pWifiItem->SetNoteDisplayAndType(LANG_TRANSLATE(strNote.c_str()), INT_ANIMATION);

                emit RefreshSoftkey();
            }
        }
    }
    else if (WIFI_SOFTKEY_WPS_PIN == strAction && !GetOpenByTestMode())
    {
        std::string strWPSPin;
        bool bPin = OR_INTERFACE_SUCCESS == _UIWifiManager.GetWifiWPSPIN(strWPSPin);
        if (bPin)
        {
            QString strNote = QString("%1%2")
                              .arg(LANG_TRANSLATE(
                                       TRID_PLEASE_INPUT_BELOW_PIN_CODE_ON_YOU_WIFI_ROUTER_AND_PRESS_OK_TO_START_CONNECTING))
                              .arg(strWPSPin.c_str());

            _UIWifiManager.Wifi_ShowCommonMsgBox(this, strNote, MESSAGEBOX_OkCancel, -1, "", "",
                                                 PopupBoxID_WPSPIN);
        }
    }
    else if (WIFI_SOFTKEY_WPS == strAction && !GetOpenByTestMode())
    {
        _UIWifiManager.StartWifiWPS();
        QString str = QString("%1").arg(TRID_WPS_HAS_OPENED_PLEASE_CONNECT_IN);
        _UIWifiManager.Wifi_ShowCommonMsgBox(this, str, MESSAGEBOX_Cancel, DEFAULT_WPS_WPSPIN_TIME
                                             , "", "", PopupBoxID_CancelWPS_WPSPIN, QVariant(), true);
        _MainWND.SetTimerLock(true);
    }
    else
    {
        return false;
    }

    return true;
}

void CDlgWifiBase::OnItemDetailAction(CListItem * pItem)
{
    if (!GetOpenByTestMode())
    {
#ifdef _T49
#ifdef QT_SLIP_GESTURE_SUPPORT
        m_pWifiDelegate->RecordOffSet();
#endif
#else
        m_pWifiDelegate->RecordOffSet();
#endif
        CreatePageByString(ENTER_WIFI_DETAIL_PAGE, ENUM_WIFI_DETAIL_PAGE_TYPE, pItem);
    }
}

void CDlgWifiBase::OnReShow()
{
    SETTINGUI_INFO("[CDlgWifiBase::OnReShow][bNeedUpd(%d)][m_bOnScan(%d)][scan(%d)]"
                   , _UIWifiManager.IsNeedUpdate(), m_bOnScan
                   , _UIWifiManager.IsScanning()
                  );
#ifdef _T49
    if (NULL != m_pTimer && !m_pTimer->isActive())
    {
#ifdef FRESH_WINDOW_WITH_MIN_CHANGED
        m_pTimer->start(TIME_TO_FRESH_PAGE_DATA);
#endif
    }
#endif

#ifndef TEMP_SLIP_SOLUTION
    if (NULL != m_pWifiSwitch && _UIWifiManager.IsNeedUpdate())
#else
    if (_UIWifiManager.IsNeedUpdate())
#endif
    {
        _UIWifiManager.SetNeedUpdate(false);
        SETTINGUI_INFO("[SettingUI][CDlgWifiBase::OnReShow()][enter]");
#ifdef TEST_WIFI
        DoRelayout(true);
#else
#ifndef TEMP_SLIP_SOLUTION
        if (!CheckSwitchChange())
        {
            DoRelayout(m_pWifiSwitch->GetData());
        }
#else
        DoRelayout(true);
#endif
#endif
    }

    CDlgBaseSubPage::OnReShow();
    m_bFirstCreate = false;
    SETTINGUI_INFO("Base reshow end");
}

/****************************************************************************************************/
void CDlgWifiBase::OnSwitchDeviceChange(bool bStatus)
{
#ifndef TEMP_SLIP_SOLUTION
    if (NULL == m_pWifiSwitch)
    {
        return;
    }
#endif
    if (bStatus && OR_INTERFACE_FAIL == _UIWifiManager.IsDongleValid())
    {
        m_pWifiSwitch->SetData(false);
        DoRelayout(false);
        _UIWifiManager.Wifi_ShowCommonMsgBox(NULL
                                             , LANG_TRANSLATE(TRID_NO_FIND_WIFI_ADAPTER)
                                             , MESSAGEBOX_Cancel, SHORT_TIME);
        return;
    }

    if (bStatus && OR_INTERFACE_FAIL == _UIWifiManager.IsWifiEnable())
    {
        SETTINGUI_INFO("[CDlgWifiBase::OnSwitchDeviceChange()][enter set m_bOnScan]");
        m_bOnScan = true;
    }
    m_pPicAnimation->show();
    m_pPicAnimation->Play();

    if (!bStatus)
    {
        m_mpIDIndex.clear();
    }

    DoRelayout(bStatus);
    Operatoer_Reslut eResult = _UIWifiManager.SetWifiEnable(bStatus);

    if (OR_INTERFACE_SUCCESS == eResult)
    {
        m_pWifiSwitch->setEnabled(false);
    }
    else
    {
        m_pWifiSwitch->setEnabled(true);
    }
}

void CDlgWifiBase::SetWifiOpenClose(bool bOpen /*= false*/)
{
    SETTINGUI_INFO("[wifiBase::SetOpenClose][%d] ", bOpen);
#ifndef TEMP_SLIP_SOLUTION
    if (NULL != m_pWifiSwitch && !m_pWifiSwitch->GetData())
    {
        m_pWifiSwitch->SetData(bOpen);
    }
    else
    {
        OnAction(WIFI_SCAN);
    }
#endif
}

// set wifi item ui saved data
void CDlgWifiBase::SetWifiItemSavedDataByDataIndex(int iDataIdx, std::string strNote,
        Item_Note_Type eType
        , int eItemType /*= SETTING_ITEM_WIFI*/, const QString & strAction /*= ""*/
        , bool bConnected /*= false*/)
{
    if (iDataIdx < 0 || iDataIdx >= m_vecWidgetPair.size())
    {
        return;
    }

    SETTINGUI_INFO("[CDlgWifiBase::SetWifiItemSavedDataByDataIndex]idx[%d], note[%s], noteT[%d], itemT[%d], action[%s], connet[%d"
                   , iDataIdx, strNote.c_str(), eType, eItemType, strAction.toUtf8().data(), bConnected);

    if (SETTING_ITEM_WIFI == eItemType)
    {
        WifiAddData wifiData = m_vecWidgetPair[iDataIdx].appendVariantData.value<WifiAddData>();
        wifiData.strNote = strNote;
        wifiData.eType = eType;
        wifiData.bConnectDevice = bConnected;
        m_vecWidgetPair[iDataIdx].appendVariantData.setValue(wifiData);
    }
    else if (WIRELESS_ITEM_SEARCH == eItemType)
    {
        WirelessSearchStatisticsData searchData =
            m_vecWidgetPair[iDataIdx].appendVariantData.value<WirelessSearchStatisticsData>();
        searchData.s_strInfo = toQString(strNote.c_str());
        searchData.s_eType = eType;
        m_vecWidgetPair[iDataIdx].appendVariantData.setValue(searchData);
    }
    m_vecWidgetPair[iDataIdx].third = strAction;

    m_pWifiDelegate->SetItemDataByDataIndex(m_vecWidgetPair[iDataIdx], iDataIdx, eItemType);
}

bool CDlgWifiBase::CheckSwitchChange()
{
#ifdef IF_SUPPORT_WIFI
    bool bStatus = _UIWifiManager.IsWifiEnable() == OR_INTERFACE_SUCCESS;
    if (bStatus != m_pWifiSwitch->GetData())
    {
        m_pWifiSwitch->SetData(bStatus);
    }
    return false;
#else
    return true;
#endif
}

// 重新加载
void CDlgWifiBase::DoRelayout(bool bSwitchStatus /*= false*/)
{
    // 初始化
    m_iPreWifiID = -1;
    m_bConnecting = false;

#ifdef TEST_WIFI
    MakeReadyForRelayout();

    CreateItemByType(TRID_WIRELESS_STATUS, NULL, WIFI_STATUS, SETTING_ITEM_MENU);
    CreateItemByType(TRID_WIFI, m_pWifiSwitch, "", SETTING_ITEM_NORMAL);

    if (bSwitchStatus)
    {
        CreateItemByType(TRID_THE_STORAGE_NETWORK, NULL, WIFI_STORAGE, SETTING_ITEM_MENU);

        AddWifiItem();
    }

    AddWidget(m_vecWidgetPair);
    UpdateWnd();
#else

    MakeReadyForRelayout();

    if (!GetOpenByTestMode())
    {
        QString strStatusTitle = LANG_TRANSLATE(TRID_WIRELESS_STATUS);

        WiFiNetInfo stConnectWifiInfo;
        if (bSwitchStatus && WIFI_IsEnable() && WIFI_IsConnected()
                && WIFI_GetCurrentNetConnectInfo(stConnectWifiInfo))
        {
            strStatusTitle = QString("%1%2 %3 %4").arg(LANG_TRANSLATE(TRID_WIRELESS_STATUS)).arg(
                                 QLatin1String(":"))
                             .arg(toQString(stConnectWifiInfo.strProfileName.empty() ? stConnectWifiInfo.strSSID :
                                            stConnectWifiInfo.strProfileName))
                             .arg(LANG_TRANSLATE(TRID_CONNECTED));
        }

        CreateItemByType(strStatusTitle.toUtf8().data(), NULL, WIFI_STATUS, SETTING_ITEM_MENU);
    }
#ifndef TEMP_SLIP_SOLUTION
    CreateItemByType(TRID_WIFI, m_pWifiSwitch, "", SETTING_ITEM_NORMAL);
#else
    CreateItemByType(TRID_WIFI, m_pWifiSwitch, bSwitchStatus ? "true" : "false", SETTING_ITEM_NORMAL);
#endif

    m_iPreWifiID = -1;

    if (bSwitchStatus)
    {
        if (!GetOpenByTestMode())
        {
            CreateItemByType(TRID_THE_STORAGE_NETWORK, NULL, WIFI_STORAGE, SETTING_ITEM_MENU);
        }

        SETTINGUI_INFO("[CDlgWifiBase::DoRelayout]isScanning[%d], m_bOnScan[%d]"
                       , _UIWifiManager.IsScanning(), m_bOnScan);

        if (OR_INTERFACE_SUCCESS == _UIWifiManager.IsScanning() || m_bOnScan)
        {
            QWidgetPair qPair;

            qPair.first = "";
            qPair.second = NULL;
            qPair.third = "";
            qPair.eType = WIRELESS_ITEM_SEARCH;
            qPair.bChangeStyleOnPress = false;

            WirelessSearchStatisticsData itemData;
            itemData.s_eType = INT_ANIMATION;
            itemData.s_strInfo = LANG_TRANSLATE(TRID_SCANNING_AVAILABLE_NETWORK);
            qPair.appendVariantData.setValue(itemData);
            m_vecWidgetPair.push_back(qPair);
        }
        else
        {
            m_pPicAnimation->Stop();
            m_pPicAnimation->hide();

            // 获取扫描到的网络
            AddWifiItem();
        }
    }
    else
    {
        m_pPicAnimation->Stop();
        m_pPicAnimation->hide();
    }

    if (NULL != m_pGeneralFrame)
    {
        SETTINGUI_INFO("[CDlgWifiBase::DoRelayout()][tot size[%d]]", m_vecWidgetPair.size());
        m_pGeneralFrame->SetTotalDataCount(m_vecWidgetPair.size());
    }
#ifdef QT_SLIP_GESTURE_SUPPORT
    int iHeight = 84;
    if (GetOpenByTestMode())
    {
        iHeight += (m_vecWidgetPair.size() > 1) ? 86 + (m_vecWidgetPair.size() - 2) * 98 : 0;
        iHeight += (m_vecWidgetPair.size() - 1);
        m_pWifiDelegate->SetContentHeight(iHeight);
    }
    else
    {
        iHeight += 84;
        iHeight += (m_vecWidgetPair.size() > 2) ? 84 + 86 + (m_vecWidgetPair.size() - 4) * 98 : 0;
        iHeight += (m_vecWidgetPair.size() - 1);
        m_pWifiDelegate->SetContentHeight(iHeight);
    }
    if (593 - m_pWifiDelegate->GetRecordOffSet() > iHeight)
    {
        int iOffSet = (iHeight - 593) < 0 ? 0 : (593 - iHeight);
        SETTINGUI_INFO("----------------- enter offset[%d] ========", iOffSet);
        m_pWifiDelegate->SetRecordOffSet(iOffSet);
    }
#endif

#ifdef WIFI_BASE_OPTIMIZATION_ONE
    m_pWifiDelegate->SetItemData(m_vecWidgetPair);
#else
    AddWidget(m_vecWidgetPair, true, false, true);
#endif

    if (!m_bFirstCreate)
    {
        UpdateWnd();
    }

#endif
}

void CDlgWifiBase::CreateItemByType(std::string strDis, QWidget * pWgt, std::string strAction,
                                    SETTING_ITEM_TYPE eType)
{
    QWidgetPair qPair;

    qPair.first = LANG_TRANSLATE(strDis.c_str());
    qPair.second = pWgt;
    qPair.third = strAction.c_str();
    qPair.eType = eType;
    qPair.bAddIndex = false;
    qPair.bChangeStyleOnPress = true;
    qPair.bChangeStyleOnSelect = false;

    if (SETTING_ITEM_WIFI == eType)
    {
        WifiAddData wifiData;
        qPair.appendVariantData.setValue(wifiData);
    }
#ifdef TEMP_SLIP_SOLUTION
    else if (SETTING_ITEM_NORMAL == eType)
    {
        qPair.second = NULL;
        qPair.bAddMark = false;
        if (0 == qPair.third.compare("true"))
        {
            qPair.bAddMark = true;
        }
    }
#endif
    else if (SETTING_ITEM_NORMAL == eType)
    {
        qPair.bChangeStyleOnPress = false;
    }
    else if (SETTING_ITEM_MENU == eType)
    {
        qPair.bAddMark = false;
    }

    m_vecWidgetPair.push_back(qPair);
}

void CDlgWifiBase::AddItem(std::string strDis, QWidget * pWgt, std::string strAction,
                           SETTING_ITEM_TYPE eType)
{
    QWidgetPair qPair;

    qPair.first = LANG_TRANSLATE(strDis.c_str());
    qPair.second = pWgt;
    qPair.third = strAction.c_str();
    qPair.eType = eType;
    qPair.bAddIndex = false;
    m_vecWidgetPair.push_back(qPair);
}
#ifdef IF_SUPPORT_WIFI
void CDlgWifiBase::AddWifiItem(std::string strAction, const WiFiNetInfo & wifiScanNet,
                               std::string strNote, int iIdx)
{
    QWidgetPair qPair;

    qPair.first = "";
    qPair.second = NULL;
    qPair.third = strAction.c_str();
    qPair.eType = SETTING_ITEM_WIFI;
    qPair.bAddIndex = false;

    WifiAddData wifiData;
    wifiData.iLeftIcon = wifiScanNet.iSignal;
    wifiData.strDisName = wifiScanNet.strProfileName.empty()
                          ? toQString(wifiScanNet.strSSID)
                          : toQString(wifiScanNet.strProfileName);

    wifiData.iSecurity = wifiScanNet.iSercureMode;
    wifiData.iNetID = wifiScanNet.iNetID;
    wifiData.bConnectDevice = WIRE_DEVICE_CONNECTED_STATE == wifiScanNet.eNetState;
    wifiData.strNote = LANG_TRANSLATE(strNote.c_str()).toUtf8().data();
    if (0 == strNote.compare(TRID_CONNECTING))
    {
        wifiData.eType = INT_ANIMATION;
    }
    else
    {
        wifiData.eType = INT_FOREVER;
    }

    qPair.appendVariantData.setValue(wifiData);

    m_vecWidgetPair.push_back(qPair);

}
#endif
void CDlgWifiBase::AddWifiItem()
{
#ifdef TEST_T48_WIFI
    int iTotNum = 17;
    std::string str = iTotNum +
                      " available networks:-------===========dafadfaf7777777afasdfadfadfadfdfadf888888888afadsafd";
    //AddItem(str, NULL, "", SETTING_ITEM_NORMAL);
    QWidgetPair qPair;

    qPair.first = "";
    qPair.second = NULL;
    qPair.third = "";
    qPair.eType = WIRELESS_ITEM_SEARCH;

    WirelessSearchStatisticsData itemData;
    itemData.s_eType = INT_FOREVER;
    itemData.s_strInfo = str.c_str();
    qPair.appendVariantData.setValue(itemData);
    m_vecWidgetPair.push_back(qPair);

    static int ii = 0;
    for (int i = 0; i < iTotNum; ++i)
    {
        // 添加扫描到的网络
        QWidgetPair qPair;

        qPair.first = "";
        qPair.second = NULL;
        qPair.third = CONNECT_WIFI;
        qPair.eType = SETTING_ITEM_WIFI;
        qPair.bAddIndex = false;
        //
        WifiAddData wifiData("Testg", i % 2 ? 2 : 1, ""
                             , i % 2, i % 3);
        wifiData.bAutoPair = false;

        wifiData.strNote = "NotegTest";
        wifiData.eType = (Item_Note_Type)(ii % 3 + 1);

        qPair.appendVariantData.setValue(wifiData);
        m_vecWidgetPair.push_back(qPair);
    }
#endif // #ifdef TEST_T48_WIFI

#ifdef IF_SUPPORT_WIFI
    UI_WIFI_NETINFO_LIST wifiScanList;
    WIFI_GetScanNetworkList(wifiScanList);

    SETTINGUI_INFO("[WIFI UI] Get Wifi Item Count[%d] By WIFI_GetScanNetworkList().",
                   wifiScanList.size());

    QWidgetPair qPair;

    qPair.first = "";
    qPair.second = NULL;
    qPair.third = "";
    qPair.eType = WIRELESS_ITEM_SEARCH;

    QString strAvailableNetWork = QString("%1 %2:").arg(wifiScanList.size()).arg(LANG_TRANSLATE(
                                      TRID_AVAILABLE_NETWORK));

    WirelessSearchStatisticsData itemData;
    itemData.s_eType = INT_FOREVER;
    itemData.s_strInfo = strAvailableNetWork;
    qPair.appendVariantData.setValue(itemData);

    m_vecWidgetPair.push_back(qPair);

    int iIdx = GetOpenByTestMode() ? WIFI_ITEM_BEGIN_INDEX_BY_TESTMODE :
               WIFI_ITEM_BEGIN_INDEX_BY_MORDEL;

    // Add connect wifi device
    WiFiNetInfo stConnectWifiInfo;
    if (WIFI_IsConnected() && WIFI_GetCurrentNetConnectInfo(stConnectWifiInfo))
    {
        SETTINGUI_INFO("[CDlgWifiBase::AddWifiItem()][[%s] connected]",
                       toQString(stConnectWifiInfo.strSSID).toUtf8().data());

        AddWifiItem(DISCONNECT_WIFI, stConnectWifiInfo, TRID_CONNECT_SUCCESS, iIdx);

        // 记录Wifi对应 ItemDataIndex
        m_mpIDIndex[stConnectWifiInfo.iNetID] = iIdx++;
    }

    WiFiNetInfoItem it;
    WiFiNetInfoItem itEnd = wifiScanList.end();

    // Add other wifi devices
    for (it = wifiScanList.begin(); it != itEnd; ++it)
    {
        WiFiNetInfo & stWifiNetInfo = (*it);

        if (WIRE_DEVICE_CONNECTED_STATE == stWifiNetInfo.eNetState)
        {
            continue;
        }

        if (OR_INTERFACE_SUCCESS == _UIWifiManager.IsConnectingWifiDevice(stWifiNetInfo.iNetID))
        {
            SETTINGUI_INFO("Current connecting wifi device ID[%d]", stWifiNetInfo.iNetID);
            AddWifiItem(CONNECT_WIFI, stWifiNetInfo, TRID_CONNECTING, iIdx);

            // 记录正在连接的ID
            m_iPreWifiID = stWifiNetInfo.iNetID;
            m_bConnecting = true;
        }
        else
        {
            AddWifiItem(CONNECT_WIFI, stWifiNetInfo, "", iIdx);
        }

        // 记录Wifi对应 ItemDataIndex
        m_mpIDIndex[stWifiNetInfo.iNetID] = iIdx++;
    }
#endif //IF_SUPPORT_WIFI
}

// 更新连接结果提示语
void CDlgWifiBase::SetConnectResultNote(int iResult, int iCurID /*= -1*/)
{
    SETTINGUI_INFO("[CDlgWifiBase::SetConnectResultNote(result[%d], curID[%d])]", iResult, iCurID);
    if (-1 == iCurID || m_mpIDIndex[iCurID] >= m_vecWidgetPair.size())
    {
        return;
    }
    // 更新Softkey
    m_bConnecting = false;
    emit RefreshSoftkey();
#ifdef IF_SUPPORT_WIFI
    // 设置新Item提示
    if (WIRELESS_OP_CONNECT_FAILED == iResult)
    {
        SETTINGUI_INFO("connect failed idx[%d], str[%s], INT_NONE"
                       , m_mpIDIndex[iCurID], "");
        m_iPreWifiID = -1;
        SetWifiItemSavedDataByDataIndex(m_mpIDIndex[iCurID],
                                        LANG_TRANSLATE(TRID_CONNECT_FAILED).toUtf8().data()
                                        , INT_DISAPPEAR, SETTING_ITEM_WIFI, CONNECT_WIFI);
    }
    else if (WIRELESS_OP_CONNECT_SUCCESS == iResult)
    {
        SetWifiItemSavedDataByDataIndex(m_mpIDIndex[iCurID],
                                        LANG_TRANSLATE(TRID_CONNECT_SUCCESS).toUtf8().data()
                                        , INT_FOREVER, SETTING_ITEM_WIFI, DISCONNECT_WIFI, true);
    }
#endif
}

void CDlgWifiBase::SetDisConnectResultNote(int iResult, int iCurID /*= -1*/)
{
    SETTINGUI_INFO("[CDlgWifiBase::SetDisConnectResultNote(result[%d], iCurID[%d])]"
                   , iResult, iCurID);

    if (-1 != iCurID && m_mpIDIndex[iCurID] < m_vecWidgetPair.size())
    {
        // 消除之前连接的提示
#ifdef IF_SUPPORT_WIFI
        UI_WIFI_NETINFO_LIST wifiScanList;
        WIFI_GetScanNetworkList(wifiScanList);
        QString str = QString("%1 %2").arg(wifiScanList.size()).arg(LANG_TRANSLATE(TRID_AVAILABLE_NETWORK));
        int iIdx = GetOpenByTestMode() ? 1 : 3;
        SetWifiItemSavedDataByDataIndex(iIdx, str.toUtf8().data(), INT_FOREVER, WIRELESS_ITEM_SEARCH);
        SETTINGUI_INFO("111 idx[%d], str[%s], INT_FOREVER"
                       , iIdx, str.toUtf8().data());
#endif

        SETTINGUI_INFO("Disconnect result idx[%d], str[%s], INT_DISAPPEAR"
                       , m_mpIDIndex[iCurID], "");
        SetWifiItemSavedDataByDataIndex(m_mpIDIndex[iCurID],
                                        LANG_TRANSLATE(TRID_DISCONNECT_SUCCESS).toUtf8().data()
                                        , INT_DISAPPEAR, SETTING_ITEM_WIFI, CONNECT_WIFI);
    }
}

void CDlgWifiBase::PopPwdDialog(bool bAuthentication /*= false*/)
{
    if (WIFI_SECTION_ID_INVAILED == m_iPopWifiID || m_mpIDIndex[m_iPopWifiID] >= m_vecWidgetPair.size())
    {
        return;
    }

    SubMenuData objSubMenu;
    objSubMenu.strMenuAction = OPEN_PASSWORD_ACTION;

    int nPwdType = CDlgAskPassword::ASK_PASSWORD_WIFI_COMMON;

    if (bAuthentication)
    {
        nPwdType = CDlgAskPassword::ASK_PASSWORD_WIFI_AUTHENTICATION;
    }

    objSubMenu.pBeforeSetSubpageData = (void *) & (nPwdType);

    emit openpage(objSubMenu);

    CBaseDialog * pDlgPswdBase = UIManager_GetPrivateDlg(DLG_CDlgAskPassword);

    if (pDlgPswdBase != NULL)
    {
        if (bAuthentication)
        {
            connect(pDlgPswdBase, SIGNAL(submitNameAndPassword(const yl::string &, const yl::string &)),
                    this, SLOT(OnSubmitNameAndPassword(const yl::string &, const yl::string &)));
        }
        else
        {
            connect(pDlgPswdBase, SIGNAL(SubmitPassword(const yl::string &)), this,
                    SLOT(OnSubmitPassword(const yl::string &)));
        }

        connect(pDlgPswdBase, SIGNAL(CancelInputPassword()), this, SLOT(OnCancelPassword()));
    }
}

void CDlgWifiBase::OnSubmitNameAndPassword(const yl::string & strName, const yl::string & strPswd)
{
    if (m_iPopWifiID >= 0)
    {
        _UIWifiManager.SetOperatering(false);
        _UIWifiManager.AuthNetWork(m_iPopWifiID, strName, strPswd);
    }
}

void CDlgWifiBase::OnSubmitPassword(const yl::string & strPswd)
{
    if (m_iPopWifiID >= 0)
    {
        _UIWifiManager.SetOperatering(false);
        _UIWifiManager.AskWifiPassword(m_iPopWifiID, strPswd.c_str());
    }
}

void CDlgWifiBase::OnCancelPassword()
{
    if (m_iPopWifiID >= 0)
    {
        _UIWifiManager.SetOperatering(false);

        SetWifiItemSavedDataByDataIndex(m_mpIDIndex[m_iPopWifiID], "", INT_NONE, SETTING_ITEM_WIFI,
                                        CONNECT_WIFI);
        _UIWifiManager.CancelWifiConnect("", m_iPopWifiID);

        m_iPopWifiID = WIFI_SECTION_ID_INVAILED;
        m_iPreWifiID = WIFI_SECTION_ID_INVAILED;

        emit RefreshSoftkey();
    }
}

void CDlgWifiBase::CreatePageByString(const char * strPageAction, ENUM_WIFI_PAGE_TYPE eType,
                                      CListItem * pItem /*= NULL*/)
{
#ifdef _T49
    if (NULL != m_pTimer)
    {
        if (m_pTimer->isActive())
        {
            m_pTimer->stop();
        }
    }
#endif

    _UIWifiManager.SetCurID(-1);
    if (ENUM_WIFI_ADD_PAGE_TYPE != eType
            && NULL != pItem && pItem->inherits("CSettingWifiItem"))
    {
        CSettingWifiItem * pWifiItem = static_cast<CSettingWifiItem *>(pItem);
        if (NULL != pWifiItem)
        {
            _UIWifiManager.SetCurID(pWifiItem->GetNetID());
        }
    }
    _UIWifiManager.SetPageType(eType);

    SettingUI_OpenPage(SubMenuData(strPageAction));
}

// 处理弹出框回调
void CDlgWifiBase::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    SETTINGUI_INFO("[CDlgWifiBase::ProcessMsgBoxCallBack()]");
    if (NULL == pMessageBox)
    {
        return;
    }
    SETTINGUI_INFO("Result[%d], ID[%d]", pMessageBox->GetResult(), pMessageBox->GetID());
    SETTINGUI_INFO("ID");
    SETTINGUI_INFO("PopupBoxID_CancelScan		= %d", PopupBoxID_WPSPIN);
    SETTINGUI_INFO("PopupBoxID_DelItem			= %d", PopupBoxID_RemainTime);
    SETTINGUI_INFO("PopupBoxID_CancelWPS_WPSPIN	= %d", PopupBoxID_CancelWPS_WPSPIN);
    SETTINGUI_INFO("\nresult");
    SETTINGUI_INFO("MessageBox_Cancel	= %d", MessageBox_Cancel);
    SETTINGUI_INFO("MessageBox_OK		= %d", MessageBox_OK);
    SETTINGUI_INFO("MessageBox_Destory	= %d", MessageBox_Destory);
    SETTINGUI_INFO("MessageBox_ERROR	= %d", MessageBox_ERROR);

    int nResult = pMessageBox->GetResult();
    switch (pMessageBox->GetID())
    {
    case PopupBoxID_WPSPIN:
    {
        if (MessageBox_OK == nResult)
        {
            _UIWifiManager.StartWifiWPSPIN();
            QString str = QString("%1").arg(TRID_WPS_HAS_OPENED_PLEASE_CONNECT_IN);
            _UIWifiManager.Wifi_ShowCommonMsgBox(this, str, MESSAGEBOX_Cancel, DEFAULT_WPS_WPSPIN_TIME
                                                 , "", "", PopupBoxID_CancelWPS_WPSPIN, QVariant(), true);
            _MainWND.SetTimerLock(true);
        }
    }
    break;
    case PopupBoxID_CancelWPS_WPSPIN:
    {
        if (MessageBox_Cancel == nResult || MessageBox_Destory == nResult)
        {
            SETTINGUI_INFO("[CDlgWifiBase][cancel WPS-PIN or WPS]");
            _UIWifiManager.StopWifiWPS_WPSPIN();
        }
        _MainWND.SetTimerLock(false);
    }
    break;
    default:
        break;
    }
}

#ifdef _T49
void CDlgWifiBase::OnTimeOut()
{
    SETTINGUI_INFO("refresh CDlgWifiBase page data");
    m_pTimer->stop();
    // need
//  if (NULL != m_pWifiSwitch && !_UIWifiManager.IsOperatering())
//  {
// #ifdef QT_SLIP_GESTURE_SUPPORT
//      m_pWifiDelegate->RecordOffSet();
// #endif
//      DoRelayout(m_pWifiSwitch->GetData());
//  }
//  m_pTimer->start(TIME_TO_FRESH_PAGE_DATA);
}
#endif

/******************************* 测试模式用 **********************************/
void CDlgWifiBase::SetOpenByTestMode(bool bTestModeOpen)
{
#ifdef _T49
    m_bTestModeOpen = bTestModeOpen;
    m_pWifiDelegate->SetTestMode(bTestModeOpen);
#endif
}
