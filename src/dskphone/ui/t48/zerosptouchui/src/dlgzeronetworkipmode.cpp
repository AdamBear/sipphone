#include "dlgzeronetworkipmode.h"
#include "dlgzeronetworkwanport.h"

#include "setting/src/networkcontroller.h"
#include "setting/include/common.h"
#include "settingui/src/networksettingmanager.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qcomboboxutility.h"
#include "baseui/fontcolor.h"

#include "devicelib/phonedevice.h"
#include "devicelib/networkserver.h"
#include "include/netmsgtypedefine.h"

#include "dsklog/log.h"

Q_DECLARE_METATYPE(IP_Addr_Mode);

CDlgZeroNetworkIPMode::CDlgZeroNetworkIPMode(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_nMode(0)
{
    InitData();
}

CDlgZeroNetworkIPMode::~CDlgZeroNetworkIPMode()
{
    m_vecWidgetPair.clear();

    disconnect(m_pIPV4WANPort, SIGNAL(RefreshSoftkey()), this, SIGNAL(RefreshSoftkey()));
    disconnect(m_pIPV6WANPort, SIGNAL(RefreshSoftkey()), this, SIGNAL(RefreshSoftkey()));
}

//初始化页面属性
void CDlgZeroNetworkIPMode::Init()
{
    if (NULL == m_pIPV4WANPort || NULL == m_pIPV6WANPort)
    {
        return;
    }
    //设置页面类型，防止控件无法显示
    m_pIPV4WANPort->SetPageMode(PAGE_MODE_ZERO);
    m_pIPV6WANPort->SetPageMode(PAGE_MODE_ZERO);
}

//初始化控件
void CDlgZeroNetworkIPMode::InitData()
{
    m_pIPV4WANPort = new CDlgZeroNetworkWANPort(this);
    m_pIPV6WANPort = new CDlgZeroNetworkWANPort(this);
    m_pCmbBoxMode = new QComboBox(this);
    if (NULL == m_pCmbBoxMode || NULL == m_pIPV4WANPort || NULL == m_pIPV6WANPort)
    {
        return;
    }

    //设置网络类型
    m_pIPV4WANPort->SetMode(IPAM_IPV4);
    connect(m_pIPV4WANPort, SIGNAL(ReLayout(CBaseDialog *)), this,
            SLOT(OnWANTypeChanged(CBaseDialog *)));

    m_pIPV6WANPort->SetMode(IPAM_IPV6);
    connect(m_pIPV6WANPort, SIGNAL(ReLayout(CBaseDialog *)), this,
            SLOT(OnWANTypeChanged(CBaseDialog *)));

    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxMode, Qt::black);
    m_pCmbBoxMode->addItem(LANG_TRANSLATE(TRID_IPV4),
                           QVariant::fromValue(IPAM_IPV4));
    m_pCmbBoxMode->addItem(LANG_TRANSLATE(TRID_IPV6),
                           QVariant::fromValue(IPAM_IPV6));
    m_pCmbBoxMode->addItem(LANG_TRANSLATE(TRID_IPV4_AND_IPV6),
                           QVariant::fromValue(IPAM_BOTH));

    //为界面上各控件安装事件过滤器
    m_pCmbBoxMode->installEventFilter(this);
    m_pCmbBoxMode->setCurrentIndex(-1);
    connect(m_pCmbBoxMode, SIGNAL(currentIndexChanged(int)),
            this, SLOT(OnCmbBoxModeChanged(int)));

    connect(m_pIPV4WANPort, SIGNAL(RefreshSoftkey()), this, SIGNAL(RefreshSoftkey()));
    connect(m_pIPV6WANPort, SIGNAL(RefreshSoftkey()), this, SIGNAL(RefreshSoftkey()));
}

//加载页面数据
bool CDlgZeroNetworkIPMode::LoadPageData()
{
    if (NULL == m_pCmbBoxMode
            || NULL == m_pIPV4WANPort
            || NULL == m_pIPV6WANPort)
    {
        return true;
    }

    this->blockSignals(true);
    //获取当前的网络类型
    m_nPreMode = g_pNetworkController->GetWanIPMode();
    m_nMode = m_nPreMode;

    int nCount = 0;
    nCount = m_pCmbBoxMode->count();
    for (int i = 0; i < nCount; ++i)
    {
        if (m_nMode
                == (int)(m_pCmbBoxMode->itemData(i).value<IP_Addr_Mode>()))
        {
            m_pCmbBoxMode->setCurrentIndex(i);
            ZEROUI_INFO("CDlgZeroNetworkIPMode::LoadPageData : setCurrentIndex(%d)", i);
            break;
        }
    }

    //加载IPV4和IPV6的数据
    m_pIPV4WANPort->LoadPageData();
    m_pIPV6WANPort->LoadPageData();
    this->blockSignals(false);
    return true;
}

bool CDlgZeroNetworkIPMode::IsStatusChanged()
{
    bool bIPModeChanged = m_nPreMode != (m_pCmbBoxMode->itemData(
            m_pCmbBoxMode->currentIndex()).value<IP_Addr_Mode>());
    return bIPModeChanged || m_pIPV4WANPort->IsStatusChanged() || m_pIPV6WANPort->IsStatusChanged();
}

bool CDlgZeroNetworkIPMode::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    //判断页面输入的合法性
    switch (m_nMode)
    {
    case IPAM_IPV4:
    {
        return m_pIPV4WANPort->IsInputDataValid(objErrorContent);
    }
    break;
    case IPAM_IPV6:
    {
        return m_pIPV6WANPort->IsInputDataValid(objErrorContent);
    }
    break;
    case IPAM_BOTH:
    {
        return m_pIPV4WANPort->IsInputDataValid(objErrorContent)
               && m_pIPV6WANPort->IsInputDataValid(objErrorContent);
    }
    break;
    default:
        break;
    }
    return true;
}

bool CDlgZeroNetworkIPMode::SavePageData()
{
    if (NULL == m_pCmbBoxMode
            || NULL == m_pIPV4WANPort
            || NULL == m_pIPV6WANPort)
    {
        return true;
    }

    g_pNetworkController->SetWanIPMode(m_nMode);

    if (IsStatusChanged())
    {
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }

    //提起页面数据
    switch (m_nMode)
    {
    case IPAM_IPV4:
    {
        m_pIPV4WANPort->SavePageData();
    }
    break;
    case IPAM_IPV6:
    {
        m_pIPV6WANPort->SavePageData();
    }
    break;
    case IPAM_BOTH:
    {
        return m_pIPV4WANPort->SavePageData() && m_pIPV6WANPort->SavePageData();
    }
    default:
        break;
    }

    return true;
}


void CDlgZeroNetworkIPMode::ReturnAllEdit()
{
    if (NULL != m_pIPV6WANPort
            && NULL != m_pIPV4WANPort)
    {
        //将预输入的字符输入到控件中
        m_pIPV4WANPort->ReturnAllEdit();
        m_pIPV6WANPort->ReturnAllEdit();
    }
}

void CDlgZeroNetworkIPMode::UninstallAllIME()
{
    if (NULL != m_pIPV6WANPort
            && NULL != m_pIPV4WANPort)
    {
        m_pIPV4WANPort->UninstallAllIME();
        m_pIPV6WANPort->UninstallAllIME();
    }
}

//IP type配置选择类型变化处理操作
void CDlgZeroNetworkIPMode::OnCmbBoxModeChanged(int nIndex)
{
    DoReLayout();
    emit ReLayout(this, 0);
}

void CDlgZeroNetworkIPMode::DoReLayout()
{
    QString strLab = "";
    QWidgetPair qPair;
    //先清空控件列表
    MakeReadyForRelayout();

    int nIndex = m_pCmbBoxMode->currentIndex();
    //记录当前ip 配置类型
    m_nMode = (int)(m_pCmbBoxMode->itemData(nIndex).value<IP_Addr_Mode>());

    strLab = LANG_TRANSLATE(TRID_IP_ADDRESS_MODE);
    qPair.first = strLab;
    qPair.second = m_pCmbBoxMode;
    m_vecWidgetPair.push_back(qPair);

    //先隐藏
    m_pIPV4WANPort->hideWidget();
    m_pIPV6WANPort->hideWidget();

    //更具类型决定要加入那种网络的数据进行显示
    switch (m_nMode)
    {
    case IPAM_IPV4:
    {
        m_vecWidgetPair += m_pIPV4WANPort->GetWPVector();
    }
    break;
    case IPAM_IPV6:
    {
        m_vecWidgetPair += m_pIPV6WANPort->GetWPVector();
    }
    break;
    case IPAM_BOTH:
    {
        m_vecWidgetPair += m_pIPV4WANPort->GetWPVector();
        m_vecWidgetPair += m_pIPV6WANPort->GetWPVector();
    }
    break;
    default:
        break;
    }
}


void CDlgZeroNetworkIPMode::OnWANTypeChanged(CBaseDialog * pDlg)
{
    if (NULL == pDlg)
    {
        return;
    }
    //重调布局
    DoReLayout();

    //设置选中行
    int iSeletIndex;
    if (pDlg == m_pIPV4WANPort)
    {
        iSeletIndex = 1;
    }
    else if (pDlg == m_pIPV6WANPort)
    {
        if (IPAM_IPV6 == m_nMode)
        {
            iSeletIndex = 1;
        }
        else //IPAM_BOTH
        {
            iSeletIndex = m_pIPV4WANPort->GetWPVector().size() + 1;
        }
    }

    //发消息选中行指定行
    emit ReLayout(this, iSeletIndex);

}
