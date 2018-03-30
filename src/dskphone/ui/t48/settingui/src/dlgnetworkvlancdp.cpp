#include "dlgnetworkvlancdp.h"
#include "uicommon/qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "uicommon/uikernel/qlineeditutility.h"
#include "uicommon/uikernel/languagehelper.h"
#include "networksettingmanager.h"
#include "setting/src/networkcontroller.h"
#include "uicommon/inputmethod/inputmethod.h"
#include "uikernel/qcheckbox.h"

namespace
{
#define MAX_LEN_CDP_INTERVAL 4 //CDP interval最大输入长度
}

///////////////////////////////////////////////////////////////////////////////
CDlgNetworkVlanCDP::CDlgNetworkVlanCDP(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCheckBoxCDPStatus(NULL)
    , m_pEditInterval(NULL)
{
    InitData();
}

CDlgNetworkVlanCDP::~CDlgNetworkVlanCDP()
{
}

void CDlgNetworkVlanCDP::InitData()
{
    m_pCheckBoxCDPStatus = new qCheckBox(this);
    m_pEditInterval = new QLineEdit(this);
    if (NULL == m_pCheckBoxCDPStatus
            || NULL == m_pEditInterval)
    {
        return ;
    }

    SetLineEditDefaultStyle(m_pEditInterval);
    qInstallIME(m_pEditInterval, IME_123);
    m_pEditInterval->setMaxLength(MAX_LEN_CDP_INTERVAL);

    // 安装过滤器
    m_pCheckBoxCDPStatus->installEventFilter(this);
    m_pEditInterval->installEventFilter(this);
    this->installEventFilter(this);

    // 显示控件
    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_CDP_STATUS);
    qPair.first = strLab;
    qPair.second = m_pCheckBoxCDPStatus;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_CDP_INTERVAL);
    qPair.first = strLab;
    qPair.second = m_pEditInterval;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);
}

bool CDlgNetworkVlanCDP::LoadPageData()
{
    this->setFocus();

    // 调用控制层：加载数据
    NetworkCDPData currentPageData;
    g_pNetworkController->GetCDPData(&currentPageData);

    // 设置页面数据
    SetCurrentPageData(currentPageData);

    return true;
}

bool CDlgNetworkVlanCDP::IsStatusChanged()
{
    // 加载数据
    NetworkCDPData currentPageData;
    NetworkCDPData beforePageData;

    GetCurrentPageData(currentPageData);
    g_pNetworkController->GetCDPData(&beforePageData);

    // 比较前后数据
    return !ComparePageData(beforePageData, currentPageData);
}

bool CDlgNetworkVlanCDP::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (NULL == m_pEditInterval)
    {
        return true;
    }

    // 判断Interval输入的值是否合理
    if (m_pEditInterval->text().trimmed().isEmpty()
            || m_pEditInterval->text().toInt() < 1
            || m_pEditInterval->text().toInt() > 3600)
    {
        objErrorContent.strErrorContent = TRID_CDP_INTERVAL_SHOULD_BE_1_TO_3600;
        objErrorContent.pLineEdit = m_pEditInterval;
        return false;
    }

    return true;
}

bool CDlgNetworkVlanCDP::SavePageData()
{
    if (IsStatusChanged())
    {
        NetworkCDPData currentPageData;
        GetCurrentPageData(currentPageData);

        // 调用控制层：保存数据
        g_pNetworkController->SetCDPData(currentPageData);

        // 设置网络改变标记
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }

    return true;
}

void CDlgNetworkVlanCDP::ReturnAllEdit()
{
    qReturnEdit(m_pEditInterval);
}

void CDlgNetworkVlanCDP::UninstallAllIME()
{
    qUninstallIME(m_pEditInterval);
}

bool CDlgNetworkVlanCDP::ComparePageData(const NetworkCDPData & beforePageData,
        const NetworkCDPData & afterPageData) const
{
    if (beforePageData.m_bEnable != afterPageData.m_bEnable
            || beforePageData.m_nCDPInterval != afterPageData.m_nCDPInterval)
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

NetworkCDPData & CDlgNetworkVlanCDP::GetCurrentPageData(NetworkCDPData & currentPageData) const
{
    currentPageData.m_bEnable = m_pCheckBoxCDPStatus->GetData();

    currentPageData.m_nCDPInterval = m_pEditInterval->text().trimmed().toInt();

    return currentPageData;
}

void CDlgNetworkVlanCDP::SetCurrentPageData(NetworkCDPData & currentPageData)
{
    m_pCheckBoxCDPStatus->SetData(currentPageData.m_bEnable);

    QString strInterval = QString("%1").arg(currentPageData.m_nCDPInterval);

    m_pEditInterval->setText(strInterval);
}
