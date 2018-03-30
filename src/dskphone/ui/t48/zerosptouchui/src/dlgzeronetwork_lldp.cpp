#include "dlgzeronetwork_lldp.h"
#include "uicommon/qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "uicommon/uikernel/qlineeditutility.h"
#include "uikernel/qcheckbox.h"
#include "uicommon/uikernel/languagehelper.h"
#include "settingui/src/networksettingmanager.h"
#include "setting/src/networkcontroller.h"
#include "uicommon/inputmethod/inputmethod.h"

#include "dsklog/log.h"


CDlgZeroNetwork_LLDP::CDlgZeroNetwork_LLDP(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCheckBoxLLDPStatus(NULL)
    , m_pEditPacketInterval(NULL)
{
    InitData();
}

CDlgZeroNetwork_LLDP::~CDlgZeroNetwork_LLDP()
{
    qUninstallIME(m_pEditPacketInterval);
    m_vecWidgetPair.clear();
}

void CDlgZeroNetwork_LLDP::InitData()
{
    m_pCheckBoxLLDPStatus = new qCheckBox(this);
    m_pEditPacketInterval = new QLineEdit(this);
    if (NULL == m_pCheckBoxLLDPStatus
            || NULL == m_pEditPacketInterval)
    {
        return ;
    }

    // 设置Packet Interval控件风格
    SetLineEditDefaultStyle(m_pEditPacketInterval);
    qInstallIME(m_pEditPacketInterval, IME_123);
    m_pEditPacketInterval->setMaxLength(4);

    // 安装过滤器
    m_pCheckBoxLLDPStatus->installEventFilter(this);
    m_pEditPacketInterval->installEventFilter(this);
    this->installEventFilter(this);

    connect(m_pCheckBoxLLDPStatus, SIGNAL(checkChanged(bool)), this,
            SLOT(OnChkBoxEnableLLDPChanged(bool)));
}

bool CDlgZeroNetwork_LLDP::LoadPageData()
{
    if (NULL == m_pCheckBoxLLDPStatus
            || NULL == m_pEditPacketInterval)
    {
        return true;
    }

    this->blockSignals(true);
    // 调用控制层：加载数据
    NetworkLLDPData currentPageData;
    g_pNetworkController->GetLLDPData(&currentPageData);

    // 设置页面数据
    SetCurrentPageData(currentPageData);
    this->blockSignals(false);

    return true;
}

bool CDlgZeroNetwork_LLDP::IsStatusChanged()
{
    // 加载数据
    NetworkLLDPData currentPageData;
    NetworkLLDPData beforePageData;

    GetCurrentPageData(currentPageData);
    g_pNetworkController->GetLLDPData(&beforePageData);

    // 比较前后数据
    return !ComparePageData(beforePageData, currentPageData);
}

bool CDlgZeroNetwork_LLDP::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (NULL == m_pEditPacketInterval || NULL == m_pCheckBoxLLDPStatus)
    {
        return true;
    }

    bool  bEnable = m_pCheckBoxLLDPStatus->GetData();
    if (bEnable)
    {
        // 时间间隔在1-3600s内
        if (m_pEditPacketInterval->text().trimmed().isEmpty()
                || m_pEditPacketInterval->text().toInt() < 1
                || m_pEditPacketInterval->text().toInt() > 3600)
        {
            objErrorContent.strErrorContent = TRID_LLDP_PACKET_INTERVAL_SHOULD_BE_1_TO_3600;
            objErrorContent.pLineEdit = m_pEditPacketInterval;
            return false;
        }
    }

    return true;
}

bool CDlgZeroNetwork_LLDP::SavePageData()
{
    if (NULL == m_pCheckBoxLLDPStatus
            || NULL == m_pEditPacketInterval)
    {
        return true;
    }

    if (IsStatusChanged())
    {
        NetworkLLDPData currentPageData;
        GetCurrentPageData(currentPageData);
        if (!currentPageData.m_bEnable)
        {
            NetworkLLDPData logicPageData;

            GetCurrentPageData(currentPageData);
            g_pNetworkController->GetLLDPData(&logicPageData);
            logicPageData.m_bEnable = currentPageData.m_bEnable;

            g_pNetworkController->SetLLDPData(logicPageData);
        }
        else
        {
            // 调用控制层：保存数据
            g_pNetworkController->SetLLDPData(currentPageData);
        }

        // 设置网络改变标记
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }

    return true;
}

void CDlgZeroNetwork_LLDP::ReturnAllEdit()
{
    if (NULL != m_pEditPacketInterval)
    {
        qReturnEdit(m_pEditPacketInterval);
    }
}

void CDlgZeroNetwork_LLDP::UninstallAllIME()
{
    if (NULL != m_pEditPacketInterval)
    {
        qUninstallIME(m_pEditPacketInterval);
    }
}

bool CDlgZeroNetwork_LLDP::ComparePageData(const NetworkLLDPData & beforePageData,
        const NetworkLLDPData & afterPageData) const
{
    if (beforePageData.m_bEnable != afterPageData.m_bEnable
            || (afterPageData.m_bEnable && beforePageData.m_nPacketInterval != afterPageData.m_nPacketInterval))
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

NetworkLLDPData & CDlgZeroNetwork_LLDP::GetCurrentPageData(NetworkLLDPData & currentPageData) const
{
    currentPageData.m_bEnable = m_pCheckBoxLLDPStatus->GetData();

    currentPageData.m_nPacketInterval = m_pEditPacketInterval->text().trimmed().toInt();

    return currentPageData;
}

void CDlgZeroNetwork_LLDP::SetCurrentPageData(NetworkLLDPData & currentPageData)
{
    m_pCheckBoxLLDPStatus->SetData(currentPageData.m_bEnable);

    m_pEditPacketInterval->setText(QString::number(currentPageData.m_nPacketInterval));
}

void CDlgZeroNetwork_LLDP::OnChkBoxEnableLLDPChanged(bool bOn)
{
    m_pEditPacketInterval->hide();

    QString strLab = "";
    QWidgetPair qPair;
    m_vecWidgetPair.clear();

    strLab = LANG_TRANSLATE(TRID_LLDP_STATUS);
    qPair.first = strLab;
    qPair.second = m_pCheckBoxLLDPStatus;
    m_vecWidgetPair.push_back(qPair);

    if (bOn)
    {
        strLab = LANG_TRANSLATE(TRID_LLDP_PACKETINTERVAL);
        qPair.first = strLab;
        qPair.second = m_pEditPacketInterval;
        m_vecWidgetPair.push_back(qPair);
    }

    emit ReLayout(this);
}
