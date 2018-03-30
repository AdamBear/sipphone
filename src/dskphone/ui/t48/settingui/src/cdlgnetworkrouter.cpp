#include "model.h"
#include "cdlgnetworkrouter.h"
#include "actiondefinecommon.h"

#include "setting/src/networkcontroller.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "setting/include/common.h"
#include "baseui/fontcolor.h"
#include "inputmethod/inputmethod.h"
#include "uikernel/qlineeditutility.h"
#include "networksettingmanager.h"
#include "settingui/include/modsettingui.h"
#include "messagebox/modmessagebox.h"
#include "settinguicommon.h"

enum eMenuIndex
{
    ROUTERIP = 0,
    SUBNETMASK,
    DHCPSERVER
};

const yl::string strDHCPServer             = "OpenSubPage(network_dhcp_server)";

//增添QVriant兼容类型
Q_DECLARE_METATYPE(WebServerType);
#define doNetworkControl (NS_SETTING::CNetworkController::GetInstance())

CDlgNetworkRouter::CDlgNetworkRouter(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pLabDHCPServer(NULL)
    , m_pEditRouterIP(NULL)
    , m_pEditSubnetMask(NULL)
{
    InitData();
}

CDlgNetworkRouter::~CDlgNetworkRouter()
{
}

void CDlgNetworkRouter::InitData()
{
    //设置标题
    m_strTitle = LANG_TRANSLATE(TRID_ROUTER).toUtf8().data();

    m_pLabDHCPServer = new QLabel(this);
    m_pEditRouterIP = new QLineEdit(this);
    m_pEditSubnetMask = new QLineEdit(this);

    //point can't be null
    if (NULL == m_pLabDHCPServer
            || NULL == m_pEditRouterIP
            || NULL == m_pEditSubnetMask)
    {
        return;
    }


    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::black);
    m_pLabDHCPServer->setPalette(pe);
    //默认显示Disable
    m_pLabDHCPServer->setText(LANG_TRANSLATE(TRID_DISABLED));

    //设置输入框
    qLineEditUtility::SetLineEditStyle(m_pEditRouterIP, THEME_TEXT_COLOR, THEME_EDIT_HL_COLOR,
                                       THEME_EDIT_HL_TEXT_COLOR);
    qLineEditUtility::SetLineEditStyle(m_pEditSubnetMask, THEME_TEXT_COLOR, THEME_EDIT_HL_COLOR,
                                       THEME_EDIT_HL_TEXT_COLOR);

    //最大输入长度限制
    qLineEditUtility::SetMaxLength(m_pEditRouterIP, EDIT_TYPE_IPV4);
    qLineEditUtility::SetMaxLength(m_pEditSubnetMask, EDIT_TYPE_IPV4);

    qInstallIME(m_pEditRouterIP, IME_ALL);
    qInstallIME(m_pEditSubnetMask, IME_ALL);

    QWidgetPair qPair;
    QString strLab;


    strLab = LANG_TRANSLATE(TRID_IP_ADDRESS);
    qPair.first = strLab;
    qPair.second = m_pEditRouterIP;
    m_vecWidgetPair.push_back(qPair);


    strLab = LANG_TRANSLATE(TRID_SUBNET_MASK);
    qPair.first = strLab;
    qPair.second = m_pEditSubnetMask;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_DHCP_SERVER);
    qPair.first = strLab;
    qPair.second = m_pLabDHCPServer;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    //install event filter
    m_pEditRouterIP->installEventFilter(this);
    m_pEditSubnetMask->installEventFilter(this);

    this->installEventFilter(this);
}

// 设置子页面的SoftKey
bool CDlgNetworkRouter::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    return false;
}

//通过控制获取已有配置数据，通过V层在界面上显示出来
bool CDlgNetworkRouter::LoadPageData()
{
    //设置首选输入法
    qLineEditUtility::SetCurrentInputMethod(m_pEditRouterIP, EDIT_TYPE_IP_PORT);
    qLineEditUtility::SetCurrentInputMethod(m_pEditSubnetMask, EDIT_TYPE_IP_PORT);

    NetworkRouterData currentPageData;

    //调用控制层：加载数据
    bool bIsSuccess = doNetworkControl->GetRouterData(&currentPageData);

    if (bIsSuccess)
    {
        //加载数据成功，设置当前页面数据,并返回true
        SetCurrentPageData(currentPageData);
        return true;
    }
    else
    {
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE("Load Config Data Failed!"),
                                    MESSAGEBOX_NOTE, SHORT_TIME);
        //加载数据失败，返回false
        return false;
    }
}

//通过控制获取配置数据，与GetPageData值比较，有改动时返回true
bool CDlgNetworkRouter::IsStatusChanged()
{
    NetworkRouterData currentPageData;
    NetworkRouterData beforePageData;

    GetCurrentPageData(currentPageData);

    //调用控制层：加载数据
    bool bIsSuccess = doNetworkControl->GetRouterData(&beforePageData);

    if (bIsSuccess)
    {
        //加载数据成功，设置当前页面数据,并返回true
        if (ComparePageData(beforePageData, currentPageData))
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

bool CDlgNetworkRouter::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (m_pEditRouterIP != NULL && m_pEditSubnetMask != NULL)
    {
        //验证数据是否非法
        if (m_pEditRouterIP->text().trimmed().isEmpty()
                || (!qLineEditUtility::IsFormatValid(m_pEditRouterIP->text().trimmed(), EDIT_TYPE_IPV4)))
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE("Invalid IP Address!");
            objErrorContent.pLineEdit = m_pEditRouterIP;
            return false;
        }
        if (m_pEditSubnetMask->text().trimmed().isEmpty()
                || (!qLineEditUtility::IsFormatValid(m_pEditSubnetMask->text().trimmed(), EDIT_TYPE_IPV4_NETMASK)))
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE("Invalid Subnet Mask!");
            objErrorContent.pLineEdit = m_pEditSubnetMask;
            return false;
        }
    }
    return true;
}

//读取当前界面的配置，通过C层保存
bool CDlgNetworkRouter::SavePageData()
{
    bool bNetWorkChanged = false;

    //如果RouterData有改变或者连接模式不为Router都应进行数据保存
    if (doNetworkControl->GetPCPortMode() != ROUTER)
    {
        //设置网络连接模式
        // 0-Router, 1-Bridge
        doNetworkControl->SetPCPortMode(ROUTER);
        bNetWorkChanged = true;
    }

    if (IsStatusChanged())
    {
        NetworkRouterData currentPageData;
        //调用控制层：取得原先数据
        bool bIsSuccess = doNetworkControl->GetRouterData(&currentPageData);
        if (bIsSuccess)
        {
            GetCurrentPageData(currentPageData);

            //调用控制层：保存数据
            if (doNetworkControl->SetRouterData(&currentPageData))
            {
                bNetWorkChanged = true;
            }
            else
            {
                //保存失败
                MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_SAVE_CONFIG_FAILED_PLEASE_RETRY),
                                            MESSAGEBOX_NOTE, SHORT_TIME);
                return false;
            }
        }
        else
        {
            //加载原先数据失败，不保存
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_SAVE_CONFIG_FAILED_PLEASE_RETRY),
                                        MESSAGEBOX_NOTE, SHORT_TIME);
        }

    }

    if (bNetWorkChanged)
    {
        //设置网络改变标记
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }

    return true;
}

//比较页面前后数据
bool CDlgNetworkRouter::ComparePageData(const NetworkRouterData & beforePageData,
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

//取得当前页面数据
NetworkRouterData & CDlgNetworkRouter::GetCurrentPageData(NetworkRouterData & currentPageData) const
{
    if (NULL != m_pEditRouterIP)
    {
        currentPageData.m_strIPAddress = fromQString(m_pEditRouterIP->text());
    }
    if (NULL != m_pEditSubnetMask)
    {
        currentPageData.m_strNetMask = fromQString(m_pEditSubnetMask->text());
    }

    if (NULL != m_pLabDHCPServer && m_pLabDHCPServer->text() == "Enable")
    {
        currentPageData.m_bDHCPServer = true;
    }
    else
    {
        currentPageData.m_bDHCPServer = false;
    }

    return currentPageData;
}

//设置当前页面数据
void CDlgNetworkRouter::SetCurrentPageData(NetworkRouterData & currentPageData)
{
    if (NULL != m_pEditRouterIP)
    {
        m_pEditRouterIP->setText(toQString(currentPageData.m_strIPAddress.c_str()));
    }
    if (NULL != m_pEditSubnetMask)
    {
        m_pEditSubnetMask->setText(toQString(currentPageData.m_strNetMask.c_str()));
    }

    if (NULL != m_pLabDHCPServer && currentPageData.m_bDHCPServer)
    {
        m_pLabDHCPServer->setText(LANG_TRANSLATE(TRID_ENABLED));
    }
    else
    {
        if (NULL != m_pLabDHCPServer)
        {
            m_pLabDHCPServer->setText(LANG_TRANSLATE(TRID_DISABLED));
        }
    }
}

void CDlgNetworkRouter::showEvent(QShowEvent * e)
{
    //从DHCP Server界面返回，需重新下载数据
    LoadPageData();
}

bool CDlgNetworkRouter::OnAction(const QString & strAction)
{
    if (strAction == ACTION_ENTER)
    {
        //进入DHCP Server 界面
        emit openpage(SubMenuData(strDHCPServer));
        return true;
    }
    return false;
}
