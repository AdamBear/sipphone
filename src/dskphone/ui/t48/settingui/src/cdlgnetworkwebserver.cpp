#include "model.h"
#include "cdlgnetworkwebserver.h"

#include "setting/src/networkcontroller.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "setting/include/common.h"
#include "baseui/fontcolor.h"
#include "inputmethod/inputmethod.h"
#include "networksettingmanager.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qcomboboxutility.h"

//增添QVriant兼容类型
Q_DECLARE_METATYPE(WebServerType);
#define doNetworkControl (NS_SETTING::CNetworkController::GetInstance())

CDlgNetworkWebServer::CDlgNetworkWebServer(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCmbBoxHTTP(NULL)
    , m_pCmbBoxHTTPS(NULL)
    , m_pEditHTTP_Port(NULL)
    , m_pEditHPPTS_Port(NULL)
{
    InitData();
}

CDlgNetworkWebServer::~CDlgNetworkWebServer()
{
}

void CDlgNetworkWebServer::InitData()
{
    m_pCmbBoxHTTP = new QComboBox(this);
    m_pCmbBoxHTTPS = new QComboBox(this);
    m_pEditHTTP_Port = new QLineEdit(this);
    m_pEditHPPTS_Port = new QLineEdit(this);

    //point can't be null
    if (NULL == m_pCmbBoxHTTP
            || NULL == m_pCmbBoxHTTPS
            || NULL == m_pEditHTTP_Port
            || NULL == m_pEditHPPTS_Port)
    {
        return;
    }
    //设置下拉框
    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxHTTP, Qt::black);
    m_pCmbBoxHTTP->addItem(LANG_TRANSLATE(TRID_DISABLED), false);
    m_pCmbBoxHTTP->addItem(LANG_TRANSLATE(TRID_ENABLED), true);

    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxHTTPS, Qt::black);
    m_pCmbBoxHTTPS->addItem(LANG_TRANSLATE(TRID_DISABLED), false);
    m_pCmbBoxHTTPS->addItem(LANG_TRANSLATE(TRID_ENABLED), true);

    //设置输入框
    qLineEditUtility::SetLineEditStyle(m_pEditHTTP_Port, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditHPPTS_Port, Qt::black);

    //最大输入长度限制
    qLineEditUtility::SetMaxLength(m_pEditHTTP_Port, EDIT_TYPE_IP_PORT);
    qLineEditUtility::SetMaxLength(m_pEditHPPTS_Port, EDIT_TYPE_IP_PORT);

    qInstallIME(m_pEditHTTP_Port, IME_123);
    qInstallIME(m_pEditHPPTS_Port, IME_123);

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_HTTP_STATUS);
    qPair.first = strLab;
    qPair.second = m_pCmbBoxHTTP;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_HTTP_PORT);
    qPair.first = strLab;
    qPair.second = m_pEditHTTP_Port;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_HTTPS_STATUS);
    qPair.first = strLab;
    qPair.second = m_pCmbBoxHTTPS;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_HTTPS_PORT);
    qPair.first = strLab;
    qPair.second = m_pEditHPPTS_Port;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    //install event filter
    m_pCmbBoxHTTP->installEventFilter(this);
    m_pEditHTTP_Port->installEventFilter(this);
    m_pCmbBoxHTTPS->installEventFilter(this);
    m_pEditHPPTS_Port->installEventFilter(this);

    this->installEventFilter(this);
}

//通过控制获取已有配置数据，通过V层在界面上显示出来
bool CDlgNetworkWebServer::LoadPageData()
{
    this->setFocus();
    //设置首选输入法
    qLineEditUtility::SetCurrentInputMethod(m_pEditHTTP_Port, EDIT_TYPE_IP_PORT);
    qLineEditUtility::SetCurrentInputMethod(m_pEditHPPTS_Port, EDIT_TYPE_IP_PORT);

    NetworkWebServerData currentPageData;

    //调用控制层：加载数据
    bool bIsSuccess =
        doNetworkControl->GetWebServerData(&currentPageData);

    if (bIsSuccess)
    {
        //加载数据成功，设置当前页面数据,并返回true
        SetCurrentPageData(currentPageData);
        return true;
    }
    else
    {
        //加载数据失败，返回false
        return false;
    }
}

//通过控制获取配置数据，与GetPageData值比较，有改动时返回true
bool CDlgNetworkWebServer::IsStatusChanged()
{
    NetworkWebServerData currentPageData;
    NetworkWebServerData beforePageData;

    GetCurrentPageData(currentPageData);

    //调用控制层：加载数据
    bool bIsSuccess =
        doNetworkControl->GetWebServerData(&beforePageData);

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
        return false;
    }
}

bool CDlgNetworkWebServer::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (m_pEditHTTP_Port != NULL && m_pEditHPPTS_Port != NULL)
    {
        //两种情况下数据非法：
        //1.非空时不满足格式
        //2.开关开启时非空
        if (m_pEditHTTP_Port->text().trimmed().isEmpty()
                || (!qLineEditUtility::IsFormatValid(m_pEditHTTP_Port->text().trimmed(), EDIT_TYPE_IP_PORT)))
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_HTTP_PORT_SHOULD_BE_1_TO_65535);
            objErrorContent.pLineEdit = m_pEditHTTP_Port;
            return false;
        }
        if (m_pEditHPPTS_Port->text().trimmed().isEmpty()
                || (!qLineEditUtility::IsFormatValid(m_pEditHPPTS_Port->text().trimmed(), EDIT_TYPE_IP_PORT)))
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_HTTPS_PORT_SHOULD_BE_1_TO_65535);
            objErrorContent.pLineEdit = m_pEditHPPTS_Port;
            return false;
        }
    }
    return true;
}

//读取当前界面的配置，通过C层保存
bool CDlgNetworkWebServer::SavePageData()
{
    //提起页面数据
    if (IsStatusChanged())
    {
        NetworkWebServerData currentPageData;
        GetCurrentPageData(currentPageData);
        //调用控制层：保存数据
        doNetworkControl->SetWebServerData(currentPageData);

        //设置网络改变标记
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }
    return true;
}

//比较页面前后数据
bool CDlgNetworkWebServer::ComparePageData(const NetworkWebServerData & beforePageData,
        const NetworkWebServerData & afterPageData) const
{
    if (beforePageData.m_bHTTP_On != afterPageData.m_bHTTP_On
            || beforePageData.m_strHTTP_Port != afterPageData.m_strHTTP_Port
            || beforePageData.m_bHTTPS_On != afterPageData.m_bHTTPS_On
            || beforePageData.m_strHTTPS_Port != afterPageData.m_strHTTPS_Port)
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
NetworkWebServerData & CDlgNetworkWebServer::GetCurrentPageData(NetworkWebServerData &
        currentPageData) const
{
    if (NULL != m_pCmbBoxHTTP)
    {
        currentPageData.m_bHTTP_On = m_pCmbBoxHTTP
                                     ->itemData(m_pCmbBoxHTTP->currentIndex(), Qt::UserRole).value<bool>();
    }
    if (NULL != m_pCmbBoxHTTPS)
    {
        currentPageData.m_bHTTPS_On = m_pCmbBoxHTTPS
                                      ->itemData(m_pCmbBoxHTTPS->currentIndex(), Qt::UserRole).value<bool>();
    }
    if (NULL != m_pEditHTTP_Port)
    {
        currentPageData.m_strHTTP_Port = fromQString(m_pEditHTTP_Port->text());
    }
    if (NULL != m_pEditHPPTS_Port)
    {
        currentPageData.m_strHTTPS_Port = fromQString(m_pEditHPPTS_Port->text());
    }

    return currentPageData;
}

//设置当前页面数据
void CDlgNetworkWebServer::SetCurrentPageData(NetworkWebServerData & currentPageData)
{
    if (NULL != m_pCmbBoxHTTP)
    {
        m_pCmbBoxHTTP->setCurrentIndex(m_pCmbBoxHTTP
                                       ->findData(currentPageData.m_bHTTP_On, Qt::UserRole));
    }
    if (NULL != m_pCmbBoxHTTPS)
    {
        m_pCmbBoxHTTPS->setCurrentIndex(m_pCmbBoxHTTPS
                                        ->findData(currentPageData.m_bHTTPS_On, Qt::UserRole));
    }
    if (NULL != m_pEditHTTP_Port)
    {
        m_pEditHTTP_Port->setText(toQString(currentPageData.m_strHTTP_Port.c_str()));
    }
    if (NULL != m_pEditHPPTS_Port)
    {
        m_pEditHPPTS_Port->setText(toQString(currentPageData.m_strHTTPS_Port.c_str()));
    }
}

