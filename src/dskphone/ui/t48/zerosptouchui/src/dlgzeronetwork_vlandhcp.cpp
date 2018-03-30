#include "dlgzeronetwork_vlandhcp.h"
#include "uicommon/qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "uicommon/uikernel/qlineeditutility.h"
#include "uikernel/qcheckbox.h"
#include "uicommon/uikernel/languagehelper.h"
#include "settingui/src/networksettingmanager.h"
#include "setting/src/networkcontroller.h"
#include "uicommon/inputmethod/inputmethod.h"

#define MAX_LEN_VLAN_DHCP_OPTION 128 //VLAN DHCP OPTION最大长度


DlgZeroNetwork_VLANDHCP::DlgZeroNetwork_VLANDHCP(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCheckBoxVlanDhcp(NULL)
    , m_pEditOption(NULL)
{
    InitData();
}

DlgZeroNetwork_VLANDHCP::~DlgZeroNetwork_VLANDHCP()
{
    UninstallAllIME();
    m_vecWidgetPair.clear();
}

void DlgZeroNetwork_VLANDHCP::InitData()
{
    m_pCheckBoxVlanDhcp = new qCheckBox(this);
    m_pEditOption = new QLineEdit(this);
    if (NULL == m_pCheckBoxVlanDhcp
            || NULL == m_pEditOption)
    {
        return ;
    }

    // 设置VLAN DHCP Option控件风格
    SetLineEditDefaultStyle(m_pEditOption);
    qInstallIME(m_pEditOption, IME_DIGIT);
    m_pEditOption->setMaxLength(MAX_LEN_VLAN_DHCP_OPTION);

    // 安装过滤器
    m_pCheckBoxVlanDhcp->installEventFilter(this);
    m_pEditOption->installEventFilter(this);
    this->installEventFilter(this);

    connect(m_pCheckBoxVlanDhcp, SIGNAL(checkChanged(bool)), this,
            SLOT(OnChkBoxEnableVLANDHCPChanged(bool)));
}

bool DlgZeroNetwork_VLANDHCP::LoadPageData()
{
    //this->setFocus();
    if (NULL == m_pCheckBoxVlanDhcp
            || NULL == m_pEditOption)
    {
        return true;
    }

    this->blockSignals(true);
    // 调用控制层：加载数据
    NetworkVlanDHCPData currentPageData;
    g_pNetworkController->GetVlanDHCPData(&currentPageData);

    // 设置页面数据
    SetCurrentPageData(currentPageData);
    this->blockSignals(false);

    return true;
}

bool DlgZeroNetwork_VLANDHCP::IsStatusChanged()
{
    // 加载数据
    NetworkVlanDHCPData currentPageData;
    NetworkVlanDHCPData beforePageData;

    GetCurrentPageData(currentPageData);
    g_pNetworkController->GetVlanDHCPData(&beforePageData);

    // 比较前后数据
    return !ComparePageData(beforePageData, currentPageData);
}

bool DlgZeroNetwork_VLANDHCP::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (NULL == m_pEditOption || NULL == m_pCheckBoxVlanDhcp)
    {
        return true;
    }

    bool bEnable = m_pCheckBoxVlanDhcp->GetData();
    if (bEnable)
    {
        // 判断Option输入的值是否合理
        if (m_pEditOption->text().trimmed().isEmpty()
                || !IsVaildVlanDhcpOption(m_pEditOption->text()))
        {
            objErrorContent.strErrorContent = TRID_DHCP_OPTION_INVAILD;
            objErrorContent.pLineEdit = m_pEditOption;
            return false;
        }
    }

    return true;
}

bool DlgZeroNetwork_VLANDHCP::SavePageData()
{
    if (NULL == m_pCheckBoxVlanDhcp
            || NULL == m_pEditOption)
    {
        return true;
    }

    if (IsStatusChanged())
    {
        NetworkVlanDHCPData currentPageData;
        GetCurrentPageData(currentPageData);
        if (!currentPageData.m_bEnable)
        {
            NetworkVlanDHCPData logicPageData;

            g_pNetworkController->GetVlanDHCPData(&logicPageData);
            logicPageData.m_bEnable = currentPageData.m_bEnable;

            g_pNetworkController->SetVlanDHCPData(logicPageData);
        }
        else
        {
            // 调用控制层：保存数据
            g_pNetworkController->SetVlanDHCPData(currentPageData);
        }

        // 调用控制层：保存数据
        g_pNetworkController->SetVlanDHCPData(currentPageData);

        // 设置网络改变标记
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }

    return true;
}

void DlgZeroNetwork_VLANDHCP::ReturnAllEdit()
{
    if (NULL != m_pEditOption)
    {
        qReturnEdit(m_pEditOption);
    }
}

void DlgZeroNetwork_VLANDHCP::UninstallAllIME()
{
    if (NULL != m_pEditOption)
    {
        qUninstallIME(m_pEditOption);
    }
}

bool DlgZeroNetwork_VLANDHCP::ComparePageData(const NetworkVlanDHCPData & beforePageData,
        const NetworkVlanDHCPData & afterPageData) const
{
    if (beforePageData.m_bEnable != afterPageData.m_bEnable
            || (afterPageData.m_bEnable && beforePageData.m_strOption != afterPageData.m_strOption))
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

NetworkVlanDHCPData & DlgZeroNetwork_VLANDHCP::GetCurrentPageData(NetworkVlanDHCPData &
        currentPageData) const
{
    currentPageData.m_bEnable = m_pCheckBoxVlanDhcp->GetData();

    currentPageData.m_strOption = m_pEditOption->text().trimmed().toUtf8().data();

    return currentPageData;
}

void DlgZeroNetwork_VLANDHCP::SetCurrentPageData(NetworkVlanDHCPData & currentPageData)
{
    m_pCheckBoxVlanDhcp->SetData(currentPageData.m_bEnable);

    m_pEditOption->setText(currentPageData.m_strOption.c_str());
}


bool DlgZeroNetwork_VLANDHCP::IsVaildVlanDhcpOption(const QString & strValue)
{
    QString strCode(strValue);
    QRegExp expr("(([1-9]\\d?|0[1-9]\\d?|00[1-9]|1\\d\\d|2[0-4]\\d|25[0-5]),){0,4}([1-9]\\d?|0[1-9]\\d?|00[1-9]|1\\d\\d|2[0-4]\\d|25[0-5])");
    return expr.exactMatch(strCode);
}

void DlgZeroNetwork_VLANDHCP::OnChkBoxEnableVLANDHCPChanged(bool bOn)
{
    m_pEditOption->hide();

    QString strLab = "";
    QWidgetPair qPair;
    m_vecWidgetPair.clear();

    strLab = LANG_TRANSLATE(TRID_VLAN_DHCP);
    qPair.first = strLab;
    qPair.second = m_pCheckBoxVlanDhcp;
    m_vecWidgetPair.push_back(qPair);

    if (bOn)
    {
        strLab = LANG_TRANSLATE(TRID_VLAN_DHCP_OPTION);
        qPair.first = strLab;
        qPair.second = m_pEditOption;
        m_vecWidgetPair.push_back(qPair);
    }

    emit ReLayout(this);
}
