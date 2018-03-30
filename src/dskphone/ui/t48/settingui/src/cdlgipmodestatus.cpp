#include "cdlgipmodestatus.h"
#include "setting/include/modmenu.h"
#include "mainwnd/mainwnd.h"
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "uimanager/moduimanager.h"
#include "setting/src/statuscontroller.h"

CDlgIPModeStatus::CDlgIPModeStatus(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pLabWANType(NULL)
    , m_pLabWANIP(NULL)
    , m_pLabWANMask(NULL)
    , m_pLabGateway(NULL)
    , m_pLabPriDNS(NULL)
    , m_pLabSecDNS(NULL)
    , m_ePageType(IP_STATUS_IPV4)
{
    InitPageData();
    m_pGeneralFrame->SetItemClickBG("");
}

CDlgIPModeStatus::~CDlgIPModeStatus()
{

}

void CDlgIPModeStatus::BeforeSetSubPage(void * pData)
{
    if (NULL == pData)
    {
        m_ePageType = IP_STATUS_IPV4;
    }
    else
    {
        m_ePageType = (IP_STATUS_PAGE_TYPE) * ((int *)pData);
    }

    if (IP_STATUS_IPV4 == m_ePageType)
    {
        m_strTitle = LANG_TRANSLATE(TRID_IPV4_STATUS).toUtf8().data();
    }
    else
    {
        m_strTitle = LANG_TRANSLATE(TRID_IPV6_STATUS).toUtf8().data();
    }

    AddPageContent();
}

void CDlgIPModeStatus::InitPageData()
{
    m_pLabWANType = new QLabel(this);
    m_pLabWANIP = new qScrollTextLabel(this);
    m_pLabWANMask = new qScrollTextLabel(this);
    m_pLabGateway = new qScrollTextLabel(this);
    m_pLabPriDNS = new qScrollTextLabel(this);
    m_pLabSecDNS = new qScrollTextLabel(this);

    if (m_pLabWANType != NULL
            && m_pLabWANIP != NULL
            && m_pLabWANMask != NULL
            && m_pLabGateway != NULL
            && m_pLabPriDNS != NULL
            && m_pLabSecDNS != NULL)
    {
        SetWgtFontColor(m_pLabWANType, FONT_TYPE_TEXT);
        SetWgtFontColor(m_pLabWANIP, FONT_TYPE_TEXT);
        SetWgtFontColor(m_pLabWANMask, FONT_TYPE_TEXT);
        SetWgtFontColor(m_pLabGateway, FONT_TYPE_TEXT);
        SetWgtFontColor(m_pLabPriDNS, FONT_TYPE_TEXT);
        SetWgtFontColor(m_pLabSecDNS, FONT_TYPE_TEXT);

        m_pLabWANType->installEventFilter(this);
        m_pLabWANIP->installEventFilter(this);
        m_pLabWANMask->installEventFilter(this);
        m_pLabGateway->installEventFilter(this);
        m_pLabPriDNS->installEventFilter(this);
        m_pLabSecDNS->installEventFilter(this);
    }
}

//添加内容
void CDlgIPModeStatus::AddPageContent()
{
    if (m_pLabWANType != NULL
            && m_pLabWANIP != NULL
            && m_pLabWANMask != NULL
            && m_pLabGateway != NULL
            && m_pLabPriDNS != NULL
            && m_pLabSecDNS != NULL
            && m_pGeneralFrame != NULL)
    {
        MakeReadyForRelayout();

        QWidgetPair qPair;

        qPair.first = LANG_TRANSLATE(TRID_WAN_TYPE);
        qPair.second = m_pLabWANType;
        m_vecWidgetPair.push_back(qPair);

        qPair.first = LANG_TRANSLATE(TRID_WAN_IP);
        qPair.second = m_pLabWANIP;
        m_vecWidgetPair.push_back(qPair);

        PhoneNetworkStatusData networkData;
        g_pStatusController->GetPhoneIPv4NetworkStatus(&networkData);

        //IPV6以及pppoe没有MASK
        if (IP_STATUS_IPV4 == m_ePageType
                && networkData.m_iWanType != WAN_PPPOE)
        {
            qPair.first = LANG_TRANSLATE(TRID_WAN_MASK);
            qPair.second = m_pLabWANMask;
            m_vecWidgetPair.push_back(qPair);
        }


        qPair.first = LANG_TRANSLATE(TRID_DEFAULT_GATEWAY);
        qPair.second = m_pLabGateway;
        m_vecWidgetPair.push_back(qPair);

        qPair.first = LANG_TRANSLATE(TRID_RRIMARY_DNS);
        qPair.second = m_pLabPriDNS;
        m_vecWidgetPair.push_back(qPair);

        qPair.first = LANG_TRANSLATE(TRID_SECONDARY_DNS);
        qPair.second = m_pLabSecDNS;
        m_vecWidgetPair.push_back(qPair);

        AddWidget(m_vecWidgetPair);
    }
}

bool CDlgIPModeStatus::LoadPageData()
{
    // 控件指针判空
    if (NULL == m_pLabWANType
            || NULL == m_pLabWANIP
            || NULL == m_pLabWANMask
            || NULL == m_pLabGateway
            || NULL == m_pLabPriDNS
            || NULL == m_pLabSecDNS)
    {
        return false;
    }

    PhoneNetworkStatusData networkData;
    bool bResult = false;
    if (IP_STATUS_IPV4 == m_ePageType)
    {
        bResult = g_pStatusController->GetPhoneIPv4NetworkStatus(&networkData);
    }
    else
    {
        bResult = g_pStatusController->GetPhoneIPv6NetworkStatus(&networkData);
    }

    if (!bResult)
    {
        return false;
    }

    QString strWANType;
    switch (networkData.m_iWanType)
    {
    case WAN_DHCP:
        strWANType = LANG_TRANSLATE(TRID_DHCP);
        break;
#if IF_FEATURE_PPPOE
    case WAN_PPPOE:
        strWANType = LANG_TRANSLATE(TRID_PPPOE);
        break;
#endif
    case WAN_STATIC:
        strWANType = LANG_TRANSLATE(TRID_STATIC_IP);
        break;
    default:
        strWANType = LANG_TRANSLATE(TRID_UNKNOWN);
        break;
    }

    m_pLabWANType->setText(strWANType);
    m_pLabWANIP->setText(networkData.m_strWanIP.c_str());
    if (IP_STATUS_IPV4 == m_ePageType
            && networkData.m_iWanType != WAN_PPPOE)
    {
        m_pLabWANMask->setText(networkData.m_strWanMask.c_str());
    }
    else
    {
        m_pLabWANMask->setText("");
    }

    m_pLabGateway->setText(networkData.m_strGateway.c_str());
    m_pLabPriDNS->setText(networkData.m_strDnsPrimary.c_str());
    m_pLabSecDNS->setText(networkData.m_strDnsSecond.c_str());

    return true;
}

// 设置子页面的SoftKey
bool CDlgIPModeStatus::GetSoftkeyData(CArraySoftKeyBarData & objSoftData)
{
    // 该界面不需要save按键
    objSoftData[3].m_strSoftkeyAction = "";
    objSoftData[3].m_strSoftkeyTitle  = "";

    return true;
}

bool CDlgIPModeStatus::IsStatusChanged()
{
    return false;
}

bool CDlgIPModeStatus::SavePageData()
{
    return false;
}
