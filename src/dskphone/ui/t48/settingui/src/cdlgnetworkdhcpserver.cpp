#include "model.h"
#include "cdlgnetworkdhcpserver.h"
#include "actiondefinecommon.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qcomboboxutility.h"
#include "setting/src/networkcontroller.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "setting/include/common.h"
#include "baseui/fontcolor.h"
#include "inputmethod/inputmethod.h"
#include "networksettingmanager.h"
#include "messagebox/modmessagebox.h"

enum eMenuIndex
{
    ROUTERIP = 0,
    SUBNETMASK,
    DHCPSERVER

};

//增添QVriant兼容类型
Q_DECLARE_METATYPE(WebServerType);
#define doNetworkControl (NS_SETTING::CNetworkController::GetInstance())
///////////////////////////////////////////////////////////////////////////////
CDlgNetworkDHCPServer::CDlgNetworkDHCPServer(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pComServerStatus(NULL)
    , m_pEditStartIP(NULL)
    , m_pEditEndIP(NULL)
{
    InitData();
}

CDlgNetworkDHCPServer::~CDlgNetworkDHCPServer()
{
}

void CDlgNetworkDHCPServer::InitData()
{
    //设置标题
    m_strTitle = LANG_TRANSLATE(TRID_ROUTER).toUtf8().data();

    m_pComServerStatus = new QComboBox(this);
    m_pEditStartIP = new QLineEdit(this);
    m_pEditEndIP = new QLineEdit(this);

    //point can't be null
    if (NULL == m_pComServerStatus
            || NULL == m_pEditStartIP
            || NULL == m_pEditEndIP)
    {
        return;
    }

    //设置下拉框
    qComboBoxUtility::SetComboBoxStyle(m_pComServerStatus, Qt::black);
    m_pComServerStatus->addItem(LANG_TRANSLATE(TRID_DISABLED), false);
    m_pComServerStatus->addItem(LANG_TRANSLATE(TRID_ENABLED), true);

    //设置输入框
    qLineEditUtility::SetLineEditStyle(m_pEditStartIP, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditEndIP, Qt::black);

    //最大输入长度限制
    qLineEditUtility::SetMaxLength(m_pEditStartIP, EDIT_TYPE_IPV4);
    qLineEditUtility::SetMaxLength(m_pEditEndIP, EDIT_TYPE_IPV4);

    qInstallIME(m_pEditStartIP, IME_ALL);
    qInstallIME(m_pEditEndIP, IME_ALL);

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_SEVER_STATUS);
    qPair.first = strLab;
    qPair.second = m_pComServerStatus;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_START_IP);
    qPair.first = strLab;
    qPair.second = m_pEditStartIP;
    m_vecWidgetPair.push_back(qPair);


    strLab = LANG_TRANSLATE(TRID_END_IP);
    qPair.first = strLab;
    qPair.second = m_pEditEndIP;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    //install event filter
    m_pEditStartIP->installEventFilter(this);
    m_pEditEndIP->installEventFilter(this);
    m_pComServerStatus->installEventFilter(this);

    this->installEventFilter(this);
}


//通过控制获取已有配置数据，通过V层在界面上显示出来
bool CDlgNetworkDHCPServer::LoadPageData()
{
    //设置首选输入法
    qLineEditUtility::SetCurrentInputMethod(m_pEditStartIP, EDIT_TYPE_IP_PORT);
    qLineEditUtility::SetCurrentInputMethod(m_pEditEndIP, EDIT_TYPE_IP_PORT);

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
bool CDlgNetworkDHCPServer::IsStatusChanged()
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
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE("GetConfigData Failed, Please Retry!"),
                                    MESSAGEBOX_NOTE, SHORT_TIME);
        return false;
    }
}

bool CDlgNetworkDHCPServer::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (m_pEditStartIP != NULL && m_pEditEndIP != NULL
            && m_pComServerStatus != NULL)
    {
        //开关开启时才验证
        if (m_pComServerStatus
                ->itemData(m_pComServerStatus->currentIndex(), Qt::UserRole).value<bool>())
        {
            //验证数据是否非法
            if (m_pEditStartIP->text().trimmed().isEmpty()
                    || (!qLineEditUtility::IsFormatValid(m_pEditStartIP->text().trimmed(), EDIT_TYPE_IPV4)))
            {
                objErrorContent.strErrorContent = LANG_TRANSLATE("Invalid DHCP Start IP Address!");
                objErrorContent.pLineEdit = m_pEditStartIP;
                return false;
            }
            if (m_pEditEndIP->text().trimmed().isEmpty()
                    || (!qLineEditUtility::IsFormatValid(m_pEditEndIP->text().trimmed(), EDIT_TYPE_IPV4)))
            {
                objErrorContent.strErrorContent = LANG_TRANSLATE("Invalid DHCP End IP Address!");
                objErrorContent.pLineEdit = m_pEditEndIP;
                return false;
            }
        }

    }
    return true;
}

//读取当前界面的配置，通过C层保存
bool CDlgNetworkDHCPServer::SavePageData()
{
    //提起页面数据
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
                //设置网络改变标记
                _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
                return true;
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

    return true;
}

//比较页面前后数据
bool CDlgNetworkDHCPServer::ComparePageData(const NetworkRouterData & beforePageData,
        const NetworkRouterData & afterPageData) const
{
    if (beforePageData.m_bDHCPServer != afterPageData.m_bDHCPServer
            || beforePageData.m_strStartIP != afterPageData.m_strStartIP
            || beforePageData.m_strEndIP != afterPageData.m_strEndIP)
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
NetworkRouterData & CDlgNetworkDHCPServer::GetCurrentPageData(NetworkRouterData & currentPageData)
const
{
    if (NULL != m_pComServerStatus)
    {
        currentPageData.m_bDHCPServer = m_pComServerStatus->itemData(m_pComServerStatus->currentIndex(),
                                        Qt::UserRole).value<bool>();
    }
    if (NULL != m_pEditStartIP)
    {
        currentPageData.m_strStartIP = fromQString(m_pEditStartIP->text());
    }
    if (NULL != m_pEditEndIP)
    {
        currentPageData.m_strEndIP = fromQString(m_pEditEndIP->text());
    }

    return currentPageData;
}

//设置当前页面数据
void CDlgNetworkDHCPServer::SetCurrentPageData(NetworkRouterData & currentPageData)
{
    if (NULL != m_pComServerStatus)
    {
        m_pComServerStatus->setCurrentIndex(m_pComServerStatus
                                            ->findData(currentPageData.m_bDHCPServer, Qt::UserRole));
    }

    if (NULL != m_pEditStartIP)
    {
        m_pEditStartIP->setText(toQString(currentPageData.m_strStartIP.c_str()));
    }
    if (NULL != m_pEditEndIP)
    {
        m_pEditEndIP->setText(toQString(currentPageData.m_strEndIP.c_str()));
    }
}

