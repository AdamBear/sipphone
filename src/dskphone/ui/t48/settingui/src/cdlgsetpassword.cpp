#include "model.h"
#include "cdlgsetpassword.h"
#include "commonapi/systemhelperapi.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "setting/src/setpasswordcontroller.h"
#include "uikernel/languagehelper.h"
#include "inputmethod/inputmethod.h"
#include "messagebox/modmessagebox.h"
#include "baseui/fontcolor.h"
#include "uikernel/qlineeditutility.h"
#include "mainwnd/mainwnd.h"
#include "feature/include/modfeature.h"
#include <algorithm>
#include "dsklog/log.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"

#if IF_FEATURE_AUTOP_IME
#include "commonunits/modcommonunits.h"
#endif

namespace
{
static const int MSG_ID_CONFIRM_SET_BLANK_PWD(100);
static const int MSG_ID_PWD_SIMILAR_TO_USERNAME(101);
static const int MSG_ID_PWD_TOO_SIMPLE(102);
}

CDlgSetPassword::CDlgSetPassword(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pEditConfirmPassword(NULL)
    , m_pEditCurrentPassword(NULL)
    , m_pEditNewPassword(NULL)
{
    InitData();
}

void CDlgSetPassword::InitData()
{
    m_pEditCurrentPassword = new QLineEdit(this);
    m_pEditNewPassword = new QLineEdit(this);
    m_pEditConfirmPassword = new QLineEdit(this);
    if (NULL == m_pEditCurrentPassword
            || NULL == m_pEditNewPassword
            || NULL == m_pEditConfirmPassword)
    {
        return;
    }

    //安装输入法
    qInstallIME(m_pEditCurrentPassword, IME_PWD);
    qInstallIME(m_pEditNewPassword, IME_PWD);
    qInstallIME(m_pEditConfirmPassword, IME_PWD);

    //最大输入长度限制
    qLineEditUtility::SetMaxLength(m_pEditCurrentPassword, EDIT_TYPE_PASSWORD);
    qLineEditUtility::SetMaxLength(m_pEditNewPassword, EDIT_TYPE_PASSWORD);
    qLineEditUtility::SetMaxLength(m_pEditConfirmPassword, EDIT_TYPE_PASSWORD);

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

    strLab = LANG_TRANSLATE(TRID_OLD_PWD);
    qPair.first = strLab;
    qPair.second = m_pEditCurrentPassword;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_NEW_PASSWORD);
    qPair.first = strLab;
    qPair.second = m_pEditNewPassword;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_CONFIRM_PWD);
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

bool CDlgSetPassword::GetPageData(PasswordData * pData)
{
    if (NULL == pData)
    {
        return false;
    }
    //进行密码确认
    if (m_pEditConfirmPassword->text() != m_pEditNewPassword->text())
    {
        return false;
    }
    pData->m_strPasswordBefore = m_pEditCurrentPassword->text().toUtf8().data();
    pData->m_strPasswordNew = m_pEditConfirmPassword->text().toUtf8().data();
    return true;
}

bool CDlgSetPassword::LoadPageData()
{
    this->setFocus();
    //设置首选输入法
#if IF_FEATURE_AUTOP_IME
    yl::string strPasswordIME = commonUnits_GetIMEByType(kszAdvancedSettingPasswordIME);
    if (!strPasswordIME.empty())
    {
        qSetCurrentIME(m_pEditCurrentPassword, strPasswordIME.c_str(), 0, kszAdvancedSettingPasswordIME);
        qSetCurrentIME(m_pEditNewPassword, strPasswordIME.c_str(), 0, kszAdvancedSettingPasswordIME);
        qSetCurrentIME(m_pEditConfirmPassword, strPasswordIME.c_str(), 0, kszAdvancedSettingPasswordIME);
    }
#else
    qLineEditUtility::SetCurrentInputMethod(m_pEditCurrentPassword, EDIT_TYPE_PASSWORD);
    qLineEditUtility::SetCurrentInputMethod(m_pEditNewPassword, EDIT_TYPE_PASSWORD);
    qLineEditUtility::SetCurrentInputMethod(m_pEditConfirmPassword, EDIT_TYPE_PASSWORD);
#endif

    //密码界面不需要初始数据的显示，因此不做操作
    return true;
}

bool CDlgSetPassword::IsStatusChanged()
{
    if (m_pEditConfirmPassword->text().isEmpty()
            && m_pEditCurrentPassword->text().isEmpty()
            && m_pEditNewPassword->text().isEmpty())
    {
        return false;
    }

    return true;
}

bool CDlgSetPassword::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (!m_pEditNewPassword->text().isEmpty()
            && !qLineEditUtility::IsFormatValid(m_pEditNewPassword->text(), EDIT_TYPE_PASSWORD))
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_NEW_PASSWORD_IS_INVALID);
        objErrorContent.pLineEdit = m_pEditNewPassword;
        return false;
    }
    return true;
}

bool CDlgSetPassword::SavePageData()
{
    PasswordData pageData;
    yl::string strUser = feature_UserModeGetUserName(feature_UserModeGetCurrType());
    yl::string strPassword = qReturnEdit(m_pEditCurrentPassword).toUtf8().data();
    if (FullKeyboardEnabled())
    {
        strPassword = fromQString(m_pEditCurrentPassword->text());
    }

    bool bPasswdCorrent = commonAPI_CheckPassword(strUser.c_str(), strPassword.c_str());

    if (bPasswdCorrent && configParser_GetConfigInt(kszUserPwdLimit) != 0
            && feature_UserModeGetCurrType() != UT_ADMIN)
    {
        if (m_pEditNewPassword->text().toUtf8().data() == strUser)
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_NEED_TO_CHANG_PASSWD), MESSAGEBOX_NOTE, 1000);
            // 重新设置蕉点
            if (NULL != m_pEditNewPassword)
            {
                m_pEditNewPassword->setText(toQString(""));
                m_pEditNewPassword->setFocus();
            }
            return false;
        }
    }

    //判断旧密码是否输入正确
    if (!bPasswdCorrent)
    {
        //输入的旧密码有误
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_THE_OLD_PASSWORD_IS_WRONG),
                                    MESSAGEBOX_Cancel);
        m_pEditCurrentPassword->setFocus();
        return false;
    }

    //获取界面数据，存入pageData
    //两次密码输入不同
    if (!GetPageData(&pageData))
    {
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_CONFIRM_PASSWORD_MISMATCH),
                                    MESSAGEBOX_Cancel);
        m_pEditNewPassword->setFocus();
        return false;
    }

    //密码过于简单时弹出提示
    if (PopPasswordNotify(&pageData))
    {
        OnConfirmSave();
    }

    return false;
}

void CDlgSetPassword::OnConfirmSave()
{
    yl::string strUser = feature_UserModeGetUserName(feature_UserModeGetCurrType());
    PasswordData pageData;
    GetPageData(&pageData);

    //将pageData设置为配置数据
    if (g_pSetPasswordController->SetPassword(strUser, pageData))
    {
        // save success and clear text content
        m_pEditConfirmPassword->clear();
        m_pEditCurrentPassword->clear();
        m_pEditNewPassword->clear();

        //保存密码成功
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_SAVING_CONFIG_FILE),
                                    MESSAGEBOX_NOTE, SHORT_TIME, "", "",
                                    MSG_ID_SHOW_NOTE_BEFORE_BACK);
        return;
    }
    else
    {
        //保存密码失败
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_FAIL_TO_MODIFY_PASSWORD), MESSAGEBOX_Cancel);
        return;
    }
}

//增加密码过于简单的提示
bool CDlgSetPassword::PopPasswordNotify(const PasswordData * pData)
{
    // 密码为空
    if (pData->m_strPasswordNew.empty())
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_ARE_YOU_SURE_TO_SET_BLANK_PASSWORD),
                                    MESSAGEBOX_OkCancel, -1, "", "",
                                    MSG_ID_CONFIRM_SET_BLANK_PWD);
        return false;
    }

    // 密码是账号的倒叙或与账号相同
    yl::string strUserName = feature_UserModeGetUserName(feature_UserModeGetCurrType());
    yl::string strUserNameFlip = strUserName;

    reverse(strUserNameFlip.begin(), strUserNameFlip.end());

    if (pData->m_strPasswordNew == strUserName
            || pData->m_strPasswordNew == strUserNameFlip)
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_NEW_PASSWORD_IS_SIMILAR_TO_USERNAME_ARE_YOU_SURE_TO_SET_IT),
                                    MESSAGEBOX_OkCancel, -1, "", "",
                                    MSG_ID_PWD_SIMILAR_TO_USERNAME);
        return false;
    }

    // 密码中包含字母、数字和特殊字符
    bool bHaveDigit = false;
    bool bHaveAlpha = false;
    bool bHavePuct = false;
    int nLength = pData->m_strPasswordNew.length();

    if (nLength >= 6)
    {
        for (int i = 0; i < nLength; ++i)
        {
            if (isdigit(pData->m_strPasswordNew[i]))
            {
                bHaveDigit = true;
            }
            else if (isalpha(pData->m_strPasswordNew[i]))
            {
                bHaveAlpha = true;
            }
            else if (ispunct(pData->m_strPasswordNew[i]))
            {
                bHavePuct = true;
            }

            if (bHaveDigit && bHaveAlpha && bHavePuct)
            {
                break;
            }
        }
    }

    if (!bHaveAlpha || !bHaveDigit || !bHavePuct || nLength < 6)
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_NEW_PASSWORD_IS_TOO_SIMILAR_ARE_YOU_SURE_TO_SET_IT),
                                    MESSAGEBOX_OkCancel, -1, "", "",
                                    MSG_ID_PWD_TOO_SIMPLE);
        return false;
    }

    return true;
}

CDlgSetPassword::~CDlgSetPassword()
{
}

void CDlgSetPassword::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int iRet = pMessageBox->GetResult();
    switch (pMessageBox->GetID())
    {
    case  MSG_ID_CONFIRM_SET_BLANK_PWD:
    case  MSG_ID_PWD_SIMILAR_TO_USERNAME:
    case  MSG_ID_PWD_TOO_SIMPLE:
    {
        if (MessageBox_OK == iRet)
        {
            OnConfirmSave();
            return;
        }
    }
    break;
    default:
        return;
    }
}
