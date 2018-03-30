#include "cdlgnetwork_lldp.h"

#include "uicommon/qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "uicommon/uikernel/qlineeditutility.h"
#include "uicommon/uikernel/languagehelper.h"
#include "networksettingmanager.h"
#include "setting/src/networkcontroller.h"
#include "uicommon/inputmethod/inputmethod.h"
#include "uikernel/qcheckbox.h"

///////////////////////////////////////////////////////////////////////////////
CDlgNetwork_LLDP::CDlgNetwork_LLDP(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCheckBoxLLDPStatus(NULL)
    , m_pEditPacketInterval(NULL)
{
    InitData();
}

CDlgNetwork_LLDP::~CDlgNetwork_LLDP()
{
}

void CDlgNetwork_LLDP::InitData()
{
    m_pCheckBoxLLDPStatus = new qCheckBox(this);
    m_pEditPacketInterval = new QLineEdit(this);
    if (NULL == m_pCheckBoxLLDPStatus
            || NULL == m_pEditPacketInterval)
    {
        return ;
    }

    // 设置LLDP Status控件风格
//  qComboBoxUtility::SetComboBoxStyle(m_pCheckBoxLLDPStatus);
//  m_pCheckBoxLLDPStatus->addItem(LANG_TRANSLATE(TRID_DISABLED), false);
//  m_pCheckBoxLLDPStatus->addItem(LANG_TRANSLATE(TRID_ENABLED), true);

    // 设置Packet Interval控件风格
    SetLineEditDefaultStyle(m_pEditPacketInterval);
    qInstallIME(m_pEditPacketInterval, IME_123);
    m_pEditPacketInterval->setMaxLength(4);

    // 安装过滤器
    m_pCheckBoxLLDPStatus->installEventFilter(this);
    m_pEditPacketInterval->installEventFilter(this);
    this->installEventFilter(this);

    // 显示控件
    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_LLDP_STATUS);
    qPair.first = strLab;
    qPair.second = m_pCheckBoxLLDPStatus;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_LLDP_PACKETINTERVAL);
    qPair.first = strLab;
    qPair.second = m_pEditPacketInterval;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);
}

bool CDlgNetwork_LLDP::LoadPageData()
{
    this->setFocus();

    // 调用控制层：加载数据
    NetworkLLDPData currentPageData;
    g_pNetworkController->GetLLDPData(&currentPageData);

    // 设置页面数据
    SetCurrentPageData(currentPageData);

    return true;
}

bool CDlgNetwork_LLDP::IsStatusChanged()
{
    // 加载数据
    NetworkLLDPData currentPageData;
    NetworkLLDPData beforePageData;

    GetCurrentPageData(currentPageData);
    g_pNetworkController->GetLLDPData(&beforePageData);

    // 比较前后数据
    return !ComparePageData(beforePageData, currentPageData);
}

bool CDlgNetwork_LLDP::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (NULL == m_pEditPacketInterval)
    {
        return true;
    }

    // 时间间隔在1-3600s内
    if (m_pEditPacketInterval->text().trimmed().isEmpty()
            || m_pEditPacketInterval->text().toInt() < 1
            || m_pEditPacketInterval->text().toInt() > 3600)
    {
        objErrorContent.strErrorContent = TRID_LLDP_PACKET_INTERVAL_SHOULD_BE_1_TO_3600;
        objErrorContent.pLineEdit = m_pEditPacketInterval;
        return false;
    }

    return true;
}

bool CDlgNetwork_LLDP::SavePageData()
{
    if (IsStatusChanged())
    {
        NetworkLLDPData currentPageData;
        GetCurrentPageData(currentPageData);

        // 调用控制层：保存数据
        g_pNetworkController->SetLLDPData(currentPageData);

        // 设置网络改变标记
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }

    return true;
}

void CDlgNetwork_LLDP::ReturnAllEdit()
{
    qReturnEdit(m_pEditPacketInterval);
}

void CDlgNetwork_LLDP::UninstallAllIME()
{
    qUninstallIME(m_pEditPacketInterval);
}

bool CDlgNetwork_LLDP::ComparePageData(const NetworkLLDPData & beforePageData,
                                       const NetworkLLDPData & afterPageData) const
{
    if (beforePageData.m_bEnable != afterPageData.m_bEnable
            || beforePageData.m_nPacketInterval != afterPageData.m_nPacketInterval)
    {
        // 如果不相等那么返回flase
        return false;
    }
    else
    {
        // 相等返回true
        return true;
    }
}

NetworkLLDPData & CDlgNetwork_LLDP::GetCurrentPageData(NetworkLLDPData & currentPageData) const
{
    currentPageData.m_bEnable = m_pCheckBoxLLDPStatus->GetData();

    currentPageData.m_nPacketInterval = m_pEditPacketInterval->text().trimmed().toInt();

    return currentPageData;
}

void CDlgNetwork_LLDP::SetCurrentPageData(NetworkLLDPData & currentPageData)
{
    m_pCheckBoxLLDPStatus->SetData(currentPageData.m_bEnable);

    m_pEditPacketInterval->setText(QString::number(currentPageData.m_nPacketInterval));
}

