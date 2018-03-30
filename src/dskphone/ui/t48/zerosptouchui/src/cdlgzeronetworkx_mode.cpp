#include "cdlgzeronetworkx_mode.h"

#include "setting/src/networkcontroller.h"
#include "setting/include/common.h"
#include "settingui/src/networksettingmanager.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qcomboboxutility.h"
#include "baseui/fontcolor.h"
#include "inputmethod/inputmethod.h"
#include "devicelib/phonedevice.h"

#include "dsklog/log.h"

Q_DECLARE_METATYPE(Network802DotxModeType);
#define doNetworkControl (NS_SETTING::CNetworkController::GetInstance())


CDlgZeroNetworkX_Mode::CDlgZeroNetworkX_Mode(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCmbBoxX_Mode(NULL)
    , m_pEditIdentity(NULL)
    , m_pEditPassword(NULL)
{
    InitData();
}

CDlgZeroNetworkX_Mode::~CDlgZeroNetworkX_Mode()
{
    qUninstallIME(m_pEditPassword);
    qUninstallIME(m_pEditIdentity);
    m_vecWidgetPair.clear();
}

void CDlgZeroNetworkX_Mode::InitData()
{
    m_pCmbBoxX_Mode = new QComboBox(this);
    m_pEditIdentity = new QLineEdit(this);
    m_pEditPassword = new QLineEdit(this);

    if (JudgetWgtNull())
    {
        return;
    }

    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxX_Mode, Qt::black);
    m_pCmbBoxX_Mode->addItem(LANG_TRANSLATE(TRID_DISABLED),
                             QVariant::fromValue(NETWOKR_802Dotx_Disable));
    m_pCmbBoxX_Mode->addItem(LANG_TRANSLATE(TRID_EAP_MD5),
                             QVariant::fromValue(NETWOKR_802Dotx_EAP_MD5));
    m_pCmbBoxX_Mode->addItem(LANG_TRANSLATE(TRID_EAP_TLS),
                             QVariant::fromValue(NETWOKR_802Dotx_EAP_TLS));
    m_pCmbBoxX_Mode->addItem(LANG_TRANSLATE(TRID_EAP_PEAP_SLASH_MSCHAP_V2),
                             QVariant::fromValue(NETWOKR_802Dotx_PEAP_MD5));
    m_pCmbBoxX_Mode->addItem(LANG_TRANSLATE(TRID_EAP_TLS_OR_EAP_MSCHAP_V2),
                             QVariant::fromValue(NETWOKR_802Dotx_EAP_TTLS_MS));
    m_pCmbBoxX_Mode->addItem(LANG_TRANSLATE(TRID_EAP_PEAP_GTC),
                             QVariant::fromValue(NETWOKR_802Dotx_PEAP_GTC));
    m_pCmbBoxX_Mode->addItem(LANG_TRANSLATE(TRID_EAP_TTLS_SLASH_PEAP_GTC),
                             QVariant::fromValue(NETWOKR_802Dotx_TTLS_GTC));
    m_pCmbBoxX_Mode->addItem(LANG_TRANSLATE(TRID_EAP_FAST),
                             QVariant::fromValue(NETWORK_802Dotx_EAP_FAST));

    SetLineEditDefaultStyle(m_pEditIdentity);
    SetLineEditDefaultStyle(m_pEditPassword);

    qInstallIME(m_pEditIdentity, IME_DIAL_2AB);
    qInstallIME(m_pEditPassword, IME_PWD);

    m_pEditPassword->setEchoMode(QLineEdit::Password);
    qLineEditUtility::SetMaxLength(m_pEditIdentity, EDIT_TYPE_NAME_SHORT);
    qLineEditUtility::SetMaxLength(m_pEditPassword, EDIT_TYPE_PASSWORD);

    m_pCmbBoxX_Mode->setCurrentIndex(-1);
    connect(m_pCmbBoxX_Mode, SIGNAL(currentIndexChanged(int)),
            this, SLOT(OnCmbBoxX_ModeCurrentIndexChanged(int)));

    //为界面上各输入控件安装事件过滤器
    m_pCmbBoxX_Mode->installEventFilter(this);
    m_pEditIdentity->installEventFilter(this);
    m_pEditPassword->installEventFilter(this);
    this->installEventFilter(this);
}

void CDlgZeroNetworkX_Mode::OnCmbBoxX_ModeCurrentIndexChanged(int nIndex)
{
    m_pEditIdentity->hide();
    m_pEditPassword->hide();
    MakeReadyForRelayout();

    QWidgetPair qPair;
    QString strLab = LANG_TRANSLATE(TRID_802_DOT_1_X_MODE);

    qPair.first = strLab;
    qPair.second = m_pCmbBoxX_Mode;
    m_vecWidgetPair.push_back(qPair);

    Network802DotxModeType currentMode = m_pCmbBoxX_Mode->itemData(
            nIndex).value<Network802DotxModeType>();
    if (NETWOKR_802Dotx_TYPE_NONE != currentMode
            && NETWOKR_802Dotx_Disable != currentMode)
    {
        strLab = LANG_TRANSLATE(TRID_IDENTIFY);
        qPair.first = strLab;
        qPair.second = m_pEditIdentity;
        m_vecWidgetPair.push_back(qPair);

        strLab = LANG_TRANSLATE(TRID_MD5_PASSWORD);
        qPair.first = strLab;
        qPair.second = m_pEditPassword;
        m_vecWidgetPair.push_back(qPair);
    }

    emit ReLayout(this);
}

//通过控制获取已有配置数据，通过V层在界面上显示出来
bool CDlgZeroNetworkX_Mode::LoadPageData()
{
    if (JudgetWgtNull())
    {
        return true;
    }

    this->blockSignals(true);
    //设置首选输入法
    qLineEditUtility::SetCurrentInputMethod(m_pEditIdentity, EDIT_TYPE_NAME);
    qLineEditUtility::SetCurrentInputMethod(m_pEditPassword, EDIT_TYPE_PASSWORD);

    Network802Dot1xData currentPageData;

    //调用控制层：加载数据
    bool bIsSuccess = doNetworkControl->Get802Dot1xData(&currentPageData);

    if (bIsSuccess)
    {
        SetCurrentPageData(currentPageData);
        this->blockSignals(false);
        return true;
    }
    else
    {
        this->blockSignals(false);
        return false;
    }
}

//通过控制获取配置数据，与GetPageData值比较，有改动时返回true
bool CDlgZeroNetworkX_Mode::IsStatusChanged()
{
    Network802Dot1xData currentPageData;
    Network802Dot1xData beforePageData;

    GetCurrentPageData(currentPageData);

    //调用控制层：加载数据
    bool bIsSuccess = doNetworkControl->Get802Dot1xData(&beforePageData);

    if (bIsSuccess)
    {
        if ((beforePageData.m_eMode == currentPageData.m_eMode)
                && (NETWOKR_802Dotx_Disable == currentPageData.m_eMode))
        {
            return false;
        }

        if (ComparePageData(beforePageData, currentPageData))
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

bool CDlgZeroNetworkX_Mode::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (NETWOKR_802Dotx_Disable != m_pCmbBoxX_Mode->itemData(
                m_pCmbBoxX_Mode->currentIndex()).value<Network802DotxModeType>())
    {
        if (!m_pEditIdentity->text().isEmpty()
                && !qLineEditUtility::IsFormatValid(m_pEditIdentity->text(), EDIT_TYPE_CODE))
        {
            objErrorContent.strErrorContent = TRID_IDENTIFY_IS_INVALID;
            objErrorContent.pLineEdit = m_pEditIdentity;
            return false;
        }
        if (!m_pEditPassword->text().isEmpty()
                && !qLineEditUtility::IsFormatValid(m_pEditPassword->text(), EDIT_TYPE_PASSWORD))
        {
            objErrorContent.strErrorContent = TRID_PASSWORD_IS_INVALID;
            objErrorContent.pLineEdit = m_pEditPassword;
            return false;
        }
    }
    return true;
}

void CDlgZeroNetworkX_Mode::ReturnAllEdit()
{
    if (NULL != m_pEditPassword && NULL != m_pEditIdentity)
    {
        qReturnEdit(m_pEditIdentity);
        qReturnEdit(m_pEditPassword);
    }
}

// 卸载界面上所有的输入法
void CDlgZeroNetworkX_Mode::UninstallAllIME()
{
    if (NULL != m_pEditPassword && NULL != m_pEditIdentity)
    {
        qUninstallIME(m_pEditIdentity);
        qUninstallIME(m_pEditPassword);
    }
}

//读取当前界面的配置，通过C层保存
bool CDlgZeroNetworkX_Mode::SavePageData()
{
    if (JudgetWgtNull())
    {
        return true;
    }

    if (IsStatusChanged())
    {
        Network802Dot1xData currentPageData;
        GetCurrentPageData(currentPageData);
        if (0 == currentPageData.m_eMode)
        {
            Network802Dot1xData logicPageData;

            //调用控制层：加载数据
            bool bIsSuccess =
                doNetworkControl->Get802Dot1xData(&logicPageData);
            logicPageData.m_eMode = currentPageData.m_eMode;

            doNetworkControl->Set802Dot1xData(logicPageData);
        }
        else
        {
            //调用控制层：保存数据
            doNetworkControl->Set802Dot1xData(currentPageData);
        }

        //设置网络改变标记
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }
    return true;
}

//比较页面前后数据
bool CDlgZeroNetworkX_Mode::ComparePageData(const Network802Dot1xData & beforePageData,
        const Network802Dot1xData & afterPageData) const
{
    if (beforePageData.m_eMode != afterPageData.m_eMode
            || (0 != afterPageData.m_eMode && beforePageData.m_strId != afterPageData.m_strId)
            || (0 != afterPageData.m_eMode && beforePageData.m_strPassword != afterPageData.m_strPassword))
    {
        return false;
    }
    else
    {
        return true;
    }
}

//取得当前页面数据
Network802Dot1xData & CDlgZeroNetworkX_Mode::GetCurrentPageData(Network802Dot1xData &
        currentPageData) const
{
    currentPageData.m_eMode = m_pCmbBoxX_Mode->itemData(
                                  m_pCmbBoxX_Mode->currentIndex()).value<Network802DotxModeType>();
    currentPageData.m_strId =
        m_pEditIdentity->text().trimmed().toUtf8().data();
    currentPageData.m_strPassword =
        m_pEditPassword->text().trimmed().toUtf8().data();

    return currentPageData;
}

//设置当前页面数据
void CDlgZeroNetworkX_Mode::SetCurrentPageData(Network802Dot1xData & currentPageData)
{
    int nCount = 0;
    nCount = m_pCmbBoxX_Mode->count();
    for (int i = 0; i < nCount; i++)
    {
        if (currentPageData.m_eMode
                == m_pCmbBoxX_Mode->itemData(i).value<Network802DotxModeType>())
        {
            m_pCmbBoxX_Mode->setCurrentIndex(i);
            break;
        }
    }
    m_pEditIdentity->setText(toQString(currentPageData.m_strId.c_str()));
    m_pEditPassword->setText(toQString(currentPageData.m_strPassword.c_str()));
}

// 控件判空
bool CDlgZeroNetworkX_Mode::JudgetWgtNull()
{
    if (NULL == m_pCmbBoxX_Mode
            || NULL == m_pEditIdentity
            || NULL == m_pEditPassword)
    {
        return true;
    }
    else
    {
        return false;
    }
}
