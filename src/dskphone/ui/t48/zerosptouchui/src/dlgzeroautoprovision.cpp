#include "dlgzeroautoprovision.h"
#include "settingui/src/model.h"
#include "setting/src/autoprovisioncontroller.h"
#include "setting/src/callforwardcontroller.h"
#include "setting/include/common.h"
#include "setting/include/zero_common.h"
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "inputmethod/inputmethod.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qlineeditutility.h"
#include "messagebox/modmessagebox.h"
#include "mainwnd/mainwnd.h"
#include "uimanager/rectdefine.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "autopcontrol/include/modautopcontrol.h"

#if IF_FEATURE_AUTOP_IME
#include "commonunits/modcommonunits.h"
#include "configiddefine.h"
#include "modconfigparser.h"
#endif

DlgZeroAutoProvision::DlgZeroAutoProvision(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pEditURL(NULL)
    , m_pEditUserName(NULL)
    , m_pEditPassword(NULL)
    , m_nPageType(AUTOP_PAGE_NORMAL)
{
    this->setFixedSize(RECT_ZERO.width() - 200, RECT_ZERO.height());
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);

    if (NULL != m_pGeneralFrame)
    {
        m_pGeneralFrame->setGeometry(0, 0, width(), height());
    }

    InitData();

    InitListItem();

    connect(this, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));
}

DlgZeroAutoProvision::~DlgZeroAutoProvision()
{
    qUninstallIME(m_pEditPassword);
    qUninstallIME(m_pEditURL);
    qUninstallIME(m_pEditUserName);
}

//通过C层获取已有配置数据，通过V层在界面上显示出来
bool DlgZeroAutoProvision::LoadPageData()
{
    if (JudgetWgtNull())
    {
        return false;
    }

    this->setFocus();
    m_pGeneralFrame->SelectItemByDataIndex(0);

    qLineEditUtility::SetCurrentInputMethod(m_pEditURL, EDIT_TYPE_URL);
#if IF_FEATURE_AUTOP_IME
    yl::string strUserIME = commonUnits_GetIMEByType(kszZeroUserIME);
    if (!strUserIME.empty())
    {
        qSetCurrentIME(m_pEditUserName, strUserIME.c_str(), 0, kszZeroUserIME);
    }
    else
    {
        qLineEditUtility::SetCurrentInputMethod(m_pEditUserName, EDIT_TYPE_NAME);
    }

    yl::string strPasswordIME = commonUnits_GetIMEByType(kszZeroPasswordIME);
    if (!strPasswordIME.empty())
    {
        qSetCurrentIME(m_pEditPassword, strPasswordIME.c_str(), 0, kszZeroPasswordIME);
    }
#else
    qLineEditUtility::SetCurrentInputMethod(m_pEditUserName, EDIT_TYPE_NAME);
    qLineEditUtility::SetCurrentInputMethod(m_pEditPassword, EDIT_TYPE_PASSWORD);
#endif

    bool bIsSuccess = false;

    if (AUTOP_PAGE_AUTHENTICATION == m_nPageType)
    {
        bIsSuccess = g_pAutoProvisionController->GetAuthenticationInfo(&m_AutoProvisionData);
    }
    else
    {
        bIsSuccess = g_pAutoProvisionController->GetAutoProvisionData(&m_AutoProvisionData);
#if IF_FEATURE_METASWITCH
		if (Autop_GetUpdateType() == AUT_MTSW_HTTP_DIGEST)
		{
			m_AutoProvisionData.m_strUsername = "";
			m_AutoProvisionData.m_strPassword = "";
		}
#endif
    }

    if (bIsSuccess)
    {
        //加载数据成功，设置当前页面数据,并返回true
        SetCurrentPageData(m_AutoProvisionData);
        //
        this->UpdateWnd();
        return true;
    }
    else
    {
        //加载数据失败，返回false
        return false;
    }
}

bool DlgZeroAutoProvision::IsStatusChanged()
{
    AutoProvisionData currentPageData;
    GetCurrentPageData(currentPageData);

    if (ComparePageData(m_AutoProvisionData, currentPageData))
    {
        return false;
    }

    return true;
}

bool DlgZeroAutoProvision::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (IsPageShowUrl())
    {
        if (m_pEditURL->text().isEmpty())
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_URL_CANNOT_BE_EMPTY);
            objErrorContent.pLineEdit = m_pEditURL;
            return false;
        }
        else if (!qLineEditUtility::IsFormatValid(m_pEditURL->text(), EDIT_TYPE_URL))
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_URL_IS_INAVLID);
            objErrorContent.pLineEdit = m_pEditURL;
            return false;
        }
    }

    if (!m_pEditUserName->text().isEmpty()
            && !qLineEditUtility::IsFormatValid(m_pEditUserName->text(), EDIT_TYPE_NAME))
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_USER_NAME_IS_INVALID);
        objErrorContent.pLineEdit = m_pEditUserName;
        return false;
    }

    if (!m_pEditPassword->text().isEmpty()
            && !qLineEditUtility::IsFormatValid(m_pEditPassword->text(), EDIT_TYPE_PASSWORD))
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_PASSWORD_IS_INVALID);
        objErrorContent.pLineEdit = m_pEditPassword;
        return false;
    }

    return true;
}

bool DlgZeroAutoProvision::SavePageData()
{
    if (JudgetWgtNull())
    {
        return true;
    }

    InputDataErrorInfo objErrorInfo;
    if (!IsInputDataValid(objErrorInfo))
    {
        SetErrorFocus(objErrorInfo.pLineEdit);

        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(objErrorInfo.strErrorContent)
                                    , MESSAGEBOX_Cancel, MESSAGE_BOX_EXIST_TIME);

        return false;
    }

    ReturnAllEdit();

    GetCurrentPageData(m_AutoProvisionData);

    if (AUTOP_PAGE_AUTHENTICATION == m_nPageType)
    {
        AUTOP_UPDATE_TYPE eType = Autop_GetUpdateType();

        if (AUT_ZERO == eType) //ZERO 需要将配置先写入
        {
            g_pAutoProvisionController->SetAutoProvisionData(m_AutoProvisionData, false);
        }

        g_pAutoProvisionController->StartCurrentConnect(&m_AutoProvisionData);
    }
    else
    {
        bool bSaveUrl = false;
        if (IsPageShowUrl())
        {
            bSaveUrl = true;
        }

        g_pAutoProvisionController->SetAutoProvisionData(m_AutoProvisionData, bSaveUrl);
    }

    return true;
}

//用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
void DlgZeroAutoProvision::InitData()
{
    m_pEditURL = new QLineEdit(this);
    m_pEditUserName = new QLineEdit(this);
    m_pEditPassword = new QLineEdit(this);

    if (JudgetWgtNull())
    {
        return;
    }

    //为输入控件安装输入法
    qInstallIME(m_pEditURL, IME_DIAL);
#if IF_FEATURE_AUTOP_IME
    qInstallIME(m_pEditUserName, IME_Name);
#else
    qInstallIME(m_pEditUserName, IME_DIAL);
#endif
    qInstallIME(m_pEditPassword, IME_PWD);

    //最大输入长度
    qLineEditUtility::SetMaxLength(m_pEditURL, EDIT_TYPE_URL);
    qLineEditUtility::SetMaxLength(m_pEditUserName, EDIT_TYPE_PHONE_NUMBER);        //特殊限制32个?
    qLineEditUtility::SetMaxLength(m_pEditPassword, EDIT_TYPE_PASSWORD);

    //设置输入框风格
    SetLineEditDefaultStyle(m_pEditURL);
    SetLineEditDefaultStyle(m_pEditUserName);
    SetLineEditDefaultStyle(m_pEditPassword);

    //设置密码回显示方式
    m_pEditPassword->setEchoMode(QLineEdit::Password);

    //为所有控件和本界面安装事件过滤器
    m_pEditURL->installEventFilter(this);
    m_pEditUserName->installEventFilter(this);
    m_pEditPassword->installEventFilter(this);

    this->installEventFilter(this);
}

bool DlgZeroAutoProvision::ComparePageData(const AutoProvisionData & beforePageData,
        const AutoProvisionData & afterPageData) const
{
    if (IsPageShowUrl())
    {
        return beforePageData.m_strUsername == afterPageData.m_strUsername
               && beforePageData.m_strPassword == afterPageData.m_strPassword
               && beforePageData.m_strUrl == afterPageData.m_strUrl;
    }

    return beforePageData.m_strUsername == afterPageData.m_strUsername
           && beforePageData.m_strPassword == afterPageData.m_strPassword;
}

//取得当前页面数据
AutoProvisionData & DlgZeroAutoProvision::GetCurrentPageData(AutoProvisionData & currentPageData)
const
{
    if (IsPageShowUrl())
    {
        qReturnEdit(m_pEditURL);
        currentPageData.m_strUrl = m_pEditURL->text().trimmed().toUtf8().data();
    }

    qReturnEdit(m_pEditUserName);
    qReturnEdit(m_pEditPassword);
    currentPageData.m_strUsername = m_pEditUserName->text().trimmed().toUtf8().data();
    currentPageData.m_strPassword = m_pEditPassword->text().trimmed().toUtf8().data();

    return currentPageData;
}

//设置当前页面数据
void DlgZeroAutoProvision::SetCurrentPageData(AutoProvisionData & currentPageData)
{
    m_pEditUserName->setText(toQString(currentPageData.m_strUsername.c_str()));

    if (m_nPageType != AUTOP_PAGE_AUTHENTICATION)
    {
        m_pEditURL->setText(toQString(currentPageData.m_strUrl.c_str()));
        m_pEditPassword->setText(toQString(currentPageData.m_strPassword.c_str()));
    }
    else
    {
        m_pEditPassword->setText("");
    }
}

bool DlgZeroAutoProvision::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL == pObject || NULL == pEvent)
    {
        return false;
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

void DlgZeroAutoProvision::OnRefreshSoftkey()
{
    // 刷新softkey
    _MainWND.DrawSoftkey(this);
    _MainWND.DisplayIMESoftkey(focusWidget());
}

// 控件判空
bool DlgZeroAutoProvision::JudgetWgtNull()
{
    if (NULL == m_pEditURL
            || NULL == m_pEditUserName
            || NULL == m_pEditPassword)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void DlgZeroAutoProvision::SetPageType(int nPageType)
{
    if (nPageType == m_nPageType)
    {
        return;
    }

    m_nPageType = nPageType;

    InitListItem();
}

void DlgZeroAutoProvision::InitListItem()
{
    MakeReadyForRelayout();

    QWidgetPair qPair;
    QString strLab;

    if (IsPageShowUrl())
    {
        strLab = LANG_TRANSLATE(TRID_URL);
        qPair.first = strLab;
        qPair.second = m_pEditURL;
        m_vecWidgetPair.push_back(qPair);
    }
    else
    {
        m_pEditURL->hide();
    }

    strLab = LANG_TRANSLATE(TRID_USER_NAME);
    qPair.first = strLab;
    qPair.second = m_pEditUserName;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_PASSWORD);
    qPair.first = strLab;
    qPair.second = m_pEditPassword;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);
}

bool DlgZeroAutoProvision::IsPageShowUrl() const
{
    if (AUTOP_PAGE_AUTHENTICATION == m_nPageType)
    {
        return false;
    }

    return (configParser_GetConfigInt(kszZeroNetworkAndURLEnable) == 1
            && configParser_GetConfigInt(kszHideZeroURL) == 0);
}
