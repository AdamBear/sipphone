#include "cdlgstatusgeneral.h"
#include "setting/src/statuscontroller.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "setting/include/modmenu.h"
#include "mainwnd/mainwnd.h"
#include "networksettingmanager.h"
#include "setting/src/networkcontroller.h"
#include "setting/include/authorityiddefine.h"
#include "netmsgtypedefine.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>

CDlgStatusGeneral::CDlgStatusGeneral(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pLabIPV4Value(NULL)
    , m_pLabIPV6Value(NULL)
    , m_pLabMACValue(NULL)
    , m_pLabWifiMAC(NULL)
    , m_pLabFirmwareValue(NULL)
#if IF_BUG_34374
    , m_pLabCfgVerValue(NULL)
#endif
    , m_nMode(0)
{
    // 获取IP的mode
    m_nMode = g_pNetworkController->GetWanIPMode();

    //this->setGeometry(0, 0, 480, 170);

    InitPageData();

    //LoadPageData();

    m_pGeneralFrame->SetItemClickBG("");
}

CDlgStatusGeneral::~CDlgStatusGeneral()
{
}

void CDlgStatusGeneral::SetData(void * pData /* = NULL */)
{
    // 设置模板类型
    m_eWndType = eWT_SubPage;

    m_strTitle = LANG_TRANSLATE(TRID_STATUS).toUtf8().data();
}

bool CDlgStatusGeneral::GetSoftkeyData(CArraySoftKeyBarData & objSoftData)
{
    objSoftData[3].m_strSoftkeyAction = "";
    objSoftData[3].m_strSoftkeyTitle  = "";

    return true;
}

void CDlgStatusGeneral::InitPageData()
{
    m_pLabIPV4Value = new QLabel(this);
    m_pLabIPV6Value = new qScrollTextLabel(this);
    m_pLabMACValue = new QLabel(this);
    m_pLabWifiMAC = new QLabel(this);
    m_pLabFirmwareValue = new QLabel(this);
#if IF_BUG_34374
    m_pLabCfgVerValue = new qScrollTextLabel(this);
#endif
    if (m_pLabIPV4Value != NULL
            && m_pLabIPV6Value != NULL
            && m_pLabMACValue != NULL
            && m_pLabWifiMAC != NULL
            && m_pLabFirmwareValue != NULL
#if IF_BUG_34374
            && m_pLabCfgVerValue != NULL
#endif
       )
    {
        SetWgtFontColor(m_pLabIPV4Value, FONT_TYPE_TEXT);
        SetWgtFontColor(m_pLabIPV6Value, FONT_TYPE_TEXT);
        SetWgtFontColor(m_pLabMACValue, FONT_TYPE_TEXT);
        SetWgtFontColor(m_pLabWifiMAC, FONT_TYPE_TEXT);
        SetWgtFontColor(m_pLabFirmwareValue, FONT_TYPE_TEXT);
#if IF_BUG_34374
        SetWgtFontColor(m_pLabCfgVerValue, FONT_TYPE_TEXT);
#endif
        QWidgetPair qPair;
        QString strLab;

        if (m_nMode == IPAM_IPV4)
        {
            strLab = LANG_TRANSLATE(TRID_IPV4_ADDRESS);
            qPair.first = strLab;
            qPair.second = m_pLabIPV4Value;
            qPair.bAddIndex = true;
            qPair.strAuthorId = kszAuthorityStatusIPv4;
            m_vecWidgetPair.push_back(qPair);
        }
        else if (m_nMode == IPAM_IPV6)
        {
            strLab = LANG_TRANSLATE(TRID_IPV6_ADDRESS);
            qPair.first = strLab;
            qPair.second = m_pLabIPV6Value;
            qPair.bAddIndex = true;
            qPair.strAuthorId = kszAuthorityStatusIPv6;
            m_vecWidgetPair.push_back(qPair);
        }
        else
        {
            strLab = LANG_TRANSLATE(TRID_IPV4_ADDRESS);
            qPair.first = strLab;
            qPair.second = m_pLabIPV4Value;
            qPair.bAddIndex = true;
            qPair.strAuthorId = kszAuthorityStatusIPv4;
            m_vecWidgetPair.push_back(qPair);

            strLab = LANG_TRANSLATE(TRID_IPV6_ADDRESS);
            qPair.first = strLab;
            qPair.second = m_pLabIPV6Value;
            qPair.bAddIndex = true;
            qPair.strAuthorId = kszAuthorityStatusIPv6;
            m_vecWidgetPair.push_back(qPair);

        }

        strLab = LANG_TRANSLATE(TRID_MAC);
        qPair.first = strLab;
        qPair.second = m_pLabMACValue;
        qPair.bAddIndex = true;
        qPair.strAuthorId = kszAuthorityStatusMAC;
        m_vecWidgetPair.push_back(qPair);

        qPair.first = LANG_TRANSLATE(TRID_WIFI_MAC);
        qPair.second = m_pLabWifiMAC;
        qPair.bAddIndex = true;
        qPair.strAuthorId = kszAuthorityStatusWifiMAC;
        m_vecWidgetPair.push_back(qPair);

        strLab = LANG_TRANSLATE(TRID_FIRMWARE);
        qPair.first = strLab;
        qPair.second = m_pLabFirmwareValue;
        qPair.bAddIndex = true;
        qPair.strAuthorId = kszAuthorityStatusHW;
        m_vecWidgetPair.push_back(qPair);

#if IF_BUG_34374
        yl::string strValue = configParser_GetConfigString(kszCfgVersion);
        if (!strValue.empty())
        {
            strLab = LANG_TRANSLATE(TRID_CFG_VER);
            qPair.first = strLab;
            qPair.second = m_pLabCfgVerValue;
            qPair.strAuthorId = kszAuthorityCfgVer;
            m_vecWidgetPair.push_back(qPair);
        }
#endif

        AddWidget(m_vecWidgetPair);
    }

    //给自己安装过滤器
    this->installEventFilter(this);
}

bool CDlgStatusGeneral::LoadPageData()
{
    //指针判空
    if (m_pLabIPV4Value == NULL || m_pLabIPV6Value == NULL
            || m_pLabMACValue == NULL || m_pLabFirmwareValue == NULL
            || NULL == m_pLabWifiMAC)
    {
        return false;
    }

    SetCurrentIndex(0);
    this->setFocus();

    //加载数据
    PhoneStatusData phoneData;
    if (g_pStatusController->GetPhoneStatusData(&phoneData))
    {
        //MAC
        m_pLabMACValue->setText(toQString(phoneData.m_strMac.c_str()));

        //Firmware
        m_pLabFirmwareValue->setText(toQString(phoneData.m_strFirmWare.c_str()));
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

    PhoneNetworkStatusData networkData;

    if (m_nMode == IPAM_IPV4)
    {
        //IPV4
        g_pStatusController->GetPhoneIPv4NetworkStatus(&networkData);
        m_pLabIPV4Value->setText(toQString(networkData.m_strWanIP.c_str()));
        m_pLabIPV6Value->setText("");
    }
    else if (m_nMode == IPAM_IPV6)
    {
        //IPV6
        g_pStatusController->GetPhoneIPv6NetworkStatus(&networkData);
        m_pLabIPV6Value->setText(toQString(networkData.m_strWanIP.c_str()));
        m_pLabIPV4Value->setText("");
    }
    else
    {
        //IPV4
        g_pStatusController->GetPhoneIPv4NetworkStatus(&networkData);
        m_pLabIPV4Value->setText(toQString(networkData.m_strWanIP.c_str()));
        //IPV6
        networkData.Reset();
        g_pStatusController->GetPhoneIPv6NetworkStatus(&networkData);
        m_pLabIPV6Value->setText(toQString(networkData.m_strWanIP.c_str()));
    }

#if IF_BUG_34374
    yl::string strValue = configParser_GetConfigString(kszCfgVersion);
    if (!strValue.empty())
    {
        m_pLabCfgVerValue->setText(toQString(strValue.c_str()));
    }
#endif

    return true;
}

bool CDlgStatusGeneral::IsStatusChanged()
{
    return false;
}

bool CDlgStatusGeneral::SavePageData()
{
    return true;
}

