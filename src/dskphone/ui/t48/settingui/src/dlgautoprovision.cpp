#include "model.h"
#include "dlgautoprovision.h"
#include "setting/src/autoprovisioncontroller.h"
#include "setting/src/callforwardcontroller.h"

#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "inputmethod/inputmethod.h"
#include "uikernel/qlineeditutility.h"
#include "messagebox/modmessagebox.h"
#include "mainwnd/mainwnd.h"
#include "settinguicommon.h"
#include "baseui/t4xpicpath.h"

#if IF_FEATURE_AUTOP_IME
#include "commonunits/modcommonunits.h"
#include "configiddefine.h"
#include "modconfigparser.h"
#endif

CDlgAutoProvision::CDlgAutoProvision(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pEditURL(NULL)
    , m_pEditUserName(NULL)
    , m_pEditPassword(NULL)
    , m_pEditCommon(NULL)
    , m_pEditMACOriented(NULL)
    , m_bWidgetInitSucceed(false)
{
    //初始化数据
    InitData();
}

CDlgAutoProvision::~CDlgAutoProvision()
{

}

//通过C层获取已有配置数据，通过V层在界面上显示出来
bool CDlgAutoProvision::LoadPageData()
{
    this->setFocus();
    //设置首选输入法
    qLineEditUtility::SetCurrentInputMethod(m_pEditURL, EDIT_TYPE_URL);
    qLineEditUtility::SetCurrentInputMethod(m_pEditCommon, EDIT_TYPE_AES);
    qLineEditUtility::SetCurrentInputMethod(m_pEditMACOriented, EDIT_TYPE_AES);

#if IF_FEATURE_AUTOP_IME
    yl::string strUserIME = commonUnits_GetIMEByType(kszZeroUserIME);
    if (!strUserIME.empty())
    {
        qSetCurrentIME(m_pEditUserName, strUserIME.c_str(), 0, kszZeroUserIME);
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

    //调用控制层：加载数据
    bool bIsSuccess =
        g_pAutoProvisionController->GetAutoProvisionData(&m_AutoProvisionData);

    if (bIsSuccess)
    {
        //加载数据成功，设置当前页面数据,并返回true
        SetCurrentPageData(m_AutoProvisionData);
        return true;
    }
    else
    {
        //加载数据失败，返回false
        return false;
    }
}


//通过C层获取配置数据，与GetPageData值比较，有改动时返回true
bool CDlgAutoProvision::IsStatusChanged()
{
    AutoProvisionData currentPageData;
    GetCurrentPageData(currentPageData);
    if (m_AutoProvisionData == currentPageData)
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

bool CDlgAutoProvision::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (NULL != m_pEditURL && !m_pEditURL->text().isEmpty()
            && !qLineEditUtility::IsFormatValid(m_pEditURL->text(), EDIT_TYPE_URL))
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_URL_IS_INAVLID);
        objErrorContent.pLineEdit = m_pEditURL;
        return false;
    }
    if (NULL != m_pEditUserName && !m_pEditUserName->text().isEmpty()
            && !qLineEditUtility::IsFormatValid(m_pEditUserName->text(), EDIT_TYPE_NAME))
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_USER_NAME_IS_INVALID);
        objErrorContent.pLineEdit = m_pEditUserName;
        return false;
    }
    if (NULL != m_pEditPassword && !m_pEditPassword->text().isEmpty()
            && !qLineEditUtility::IsFormatValid(m_pEditPassword->text(), EDIT_TYPE_PASSWORD))
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_PASSWORD_IS_INVALID);
        objErrorContent.pLineEdit = m_pEditPassword;
        return false;
    }

    if (NULL != m_pEditCommon && !m_pEditCommon->text().isEmpty()
            && !qLineEditUtility::IsFormatValid(m_pEditCommon->text(), EDIT_TYPE_AES))
    {
        objErrorContent.strErrorContent = TRID_COMMON_AES_KEY_IS_INVALID;
        objErrorContent.pLineEdit = m_pEditCommon;
        return false;
    }

    if (NULL != m_pEditMACOriented && !m_pEditMACOriented->text().isEmpty()
            && !qLineEditUtility::IsFormatValid(m_pEditMACOriented->text(), EDIT_TYPE_AES))
    {
        objErrorContent.strErrorContent = TRID_MAC_ORIENTED_IS_INVALID;
        objErrorContent.pLineEdit = m_pEditMACOriented;
        return false;
    }

    return true;
}

//从V层读取当前界面的配置，通过C层保存
bool CDlgAutoProvision::SavePageData()
{
    if (IsStatusChanged())
    {
        //提起当前页面数据
        GetCurrentPageData(m_AutoProvisionData);
        //调用控制层：保存设置数据
        g_pAutoProvisionController->SetAutoProvisionData(m_AutoProvisionData);
    }

    //Zero模式下或者填写的URL为空，不弹出更新提示
    if (m_ePageMode != PAGE_MODE_ZERO
            && !m_pEditURL->text().trimmed().isEmpty())
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_IF_PROVISION_NOW),
                                    MESSAGEBOX_OkCancel);
    }
    else
    {
        return true;
    }

    return false;
}

void CDlgAutoProvision::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL != pMessageBox
            && pMessageBox->GetResult() == MessageBox_OK)
    {
        g_pAutoProvisionController->BeginAutoProvision();
    }

    emit BtnBackClick();
}

//用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
void CDlgAutoProvision::InitData()
{
    m_pEditURL = new QLineEdit(this);
    m_pEditUserName = new QLineEdit(this);
    m_pEditPassword = new QLineEdit(this);
    m_pEditCommon = new QLineEdit(this);
    m_pEditMACOriented = new QLineEdit(this);

    m_bWidgetInitSucceed = true;

    if (NULL == m_pEditURL
            || NULL == m_pEditUserName
            || NULL == m_pEditPassword
            || NULL == m_pEditCommon
            || NULL == m_pEditMACOriented)
    {
        m_bWidgetInitSucceed = false;
        return;
    }

    //为输入控件安装输入法
    qInstallIME(m_pEditURL, IME_ALL);
#if IF_FEATURE_AUTOP_IME
    qInstallIME(m_pEditUserName, IME_Name);
#else
    qInstallIME(m_pEditUserName, IME_DIAL);
#endif

    qInstallIME(m_pEditPassword, IME_PWD);
    qInstallIME(m_pEditCommon, IME_LOWER);
    qInstallIME(m_pEditMACOriented, IME_LOWER);

    //最大输入长度
    qLineEditUtility::SetMaxLength(m_pEditURL, EDIT_TYPE_URL);
    qLineEditUtility::SetMaxLength(m_pEditUserName, EDIT_TYPE_PHONE_NUMBER);        //特殊限制32个?
    qLineEditUtility::SetMaxLength(m_pEditPassword, EDIT_TYPE_PASSWORD);
    qLineEditUtility::SetMaxLength(m_pEditCommon, EDIT_TYPE_AES);
    qLineEditUtility::SetMaxLength(m_pEditMACOriented, EDIT_TYPE_AES);

    //设置输入框风格
    qLineEditUtility::SetLineEditStyle(m_pEditURL, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditUserName, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditPassword, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditCommon, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditMACOriented, Qt::black);

    //设置密码回显示方式
    m_pEditPassword->setEchoMode(QLineEdit::Password);
    m_pEditCommon->setEchoMode(QLineEdit::Password);
    m_pEditMACOriented->setEchoMode(QLineEdit::Password);

    AddWidgetToDelegate();

    //为所有控件和本界面安装事件过滤器
    m_pEditURL->installEventFilter(this);
    m_pEditUserName->installEventFilter(this);
    m_pEditPassword->installEventFilter(this);
    m_pEditCommon->installEventFilter(this);
    m_pEditMACOriented->installEventFilter(this);

    this->installEventFilter(this);
}

//取得当前页面数据
void CDlgAutoProvision::GetCurrentPageData(AutoProvisionData & currentPageData) const
{
    if (!IsInitSucceed())
    {
        return;
    }

    //提起数据：user name
    currentPageData.m_strUsername =
        m_pEditUserName->text().trimmed().toUtf8().data();
    //提起数据：URL
    currentPageData.m_strUrl =
        m_pEditURL->text().trimmed().toUtf8().data();
    //提起数据：password
    currentPageData.m_strPassword =
        m_pEditPassword->text().trimmed().toUtf8().data();

    // 提取数据：Common
    currentPageData.m_strAesKey = fromQString(m_pEditCommon->text().trimmed());
    // 提取数据：MAC-Oriented
    currentPageData.m_strAesMacKey = fromQString(m_pEditMACOriented->text().trimmed());
}

//设置当前页面数据
void CDlgAutoProvision::SetCurrentPageData(AutoProvisionData & currentPageData)
{
    if (!IsInitSucceed())
    {
        return;
    }

    //设置数据：user name
    m_pEditUserName->setText(toQString(currentPageData.m_strUsername.c_str()));
    //设置数据：URL
    m_pEditURL->setText(toQString(currentPageData.m_strUrl.c_str()));
    //设置数据：password
    m_pEditPassword->setText(toQString(currentPageData.m_strPassword.c_str()));
    // 设置数据：Common
    m_pEditCommon->setText(toQString(currentPageData.m_strAesKey.c_str()));
    // 设置数据：MAC-Oriented
    m_pEditMACOriented->setText(toQString(currentPageData.m_strAesMacKey.c_str()));
}

// 设置页面的SoftKey
bool CDlgAutoProvision::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    if (m_ePageMode != PAGE_MODE_ZERO)
    {
        // 设置二级菜单界面对应SoftKeybar的数据
        m_SoftkeyData[3].m_strSoftkeyAction = ACTION_SAVE_DATA;
        m_SoftkeyData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SAVE);
    }
    else if (m_ePageMode == PAGE_MODE_ZERO)
    {
        objWndData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_OK);
        objWndData[3].m_strSoftkeyAction = ACTION_NEXT_PAGE;
        objWndData[3].m_strIconPath = PIC_SOFTKEY_NEXT;
    }

    return true;
}

void CDlgAutoProvision::UninstallAllIME()
{
    qUninstallIME(m_pEditURL);
    qUninstallIME(m_pEditUserName);
    qUninstallIME(m_pEditPassword);
    qUninstallIME(m_pEditCommon);
    qUninstallIME(m_pEditMACOriented);
}
void CDlgAutoProvision::AddWidgetToDelegate()
{
    if (!IsInitSucceed())
    {
        return;
    }

    MakeReadyForRelayout();

    QWidgetPair qPair;
    QString strLab;

    qPair.first = LANG_TRANSLATE(TRID_URL);
    qPair.second = m_pEditURL;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_USER_NAME);
    qPair.second = m_pEditUserName;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_PASSWORD);
    qPair.second = m_pEditPassword;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_COMMON_AES_KEY);
    qPair.second = m_pEditCommon;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_MAC_ORIENTED);
    qPair.second = m_pEditMACOriented;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);
}

bool CDlgAutoProvision::IsInitSucceed() const
{
    return m_bWidgetInitSucceed;
}
