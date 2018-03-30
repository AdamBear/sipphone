#include "wifi_inc.h"

namespace
{
// action
// #define SOFTKEY_REFRESH         "Refresh"
// string 待定
const char * strSecurity[9] = { "NONE", "WEP", "WPA", "WPA2", "WPA_PSK", "WPA2_PSK", "WPA_EAP", "WPA2_EAP", "WPS", };
const char * strEncryption[7] = { "NONE", "WEP40", "WEP104", "TKIP", "AES", "AUTO", "GCMP", };
// signal level
const char * strSignalLevel[5] = { "Weak", "Low", "Middle", "High", "Strong" };
}

// 处理消息.
static LRESULT OnWifiStatusUIMessage(msgObject & refMessage)
{
#ifdef IF_SUPPORT_WIFI
    SETTINGUI_INFO("[CDlgWifiStatus::OnWifiBaseUIMessage] message[%d], lParam[%d], wParam[%d], UpUI[%d]",
                   refMessage.message
                   ,  refMessage.lParam, refMessage.wParam, WIFI_MSG_UPDATE_UI);
#endif

    CDlgWifiStatus * pWifiStatus = qobject_cast<CDlgWifiStatus *>(UIManager_GetPrivateDlg(
                                       DLG_CDlgWifiStatus));
    if (NULL != pWifiStatus && pWifiStatus->IsTopPage())
    {
#ifdef IF_SUPPORT_WIFI
#ifdef FRESH_WINDOW_WITH_MIN_CHANGED
        WirelessOperationMsg * wifiMsg = (WirelessOperationMsg *)refMessage.GetExtraData();
#ifdef _T49
        if (NULL != wifiMsg && OP_WIFI_CLOSE_RESULT == wifiMsg->iOperationType)
        {
            pWifiStatus->OnTimeOut();
        }
        else
#endif
            if (NULL != wifiMsg && OP_WIFI_OPEN_RESULT == wifiMsg->iOperationType)
            {
                _UIWifiManager.SetNeedUpdate(true);
            }
#endif
#endif
    }

    return 0;
}

/******************************************* CDlgWifiStatus ********************************************/
CDlgWifiStatus::CDlgWifiStatus(QWidget * parent /*= NULL*/)
    : CDlgBaseSubPage(parent)
#ifndef TEMP_SLIP_SOLUTION
    , m_pLabQos(NULL)
    , m_pLabSecurityMode(NULL)
#ifdef _T49
    , m_pSignalItem(NULL)
#else
    , m_pLabSignalStrength(NULL)
#endif
    , m_pLabSSID(NULL)
    , m_pLabTransmitRate(NULL)
    , m_pLabWirelessStatus(NULL)
    , m_pLabProfileName(NULL)
    , m_pLabFrequency(NULL)
    , m_pLabEncryptionType(NULL)
    , m_pLabChannel(NULL)
    , m_pLabAPMacAddress(NULL)
#endif
#ifdef _T49
    , m_pWifiStatusDelegate(NULL)
    , m_pTimer(NULL)
    , m_iSignalLevel(-1)
#endif
{
    InitData();
    //设置标题
    SetWindowTitle(LANG_TRANSLATE(TRID_WIRELESS_STATUS));

#ifdef IF_SUPPORT_WIFI
    etl_RegisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, OnWifiStatusUIMessage);
#endif
}

CDlgWifiStatus::~CDlgWifiStatus(void)
{
#ifdef _T49
    if (NULL != m_pSignalItem)
    {
        m_pSignalItem = NULL;
    }

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
    etl_UnregisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, OnWifiStatusUIMessage);
#endif
}

void CDlgWifiStatus::InitData()
{
#ifndef TEMP_SLIP_SOLUTION
    m_pLabAPMacAddress = new QLabel(this);
    m_pLabChannel = new QLabel(this);
    m_pLabEncryptionType = new QLabel(this);
    m_pLabFrequency = new QLabel(this);
    m_pLabProfileName = new QLabel(this);
    m_pLabQos = new QLabel(this);
    m_pLabSecurityMode = new QLabel(this);
    m_pLabSignalStrength = new QLabel(this);
    m_pLabSSID = new QLabel(this);
    m_pLabTransmitRate = new QLabel(this);
    m_pLabWirelessStatus = new QLabel(this);
#endif

#ifdef _T49
    m_pTimer = new QTimer(this);
    m_pWifiStatusDelegate = new WifiStatusDelegate();
#endif

    if (
#ifndef TEMP_SLIP_SOLUTION
        NULL == m_pLabWirelessStatus || NULL == m_pLabTransmitRate || NULL == m_pLabSSID
        || NULL == m_pLabSecurityMode || NULL == m_pLabQos
        || NULL == m_pLabProfileName || NULL == m_pLabFrequency || NULL == m_pLabEncryptionType
        || NULL == m_pLabChannel || NULL == m_pLabAPMacAddress
#endif
        || NULL == m_pLabSignalStrength
#ifdef _T49
        || NULL == m_pTimer
        || NULL == m_pWifiStatusDelegate
#endif
    )
    {
        return;
    }

#ifdef _T49
#ifdef FRESH_WINDOW_WITH_MIN_CHANGED
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnTimeOut()));
#endif

    m_pWifiStatusDelegate->setParent(this);
    m_pWifiStatusDelegate->BindFramelist(m_pGeneralFrame);
    m_pWifiStatusDelegate->InitSettingListStyle();
    m_pListDelegate = m_pWifiStatusDelegate;

    m_pListDelegate->SetUiActionProcessor(this);
    m_pWifiStatusDelegate->SetUiActionProcessor(this);
#endif // _T49

#ifndef TEMP_SLIP_SOLUTION
    //设置文字颜色
    SetWgtFontColor(m_pLabWirelessStatus, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabTransmitRate, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabSSID, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabSignalStrength, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabSecurityMode, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabQos, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabProfileName, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabFrequency, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabEncryptionType, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabChannel, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabAPMacAddress, FONT_TYPE_TEXT);
#endif

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_WIRELESS_STATUS);
    qPair.first = strLab;
#ifndef TEMP_SLIP_SOLUTION
    qPair.second = m_pLabWirelessStatus;
#else
    qPair.second = NULL;
#endif
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_PROFILE_NAME);
    qPair.first = strLab;
#ifndef TEMP_SLIP_SOLUTION
    qPair.second = m_pLabProfileName;
#else
    qPair.second = NULL;
#endif
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_SSID);
    qPair.first = strLab;
#ifndef TEMP_SLIP_SOLUTION
    qPair.second = m_pLabSSID;
#else
    qPair.second = NULL;
#endif
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

    strLab = LANG_TRANSLATE(TRID_CHANNEL);
    qPair.first = strLab;
#ifndef TEMP_SLIP_SOLUTION
    qPair.second = m_pLabChannel;
#else
    qPair.second = NULL;
#endif
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_AP_MAC_ADDRESS);
    qPair.first = strLab;
#ifndef TEMP_SLIP_SOLUTION
    qPair.second = m_pLabAPMacAddress;
#else
    qPair.second = NULL;
#endif
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_SECURITY_MODE);
    qPair.first = strLab;
#ifndef TEMP_SLIP_SOLUTION
    qPair.second = m_pLabSecurityMode;
#else
    qPair.second = NULL;
#endif
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_ENCRYPTION_TYPE);
    qPair.first = strLab;
#ifndef TEMP_SLIP_SOLUTION
    qPair.second = m_pLabEncryptionType;
#else
    qPair.second = NULL;
#endif
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_TRANSMIT_RATE);
    qPair.first = strLab;
#ifndef TEMP_SLIP_SOLUTION
    qPair.second = m_pLabTransmitRate;
#else
    qPair.second = NULL;
#endif
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_FREQUENCY);
    qPair.first = strLab;
#ifndef TEMP_SLIP_SOLUTION
    qPair.second = m_pLabFrequency;
#else
    qPair.second = NULL;
#endif
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_QOS);
    qPair.first = strLab;
#ifndef TEMP_SLIP_SOLUTION
    qPair.second = m_pLabQos;
#else
    qPair.second = NULL;
#endif
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

#ifndef TEMP_SLIP_SOLUTION
    AddWidget(m_vecWidgetPair);

    m_pLabQos->installEventFilter(this);
    m_pLabAPMacAddress->installEventFilter(this);
    m_pLabChannel->installEventFilter(this);
    m_pLabEncryptionType->installEventFilter(this);
    m_pLabFrequency->installEventFilter(this);
    m_pLabProfileName->installEventFilter(this);
    m_pLabSecurityMode->installEventFilter(this);
    m_pLabSSID->installEventFilter(this);
    m_pLabSignalStrength->installEventFilter(this);
    m_pLabTransmitRate->installEventFilter(this);
    m_pLabWirelessStatus->installEventFilter(this);
#endif

#ifdef _T49
#ifdef FRESH_WINDOW_WITH_MIN_CHANGED
    m_pTimer->start(TIME_TO_FRESH_PAGE_DATA);
#endif
#endif // _T49
}

/*************************************** 继承CListSubPage *****************************************/
bool CDlgWifiStatus::LoadPageData()
{
#ifndef TEMP_SLIP_SOLUTION
    if (NULL == m_pLabWirelessStatus || NULL == m_pLabTransmitRate
            || NULL == m_pLabSSID || NULL == m_pLabSecurityMode
            || NULL == m_pLabQos || NULL == m_pLabProfileName || NULL == m_pLabFrequency
            || NULL == m_pLabEncryptionType || NULL == m_pLabChannel || NULL == m_pLabAPMacAddress
            || NULL == m_pLabSignalStrength)
    {
        return false;
    }
#endif

    CleanLabelText();

#ifdef IF_SUPPORT_WIFI
    if (OR_INTERFACE_FAIL == _UIWifiManager.IsDongleValid())
    {
        m_pLabWirelessStatus->setText(LANG_TRANSLATE(TRID_DISABLE));
    }
    else if (OR_INTERFACE_SUCCESS == _UIWifiManager.IsWifiHasConnected())
    {
        RefreshPageData();
    }
    else
    {
#ifndef TEMP_SLIP_SOLUTION
        m_pLabWirelessStatus->setText(LANG_TRANSLATE(TRID_DISCONNECT));
#ifdef _T49
        m_vecWidgetPair[3].appendVariantData.setValue(m_iSignalLevel);

        DoRelayout();
#endif // _T49
#else
        m_vecWidgetPair[0].strAuthorId = TRID_DISCONNECT;
#endif
    }

#ifdef TEMP_SLIP_SOLUTION
    AddWidget(m_vecWidgetPair);
#endif

#endif
    return true;
}

bool CDlgWifiStatus::GetSoftkeyData(CArraySoftKeyBarData & objWndData
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

/**************************************** used only in class *******************************************/
// clean label text
void CDlgWifiStatus::CleanLabelText()
{
#ifndef TEMP_SLIP_SOLUTION
    m_pLabAPMacAddress->clear();
    m_pLabChannel->clear();
    m_pLabEncryptionType->clear();
    m_pLabFrequency->clear();
    m_pLabProfileName->clear();
    m_pLabQos->clear();
    m_pLabSecurityMode->clear();
    m_pLabSSID->clear();
    m_pLabSignalStrength->clear();
    m_pLabTransmitRate->clear();
    m_pLabWirelessStatus->clear();
#ifdef _T49
    m_iSignalLevel = -1;
#endif // _T49
#endif
}

bool CDlgWifiStatus::JudgeWidgetNULL()
{
    return (NULL == m_pLabAPMacAddress || NULL == m_pLabChannel
            || NULL == m_pLabEncryptionType || NULL == m_pLabFrequency
            || NULL == m_pLabProfileName || NULL == m_pLabQos
            || NULL == m_pLabSecurityMode || NULL == m_pLabSSID
            || NULL == m_pLabTransmitRate || NULL == m_pLabWirelessStatus
            || NULL == m_pLabSignalStrength);
}

void CDlgWifiStatus::RefreshPageData()
{
#ifdef IF_SUPPORT_WIFI
    WiFiNetInfo curWifiNet;
    WIFI_GetCurrentNetConnectInfo(curWifiNet);

#ifndef TEMP_SLIP_SOLUTION
    // load data
    m_pLabWirelessStatus->setText(LANG_TRANSLATE(TRID_CONNECTED));
    m_pLabSSID->setText(toQString(curWifiNet.strSSID));
    if (0 == strcmp("", curWifiNet.strProfileName.c_str()))
    {
        m_pLabProfileName->setText(toQString(curWifiNet.strSSID));
    }
    else
    {
        m_pLabProfileName->setText(toQString(curWifiNet.strProfileName));
    }
#ifdef _T49
    m_iSignalLevel = curWifiNet.iSignal;
#else
    m_pLabSignalStrength->setText(QString::number(curWifiNet.iSignal, 10));
#endif
    m_pLabChannel->setText(QString::number(curWifiNet.iChannel, 10));
    m_pLabAPMacAddress->setText(curWifiNet.strMac.c_str());
    m_pLabSecurityMode->setText(strSecurity[curWifiNet.iSercureMode]);
    m_pLabEncryptionType->setText(strEncryption[curWifiNet.iEncrypt]);
    QString strTrans = QString("%1%2").arg(curWifiNet.iSpeed).arg(LANG_TRANSLATE("M/s"));
    m_pLabTransmitRate->setText(strTrans);
    QString strFre = QString("%1%2").arg(curWifiNet.iFrequence).arg("MHz");
    m_pLabFrequency->setText(strFre);
    m_pLabQos->setText(QString::number(curWifiNet.iQos, 10));
#else
    // load data
    m_vecWidgetPair[0].strAuthorId = TRID_CONNECTED;
    m_vecWidgetPair[1].strAuthorId = curWifiNet.strSSID;
    if (0 == strcmp("", curWifiNet.strProfileName.c_str()))
    {
        m_vecWidgetPair[2].strAuthorId = curWifiNet.strSSID;
    }
    else
    {
        m_vecWidgetPair[2].strAuthorId = curWifiNet.strProfileName;
    }
    m_vecWidgetPair[3].appendVariantData.setValue(curWifiNet.iSignal);
    QString strChannel = (QString::number(curWifiNet.iChannel, 10));
    m_vecWidgetPair[4].strAuthorId = strChannel.toUtf8().data();
    m_vecWidgetPair[5].strAuthorId = curWifiNet.strMac;
    m_vecWidgetPair[6].strAuthorId = strSecurity[curWifiNet.iSercureMode];
    m_vecWidgetPair[7].strAuthorId = strEncryption[curWifiNet.iEncrypt];
    QString strTrans = QString("%1%2").arg(curWifiNet.iSpeed).arg(LANG_TRANSLATE("M/s"));
    m_vecWidgetPair[8].strAuthorId = strTrans.toUtf8().data();
    QString strFre = QString("%1%2").arg(curWifiNet.iFrequence).arg("MHz");
    m_vecWidgetPair[9].strAuthorId = strFre.toUtf8().data();
    QString strQos = (QString::number(curWifiNet.iQos, 10));
    m_vecWidgetPair[10].strAuthorId = strQos.toUtf8().data();
#endif
    DoRelayout();
#endif
}

void CDlgWifiStatus::DoRelayout()
{
    if (JudgeWidgetNULL())
    {
        return;
    }
    MakeReadyForRelayout();

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_WIRELESS_STATUS);
    qPair.first = strLab;
    qPair.second = m_pLabWirelessStatus;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

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

    strLab = LANG_TRANSLATE(TRID_CHANNEL);
    qPair.first = strLab;
    qPair.second = m_pLabChannel;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_AP_MAC_ADDRESS);
    qPair.first = strLab;
    qPair.second = m_pLabAPMacAddress;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

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

    strLab = LANG_TRANSLATE(TRID_TRANSMIT_RATE);
    qPair.first = strLab;
    qPair.second = m_pLabTransmitRate;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_FREQUENCY);
    qPair.first = strLab;
    qPair.second = m_pLabFrequency;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_QOS);
    qPair.first = strLab;
    qPair.second = m_pLabQos;
    qPair.eType = SETTING_ITEM_NORMAL;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);
}

#ifdef _T49
/*************************************** slots *****************************************/
void CDlgWifiStatus::OnTimeOut()
{
    SETTINGUI_INFO("refresh CDlgWifiStatus page data");
    m_pTimer->stop();
    //m_pWifiStatusDelegate->RecordOffSet();
    LoadPageData();
    UpdateWnd();
    m_pTimer->start(TIME_TO_FRESH_PAGE_DATA);
}
#endif
