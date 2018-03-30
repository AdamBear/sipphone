#include "model.h"
#include "cdlgnetworkpcport.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "setting/src/languagecontroller.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "mainwnd/mainwnd.h"
#include "networksettingmanager.h"
#include "setting/src/networkcontroller.h"
#include "settingui/include/modsettingui.h"
#include "messagebox/modmessagebox.h"
#include "settinguicommon.h"
#include "actiondefinecommon.h"
#include "uikernel/qcomboboxutility.h"
#include "uikernel/qlineeditutility.h"
#include "inputmethod/inputmethod.h"

#define doNetworkControl (NS_SETTING::CNetworkController::GetInstance())

//static const yl::string strRouterPage = "OpenSubPage(network_router)";
static const yl::string kszDHCPServer = "OpenSubPage(network_dhcp_server)";
namespace
{
// #define ENTER_ROUTER_ACTION "Enter_Router"
#define ACTION_REFRESH_SOFTKEY "Refresh_Softkey"
}

CDlgNetworkPCPort::CDlgNetworkPCPort(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCmbType(NULL)
    , m_pLabDHCPServer(NULL)
    , m_pEditRouterIP(NULL)
    , m_pEditSubnetMask(NULL)
    , m_bDHCPEnabled(false)
{
    InitData();
}

void CDlgNetworkPCPort::InitData()
{
    if (m_pGeneralFrame != NULL)
    {
        this->setFocusPolicy(Qt::StrongFocus);
        this->setFocus();

        m_pCmbType = new QComboBox(this);
        m_pLabDHCPServer = new QLabel(this);
        m_pEditRouterIP = new QLineEdit(this);
        m_pEditSubnetMask = new QLineEdit(this);

        //point can't be null
        if (NULL == m_pCmbType
                || NULL == m_pLabDHCPServer
                || NULL == m_pEditRouterIP
                || NULL == m_pEditSubnetMask)
        {
            return;
        }


        QPalette pe;
        pe.setColor(QPalette::WindowText, Qt::black);
        m_pLabDHCPServer->setPalette(pe);
        //默认显示Disable
        m_pLabDHCPServer->setText(LANG_TRANSLATE("Disabled"));

        m_pCmbType->addItem(LANG_TRANSLATE("Bridge"), BRIDGE);
        m_pCmbType->addItem(LANG_TRANSLATE("Router"), ROUTER);
        qComboBoxUtility::SetComboBoxStyle(m_pCmbType, Qt::black);
        //设置输入框
        qLineEditUtility::SetLineEditStyle(m_pEditRouterIP, Qt::black);
        qLineEditUtility::SetLineEditStyle(m_pEditSubnetMask, Qt::black);

        //最大输入长度限制
        qLineEditUtility::SetMaxLength(m_pEditRouterIP, EDIT_TYPE_IPV4);
        qLineEditUtility::SetMaxLength(m_pEditSubnetMask, EDIT_TYPE_IPV4);

        qInstallIME(m_pEditRouterIP, IME_ALL);
        qInstallIME(m_pEditSubnetMask, IME_ALL);

        connect(m_pCmbType, SIGNAL(currentIndexChanged(int)),
                this, SLOT(OnCmbTypeChanged(int)));
    }

    this->installEventFilter(this);
}

void CDlgNetworkPCPort::OnCmbTypeChanged(int)
{
    if (NULL == m_pGeneralFrame || NULL == m_pCmbType)
    {
        return;
    }

    m_pLabDHCPServer->hide();
    m_pEditRouterIP->hide();
    m_pEditSubnetMask->hide();
    //重置页面
    MakeReadyForRelayout();

    QWidgetPair qPair;
    qPair.first = LANG_TRANSLATE("Type");
    qPair.second = m_pCmbType;
    qPair.third = ACTION_REFRESH_SOFTKEY;
    m_vecWidgetPair.push_back(qPair);

    if (m_pCmbType->itemData(m_pCmbType->currentIndex()).value<int>() == ROUTER)
    {
        QString strLab;
        strLab = LANG_TRANSLATE("IP");
        qPair.first = strLab;
        qPair.second = m_pEditRouterIP;
        m_vecWidgetPair.push_back(qPair);

        strLab = LANG_TRANSLATE("Subnet Mask");
        qPair.first = strLab;
        qPair.second = m_pEditSubnetMask;
        m_vecWidgetPair.push_back(qPair);

        strLab = LANG_TRANSLATE("DHCP Server");
        qPair.first = strLab;
        qPair.second = m_pLabDHCPServer;
        qPair.eType = SETTING_ITEM_MENU;
        qPair.third = ACTION_ENTER;
        qPair.bWithStatus = true;
        m_vecWidgetPair.push_back(qPair);
    }

    AddWidget(m_vecWidgetPair);
    m_pGeneralFrame->RefreshData(0);
}

//取得当前页面数据
NetworkRouterData & CDlgNetworkPCPort::GetPageRouterData(NetworkRouterData & currentPageData) const
{
    currentPageData.m_strIPAddress = fromQString(m_pEditRouterIP->text());
    currentPageData.m_strNetMask = fromQString(m_pEditSubnetMask->text());
    currentPageData.m_bDHCPServer = m_bDHCPEnabled;

    return currentPageData;
}

//比较页面前后数据
bool CDlgNetworkPCPort::CompareRouterData(const NetworkRouterData & beforePageData,
        const NetworkRouterData & afterPageData) const
{
    if (beforePageData.m_bDHCPServer != afterPageData.m_bDHCPServer
            || beforePageData.m_strIPAddress != afterPageData.m_strIPAddress
            || beforePageData.m_strNetMask != afterPageData.m_strNetMask)
    {
        //如果不相等那么返回flase
        return false;
    }
    else
    {
        //相等返回true
        return true;
    }
}

bool CDlgNetworkPCPort::IsStatusChanged()
{
    if (m_pGeneralFrame == NULL || m_pCmbType == NULL)
    {
        return false;
    }

    int iMode = m_pCmbType->itemData(m_pCmbType->currentIndex()).value<int>();
    if (m_eCurrentMode != iMode)
    {
        return true;
    }

    if (iMode == ROUTER)
    {
        NetworkRouterData currentPageData;
        NetworkRouterData beforePageData;

        GetPageRouterData(currentPageData);

        //调用控制层：加载数据
        bool bIsSuccess = doNetworkControl->GetRouterData(&beforePageData);

        if (bIsSuccess)
        {
            //加载数据成功，设置当前页面数据,并返回true
            if (CompareRouterData(beforePageData, currentPageData))
            {
                //前后页面数据没有改变，返回false
                return false;
            }
            else
            {
                //前后页面数据改变，返回true
                return true;
            }
        }
        else
        {
            //加载数据失败，返回false
            //保存失败
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE("GetConfigData Failed, Please Retry!"),
                                        MESSAGEBOX_NOTE, SHORT_TIME);
            return false;
        }
    }

    return false;
}

//设置当前页面数据
void CDlgNetworkPCPort::SetRouterData(NetworkRouterData & currentPageData)
{
    if (NULL == m_pEditRouterIP || NULL == m_pEditSubnetMask
            || NULL == m_pLabDHCPServer)
    {
        return;
    }

    m_pEditRouterIP->setText(toQString(currentPageData.m_strIPAddress.c_str()));
    m_pEditSubnetMask->setText(toQString(currentPageData.m_strNetMask.c_str()));

    m_bDHCPEnabled = currentPageData.m_bDHCPServer;
    if (m_bDHCPEnabled)
    {
        m_pLabDHCPServer->setText(LANG_TRANSLATE("Enabled"));
    }
    else
    {
        m_pLabDHCPServer->setText(LANG_TRANSLATE("Disabled"));
    }
}

bool CDlgNetworkPCPort::LoadPageData()
{
    return LoadData();
}

bool CDlgNetworkPCPort::LoadData()
{
    if (m_pGeneralFrame == NULL || m_pCmbType == NULL)
    {
        return false;
    }

    //获取当前连接模式
    m_eCurrentMode = (PCPORT_MODE)(doNetworkControl->GetPCPortMode());

    m_pCmbType->setCurrentIndex(m_pCmbType->findData(m_eCurrentMode, Qt::UserRole));

    OnCmbTypeChanged(m_pCmbType->currentIndex());

    NetworkRouterData routerData;

    //调用控制层：加载数据
    bool bIsSuccess = doNetworkControl->GetRouterData(&routerData);

    if (bIsSuccess)
    {
        //加载数据成功，设置当前页面数据,并返回true
        SetRouterData(routerData);
        return true;
    }
    else
    {
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE("Load Config Data Failed!"),
                                    MESSAGEBOX_NOTE, SHORT_TIME);
        //加载数据失败，返回false
        return false;
    }

    return true;
}

bool CDlgNetworkPCPort::SavePageData()
{
    if (m_pGeneralFrame == NULL)
    {
        return false;
    }
    if (!IsStatusChanged())
    {
        return true;
    }

    if (m_pCmbType->itemData(m_pCmbType->currentIndex()).value<int>() == BRIDGE)
    {
        //设置网络连接模式
        // 0-Router, 1-Bridge
        if (doNetworkControl->SetPCPortMode(BRIDGE))
        {
            //设置网络改变标记
            _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
            return true;
        }
        else
        {
            //保存失败
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE("Save Config Failed, Please Retry!"),
                                        MESSAGEBOX_NOTE, SHORT_TIME);
            return false;
        }
    }
    else
    {
        bool bNetWorkChanged = true;
        NetworkRouterData currentPageData;
        //调用控制层：取得原先数据
        bool bIsSuccess = doNetworkControl->GetRouterData(&currentPageData);
        if (bIsSuccess)
        {
            GetPageRouterData(currentPageData);

            //调用控制层：保存数据
            if (doNetworkControl->SetPCPortMode(ROUTER) && doNetworkControl->SetRouterData(&currentPageData))
            {
                bNetWorkChanged = true;
            }
            else
            {
                //保存失败
                MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE("Save Config Failed, Please Retry!"),
                                            MESSAGEBOX_NOTE, SHORT_TIME);
                return false;
            }
        }
        else
        {
            //加载原先数据失败，不保存
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE("Save Config Failed, Please Retry!"),
                                        MESSAGEBOX_NOTE, SHORT_TIME);
        }

        if (bNetWorkChanged)
        {
            //设置网络改变标记
            _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
        }
    }
    return false;
}

bool CDlgNetworkPCPort::eventFilter(QObject * pObject, QEvent * pEvent)
{
    // 不处理非自身的消息
    if (!isChildOf(pObject, this) && pObject != this)
    {
        return false;
    }

    if (pEvent->type() == QEvent::Show)
    {
        LoadData();
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

// 设置子页面的SoftKey
bool CDlgNetworkPCPort::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    return false;
}

CDlgNetworkPCPort::~CDlgNetworkPCPort()
{
}


bool CDlgNetworkPCPort::OnAction(const QString & strAction)
{
    if (ACTION_ENTER == strAction)
    {
        emit RefreshSoftkey();
        //进入DHCP界面
        emit openpage(SubMenuData(kszDHCPServer));
        return true;
    }
    else if (ACTION_REFRESH_SOFTKEY == strAction)
    {
        emit RefreshSoftkey();
    }
    return false;
}

bool CDlgNetworkPCPort::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (m_pCmbType->itemData(m_pCmbType->currentIndex()).value<int>() == ROUTER)
    {
        if (m_pEditRouterIP != NULL && m_pEditSubnetMask != NULL)
        {
            //验证数据是否非法
            if (m_pEditRouterIP->text().trimmed().isEmpty()
                    || (!qLineEditUtility::IsFormatValid(m_pEditRouterIP->text().trimmed(), EDIT_TYPE_IPV4)))
            {
                objErrorContent.strErrorContent = "Invalid IP Address!";
                objErrorContent.pLineEdit = m_pEditRouterIP;
                return false;
            }
            if (m_pEditSubnetMask->text().trimmed().isEmpty()
                    || (!qLineEditUtility::IsFormatValid(m_pEditSubnetMask->text().trimmed(), EDIT_TYPE_IPV4_NETMASK)))
            {
                objErrorContent.strErrorContent = "Invalid Subnet Mask!";
                objErrorContent.pLineEdit = m_pEditSubnetMask;
                return false;
            }
        }
    }

    return true;
}

// 卸载界面上所有的输入法
void CDlgNetworkPCPort::UninstallAllIME()
{
    qUninstallIME(m_pEditRouterIP);
    qUninstallIME(m_pEditSubnetMask);
}
