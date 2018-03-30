#include "dlgzeronetwork_cdp.h"
#include "uicommon/qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "uicommon/uikernel/qlineeditutility.h"
#include "uikernel/qcheckbox.h"
#include "uicommon/uikernel/languagehelper.h"
#include "settingui/src/networksettingmanager.h"
#include "setting/src/networkcontroller.h"
#include "uicommon/inputmethod/inputmethod.h"

#include "dsklog/log.h"


CDlgZeroNetwork_CDP::CDlgZeroNetwork_CDP(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCheckBoxCDPStatus(NULL)
    , m_pEditInterval(NULL)
{
    InitData();
}

CDlgZeroNetwork_CDP::~CDlgZeroNetwork_CDP()
{
    qUninstallIME(m_pEditInterval);
    m_vecWidgetPair.clear();
}

void CDlgZeroNetwork_CDP::InitData()
{
    m_pCheckBoxCDPStatus = new qCheckBox(this);
    m_pEditInterval = new QLineEdit(this);
    if (NULL == m_pCheckBoxCDPStatus
            || NULL == m_pEditInterval)
    {
        return ;
    }

    // 设置VLAN DHCP Option控件风格
    SetLineEditDefaultStyle(m_pEditInterval);
    qInstallIME(m_pEditInterval, IME_DIGIT);
    m_pEditInterval->setMaxLength(4);

    // 安装过滤器
    m_pCheckBoxCDPStatus->installEventFilter(this);
    m_pEditInterval->installEventFilter(this);
    this->installEventFilter(this);

    connect(m_pCheckBoxCDPStatus, SIGNAL(checkChanged(bool)), this,
            SLOT(OnChkBoxEnableCDPChanged(bool)));
}

bool CDlgZeroNetwork_CDP::LoadPageData()
{
    if (NULL == m_pCheckBoxCDPStatus
            || NULL == m_pEditInterval)
    {
        return true;
    }

    this->blockSignals(true);
    NetworkCDPData currentPageData;
    g_pNetworkController->GetCDPData(&currentPageData);

    SetCurrentPageData(currentPageData);
    this->blockSignals(false);

    return true;
}

bool CDlgZeroNetwork_CDP::IsStatusChanged()
{
    NetworkCDPData currentPageData;
    NetworkCDPData beforePageData;

    GetCurrentPageData(currentPageData);
    g_pNetworkController->GetCDPData(&beforePageData);

    return !ComparePageData(beforePageData, currentPageData);
}

bool CDlgZeroNetwork_CDP::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (NULL == m_pEditInterval || NULL == m_pCheckBoxCDPStatus)
    {
        return true;
    }

    bool bEnable = m_pCheckBoxCDPStatus->GetData();
    if (bEnable)
    {
        // 判断Interval输入的值是否合理
        if (m_pEditInterval->text().trimmed().isEmpty()
                || !IsVaildVlanCDPInterval(m_pEditInterval->text()))
        {
            objErrorContent.strErrorContent = TRID_CDP_INTERVAL_SHOULD_BE_1_TO_3600;
            objErrorContent.pLineEdit = m_pEditInterval;
            return false;
        }
    }

    return true;
}

bool CDlgZeroNetwork_CDP::SavePageData()
{
    if (NULL == m_pCheckBoxCDPStatus
            || NULL == m_pEditInterval)
    {
        return true;
    }

    if (IsStatusChanged())
    {
        NetworkCDPData currentPageData;
        GetCurrentPageData(currentPageData);
        if (!currentPageData.m_bEnable)
        {
            NetworkCDPData logicPageData;

            g_pNetworkController->GetCDPData(&logicPageData);
            logicPageData.m_bEnable = currentPageData.m_bEnable;

            g_pNetworkController->SetCDPData(logicPageData);
        }
        else
        {
            // 调用控制层：保存数据
            g_pNetworkController->SetCDPData(currentPageData);
        }

        // 设置网络改变标记
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }

    return true;
}

void CDlgZeroNetwork_CDP::ReturnAllEdit()
{
    if (NULL != m_pEditInterval)
    {
        qReturnEdit(m_pEditInterval);
    }
}

void CDlgZeroNetwork_CDP::UninstallAllIME()
{
    if (NULL != m_pEditInterval)
    {
        qUninstallIME(m_pEditInterval);
    }
}

bool CDlgZeroNetwork_CDP::ComparePageData(const NetworkCDPData & beforePageData,
        const NetworkCDPData & afterPageData) const
{
    if (beforePageData.m_bEnable != afterPageData.m_bEnable
            || (afterPageData.m_bEnable && beforePageData.m_nCDPInterval != afterPageData.m_nCDPInterval))
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

NetworkCDPData & CDlgZeroNetwork_CDP::GetCurrentPageData(NetworkCDPData & currentPageData) const
{
    currentPageData.m_bEnable = m_pCheckBoxCDPStatus->GetData();

    currentPageData.m_nCDPInterval = m_pEditInterval->text().trimmed().toInt();

    return currentPageData;
}

void CDlgZeroNetwork_CDP::SetCurrentPageData(NetworkCDPData & currentPageData)
{
    m_pCheckBoxCDPStatus->SetData(currentPageData.m_bEnable);

    QString strInterval = QString("%1").arg(currentPageData.m_nCDPInterval);
    m_pEditInterval->setText(strInterval);
}


bool CDlgZeroNetwork_CDP::IsVaildVlanCDPInterval(const QString & strValue)
{
    int iValue = 0;
    iValue = strValue.toInt();
    if (strValue.isEmpty() || iValue < 1 || iValue > 3600)
    {
        return false;
    }

    return true;
}

void CDlgZeroNetwork_CDP::OnChkBoxEnableCDPChanged(bool bOn)
{
    m_pEditInterval->hide();

    QString strLab = "";
    QWidgetPair qPair;
    m_vecWidgetPair.clear();

    strLab = LANG_TRANSLATE(TRID_CDP_STATUS);
    qPair.first = strLab;
    qPair.second = m_pCheckBoxCDPStatus;
    m_vecWidgetPair.push_back(qPair);

    if (bOn)
    {
        strLab = LANG_TRANSLATE(TRID_CDP_INTERVAL);
        qPair.first = strLab;
        qPair.second = m_pEditInterval;
        m_vecWidgetPair.push_back(qPair);
    }

    emit ReLayout(this);
}
