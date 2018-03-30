#include "wifi_inc.h"
#include "settingui/src/cdlgaskpassword.h"


namespace
{
#define ENTER_WIFI_ADD_EDIT_PAGE        "OpenSubPage(wifiaddedit)"

const QRect RECT_TITLE_BTN_CON(345, 17, 29, 56);
const QRect RECT_TITLE_BTN_ADD(426, 17, 29, 56);

const QRect RECT_FRAME_LIST(132, 0, 536, 336);
// Note位置
const QRect RECT_NOTE_LOCATION(213, 280, 190, 45);

#define MAX_ITEM_COUNT                  5
#define TITLE_BTN_COUNT                 2
// signal strong
// #define MIN_SIGNAL                      -85
// #define LOW_SIGNAL                      -75
// #define MID_SIGNAL                      -65
// #define HIGH_SIGNAL                     -55
// security and encryption
const char * strSecure[9] = { "NONE", "WEP", "WPA", "WPA2", "WPA_PSK", "WPA2_PSK", "WPA_EAP", "WPA2_EAP", "WPS", };
const char * strEncryp[6] = { "NONE", "WEP", "WEP", "TKIP", "AES", "TKIP+AES", };
// signal level
const char * strSignalLevel[5] = { "Weak", "Low", "Middle", "High", "Strong" };
// framelist title
#define FRAMELIST_TITLE_DISCONNECT "Disconnect"
#define FRAMELIST_TITLE_ADD "Add"

const QRect RECT_WIFI(0, DEFAULT_DLG_TOP, 1280, 647);

#define OPEN_PASSWORD_ACTION    "OpenSubPage(enter_password)"
}

// 处理消息.
static LRESULT OnWifiDetailUIMessage(msgObject & refMessage)
{
#ifdef IF_SUPPORT_WIFI
    SETTINGUI_INFO("[SettingUI][OnWifiDetailUIMessage()] message[%d], lParam[%d], wParam[%d], UpUI[%d]!!"
                   , refMessage.message, refMessage.lParam, refMessage.wParam, WIFI_MSG_UPDATE_UI);

    CDlgWifiDetail * pWifiDetail = qobject_cast<CDlgWifiDetail *>(UIManager_GetPrivateDlg(
                                       DLG_CDlgWifiDetail));
    if (NULL == pWifiDetail || !pWifiDetail->IsTopPage())
    {
        SETTINGUI_INFO("CDlgWifiDetail not top page");
        WirelessOperationMsg * wifiMsg = (WirelessOperationMsg *)refMessage.GetExtraData();
        if (NULL != pWifiDetail && NULL != wifiMsg
                && OP_WIFI_CLOSE_RESULT == wifiMsg->iOperationType)
        {
            pWifiDetail->Load();
        }
        return TRUE;
    }

    if (WIFI_MSG_UPDATE_UI != refMessage.message)
    {
        return TRUE;
    }

    WirelessOperationMsg * wifiMsg = (WirelessOperationMsg *)refMessage.GetExtraData();
    if (NULL != wifiMsg)
    {
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
        case OP_CONNECT_WIFI_RESULT:
            {
                _UIWifiManager.SetNeedUpdate(true);

                //连接结果  lparam :0/1/2 success/wrong pwd/failed
                if (wifiMsg->lSessionID == pWifiDetail->GetID())
                {
                    if (WIRELESS_OP_CONNECT_SUCCESS == wifiMsg->iOperationResult)
                    {
                        pWifiDetail->Load();
                    }
                    pWifiDetail->SetConnectResult(wifiMsg->iOperationResult);
                }
            }
            break;
        case OP_ENTER_WIFI_PWD:
            {
                if (wifiMsg->lSessionID == pWifiDetail->GetID())
                {
                    bool bAuthentication = (refMessage.lParam == 1);

                    pWifiDetail->PopPwdDialog(bAuthentication);
                }
            }
            break;
        case OP_DISCONNECT_WIFI_RESULT:
            {
                _UIWifiManager.SetNeedUpdate(true);

                //断开连接结果lparam:0/1 failed/success
                if (wifiMsg->lSessionID == pWifiDetail->GetID())
                {
                    if (WIRELESS_OP_DISCONNECT_SUCCESS == wifiMsg->iOperationResult)
                    {
                        pWifiDetail->Load();
                    }
                    pWifiDetail->SetDisConnectResult(wifiMsg->iOperationResult);
                }
                // not deal with other ID, the same to Storage
            }
            break;
        default:
            break;
        }
    }
#endif
    return TRUE;
}

CDlgWifiDetail::CDlgWifiDetail(QWidget * parent /*= NULL*/)
    : CDlgBaseSubPage(parent)
    , m_pLabEncryptionType(NULL)
    , m_pLabSecurityMode(NULL)
    , m_pLabSSID(NULL)
    , m_pLabFrequence(NULL)
    , m_bConnectOrNot(false)
    , m_iCurNetID(-1)
    , m_pLabProfileName(NULL)
    , m_pLabAPMacAddress(NULL)
#ifdef _T49
    , m_iSignalLevel(-1)
    , m_pTimer(NULL)
#else
    , m_pLabNote(NULL)
#endif
{
    SetWindowType(eWT_SubPage);

    InitData();

    //设置标题
    SetWindowTitle(LANG_TRANSLATE(TRID_NETWORK_DETAILS));

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
#ifdef IF_SUPPORT_WIFI
    etl_RegisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, OnWifiDetailUIMessage);
#endif

    // reset framelist's location
    QRect rc = m_pGeneralFrame->geometry();
    m_pGeneralFrame->setGeometry(rc.x() + 12, rc.y(), rc.width() - 24, RECT_FRAME_LIST.height());
    m_pGeneralFrame->SetMaxPageItemCount(MAX_ITEM_COUNT);
}

CDlgWifiDetail::~CDlgWifiDetail(void)
{
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

#ifdef IF_SUPPORT_WIFI
    etl_UnregisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, OnWifiDetailUIMessage);
#endif
}

/********************************* 继承自CListSubPage ***********************************/
void CDlgWifiDetail::Init()
{
}

bool CDlgWifiDetail::LoadPageData()
{
    m_iCurNetID = _UIWifiManager.GetCurID();
    SETTINGUI_INFO("[CDlgWifiDetail::LoadPageData()] iCurID[%d]", m_iCurNetID);

    if (0 <= m_iCurNetID)
    {
        Load();
    }

    return true;
}

void CDlgWifiDetail::Load()
{
    if (JudgeWidgetNULL())
    {
        return;
    }

#ifdef IF_SUPPORT_WIFI
    WiFiNetInfo curWifi;
    WIFI_GetNetworkInfoByID(m_iCurNetID, curWifi);
    SETTINGUI_INFO("[WifiDetail::load()]state[%d], SSID[%s] frequence[%d], signal[%d], secureMode[%d]"
                   , curWifi.eNetState, curWifi.strSSID.c_str(), curWifi.iFrequence, curWifi.iSignal,
                   curWifi.iSercureMode);

    m_bConnectOrNot = curWifi.eNetState == WIRE_DEVICE_CONNECTED_STATE ? true : false;

    WifiDetailTitlePtr pDetailTitle = NULL;
    CFrameListTitlePtr pTitle = m_pGeneralFrame->GetFrameListTitle();

    if (NULL != pTitle && WifiDetailTitle::IsWifiDetailTitle(pTitle))
    {
        pDetailTitle = (WifiDetailTitlePtr)pTitle;
        pDetailTitle->SetConnectOrNot(m_bConnectOrNot);
    }

    // set label value
    m_pLabProfileName->setText(toQString(curWifi.strProfileName));
    m_pLabAPMacAddress->setText(curWifi.strMac.c_str());

    QString strFre = QString("%1%2").arg(curWifi.iFrequence).arg("MHz");
    m_pLabFrequence->setText(strFre);
    m_pLabSSID->setText(curWifi.strSSID.c_str());
#ifdef _T49
    m_iSignalLevel = curWifi.iSignal;
#else
    m_pLabSignalStrength->setText(QString::number(curWifi.iSignal, 10));
#endif

    if (curWifi.iSercureMode < 0 || curWifi.iSercureMode > 8)
    {
        m_pLabSecurityMode->setText(TRID_NONE);
    }
    else
    {
        m_pLabSecurityMode->setText(strSecure[curWifi.iSercureMode]);
    }
    if (curWifi.iEncrypt < 0 || curWifi.iEncrypt > 5)
    {
        m_pLabEncryptionType->setText(TRID_NONE);
    }
    else
    {
        m_pLabEncryptionType->setText(strEncryp[curWifi.iEncrypt]);
    }
#endif

    DoRelayout();

    if (m_iCurNetID >= 0
            && OR_INTERFACE_SUCCESS == _UIWifiManager.IsConnectingWifiDevice(m_iCurNetID))
    {
        m_pLabNote->SetLabelType(INT_ANIMATION);
        m_pLabNote->SetLabelText(LANG_TRANSLATE(TRID_CONNECTING));
        m_pLabNote->raise();
        m_pLabNote->show();
    }
}

/*********************************************** others ************************************************/
//获取Softkey数据
bool CDlgWifiDetail::GetSoftkeyData(CArraySoftKeyBarData & objWndData
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
    return true;
}

void CDlgWifiDetail::InitData()
{
    // init Title
    WifiDetailTitlePtr pDetailTitle = NULL;
    CFrameListTitlePtr pTitle = m_pGeneralFrame->GetFrameListTitle();

    if (NULL != pTitle && WifiDetailTitle::IsWifiDetailTitle(pTitle))
    {
        pDetailTitle = (WifiDetailTitlePtr)pTitle;
    }
    else
    {
        pDetailTitle = new WifiDetailTitle;
        if (NULL == pDetailTitle)
        {
            return;
        }
    }

    pDetailTitle->SetBtnCount(TITLE_BTN_COUNT);
    pDetailTitle->AddButton(WIFI_TITLE_ITEM_ACTION_CON, LANG_TRANSLATE(FRAMELIST_TITLE_DISCONNECT));
    pDetailTitle->AddButton(WIFI_TITLE_ITEM_ACTION_ADD, LANG_TRANSLATE(FRAMELIST_TITLE_ADD));

    pDetailTitle->SetAction(this);
    m_pGeneralFrame->SetFrameListTitle(pDetailTitle);

    // create widget
    m_pLabProfileName = new QLabel(this);
    m_pLabAPMacAddress = new QLabel(this);

#ifndef _T49
    m_pLabNote = new NoteLabel(this);
#endif

    m_pLabFrequence = new QLabel(this);
    m_pLabEncryptionType = new QLabel(this);
    m_pLabSecurityMode = new QLabel(this);
    m_pLabSignalStrength = new QLabel(this);
    m_pLabSSID = new QLabel(this);
#ifdef _T49
    m_pTimer = new QTimer(this);
#endif
    if (JudgeWidgetNULL())
    {
        return;
    }
#ifdef _T49
#ifdef FRESH_WINDOW_WITH_MIN_CHANGED
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnTimeOut()));
#endif
#endif // _T49
    //设置文字颜色
    SetWgtFontColor(m_pLabEncryptionType, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabFrequence, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabSSID, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabSecurityMode, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabProfileName, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabAPMacAddress, FONT_TYPE_TEXT);

#ifndef _T49
    SetWgtFontColor(m_pLabSignalStrength, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabNote, FONT_TYPE_TEXT);

    m_pLabNote->setGeometry(RECT_NOTE_LOCATION);
    m_pLabNote->hide();
#endif

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_PROFILE_NAME);
    qPair.first = strLab;
    qPair.second = m_pLabProfileName;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_SSID);
    qPair.first = strLab;
    qPair.second = m_pLabSSID;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_AP_MAC_ADDRESS);
    qPair.first = strLab;
    qPair.second = m_pLabAPMacAddress;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

#ifdef _T49
    strLab = LANG_TRANSLATE(TRID_SIGNAL_STRENGTH);
    qPair.first = strLab;
    qPair.second = NULL;
    qPair.bAddIndex = true;
    qPair.appendVariantData.setValue(m_iSignalLevel);
    qPair.eType = SETTING_ITEM_WIFI_SIGNAL;
    m_vecWidgetPair.push_back(qPair);
#else
    strLab = LANG_TRANSLATE(TRID_SIGNAL_STRENGTH);
    qPair.first = strLab;
    qPair.second = m_pLabSignalStrength;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);
#endif

    strLab = LANG_TRANSLATE(TRID_SECURITY_MODE);
    qPair.first = strLab;
    qPair.second = m_pLabSecurityMode;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_ENCRYPTION_TYPE);
    qPair.first = strLab;
    qPair.second = m_pLabEncryptionType;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_FREQUENCY);
    qPair.first = strLab;
    qPair.second = m_pLabFrequence;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    m_pLabFrequence->installEventFilter(this);
    m_pLabEncryptionType->installEventFilter(this);
    m_pLabSecurityMode->installEventFilter(this);
    m_pLabSSID->installEventFilter(this);
    m_pLabProfileName->installEventFilter(this);
    m_pLabAPMacAddress->installEventFilter(this);

#ifdef _T49
#ifdef FRESH_WINDOW_WITH_MIN_CHANGED
    m_pTimer->start(TIME_TO_FRESH_PAGE_DATA);
#endif
#else
    m_pLabNote->installEventFilter(this);
#endif // _T49
}

// 标题框Action处理
void CDlgWifiDetail::OnTitleAction(CFrameList * pFrameList, CFrameListTitle * pTitle, int nAction)
{
    // check dongle
    if (OR_INTERFACE_FAIL == _UIWifiManager.IsDongleValid())
    {
        _UIWifiManager.Wifi_ShowCommonMsgBox(NULL
                                             , LANG_TRANSLATE(TRID_NO_FIND_WIFI_ADAPTER)
                                             , MESSAGEBOX_Cancel, SHORT_TIME);
        return;
    }

    if (WIFI_TITLE_ITEM_ACTION_CON == nAction)
    {
        _UIWifiManager.SetNeedUpdate(true);

        if (m_bConnectOrNot && -1 != m_iCurNetID)
        {
            m_pLabNote->SetLabelType(INT_ANIMATION);
            m_pLabNote->SetLabelText(LANG_TRANSLATE(TRID_DISCONNECTING));
#ifdef IF_SUPPORT_WIFI
            _UIWifiManager.DisconnectWifi("CC", m_iCurNetID);
#endif
        }
        else if (-1 != m_iCurNetID)
        {
            m_pLabNote->SetLabelType(INT_ANIMATION);
            m_pLabNote->SetLabelText(LANG_TRANSLATE(TRID_CONNECTING));
#ifdef IF_SUPPORT_WIFI
            _UIWifiManager.ConnectWifi("CC", m_iCurNetID);
#endif
        }
    }
    else if (WIFI_TITLE_ITEM_ACTION_ADD == nAction)
    {
#ifdef IF_SUPPORT_WIFI
        if (MAX_SAVED_NUM <= WIFI_GetSavedNetworkNum())
        {
            _UIWifiManager.Wifi_ShowCommonMsgBox(NULL,
                                                 LANG_TRANSLATE(TRID_LIST_IS_FULL_PLEASE_DELETE_BEFORE_ADD)
                                                 , MESSAGEBOX_Cancel, SHORT_TIME);
        }
        else
#endif
            if (-1 != m_iCurNetID)
            {
                _UIWifiManager.SetCurID(m_iCurNetID);
                _UIWifiManager.SetPageType(ENUM_WIFI_ADD_PAGE_BY_WIFI_DETAIL_TYPE);
                SettingUI_OpenPage(SubMenuData(ENTER_WIFI_ADD_EDIT_PAGE));
            }
    }
}

void CDlgWifiDetail::PopPwdDialog(bool bAuthentication/* = false*/)
{
    if (m_iCurNetID >= 0)
    {
#ifdef IF_SUPPORT_WIFI
        _UIWifiManager.SetOperatering(true);

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
#endif
    }
}

void CDlgWifiDetail::OnSubmitNameAndPassword(const yl::string & strName, const yl::string & strPswd)
{
    if (m_iCurNetID >= 0)
    {
        _UIWifiManager.SetOperatering(false);

        _UIWifiManager.AuthNetWork(m_iCurNetID, strName, strPswd);
    }
}

void CDlgWifiDetail::OnSubmitPassword(const yl::string & strPswd)
{
    if (m_iCurNetID >= 0)
    {
        _UIWifiManager.SetOperatering(false);

        _UIWifiManager.AskWifiPassword(m_iCurNetID, strPswd.c_str());
    }
}

void CDlgWifiDetail::OnCancelPassword()
{
    if (m_iCurNetID >= 0)
    {
        _UIWifiManager.SetOperatering(false);

        _UIWifiManager.CancelWifiConnect("", m_iCurNetID);
    }
}

/**************************************** used only in class *******************************************/
void CDlgWifiDetail::DoRelayout()
{
    if (JudgeWidgetNULL())
    {
        return;
    }
    MakeReadyForRelayout();

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_PROFILE_NAME);
    qPair.first = strLab;
    qPair.second = m_pLabProfileName;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_SSID);
    qPair.first = strLab;
    qPair.second = m_pLabSSID;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_AP_MAC_ADDRESS);
    qPair.first = strLab;
    qPair.second = m_pLabAPMacAddress;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

#ifdef _T49
    strLab = LANG_TRANSLATE(TRID_SIGNAL_STRENGTH);
    qPair.first = strLab;
    qPair.second = NULL;
    qPair.bAddIndex = true;
    qPair.appendVariantData.setValue(m_iSignalLevel);
    qPair.eType = SETTING_ITEM_WIFI_SIGNAL;
    m_vecWidgetPair.push_back(qPair);
#else
    strLab = LANG_TRANSLATE(TRID_SIGNAL_STRENGTH);
    qPair.first = strLab;
    qPair.second = m_pLabSignalStrength;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);
#endif

    strLab = LANG_TRANSLATE(TRID_SECURITY_MODE);
    qPair.first = strLab;
    qPair.second = m_pLabSecurityMode;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_ENCRYPTION_TYPE);
    qPair.first = strLab;
    qPair.second = m_pLabEncryptionType;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_FREQUENCY);
    qPair.first = strLab;
    qPair.second = m_pLabFrequence;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);
    UpdateWnd();
}

bool CDlgWifiDetail::JudgeWidgetNULL()
{
    return (NULL == m_pLabSecurityMode || NULL == m_pLabSSID
            || NULL == m_pLabAPMacAddress || NULL == m_pLabProfileName
            || NULL == m_pLabFrequence || NULL == m_pLabEncryptionType
#ifdef _T49
            || NULL == m_pTimer
#else
            || NULL == m_pLabSignalStrength
#endif
           );
}

// 设置连接结果提示
void CDlgWifiDetail::SetConnectResult(int iResult)
{
    m_pLabNote->SetLabelType(INT_DISAPPEAR);
    if (0 == iResult)
    {
        m_pLabNote->SetLabelText(LANG_TRANSLATE(TRID_CONNECT_SUCCESS));
    }
    else if (2 == iResult || 1 == iResult)
    {
        m_pLabNote->SetLabelText(LANG_TRANSLATE(TRID_CONNECT_FAILED));
    }
    m_pLabNote->raise();
    m_pLabNote->show();
}

void CDlgWifiDetail::SetDisConnectResult(int iResult)
{
    m_pLabNote->SetLabelType(INT_DISAPPEAR);
    if (1 == iResult)
    {
        m_pLabNote->SetLabelText(LANG_TRANSLATE(TRID_DISCONNECT_SUCCESS));
    }
    else
    {
        m_pLabNote->SetLabelText(LANG_TRANSLATE(TRID_DISCONNECT_FAILED));
    }
    m_pLabNote->raise();
    m_pLabNote->show();
}

#ifdef _T49
/*************************************** slots *****************************************/
void CDlgWifiDetail::OnTimeOut()
{
    SETTINGUI_INFO("refresh CDlgWifiDetail page data");
    m_pTimer->stop();
    Load();
    UpdateWnd();
    m_pTimer->start(TIME_TO_FRESH_PAGE_DATA);
}
#endif
