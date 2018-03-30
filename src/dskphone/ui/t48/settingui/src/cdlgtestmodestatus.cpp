#include "cdlgtestmodestatus.h"
#include "commonunits/modcommonunits.h"
#include "configparser/modconfigparser.h"
#include "devicelib/phonedevice.h"
#include <configiddefine.h>

#define kszStatusRPS                "status_RPS"
// #define kszStatusFactory            "status_Factory"
// #define kszStatusFirmware           "status_Firmware"
#define kszStatusReset              "resetflag"

CDlgTestModeStatus::CDlgTestModeStatus(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pLabRPSStatus(NULL)
    , m_pLabResetStatus(NULL)
#if IF_TESTMODE_DISPLAY_FIRM
    , m_pLabFirmware(NULL)
#endif
{
    InitPageData();
}

CDlgTestModeStatus::~CDlgTestModeStatus()
{
}
void CDlgTestModeStatus::SetData(void * pData /* = NULL */)
{
    // 设置模板类型
    m_eWndType = eWT_SubPage;

    m_strTitle = "TestMode Status";
}

bool CDlgTestModeStatus::GetSoftkeyData(CArraySoftKeyBarData & objSoftData)
{
    objSoftData[3].m_strSoftkeyAction = "";
    objSoftData[3].m_strSoftkeyTitle  = "";

    return true;
}

bool CDlgTestModeStatus::OnAction(const QString & strAction)
{
    return false;
}

void CDlgTestModeStatus::InitPageData()
{
    QWidgetPair qPair;
    QString strLab;

    m_pLabRPSStatus = new QLabel(this);
    if (m_pLabRPSStatus != NULL)
    {
        strLab = "RPS Status:";
        qPair.first = strLab;
        qPair.second = m_pLabRPSStatus;
        qPair.strAuthorId = kszStatusRPS;
        m_vecWidgetPair.push_back(qPair);
    }
    m_pLabResetStatus = new QLabel(this);
    if (m_pLabResetStatus != NULL)
    {
        strLab = "Factory Status:";
        qPair.first = strLab;
        qPair.second = m_pLabResetStatus;
        qPair.strAuthorId = kszStatusRPS;
        m_vecWidgetPair.push_back(qPair);
    }
#if IF_TESTMODE_DISPLAY_FIRM
    m_pLabFirmware = new QLabel(this);
    if (m_pLabFirmware != NULL)
    {
        strLab = "Firmware:";
        qPair.first = strLab;
        qPair.second = m_pLabFirmware;
        qPair.strAuthorId = kszStatusFirmware;
        m_vecWidgetPair.push_back(qPair);
    }
#endif
    AddWidget(m_vecWidgetPair, true, false);
    //给自己安装过滤器
    this->installEventFilter(this);
}

bool CDlgTestModeStatus::LoadPageData()
{
    SetCurrentIndex(0);
    this->setFocus();
    if (NULL != m_pLabResetStatus)
    {
        yl::string strResetStatus = commUnits_GetUserEnvString(kszStatusReset);
        //判断是否恢复过出厂设置(返回值未空为未恢复)
        if (strResetStatus.empty())
        {
            m_pLabResetStatus->setText("No Restored");
        }
        else
        {
            m_pLabResetStatus->setText("Restored");
        }
    }

    if (NULL != m_pLabRPSStatus)
    {
        //判断RPS是否开启
        if (1 == configParser_GetConfigInt(kszRedirectEnable))
        {
            m_pLabRPSStatus->setText("RPS Started");
        }
        else
        {
            m_pLabRPSStatus->setText("RPS Closed");
        }
    }
#if IF_TESTMODE_DISPLAY_FIRM
    if (NULL != m_pLabFirmware)
    {
yl:
        string strFirmware  = devicelib_GetHardwareVersion();
        if (!strFirmware.empty())
        {
            m_pLabFirmware->setText(strFirmware.c_str());
        }
    }
#endif
    return true;
}

bool CDlgTestModeStatus::IsStatusChanged()
{
    return false;
}

bool CDlgTestModeStatus::SavePageData()
{
    return true;
}

