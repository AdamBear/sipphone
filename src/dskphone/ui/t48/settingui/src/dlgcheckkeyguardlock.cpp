#include "dlgcheckkeyguardlock.h"

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
#include "modsettingui.h"
#include "configiddefine.h"
#include "commonunits/modcommonunits.h"
#include "keyguard/include/modkeyguard.h"
#include "cdlgunlockpin.h"

CDlgCheckKeyGuardLock::CDlgCheckKeyGuardLock(QWidget * parent /* = 0 */)
    : CDlgBaseSubPage(parent)
{
    InitData();
}

CDlgCheckKeyGuardLock::~CDlgCheckKeyGuardLock()
{
}

bool CDlgCheckKeyGuardLock::LoadPageData()
{
    if (NULL == m_pEditPassword)
    {
        return false;
    }

    this->setFocus();

    return true;
}

bool CDlgCheckKeyGuardLock::IsStatusChanged()
{
    if (NULL != m_pEditPassword && !m_pEditPassword->text().isEmpty())
    {
        return true;
    }

    return false;
}

bool CDlgCheckKeyGuardLock::OnAction(const QString & strAction)
{
    if (ACTION_SAVE_DATA == strAction)
    {
        CheckPassword();
        return true;
    }

    return CDlgBaseSubPage::OnAction(strAction);
}

void CDlgCheckKeyGuardLock::InitData()
{
    m_pEditPassword = new QLineEdit(this);

    if (NULL == m_pEditPassword)
    {
        return;
    }

    qInstallIME(m_pEditPassword, IME_PWD);
    // 设置默认的输入法
#if IF_FEATURE_AUTOP_IME
    yl::string strIME = commonUnits_GetIMEByType(kszUnlockPinIME);

    if (!strIME.empty())
    {
        qSetCurrentIME(m_pEditPassword, strIME.c_str(), 0, kszUnlockPinIME);
    }
#else
    qSetCurrentIME(m_pEditPassword, IME_123_Pwd, 0, kszUnlockPinIME);
#endif

    qLineEditUtility::SetMaxLength(m_pEditPassword, EDIT_TYPE_PASSWORD);
    qLineEditUtility::SetLineEditStyle(m_pEditPassword, Qt::black);

    m_pEditPassword->setEchoMode(QLineEdit::Password);

    MakeReadyForRelayout();

    QWidgetPair qPair;
    QString strLab;

    qPair.first = LANG_TRANSLATE(TRID_PLEASE_ENTER_UNLOCK_PIN);
    qPair.second = m_pEditPassword;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);
}

void CDlgCheckKeyGuardLock::CheckPassword()
{
    if (NULL == m_pEditPassword)
    {
        return;
    }

    ReturnAllEdit();
    yl::string strInputText = fromQString(m_pEditPassword->text());
    PinType ptResult = keyGuard_GetPinType(strInputText);

    switch (ptResult)
    {
    case PIN_CORRECT_LOCK:
        {
            emit openpage(SubMenuData("OpenSubPage(key_guard_lock)"));
        }
        break;
    case PIN_CORRECT_ADMIN:
        {
            bool bAdminInput = true;
            int nEnterPageType = ENTER_CHANGE_PHONE_LOCK_PIN_BY_CHANGE_UNLOCK_INFO;

            SubMenuData objSubMenuData;
            objSubMenuData.strMenuAction = "unlock_pin";
            objSubMenuData.pBeforeSetSubpageData = &bAdminInput;
            objSubMenuData.pAfterSetSubpageData = &nEnterPageType;
            emit openpage(objSubMenuData);
        }
        break;
    default:
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_CURRENT_PIN_IS_INCORRECT), MESSAGEBOX_Cancel,
                                        2000);
        }
        break;
    }

    m_pEditPassword->setText("");
}
