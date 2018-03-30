#if IF_FEATURE_34162
#include "ldappasswordbox.h"
#include "modmessagebox.h"
#include "baseui/fontcolor.h"
#include "baseui/t4xpicpath.h"
#include "directoryui/include/directorycommon.h"
#include "keymap.h"
#include "imagemanager/modimagemanager.h"
#include "inputmethod/inputmethod.h"
#include "mainwnd/mainwnd.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "uikernel/quipushbutton.h"
#include "uikernel/qwidgetutility.h"
#include "uikernel/qscrolltextlabel.h"

#if IF_FEATURE_AUTOP_IME
#include "commonunits/modcommonunits.h"
#include "configiddefine.h"
#include "modconfigparser.h"
#endif

namespace
{
#define DLG_XSI_PWD_BTN_SET_LEFT_MARGIN 8
#define DLG_XSI_PWD_BTN_SET_RIGHT_MARGIN 8
#define DLG_XSI_PWD_BTN_SET_BOTTOM_MARGIN 9
#define DLG_XSI_PWD_BTN_HEIGHT 53
#define RECT_XSI_PWD_BOX QRect(171, 152, 458, 177)
#define RECT_XSI_USER_TITLE QRect(31, 20, 156, 36)
#define RECT_XSI_USER_VALUE QRect(190, 20, 246, 36)
#define RECT_XSI_PWD_TITLE QRect(31, 64, 156, 36)
#define RECT_XSI_PWD_EDIT QRect(190, 64, 246, 36)
}

CLdapPasswordBox::CLdapPasswordBox(CMaskFrame * pMaskFrame/* = NULL*/)
    : CMessageBoxBase(pMaskFrame)
    , m_pBtnIME(NULL)
    , m_pBtnDelete(NULL)
{
    m_LabUserId.setParent(this);
    m_EditUserId.setParent(this);
    m_LabPwd.setParent(this);
    m_EditPwd.setParent(this);

    m_LabUserId.setObjectName("LabUserId");
    m_EditUserId.setObjectName("EditUserId");
    m_LabPwd.setObjectName("LabPwd");
    m_EditPwd.setObjectName("EditPwd");

    Init();
}

CLdapPasswordBox::~CLdapPasswordBox(void)
{
    qUninstallIME(&m_EditPwd);
}

void CLdapPasswordBox::Init()
{
    setAutoFillBackground(true);
    setGeometry(RECT_XSI_PWD_BOX);

    QPalette palBG;
    QPixmap pmBG = THEME_GET_BMP(PIC_MSGBOX_BG_XSI_PWD);

    palBG.setBrush(QPalette::Window, QBrush(pmBG));
    setPalette(palBG);

    UpdateSoftkeyData();

    UpdateEditLineData();

    SetUserStr();
}

void CLdapPasswordBox::UpdateEditLineData()
{
    // UserId标题
    SetWgtFontColor(&m_LabUserId, FONT_TYPE_TITLE, Qt::white);
    m_LabUserId.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_LabUserId.setGeometry(RECT_XSI_USER_TITLE);

    // UserId值
    m_EditUserId.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_EditUserId.setGeometry(RECT_XSI_USER_VALUE);
    qInstallIME(&m_EditUserId, IME_ALL);
    qLineEditUtility::SetCurrentInputMethod(&m_EditUserId, EDIT_TYPE_NAME);
    qLineEditUtility::SetMaxLength(&m_EditUserId, EDIT_TYPE_NAME);
#if IF_FEATURE_AUTOP_IME
    yl::string strUserIME = commonUnits_GetIMEByType(kszLDAPUserIME);

    if (!strUserIME.empty())
    {
        qSetCurrentIME(&m_EditUserId, strUserIME.c_str(), 0, kszLDAPUserIME);
    }
#else
    qSetCurrentIME(&m_EditUserId, IME_2aB, 0, kszLDAPUserIME);
#endif

    qUseKeyBoardFromIME(&m_EditUserId, INPUT_DIALOG_ENTER, true);

    // Passwod标题
    SetWgtFontColor(&m_LabPwd, FONT_TYPE_TITLE, Qt::white);
    m_LabPwd.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_LabPwd.setGeometry(RECT_XSI_PWD_TITLE);

    // Password输入框
    m_EditPwd.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_EditPwd.setGeometry(RECT_XSI_PWD_EDIT);
    m_EditPwd.setEchoMode(QLineEdit::Password);
    qInstallIME(&m_EditPwd, IME_PWD);
    qLineEditUtility::SetCurrentInputMethod(&m_EditPwd, EDIT_TYPE_PASSWORD);
    qLineEditUtility::SetMaxLength(&m_EditPwd, EDIT_TYPE_PASSWORD);
#if IF_FEATURE_AUTOP_IME
    yl::string strPasswordIME = commonUnits_GetIMEByType(kszLDAPPasswordIME);

    if (!strPasswordIME.empty())
    {
        qSetCurrentIME(&m_EditPwd, strPasswordIME.c_str(), 0, kszLDAPPasswordIME);
    }
#else
    qSetCurrentIME(&m_EditPwd, IME_abc, 0, kszLDAPPasswordIME);
#endif
    qUseKeyBoardFromIME(&m_EditPwd, INPUT_DIALOG_ENTER, true);

    m_EditPwd.setFocus();
//  m_pBtnIME->setText(toQString(qGetCurrentIMETitle(&m_EditPwd).c_str()));//显示输入法标题
}

void CLdapPasswordBox::UpdateSoftkeyData()
{
    int nBtnTop = height() - DLG_XSI_PWD_BTN_SET_BOTTOM_MARGIN - DLG_XSI_PWD_BTN_HEIGHT;
    int nBtnWidth = width() - (DLG_XSI_PWD_BTN_SET_LEFT_MARGIN + DLG_XSI_PWD_BTN_SET_RIGHT_MARGIN);

    m_BtnSet.setGeometry(DLG_XSI_PWD_BTN_SET_LEFT_MARGIN, nBtnTop,
                         nBtnWidth, DLG_XSI_PWD_BTN_HEIGHT);
    m_BtnSet.show();

    // Cancel按钮
    m_pBtnCancel = new qUIPushButton();
    if (m_pBtnCancel)
    {
        m_pBtnCancel->setObjectName("BtnCancel");
        m_pBtnCancel->set3StatePic("", PIC_BTN_MSG_PRESS, "");
        m_pBtnCancel->setText(LANG_TRANSLATE(TRID_CANCEL));

        connect(m_pBtnCancel, SIGNAL(clicked()), this, SLOT(OnBtnCancelClicked()));
        m_BtnSet.AddButton(m_pBtnCancel);
    }

    // 输入法按钮
    if (!FullKeyboardEnabled())
    {
        m_pBtnIME = new qUIPushButton();
        if (m_pBtnIME)
        {
            m_pBtnIME->setObjectName("BtnIME");
            m_pBtnIME->set3StatePic("", PIC_BTN_MSG_PRESS, "");

            connect(m_pBtnIME, SIGNAL(clicked()), this, SLOT(OnBtnIMEClicked()));

            m_BtnSet.AddButton(m_pBtnIME);
        }
    }

    // 删除按钮
    if (!FullKeyboardEnabled())
    {
        m_pBtnDelete = new qUIPushButton();
        if (m_pBtnDelete)
        {
            m_pBtnDelete->setObjectName("BtnDelete");
            m_pBtnDelete->set3StatePic("", PIC_BTN_MSG_PRESS, "");
            m_pBtnDelete->setText(LANG_TRANSLATE(TRID_DELETE));

            connect(m_pBtnDelete, SIGNAL(clicked()), this, SLOT(OnBtnDeleteClicked()));

            m_BtnSet.AddButton(m_pBtnDelete);
        }
    }

    // OK按钮
    m_pBtnOk = new qUIPushButton();
    if (m_pBtnOk)
    {
        m_pBtnOk->setObjectName("BtnOk");
        m_pBtnOk->set3StatePic("", PIC_BTN_MSG_PRESS, "");
        m_pBtnOk->setText(LANG_TRANSLATE(TRID_OK));

        connect(m_pBtnOk, SIGNAL(clicked()), this, SLOT(OnBtnOKClicked()));
        m_BtnSet.AddButton(m_pBtnOk);

        m_bAllowOkButton = true;
    }

    m_EditUserId.installEventFilter(this);
    m_EditPwd.installEventFilter(this);
    //connect(&m_EditPwd, SIGNAL(focusInEvent(QFocusEvent *)), this, SLOT(OnSwitchEventFilter(QEvent *)));
}

void CLdapPasswordBox::SetUserStr()
{
    QString strUserTitle = LANG_TRANSLATE(TRID_USER_NAME);
    QString strPasswordTitle = LANG_TRANSLATE(TRID_PASSWORD);
    m_LabUserId.setText(strUserTitle);
    m_LabPwd.setText(strPasswordTitle);

    yl::string strUserName = configParser_GetConfigString(kszLDAPuser);
    yl::string strUserPass = configParser_GetConfigString(kszLDAPpswd);

    if (!strUserName.empty())
    {
        m_EditUserId.setText(toQString(strUserName.c_str()));
    }

    if (!strUserPass.empty())
    {
        m_EditPwd.setText(toQString(strUserPass.c_str()));
    }
}

void CLdapPasswordBox::PrintMsgBoxInfo(bool bAdd)
{
    if (bAdd)
    {
        POPUPBOX_INFO("Add CLdapPasswordBox::PrintMsgBoxInfo: %s", m_LabUserId.text().toUtf8().data());
    }
    else
    {
        POPUPBOX_INFO("Del CLdapPasswordBox::PrintMsgBoxInfo: %s", m_LabUserId.text().toUtf8().data());
    }
}

bool CLdapPasswordBox::CheckPassword()
{
    // 获取用户输入内容

    m_strUser = qReturnEdit(&m_EditUserId);
    if (FullKeyboardEnabled())
    {
        m_strUser = m_EditUserId.text();
    }

    if (m_strUser.isEmpty())
    {
        MessageBox_ShowCommonMsgBox(NULL,
                                    LANG_TRANSLATE(TRID_PASSWORD_CANNOT_BE_EMPTY),
                                    MESSAGEBOX_Cancel,
                                    3000);

        return false;
    }

    m_strPwd = qReturnEdit(&m_EditPwd);
    if (FullKeyboardEnabled())
    {
        m_strPwd = m_EditPwd.text();
    }

    if (m_strPwd.isEmpty())
    {
        MessageBox_ShowCommonMsgBox(NULL,
                                    LANG_TRANSLATE(TRID_PASSWORD_CANNOT_BE_EMPTY),
                                    MESSAGEBOX_Cancel,
                                    3000);

        return false;
    }

    if (!qLineEditUtility::IsFormatValid(m_strPwd, EDIT_TYPE_PASSWORD))
    {
        MessageBox_ShowCommonMsgBox(NULL,
                                    LANG_TRANSLATE(TRID_PASSWORD_IS_INVALID),
                                    MESSAGEBOX_Cancel,
                                    3000);

        return false;
    }

    return true;
}

void CLdapPasswordBox::OnBtnCancelClicked()
{
    ExitMessageBox(MessageBox_Cancel);
}

void CLdapPasswordBox::OnBtnOKClicked()
{
    // 只有此弹出框有确认按钮时，按ok才有效果
    //if (CheckPassword())
    {
        yl::string strName = m_EditUserId.text().toUtf8().data();
        yl::string strPassword = m_EditPwd.text().toUtf8().data();

        configParser_SetConfigString(kszLDAPuser, strName.c_str());

        configParser_SetConfigString(kszLDAPpswd, strPassword.c_str());

        LDAP_ReSetLdapOption();

        qSetIsShowWnd(&m_EditPwd, false);

        ExitMessageBox(MessageBox_OK);
    }
}

void CLdapPasswordBox::OnBtnDeleteClicked()
{
    qOnBackspaceClicked(this);
}

void CLdapPasswordBox::OnBtnIMEClicked()
{
    QWidget * pFocusWidget = focusWidget();
    if (pFocusWidget != NULL)
    {
        QString strIMEName = qShift2NextIME(pFocusWidget);
        if (NULL != m_pBtnIME)
        {
            m_pBtnIME->setText(strIMEName);
        }
    }
}


bool CLdapPasswordBox::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (isActiveWindow() && bPress)
    {
        switch (nKeyCode)
        {
        case PHONE_KEY_0:
        case PHONE_KEY_1:
        case PHONE_KEY_2:
        case PHONE_KEY_3:
        case PHONE_KEY_4:
        case PHONE_KEY_5:
        case PHONE_KEY_6:
        case PHONE_KEY_7:
        case PHONE_KEY_8:
        case PHONE_KEY_9:
        case PHONE_KEY_STAR:
        case PHONE_KEY_POUND:
        case PHONE_KEY_LEFT:
        case PHONE_KEY_RIGHT:
            {
                QWidget * pFocusWidget = focusWidget();
                QKeyEvent keyEvent(QEvent::KeyPress, nKeyCode, Qt::NoModifier);
                if (qPassEventToEdit(pFocusWidget, &keyEvent))
                {
                    return true;
                }
            }
            break;
        case PHONE_KEY_UP:
        case PHONE_KEY_DOWN:
            {
                QKeyEvent keyEvent(QEvent::KeyPress, nKeyCode, Qt::NoModifier);
                OnSwitchEventFilter(&keyEvent);
            }
            break;
        case PHONE_KEY_OK:
            {
                OnBtnOKClicked();
                return true;
            }
            break;
        case PHONE_KEY_CANCEL:
            {
                OnBtnCancelClicked();
                return true;
            }
            break;
        case PHONE_KEY_HANDFREE:
        case PHONE_KEY_HANDSET:
        case PHONE_KEY_HANDSET_ON_HOOK:
        case PHONE_KEY_HANDSET_OFF_HOOK:
        case PHONE_KEY_HEADSET_CTRL:
        case PHONE_KEY_EHS_OPEN:
        case PHONE_KEY_EHS_CLOSE:
        case PHONE_KEY_VOLUME_DECREASE:
        case PHONE_KEY_VOLUME_INCREASE:
            {
                return CMessageBoxBase::ProcessKeyEvent(nKeyCode, bPress);
            }
            break;
        default:
            break;
        }
    }

    return !FullKeyboardEnabled();
}

bool CLdapPasswordBox::event(QEvent * pEvent)
{
    if (NULL != pEvent && isActiveWindow())
    {
        //界面存在lineEdit,不需要进行按键事件的判断
        int nEventType = pEvent->type();

        if (pEvent->type() == QEvent::Show)
        {
            if (m_EditUserId.text().isEmpty())
            {
                m_EditUserId.setFocus();
            }
            else
            {
                m_EditPwd.setFocus();
            }
        }
        else if (QEvent::KeyPress == nEventType || QEvent::KeyRelease == nEventType)
        {
            return false;
        }
    }

    return CMessageBoxBase::event(pEvent);
}

bool CLdapPasswordBox::OnSwitchEventFilter(QEvent * pEvent)
{
    bool bSwitchFocus = false;
    QWidget * pFocusWidget = focusWidget();

    if (NULL != pFocusWidget)
    {
        if (pFocusWidget == &m_EditPwd)
        {
            m_EditUserId.setFocus();
            if (m_pBtnIME != NULL)
            {
                m_pBtnIME->setText(qGetCurrentIMETitle(&m_EditUserId));//显示输入法标题
            }
            if (qPassEventToEdit(&m_EditUserId, pEvent))
            {
                return true;
            }
        }
        else
        {
            m_EditPwd.setFocus();
            if (m_pBtnIME != NULL)
            {
                m_pBtnIME->setText(qGetCurrentIMETitle(&m_EditPwd));//显示输入法标题
            }
            if (qPassEventToEdit(&m_EditPwd, pEvent))
            {
                return true;
            }
        }
    }

    return false;
}

bool CLdapPasswordBox::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL == pEvent)
    {
        return false;
    }

    //判断事件类型
    switch (pEvent->type())
    {
    //焦点集中事件
    case QEvent::FocusIn:
        {
            //切换焦点转换输入法
            QWidget * pFocusWidget = focusWidget();
            if (pFocusWidget == &m_EditUserId)
            {
                if (m_pBtnIME != NULL)
                {
                    m_pBtnIME->setText(qGetCurrentIMETitle(&m_EditUserId));//显示输入法标题
                }
                qSetIsShowWnd(pFocusWidget, true);
            }
            else if (pFocusWidget == &m_EditPwd)
            {
                if (m_pBtnIME != NULL)
                {
                    m_pBtnIME->setText(qGetCurrentIMETitle(&m_EditPwd));//显示输入法标题
                }
            }
        }
        break;
    default:
        break;
    }

    return CMessageBoxBase::eventFilter(pObject, pEvent);
}
#endif
