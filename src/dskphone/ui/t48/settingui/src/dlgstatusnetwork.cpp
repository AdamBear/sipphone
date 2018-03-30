#include "model.h"
#include "dlgstatusnetwork.h"
#include "devicelib/networkserver.h"
#include "setting/src/statuscontroller.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "setting/include/modmenu.h"
#include "messagebox/modmessagebox.h"
#include "setting/src/networkcontroller.h"
#include "settingui/include/modsettingui.h"
#include "keymap.h"

CDlgStatusNetwork::CDlgStatusNetwork(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pLabMAC(NULL)
    , m_pLabWifiMAC(NULL)
    , m_pLabIPMode(NULL)
#ifndef _T4X
    , m_pLabLanType(NULL)
    , m_pLabLanIP(NULL)
    , m_pLabLanMask(NULL)
#endif
    , m_iIPMode(0)

{
    InitPageData();
}

CDlgStatusNetwork::~CDlgStatusNetwork()
{

}

bool CDlgStatusNetwork::OnAction(const QString & strAction)
{
    if (NULL == m_pGeneralFrame)
    {
        return false;
    }

    int nCurrIdx = m_pGeneralFrame->GetFocusItemDataIndex();

    //不是在第1，2项不响应Enter
    if (nCurrIdx > 1)
    {
        return false;
    }

    bool bResult = false;
    if (strAction == "enter")
    {
        SubMenuData objData;
        objData.strMenuAction = "OpenSubPage(status_ip_mode_status)";

        int nIPMode = m_iIPMode;
        if (2 == nIPMode)
        {
            nIPMode = nCurrIdx == 0 ? 0 : 1;
        }
        objData.pBeforeSetSubpageData = (void *)&nIPMode;
        emit openpage(objData);

        bResult = true;
    }
    else
    {
        //none
    }

    return bResult;
}

// 过滤器
bool CDlgStatusNetwork::eventFilter(QObject * pObject, QEvent * pEvent)
{
    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

// page init
void CDlgStatusNetwork::InitPageData()
{
    m_pLabMAC = new QLabel(this);
    m_pLabWifiMAC = new QLabel(this);
    m_pLabIPMode = new QLabel(this);

    if (m_pLabIPMode == NULL || m_pLabIPMode == NULL || NULL == m_pLabWifiMAC)
    {
        return;
    }

    //设置文字颜色
    SetWgtFontColor(m_pLabIPMode, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabMAC, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabWifiMAC, FONT_TYPE_TEXT);

#ifndef _T4X
    m_pLabLanType = new QLabel(this);
    m_pLabLanIP = new QLabel(this);
    m_pLabLanMask = new QLabel(this);

    if (m_pLabLanType == NULL || m_pLabLanIP == NULL || m_pLabLanMask == NULL)
    {
        return;
    }

    SetWgtFontColor(m_pLabLanType, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabLanIP, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabLanMask, FONT_TYPE_TEXT);
#endif

    //给自己安装过滤器
    this->installEventFilter(this);
}

// 0-IPv4, 1-IPv6, 2-IPv4 and IPv6
void CDlgStatusNetwork::SetViewByIPMode(int eMode)
{
    MakeReadyForRelayout();

    QWidgetPair qPair;
    QString strLab;

    m_iIPMode = eMode;

    qPair.eType = SETTING_ITEM_MENU;
    qPair.bChangeStyleOnPress = true;
    qPair.bAddIndex = true;
    qPair.bAddMark = false;
    switch (eMode)
    {
    case 0:
        {
            strLab = LANG_TRANSLATE(TRID_IPV4_STATUS);
            qPair.first = strLab;
            qPair.second = NULL;
            qPair.third = "enter";
            m_vecWidgetPair.push_back(qPair);
        }
        break;;
    case 1:
        {
            strLab = LANG_TRANSLATE(TRID_IPV6_STATUS);
            qPair.first = strLab;
            qPair.second = NULL;
            qPair.third = "enter";
            m_vecWidgetPair.push_back(qPair);
        }
        break;
    case 2:
        {
            strLab = LANG_TRANSLATE(TRID_IPV4_STATUS);
            qPair.first = strLab;
            qPair.second = NULL;
            qPair.third = "enter";
            m_vecWidgetPair.push_back(qPair);

            strLab = LANG_TRANSLATE(TRID_IPV6_STATUS);
            qPair.first = strLab;
            qPair.second = NULL;
            qPair.third = "enter";
            m_vecWidgetPair.push_back(qPair);
        }
        break;
    default:
        break;
    }

    //后面的行没有点击效果
    qPair.eType = SETTING_ITEM_NORMAL;
    qPair.bChangeStyleOnPress = false;
    qPair.bAddIndex = true;
    qPair.bAddMark = true;
    qPair.third = "";

    strLab = LANG_TRANSLATE(TRID_IP_ADDRESS_MODE);
    qPair.first = strLab;
    qPair.second = m_pLabIPMode;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_MAC);
    qPair.first = strLab;
    qPair.second = m_pLabMAC;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_WIFI_MAC);
    qPair.second = m_pLabWifiMAC;
    m_vecWidgetPair.push_back(qPair);

#ifndef _T4X
    strLab = LANG_TRANSLATE(TRID_LAN_TYPE);
    qPair.first = strLab;
    qPair.second = m_pLabLanType;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_LAN_IP_ADDRESS);
    qPair.first = strLab;
    qPair.second = m_pLabLanIP;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_LAN_MASK);
    qPair.first = strLab;
    qPair.second = m_pLabLanMask;
    m_vecWidgetPair.push_back(qPair);
#endif


    AddWidget(m_vecWidgetPair, true, true, true);
}

bool CDlgStatusNetwork::LoadPageData()
{
    this->setFocus();

    if (m_pLabIPMode == NULL || m_pLabIPMode == NULL || NULL == m_pLabWifiMAC)
    {
        return false;
    }

    PhoneNetworkStatusData phoneNetworkStatusData;
    if (g_pStatusController->GetPhoneNetworkStatus(&phoneNetworkStatusData))
    {
#ifndef _T4X
        switch (phoneNetworkStatusData.m_eWanType)
        {
        case WAN_TYPE_NONE:
            m_pLabLanType->setText(LANG_TRANSLATE("Invalid WAN type"));
            break;
        case WAN_DHCP:
            m_pLabLanType->setText(LANG_TRANSLATE("DHCP"));
            break;
#if IF_FEATURE_PPPOE
        case WAN_PPPOE:
            m_pLabLanType->setText(LANG_TRANSLATE("PPPoE"));
            break;
#endif
        case WAN_STATIC:
            m_pLabLanType->setText(LANG_TRANSLATE("Static IP"));
        case WAN_TYPE_MAX:
            break;
        default :
            break;
        }
#endif

        QString strIPModeText;
        int iMode = g_pNetworkController->GetWanIPMode();
        switch (iMode)
        {
        case 0:
            strIPModeText = LANG_TRANSLATE(TRID_IPV4);
            break;
        case 1:
            strIPModeText = LANG_TRANSLATE(TRID_IPV6);
            break;
        case 2:
            strIPModeText = LANG_TRANSLATE(TRID_IPV4_AND_IPV6);
            break;
        default:
            break;
        }

        SetViewByIPMode(iMode);

        // 需要接口获取IP类型, 数据要替换，根据不同的IP类型替换成不同的数据
        m_pLabIPMode->setText(strIPModeText);
        m_pLabMAC->setText(toQString(netGetMACText('-')));
    }

    ExternalDeviceData stExtrernalDevice;
    if (g_pStatusController->GetExternalDeviceData(&stExtrernalDevice))
    {
        if (stExtrernalDevice.m_strWifiMac.empty())
        {
            m_pLabWifiMAC->setText(LANG_TRANSLATE(TRID_NA));
        }
        else
        {
            m_pLabWifiMAC->setText(toQString(stExtrernalDevice.m_strWifiMac));
        }
    }

    return true;
}

//状态显示softkey没有保存项
bool CDlgStatusNetwork::GetSoftkeyData(CArraySoftKeyBarData & objSoftData)
{
    objSoftData[3].m_strSoftkeyAction = "";
    objSoftData[3].m_strSoftkeyTitle  = "";

    return true;
}

bool CDlgStatusNetwork::IsStatusChanged()
{
    return false;
}

bool CDlgStatusNetwork::SavePageData()
{
    return true;
}
