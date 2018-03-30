//#include "model.h"
#include "cdlgunlockpin.h"
#include "commonapi/systemhelperapi.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "uikernel/languagehelper.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "commonunits/modcommonunits.h"
#include "uikernel/qlineeditutility.h"
#include "settinguicommon.h"

#include "mainwndhelp/keyguardhelp.h"
#include "keyguard/include/modkeyguard.h"

CDlgUnlockPIN::CDlgUnlockPIN(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pEditConfirmPassword(NULL)
    , m_pEditCurrentPassword(NULL)
    , m_pEditNewPassword(NULL)
    , m_bHaveOriginPassword(true)
    , m_nEnterType(ENTER_CHANGE_PHONE_LOCK_PIN_BY_NORMAL)
{
    InitData();
}

void CDlgUnlockPIN::InitData()
{
    m_pEditCurrentPassword = new QLineEdit(this);
    m_pEditNewPassword = new QLineEdit(this);
    m_pEditConfirmPassword = new QLineEdit(this);

    //指针判空
    if ((NULL == m_pEditCurrentPassword)
            || NULL == m_pEditNewPassword
            || NULL == m_pEditConfirmPassword)
    {
        return;
    }

    //安装输入法
    //只能使用数字输入
    qInstallIME(m_pEditCurrentPassword, IME_Number);
    qInstallIME(m_pEditNewPassword, IME_Number);
    qInstallIME(m_pEditConfirmPassword, IME_Number);

    //最大输入长度限制
    qLineEditUtility::SetMaxLength(m_pEditCurrentPassword, EDIT_TYPE_UNLOCKPIN);
    qLineEditUtility::SetMaxLength(m_pEditNewPassword, EDIT_TYPE_UNLOCKPIN);
    qLineEditUtility::SetMaxLength(m_pEditConfirmPassword, EDIT_TYPE_UNLOCKPIN);

    //设置输入框风格
    qLineEditUtility::SetLineEditStyle(m_pEditCurrentPassword, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditNewPassword, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditConfirmPassword, Qt::black);

    //密码回显
    m_pEditCurrentPassword->setEchoMode(QLineEdit::Password);
    m_pEditNewPassword->setEchoMode(QLineEdit::Password);
    m_pEditConfirmPassword->setEchoMode(QLineEdit::Password);

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_CURRENT_PIN);
    qPair.first = strLab;
    qPair.second = m_pEditCurrentPassword;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_NEW_PIN);
    qPair.first = strLab;
    qPair.second = m_pEditNewPassword;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_CONFIRM_PIN);
    qPair.first = strLab;
    qPair.second = m_pEditConfirmPassword;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    //为界面上各输入控件安装事件过滤器
    m_pEditCurrentPassword->installEventFilter(this);
    m_pEditNewPassword->installEventFilter(this);
    m_pEditConfirmPassword->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgUnlockPIN::Uninit()
{
    if (NULL != m_pEditCurrentPassword)
    {
        m_pEditCurrentPassword->clear();
    }

    if (NULL != m_pEditNewPassword)
    {
        m_pEditNewPassword->clear();
    }

    if (NULL != m_pEditConfirmPassword)
    {
        m_pEditConfirmPassword->clear();
    }
}

bool CDlgUnlockPIN::LoadPageData()
{
    SETTINGUI_INFO("CDlgUnlockPIN::LoadPageData");

    this->setFocus();

    //因为设置了只能使用数字的输入法，因此不需要设置首选输入法
    //设置首选输入法
//  if (m_bHaveOriginPassword)
//  {
//      qLineEditUtility::SetCurrentInputMethod(m_pEditCurrentPassword, EDIT_TYPE_UNLOCKPIN);
//  }
//  qLineEditUtility::SetCurrentInputMethod(m_pEditNewPassword, EDIT_TYPE_UNLOCKPIN);
//  qLineEditUtility::SetCurrentInputMethod(m_pEditConfirmPassword, EDIT_TYPE_UNLOCKPIN);

    return true;
}

bool CDlgUnlockPIN::IsStatusChanged()
{
    if (m_bHaveOriginPassword)
    {
        return !m_pEditCurrentPassword->text().isEmpty()
               || !m_pEditNewPassword->text().isEmpty()
               || !m_pEditConfirmPassword->text().isEmpty();
    }
    return !m_pEditNewPassword->text().isEmpty()
           || !m_pEditConfirmPassword->text().isEmpty();
}

bool CDlgUnlockPIN::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    //指针判空
    if ((NULL == m_pEditCurrentPassword && m_bHaveOriginPassword)
            || NULL == m_pEditNewPassword
            || NULL == m_pEditConfirmPassword)
    {
        return false;
    }

    if (m_bHaveOriginPassword)
    {
        if (!keyGuard_CheckUnlockPin(m_pEditCurrentPassword->text().toUtf8().data()))
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_CURRENT_PIN_IS_INCORRECT);
            objErrorContent.pLineEdit = m_pEditCurrentPassword;
            return false;
        }
    }

    // 判断新密码的正确性
    if (!m_pEditNewPassword->text().isEmpty()
            && !qLineEditUtility::IsFormatValid(m_pEditNewPassword->text(), EDIT_TYPE_UNLOCKPIN))
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_NEW_PASSWORD_IS_INVALID);
        objErrorContent.pLineEdit = m_pEditNewPassword;
        return false;
    }

    // 判断新密码重新验证是否正确
    if (m_pEditNewPassword->text() != m_pEditConfirmPassword->text())
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_CONFIRM_PIN_MISMATCH);
        objErrorContent.pLineEdit = m_pEditNewPassword;
        return false;
    }
    return true;
}

bool CDlgUnlockPIN::SavePageData()
{
    if (NULL == m_pEditNewPassword)
    {
        return true;
    }

    keyGuard_SaveUnlockPin(m_pEditNewPassword->text().toUtf8().data());

    if (ENTER_CHANGE_PHONE_LOCK_PIN_BY_CHANGE_UNLOCK_INFO == m_nEnterType)
    {
        emit backandopenpage(SubMenuData("key_guard_lock"));

        // 返回false,退出界面由本函数完成
        return false;
    }

    return true;
}

CDlgUnlockPIN::~CDlgUnlockPIN()
{
}


// 设置子页面前的操作
void CDlgUnlockPIN::BeforeSetSubPage(void * pData)
{
    if (NULL != pData)
    {
        m_bHaveOriginPassword = !(*((bool *)pData));
    }
    else
    {
        m_bHaveOriginPassword = true;
    }

    MakeReadyForRelayout();

    QWidgetPair qPair;
    QString strLab;

    if (m_bHaveOriginPassword)
    {
        strLab = LANG_TRANSLATE(TRID_CURRENT_PIN);
        qPair.first = strLab;
        qPair.second = m_pEditCurrentPassword;
        m_vecWidgetPair.push_back(qPair);

        if (NULL != m_pEditCurrentPassword)
        {
            m_pEditCurrentPassword->show();
        }
    }
    else
    {
        if (NULL != m_pEditCurrentPassword)
        {
            m_pEditCurrentPassword->hide();
        }

    }

    strLab = LANG_TRANSLATE(TRID_NEW_PIN);
    qPair.first = strLab;
    qPair.second = m_pEditNewPassword;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_CONFIRM_PIN);
    qPair.first = strLab;
    qPair.second = m_pEditConfirmPassword;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);
}

// 卸载界面上所有的输入法
void CDlgUnlockPIN::UninstallAllIME()
{
    qUninstallIME(m_pEditCurrentPassword);
    qUninstallIME(m_pEditNewPassword);
    qUninstallIME(m_pEditConfirmPassword);
}

void CDlgUnlockPIN::AfterSetSubPage(void * pData)
{
    if (NULL == pData)
    {
        m_nEnterType = ENTER_CHANGE_PHONE_LOCK_PIN_BY_NORMAL;
        return;
    }

    m_nEnterType = *((int *)pData);
}

bool CDlgUnlockPIN::OnAction(const QString & strAction)
{
    // 当由设置Phone Lock界面进入，按Back键时，需退出该界面进入到Phone Lock设置界面
    if (ENTER_CHANGE_PHONE_LOCK_PIN_BY_CHANGE_UNLOCK_INFO == m_nEnterType
            && ACTION_QUIT == strAction)
    {
        emit backandopenpage(SubMenuData("key_guard_lock"));

        return true;
    }

    return CDlgBaseSubPage::OnAction(strAction);
}
