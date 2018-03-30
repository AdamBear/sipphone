#include "model.h"
#include "cdlgaccount.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "setting/src/accountcontroller.h"
#include "uikernel/languagehelper.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "keymap.h"
#include "setting/include/authorityiddefine.h"
#include "uikernel/qcomboboxutility.h"
#include "uikernel/qlineeditutility.h"
#include "commonapi/stringhelperapi.h"

CDlgAccount::CDlgAccount(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCmbBoxActivation(NULL)
    , m_pCmbBoxOutboundStatus(NULL)
    , m_pCmbBoxNATStatus(NULL)
{
    InitData();
}

CDlgAccount::~CDlgAccount()
{
}

void CDlgAccount::InitData()
{
    //设定滑块大小位置
    m_pCmbBoxActivation = new QComboBox(this);
    m_pCmbBoxOutboundStatus = new QComboBox(this);
    m_pCmbBoxNATStatus = new QComboBox(this);

    m_pEditDisplayName = new QLineEdit(this);
    m_pEditLabel = new QLineEdit(this);
    m_pEditOutboundProxy = new QLineEdit(this);
    m_pEditPassword = new QLineEdit(this);
    m_pEditRegisterName = new QLineEdit(this);
    m_pEditSIP_Server1 = new QLineEdit(this);
    m_pEditSIP_Server2 = new QLineEdit(this);
    m_pEditUserName = new QLineEdit(this);
    m_pEditOutboundProxy2 = new QLineEdit(this);
    m_pEditProxyFallbackInterval = new QLineEdit(this);

    if (m_pCmbBoxActivation == NULL
            || m_pCmbBoxOutboundStatus == NULL
            || NULL == m_pCmbBoxNATStatus
            || NULL == m_pEditDisplayName
            || NULL == m_pEditLabel
            || NULL == m_pEditOutboundProxy
            || NULL == m_pEditPassword
            || NULL == m_pEditRegisterName
            || NULL == m_pEditSIP_Server1
            || NULL == m_pEditUserName
            || NULL == m_pEditSIP_Server2
            || NULL == m_pEditOutboundProxy2
            || NULL == m_pEditProxyFallbackInterval
       )
    {
        //内存已满，立即返回
        return;
    }

    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxActivation, Qt::black);
    m_pCmbBoxActivation->addItem(LANG_TRANSLATE(TRID_DISABLED), false);
    m_pCmbBoxActivation->addItem(LANG_TRANSLATE(TRID_ENABLED), true);

    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxOutboundStatus, Qt::black);
    m_pCmbBoxOutboundStatus->addItem(LANG_TRANSLATE(TRID_DISABLED), false);
    m_pCmbBoxOutboundStatus->addItem(LANG_TRANSLATE(TRID_ENABLED), true);

    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxNATStatus, Qt::black);
    m_pCmbBoxNATStatus->addItem(LANG_TRANSLATE(TRID_DISABLED), false);
    m_pCmbBoxNATStatus->addItem(LANG_TRANSLATE(TRID_STUN), true);

    //密码格式
    m_pEditPassword->setEchoMode(QLineEdit::Password);

    //设置颜色和背景
    qLineEditUtility::SetLineEditStyle(m_pEditDisplayName, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditLabel, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditOutboundProxy, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditOutboundProxy2, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditProxyFallbackInterval, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditPassword, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditRegisterName, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditSIP_Server1, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditSIP_Server2, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditUserName, Qt::black);

    //设置输入框输入的最大长度
    qLineEditUtility::SetMaxLength(m_pEditLabel, EDIT_TYPE_ACCOUNT_NAME);
    qLineEditUtility::SetMaxLength(m_pEditDisplayName, EDIT_TYPE_ACCOUNT_NAME);
    qLineEditUtility::SetMaxLength(m_pEditRegisterName, EDIT_TYPE_ACCOUNT_NAME);
    qLineEditUtility::SetMaxLength(m_pEditUserName, EDIT_TYPE_PHONE_NUMBER);
    qLineEditUtility::SetMaxLength(m_pEditPassword, EDIT_TYPE_PASSWORD);
    qLineEditUtility::SetMaxLength(m_pEditSIP_Server1, EDIT_TYPE_SIP_SERVER);
    qLineEditUtility::SetMaxLength(m_pEditSIP_Server2, EDIT_TYPE_SIP_SERVER);
    qLineEditUtility::SetMaxLength(m_pEditOutboundProxy, EDIT_TYPE_PROXY);
    qLineEditUtility::SetMaxLength(m_pEditOutboundProxy2, EDIT_TYPE_PROXY);
    qLineEditUtility::SetMaxLength(m_pEditProxyFallbackInterval, EDIT_TYPE_PHONE_NUMBER);

    //安装输入法
    qInstallIME(m_pEditLabel, IME_Name);
    qInstallIME(m_pEditDisplayName, IME_Name);
    qInstallIME(m_pEditRegisterName, IME_Name);
    qInstallIME(m_pEditUserName, IME_Name);
    qInstallIME(m_pEditPassword, IME_PWD);
    qInstallIME(m_pEditSIP_Server1, IME_ALL);
    qInstallIME(m_pEditSIP_Server2, IME_ALL);
    qInstallIME(m_pEditOutboundProxy, IME_ALL);
    qInstallIME(m_pEditOutboundProxy2, IME_ALL);
    qInstallIME(m_pEditProxyFallbackInterval, IME_123);

    //添加控件
    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_ACTIVATION);
    qPair.first = strLab;
    qPair.second = m_pCmbBoxActivation;
    qPair.strAuthorId = kszAuthorityRegisterLineActive;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_LABEL);
    qPair.first = strLab;
    qPair.second = m_pEditLabel;
    qPair.strAuthorId = kszAuthorityRegisterAccountLabel;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_DISPLAY_NAME);
    qPair.first = strLab;
    qPair.second = m_pEditDisplayName;
    qPair.strAuthorId = kszAuthorityRegisterDisplayName;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_REGISTER_NAME);
    qPair.first = strLab;
    qPair.second = m_pEditRegisterName;
    qPair.strAuthorId = kszAuthorityRegisterName;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_USER_NAME);
    qPair.first = strLab;
    qPair.second = m_pEditUserName;
    qPair.strAuthorId = kszAuthorityRegisterUserName;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_PASSWORD);
    qPair.first = strLab;
    qPair.second = m_pEditPassword;
    qPair.strAuthorId = kszAuthorityRegisterAccountPassword;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_SIP_SERVER1);
    qPair.first = strLab;
    qPair.second = m_pEditSIP_Server1;
    qPair.strAuthorId = kszAuthorityRegisterSipServer;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_SIP_SERVER2);
    qPair.first = strLab;
    qPair.second = m_pEditSIP_Server2;
    qPair.strAuthorId = kszAuthorityRegisterSipServer;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_OUTBOUND_STATUS);
    qPair.first = strLab;
    qPair.second = m_pCmbBoxOutboundStatus;
    qPair.strAuthorId = kszAuthorityRegisterEnabledOutboundProxyServer;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_OUTBOUND_PROXY1);
    qPair.first = strLab;
    qPair.second = m_pEditOutboundProxy;
    qPair.third = kszAuthorityRegisterOutboundProxyServer;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_OUTBOUND_PROXY2);
    qPair.first = strLab;
    qPair.second = m_pEditOutboundProxy2;
    qPair.third = kszAuthorityRegisterOutboundProxyServer;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_FALLBACK_INTERVAL);
    qPair.first = strLab;
    qPair.second = m_pEditProxyFallbackInterval;
    qPair.third = kszAuthorityRegisterFallBackInterval;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_NAT_STATUS);
    qPair.first = strLab;
    qPair.second = m_pCmbBoxNATStatus;
    qPair.strAuthorId = kszAuthorityRegisterNATTraversal;
    m_vecWidgetPair.push_back(qPair);


    m_pCmbBoxActivation->hide();
    m_pEditLabel->hide();
    m_pEditDisplayName->hide();
    m_pEditRegisterName->hide();
    m_pEditUserName->hide();
    m_pEditPassword->hide();
    m_pEditSIP_Server1->hide();
    m_pEditSIP_Server2->hide();
    m_pCmbBoxOutboundStatus->hide();
    m_pCmbBoxNATStatus->hide();
    m_pEditOutboundProxy->hide();
    m_pEditOutboundProxy2->hide();
    m_pEditProxyFallbackInterval->hide();

    AddWidget(m_vecWidgetPair);

    //为界面上各输入控件安装事件过滤器
    m_pCmbBoxActivation->installEventFilter(this);
    m_pEditLabel->installEventFilter(this);
    m_pEditDisplayName->installEventFilter(this);
    m_pEditRegisterName->installEventFilter(this);
    m_pEditUserName->installEventFilter(this);
    m_pEditPassword->installEventFilter(this);
    m_pEditSIP_Server1->installEventFilter(this);
    m_pEditSIP_Server2->installEventFilter(this);
    m_pCmbBoxOutboundStatus->installEventFilter(this);
    m_pCmbBoxNATStatus->installEventFilter(this);
    m_pEditOutboundProxy->installEventFilter(this);
    m_pEditOutboundProxy2->installEventFilter(this);
    m_pEditProxyFallbackInterval->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

//通过控制获取已有配置数据，通过V层在界面上显示出来
bool CDlgAccount::LoadPageData()
{
    //设置输入框首选输入法
    qLineEditUtility::SetCurrentInputMethod(m_pEditLabel, EDIT_TYPE_NAME);
    qLineEditUtility::SetCurrentInputMethod(m_pEditDisplayName, EDIT_TYPE_NAME);
    qLineEditUtility::SetCurrentInputMethod(m_pEditRegisterName, EDIT_TYPE_NAME);
    qLineEditUtility::SetCurrentInputMethod(m_pEditUserName, EDIT_TYPE_NAME);

    qLineEditUtility::SetCurrentInputMethod(m_pEditSIP_Server1, EDIT_TYPE_SERVER);
    qLineEditUtility::SetCurrentInputMethod(m_pEditSIP_Server2, EDIT_TYPE_SERVER);
    qLineEditUtility::SetCurrentInputMethod(m_pEditOutboundProxy, EDIT_TYPE_SERVER);
    qLineEditUtility::SetCurrentInputMethod(m_pEditOutboundProxy2, EDIT_TYPE_SERVER);
    qLineEditUtility::SetCurrentInputMethod(m_pEditProxyFallbackInterval, EDIT_TYPE_NUMBER);
#if IF_FEATURE_AUTOP_IME
    yl::string strPasswordIME = commonUnits_GetIMEByType(kszAdvancedAccountPasswordIME);
    if (!strPasswordIME.empty())
    {
        qSetCurrentIME(m_pEditPassword, strPasswordIME.c_str(), 0, kszAdvancedAccountPasswordIME);
    }
#else
    qLineEditUtility::SetCurrentInputMethod(m_pEditPassword, EDIT_TYPE_PASSWORD);
#endif

    int iAccount = commonAPI_GetIdFromStrAction(m_strSubPageAction);
    if (iAccount >= 0)
    {
        SetWindowTitle(LANG_TRANSLATE(TRID_ACCOUNT) + QString(" ") + QString::number(iAccount + 1));
    }

    //定义当前页面数据
    AccountDetailData currentPageData;

    //调用控制层：加载数据
    bool bIsSuccess =
        g_pAccountController->GetAccountByClickAction(&currentPageData, m_strSubPageAction);

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
bool CDlgAccount::IsStatusChanged()
{
    AccountDetailData currentPageData;
    GetCurrentPageData(currentPageData);

    AccountDetailData beforePageData;
    //调用控制层：加载数据
    bool bIsSuccess =
        g_pAccountController->GetAccountByClickAction(&beforePageData, m_strSubPageAction);
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

bool CDlgAccount::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (m_pCmbBoxActivation != NULL)
    {

        if (m_pCmbBoxActivation
                ->itemData(m_pCmbBoxActivation->currentIndex(), Qt::UserRole).value<bool>())
        {
            if (m_pEditUserName->text().isEmpty()
                    || !qLineEditUtility::IsFormatValid(m_pEditUserName->text(), EDIT_TYPE_ACCOUNT_NAME))
            {
                objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_USER_NAME_IS_INVALID);
                objErrorContent.pLineEdit = m_pEditUserName;
                return false;
            }

            if (m_pEditSIP_Server1->text().isEmpty() && m_pEditSIP_Server2->text().isEmpty())
            {
                objErrorContent.strErrorContent = TRID_SIP_PROXY_CANNOT_BE_EMPTY;
                objErrorContent.pLineEdit = m_pEditSIP_Server1;
                return false;
            }
            else if (!m_pEditSIP_Server1->text().isEmpty()
                     && !qLineEditUtility::IsFormatValid(m_pEditSIP_Server1->text(), EDIT_TYPE_SIP_SERVER))
            {
                objErrorContent.strErrorContent = TRID_SIP_SERVER1_IS_INVALID;
                objErrorContent.pLineEdit = m_pEditSIP_Server1;
                return false;
            }
            else if (!m_pEditSIP_Server2->text().isEmpty()
                     && !qLineEditUtility::IsFormatValid(m_pEditSIP_Server2->text(), EDIT_TYPE_SIP_SERVER))
            {
                objErrorContent.strErrorContent = TRID_SIP_SERVER2_IS_INVALID;
                objErrorContent.pLineEdit = m_pEditSIP_Server2;
                return false;
            }

            if (m_pCmbBoxOutboundStatus != NULL)
            {
                if (m_pCmbBoxOutboundStatus
                        ->itemData(m_pCmbBoxOutboundStatus->currentIndex(), Qt::UserRole).value<bool>())
                {
                    if ((m_pEditOutboundProxy->text().isEmpty() && m_pEditOutboundProxy2->text().isEmpty())
                            || (!m_pEditOutboundProxy->text().isEmpty()
                                && !qLineEditUtility::IsFormatValid(m_pEditOutboundProxy->text(), EDIT_TYPE_PROXY))
                            || (!m_pEditOutboundProxy2->text().isEmpty()
                                && !qLineEditUtility::IsFormatValid(m_pEditOutboundProxy2->text(), EDIT_TYPE_PROXY))
                       )
                    {
                        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_OUTBOUND_PROXY_IS_INVALID);
                        objErrorContent.pLineEdit = m_pEditOutboundProxy;
                        return false;
                    }
                    if (m_pEditProxyFallbackInterval->text().isEmpty()
                            || !qLineEditUtility::IsFormatValid(m_pEditProxyFallbackInterval->text(), EDIT_TYPE_NUMBER))
                    {
                        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_PROXY_FALLBACK_INTERVAL_INVALID);
                        objErrorContent.pLineEdit = m_pEditProxyFallbackInterval;
                        return false;
                    }
                }

            }
        }
    }
    return true;
}

//读取当前界面的配置，通过C层保存
bool CDlgAccount::SavePageData()
{
    AccountDetailData currentPageData;
    GetCurrentPageData(currentPageData);

    //调用控制层：保存数据
    g_pAccountController->SetAccountByClickAction(m_strSubPageAction, currentPageData);
    return true;
}

//比较页面前后数据
bool CDlgAccount::ComparePageData(const AccountDetailData & beforePageData,
                                  const AccountDetailData & afterPageData) const
{
    if (beforePageData.m_isOutboundEnable != afterPageData.m_isOutboundEnable
            || beforePageData.m_isNATEnable != afterPageData.m_isNATEnable
            || beforePageData.m_strDisplayName != afterPageData.m_strDisplayName
            || beforePageData.m_strLabel != afterPageData.m_strLabel
            || beforePageData.m_strOutboundProxy1 != afterPageData.m_strOutboundProxy1
            || beforePageData.m_strOutboundProxy2 != afterPageData.m_strOutboundProxy2
            || beforePageData.m_strPassword != afterPageData.m_strPassword
            || beforePageData.m_strRegisterName != afterPageData.m_strRegisterName
            || beforePageData.m_strSipServer1 != afterPageData.m_strSipServer1
            || beforePageData.m_strSipServer2 != afterPageData.m_strSipServer2
            || beforePageData.m_strUserName != afterPageData.m_strUserName
            || beforePageData.m_isActivated != afterPageData.m_isActivated
            || beforePageData.m_strOutboundProxy2 != afterPageData.m_strOutboundProxy2
            || beforePageData.m_nProxyFallbackInterval != afterPageData.m_nProxyFallbackInterval
       )
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
AccountDetailData & CDlgAccount::GetCurrentPageData(AccountDetailData & currentPageData) const
{
    currentPageData.m_strDisplayName =
        m_pEditDisplayName->text().trimmed().toUtf8().data();
    currentPageData.m_strLabel =
        m_pEditLabel->text().trimmed().toUtf8().data();
    currentPageData.m_strOutboundProxy1 =
        m_pEditOutboundProxy->text().trimmed().toUtf8().data();
    currentPageData.m_strOutboundProxy2 =
        m_pEditOutboundProxy2->text().trimmed().toUtf8().data();
    currentPageData.m_nProxyFallbackInterval =
        m_pEditProxyFallbackInterval->text().trimmed().toInt();

    currentPageData.m_strPassword =
        m_pEditPassword->text().trimmed().toUtf8().data();
    currentPageData.m_strUserName =
        m_pEditUserName->text().trimmed().toUtf8().data();
    currentPageData.m_strRegisterName =
        m_pEditRegisterName->text().trimmed().toUtf8().data();
    currentPageData.m_strSipServer1 =
        m_pEditSIP_Server1->text().trimmed().toUtf8().data();
    currentPageData.m_strSipServer2 =
        m_pEditSIP_Server2->text().trimmed().toUtf8().data();

    currentPageData.m_isActivated = m_pCmbBoxActivation
                                    ->itemData(m_pCmbBoxActivation->currentIndex(), Qt::UserRole).value<bool>();

    currentPageData.m_isNATEnable = m_pCmbBoxNATStatus
                                    ->itemData(m_pCmbBoxNATStatus->currentIndex(), Qt::UserRole).value<bool>();

    currentPageData.m_isOutboundEnable = m_pCmbBoxOutboundStatus
                                         ->itemData(m_pCmbBoxOutboundStatus->currentIndex(), Qt::UserRole).value<bool>();

    return currentPageData;
}

//设置当前页面数据
void CDlgAccount::SetCurrentPageData(AccountDetailData & currentPageData)
{
    m_pEditDisplayName->setText(toQString(currentPageData.m_strDisplayName.c_str()));
    m_pEditLabel->setText(toQString(currentPageData.m_strLabel.c_str()));
    m_pEditOutboundProxy->setText(toQString(currentPageData.m_strOutboundProxy1.c_str()));
    m_pEditOutboundProxy2->setText(toQString(currentPageData.m_strOutboundProxy2.c_str()));
    char chbuf[256] = {0};
    sprintf(chbuf, "%d", currentPageData.m_nProxyFallbackInterval);
    m_pEditProxyFallbackInterval->setText(toQString(chbuf));

    m_pEditPassword->setText(toQString(currentPageData.m_strPassword.c_str()));
    m_pEditSIP_Server1->setText(toQString(currentPageData.m_strSipServer1.c_str()));
    m_pEditSIP_Server2->setText(toQString(currentPageData.m_strSipServer2.c_str()));
    m_pEditUserName->setText(toQString(currentPageData.m_strUserName.c_str()));
    m_pEditRegisterName->setText(toQString(currentPageData.m_strRegisterName.c_str()));

    m_pCmbBoxActivation->setCurrentIndex(m_pCmbBoxActivation
                                         ->findData(currentPageData.m_isActivated, Qt::UserRole));

    m_pCmbBoxOutboundStatus->setCurrentIndex(m_pCmbBoxOutboundStatus
            ->findData(currentPageData.m_isOutboundEnable, Qt::UserRole));

    m_pCmbBoxNATStatus->setCurrentIndex(m_pCmbBoxNATStatus
                                        ->findData(currentPageData.m_isNATEnable, Qt::UserRole));
}

bool CDlgAccount::eventFilter(QObject * pObject, QEvent * pEvent)
{
    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}
