#include "cdlgaskpassword.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qlineeditutility.h"
#include "baseui/fontcolor.h"
#include "inputmethod/inputmethod.h"
#include "include/keymap.h"
#include <QLineEdit>
#if IF_FEATURE_AUTOP_IME
#include "commonunits/modcommonunits.h"
#endif

#include "dsklog/log.h"

CDlgAskPassword::CDlgAskPassword(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pUserName(NULL)
    , m_pPswdEdit(NULL)
    , m_eType(ASK_PASSWORD_COMMON)
{
    SetWindowType(eWT_SubPage);

    InitData();
}

CDlgAskPassword::~CDlgAskPassword(void)
{
}

void CDlgAskPassword::Init()
{
    if (m_pPswdEdit != NULL)
    {
        m_pPswdEdit->setText("");
    }
}

void CDlgAskPassword::Uninit()
{
    disconnect(SIGNAL(SubmitPassword(const yl::string &)));
}

bool CDlgAskPassword::LoadPageData()
{
    InitEditProperty();

    InitListItem();

    return true;
}

bool CDlgAskPassword::IsStatusChanged()
{
    return true;
}

// 对输入框的数据进行验证
bool CDlgAskPassword::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (ASK_PASSWORD_WIFI_COMMON == m_eType)
    {
        return true;
    }
    else if (ASK_PASSWORD_WIFI_AUTHENTICATION == m_eType)
    {
        if (m_pUserName != NULL && m_pUserName->text().trimmed().isEmpty())
        {
            objErrorContent.pLineEdit = m_pUserName;
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_USER_NAME_COULD_NOT_BE_EMPTY);
            return false;
        }
    }
    else
    {
        if (m_pPswdEdit != NULL
                && m_pPswdEdit->text().trimmed().isEmpty())
        {
            objErrorContent.pLineEdit = m_pPswdEdit;
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_PASSWORD_CANNOT_BE_EMPTY);
            return false;
        }

        if (NULL != m_pPswdEdit
                && !qLineEditUtility::IsFormatValid(m_pPswdEdit->text(), EDIT_TYPE_PASSWORD))
        {
            objErrorContent.pLineEdit = m_pPswdEdit;
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_PASSWORD_IS_INVALID);
            return false;
        }
    }

    return true;
}

bool CDlgAskPassword::SavePageData()    // 保存页面数据
{
    yl::string strPswd;

    if (m_pPswdEdit != NULL)
    {
        strPswd = fromQString(m_pPswdEdit->text());
    }

    if (ASK_PASSWORD_WIFI_AUTHENTICATION == m_eType)
    {
        yl::string strUserName;
        if (m_pUserName != NULL)
        {
            strUserName = fromQString(m_pUserName->text());
        }

        emit submitNameAndPassword(strUserName, strPswd);
    }
    else
    {
        emit SubmitPassword(strPswd);
    }

    return true;
}

void CDlgAskPassword::OnBack()
{
    emit CancelInputPassword();
}

void CDlgAskPassword::SetData(void * pData /* = NULL */)
{
    if (ASK_PASSWORD_WIFI_AUTHENTICATION == m_eType)
    {
        m_strTitle = LANG_TRANSLATE(TRID_WIFI_AUTHENTICATION);
    }
    else
    {
        m_strTitle = LANG_TRANSLATE(TRID_ENTER_PASSWORD);
    }
}

bool CDlgAskPassword::GetSoftkeyData(CArraySoftKeyBarData & objSoftkeyData)
{
    objSoftkeyData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_OK);

    return true;
}

bool CDlgAskPassword::SaveCompleted()
{
    return false;
}

void CDlgAskPassword::InitData()
{
    m_pUserName = new QLineEdit(this);
    m_pPswdEdit  = new QLineEdit(this);

    if (NULL == m_pUserName || NULL == m_pPswdEdit)
    {
        return;
    }

    qLineEditUtility::SetLineEditStyle(m_pUserName, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pPswdEdit, Qt::black);

    m_pPswdEdit->setEchoMode(QLineEdit::Password);

    m_pUserName->installEventFilter(this);
    m_pPswdEdit->installEventFilter(this);
}

void CDlgAskPassword::InitListItem()
{
    QWidgetPair qPair;

    if (ASK_PASSWORD_WIFI_AUTHENTICATION == m_eType)
    {
        qPair.first = LANG_TRANSLATE(TRID_USER_NAME);
        qPair.second = m_pUserName;
        m_vecWidgetPair.push_back(qPair);
    }
    else if (m_pUserName != NULL)
    {
        m_pUserName->hide();
    }

    qPair.first = LANG_TRANSLATE(TRID_PASSWORD);
    qPair.second = m_pPswdEdit;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);
}

void CDlgAskPassword::InitEditProperty()
{
    if (NULL == m_pUserName || NULL == m_pPswdEdit)
    {
        return;
    }

    qInstallIME(m_pUserName, IME_Name);
    qInstallIME(m_pPswdEdit, IME_PWD);

    // 根据不同类型设置最长输入字符数
    if (ASK_PASSWORD_WIFI_AUTHENTICATION == m_eType || ASK_PASSWORD_WIFI_COMMON == m_eType)
    {
        qLineEditUtility::SetMaxLength(m_pUserName, EDIT_TYPE_NAME_SHORT);
        qLineEditUtility::SetMaxLength(m_pPswdEdit, EDIT_TYPE_WIFI_PWD);
    }
    else
    {
        qLineEditUtility::SetMaxLength(m_pPswdEdit, EDIT_TYPE_PASSWORD);
    }

    // 根据不同类型设置当前输入法
    if (ASK_PASSWORD_WIFI_AUTHENTICATION == m_eType)
    {
        qLineEditUtility::SetCurrentInputMethod(m_pUserName, EDIT_TYPE_NAME);
        qLineEditUtility::SetCurrentInputMethod(m_pPswdEdit, EDIT_TYPE_PASSWORD);
    }
    else
    {
        qLineEditUtility::SetCurrentInputMethod(m_pPswdEdit, EDIT_TYPE_NUMBER_PWD);
    }

#if IF_FEATURE_AUTOP_IME
    if (ASK_PASSWORD_WIFI_AUTHENTICATION == m_eType
            || ASK_PASSWORD_WIFI_COMMON == m_eType)
    {
        yl::string strPasswordIME = commonUnits_GetIMEByType(kszWifiPasswordIME);
        if (!strPasswordIME.empty())
        {
            qSetCurrentIME(m_pPswdEdit, strPasswordIME.c_str(), 0, kszWifiPasswordIME);
        }
    }
#endif
}

bool CDlgAskPassword::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL == pObject || NULL == pEvent)
    {
        return false;
    }

    switch (pEvent->type())
    {
    case QEvent::KeyPress:
    {
        QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;

        switch (pKeyEvent->type())
        {
        case PHONE_KEY_OK:
        {
            SavePageData();
            return true;
        }
        break;
        default:
            break;
        }
    }
    break;
    default:
        break;
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

void CDlgAskPassword::BeforeSetSubPage(void * pData)
{
    if (pData == NULL)
    {
        return;
    }

    m_eType = (ASK_PASSWORD_TYPE)(*((int *)pData));
}
