#include "cdlgnetwork_vlandhcp.h"
#include "uicommon/qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "uicommon/uikernel/qlineeditutility.h"
#include "uicommon/uikernel/languagehelper.h"
#include "networksettingmanager.h"
#include "setting/src/networkcontroller.h"
#include "uicommon/inputmethod/inputmethod.h"
#include "uikernel/qcheckbox.h"

#define MAX_LEN_VLAN_DHCP_OPTION 128 //VLAN DHCP OPTION最大长度

///////////////////////////////////////////////////////////////////////////////
CDlgNetwork_VLANDHCP::CDlgNetwork_VLANDHCP(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCheckBoxVlanDhcp(NULL)
    , m_pEditOption(NULL)
{
    InitData();
}

CDlgNetwork_VLANDHCP::~CDlgNetwork_VLANDHCP()
{
}

void CDlgNetwork_VLANDHCP::InitData()
{
    m_pCheckBoxVlanDhcp = new qCheckBox(this);
    m_pEditOption = new QLineEdit(this);
    if (NULL == m_pCheckBoxVlanDhcp
            || NULL == m_pEditOption)
    {
        return ;
    }

    // 设置VLAN DHCP控件风格
//  qComboBoxUtility::SetComboBoxStyle(m_pCheckBoxVlanDhcp);
//  m_pCheckBoxVlanDhcp->addItem(LANG_TRANSLATE(TRID_DISABLED), false);
//  m_pCheckBoxVlanDhcp->addItem(LANG_TRANSLATE(TRID_ENABLED), true);

    // 设置VLAN DHCP Option控件风格
    SetLineEditDefaultStyle(m_pEditOption);
    qInstallIME(m_pEditOption, IME_DIGIT);
    m_pEditOption->setMaxLength(MAX_LEN_VLAN_DHCP_OPTION);

    // 安装过滤器
    m_pCheckBoxVlanDhcp->installEventFilter(this);
    m_pEditOption->installEventFilter(this);
    this->installEventFilter(this);

    // 显示控件
    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_VLAN_DHCP);
    qPair.first = strLab;
    qPair.second = m_pCheckBoxVlanDhcp;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_VLAN_DHCP_OPTION);
    qPair.first = strLab;
    qPair.second = m_pEditOption;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);
}

bool CDlgNetwork_VLANDHCP::LoadPageData()
{
    this->setFocus();

    // 调用控制层：加载数据
    NetworkVlanDHCPData currentPageData;
    g_pNetworkController->GetVlanDHCPData(&currentPageData);

    // 设置页面数据
    SetCurrentPageData(currentPageData);

    return true;
}

bool CDlgNetwork_VLANDHCP::IsStatusChanged()
{
    // 加载数据
    NetworkVlanDHCPData currentPageData;
    NetworkVlanDHCPData beforePageData;

    GetCurrentPageData(currentPageData);
    g_pNetworkController->GetVlanDHCPData(&beforePageData);

    // 比较前后数据
    return !ComparePageData(beforePageData, currentPageData);
}

bool CDlgNetwork_VLANDHCP::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (NULL == m_pEditOption || NULL == m_pCheckBoxVlanDhcp)
    {
        return true;
    }

    // 判断Option输入的值是否合理
    if (m_pEditOption->text().trimmed().isEmpty()
            || !IsVaildVlanDhcpOption(m_pEditOption->text()))
    {
        objErrorContent.strErrorContent = TRID_DHCP_OPTION_INVAILD;
        objErrorContent.pLineEdit = m_pEditOption;
        return false;
    }
    else if (IsDhcpOptionCountsMoreThanFive(m_pEditOption->text()))
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(
                                              TRID_T20_THE_COUNT_OF_DHCP_OPTION_VALUE_CANNOT_BE_MORE_THAN_5_PART1)
                                          +  QLatin1String(" ")
                                          + LANG_TRANSLATE(TRID_T20_THE_COUNT_OF_DHCP_OPTION_VALUE_CANNOT_BE_MORE_THAN_5_PART2);
        objErrorContent.pLineEdit = m_pEditOption;
        return false;
    }

    return true;
}

bool CDlgNetwork_VLANDHCP::SavePageData()
{
    if (IsStatusChanged())
    {
        NetworkVlanDHCPData currentPageData;
        GetCurrentPageData(currentPageData);

        // 调用控制层：保存数据
        g_pNetworkController->SetVlanDHCPData(currentPageData);

        // 设置网络改变标记
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }

    return true;
}

void CDlgNetwork_VLANDHCP::ReturnAllEdit()
{
    qReturnEdit(m_pEditOption);
}

void CDlgNetwork_VLANDHCP::UninstallAllIME()
{
    qUninstallIME(m_pEditOption);
}

bool CDlgNetwork_VLANDHCP::ComparePageData(const NetworkVlanDHCPData & beforePageData,
        const NetworkVlanDHCPData & afterPageData) const
{
    if (beforePageData.m_bEnable != afterPageData.m_bEnable
            || beforePageData.m_strOption != afterPageData.m_strOption)
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

NetworkVlanDHCPData & CDlgNetwork_VLANDHCP::GetCurrentPageData(NetworkVlanDHCPData &
        currentPageData) const
{
    currentPageData.m_bEnable = m_pCheckBoxVlanDhcp->GetData();

    currentPageData.m_strOption = m_pEditOption->text().trimmed().toUtf8().data();

    return currentPageData;
}

void CDlgNetwork_VLANDHCP::SetCurrentPageData(NetworkVlanDHCPData & currentPageData)
{
    m_pCheckBoxVlanDhcp->SetData(currentPageData.m_bEnable);

    m_pEditOption->setText(currentPageData.m_strOption.c_str());
}


bool CDlgNetwork_VLANDHCP::IsVaildVlanDhcpOption(const QString & strValue)
{
    QString strCode(strValue);
    QRegExp expr("(([1-9]\\d?|0[1-9]\\d?|00[1-9]|1\\d\\d|2[0-4]\\d|25[0-5]),){0,}([1-9]\\d?|0[1-9]\\d?|00[1-9]|1\\d\\d|2[0-4]\\d|25[0-5])");
    return expr.exactMatch(strCode);
}

bool CDlgNetwork_VLANDHCP::IsDhcpOptionCountsMoreThanFive(const QString & strValue)
{
    return strValue.count(",") > 4;
}
