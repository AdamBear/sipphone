#include "xsipasswordbox.h"
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
#define RECT_XSI_USER_TITLE QRect(31, 30, 156, 36)
#define RECT_XSI_USER_VALUE QRect(190, 30, 246, 36)
#define RECT_XSI_PWD_TITLE QRect(31, 64, 156, 36)
#define RECT_XSI_PWD_EDIT QRect(190, 64, 246, 36)
}

CXSIPasswordBox::CXSIPasswordBox(CMaskFrame * pMaskFrame/* = NULL*/)
    : CMessageBoxBase(pMaskFrame)
    , m_pBtnIME(NULL)
    , m_pBtnDelete(NULL)
{
    m_LabUserId.setParent(this);
    m_ValueUserId.setParent(this);
    m_LabPwd.setParent(this);
    m_EditPwd.setParent(this);

    m_LabUserId.setObjectName("LabUserId");
    m_ValueUserId.setObjectName("ValueUserId");
    m_LabPwd.setObjectName("LabPwd");
    m_EditPwd.setObjectName("EditPwd");

    Init();
}

CXSIPasswordBox::~CXSIPasswordBox(void)
{
    qUninstallIME(&m_EditPwd);
}

void CXSIPasswordBox::Init()
{
    setAutoFillBackground(true);
    setGeometry(RECT_XSI_PWD_BOX);

    QPalette palBG;
    QPixmap pmBG = THEME_GET_BMP(PIC_MSGBOX_BG_XSI_PWD);

    palBG.setBrush(QPalette::Window, QBrush(pmBG));
    setPalette(palBG);

    UpdateSoftkeyData();

    UpdateEditLineData();
}

void CXSIPasswordBox::UpdateEditLineData()
{
    // UserId标题
    SetWgtFontColor(&m_LabUserId, FONT_TYPE_TITLE, Qt::white);
    m_LabUserId.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_LabUserId.setGeometry(RECT_XSI_USER_TITLE);

    // UserId值
    SetWgtFontColor(&m_ValueUserId, FONT_TYPE_TITLE, Qt::white);
    m_ValueUserId.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_ValueUserId.setGeometry(RECT_XSI_USER_VALUE);

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
    // 全机型统一，XSI账号密码输入框默认输入法为2aB
#if IF_FEATURE_AUTOP_IME
    yl::string strPasswordIME = commonUnits_GetIMEByType(kszXsiPasswordIME);
    if (!strPasswordIME.empty())
    {
        qSetCurrentIME(&m_EditPwd, strPasswordIME.c_str(), 0, kszXsiPasswordIME);
    }
#else
    qSetCurrentIME(&m_EditPwd, IME_2aB, 0, kszXsiPasswordIME);
#endif

    qUseKeyBoardFromIME(&m_EditPwd, INPUT_DIALOG_ENTER, true);
    m_EditPwd.setFocus();
    if (m_pBtnIME != NULL)
    {
        m_pBtnIME->setText(qGetCurrentIMETitle(&m_EditPwd));//显示输入法标题
    }
}

void CXSIPasswordBox::UpdateSoftkeyData()
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
}

void CXSIPasswordBox::SetUserStr(const QString & strUserTitle,
                                 const QString & strUserName,
                                 const QString & strPasswordTitle)
{
    m_LabUserId.setText(strUserTitle);
    m_ValueUserId.setText(strUserName);
    m_LabPwd.setText(strPasswordTitle);
}

void CXSIPasswordBox::PrintMsgBoxInfo(bool bAdd)
{
    if (bAdd)
    {
        POPUPBOX_INFO("Add CXSIPasswordBox::PrintMsgBoxInfo: %s", m_LabUserId.text().toUtf8().data());
    }
    else
    {
        POPUPBOX_INFO("Del CXSIPasswordBox::PrintMsgBoxInfo: %s", m_LabUserId.text().toUtf8().data());
    }
}

bool CXSIPasswordBox::CheckPassword()
{
    // 获取用户输入内容
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

        m_EditPwd.clear();
        return false;
    }

    return true;
}

void CXSIPasswordBox::OnBtnCancelClicked()
{
    ExitMessageBox(MessageBox_Cancel);
}

void CXSIPasswordBox::OnBtnOKClicked()
{
    // 只有此弹出框有确认按钮时，按ok才有效果
    if (CheckPassword())
    {
        QString strPwd = m_strPwd;
        SetExtraData(strPwd);
        qSetIsShowWnd(&m_EditPwd, false);
        ExitMessageBox(MessageBox_OK);
    }
}

void CXSIPasswordBox::OnBtnDeleteClicked()
{
    qOnBackspaceClicked(this);
}

void CXSIPasswordBox::OnBtnIMEClicked()
{
    QString strIMEName = qShift2NextIME(&m_EditPwd);
    if (NULL != m_pBtnIME)
    {
        m_pBtnIME->setText(strIMEName);
    }
}

void CXSIPasswordBox::SetDefaultIME(LPCSTR lpszIME)
{
    if (FullKeyboardEnabled())
    {
        qSetCurrentIME(&m_EditPwd, lpszIME);
        QString strIMEName = toQString(lpszIME);

        if (IME_123_Pwd == strIMEName
                || IME_123 == strIMEName)
        {
            qInstallKeyBoard(&m_EditPwd, true, INPUT_DLGTYPE_NUM);
        }
        else
        {
            qInstallKeyBoard(&m_EditPwd, true, INPUT_DLGTYPE_EN);
        }
    }
    else
    {
        qSetCurrentIME(&m_EditPwd, lpszIME);

        QString strIMEName = toQString(lpszIME);

        if (m_pBtnIME != NULL)
        {
            m_pBtnIME->setText(strIMEName);
        }
    }

}

bool CXSIPasswordBox::ProcessKeyEvent(int nKeyCode, bool bPress)
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
        case PHONE_KEY_UP:
        case PHONE_KEY_RIGHT:
        case PHONE_KEY_DOWN:
            {
                QKeyEvent keyEvent(QEvent::KeyPress, nKeyCode, Qt::NoModifier);
                if (qPassEventToEdit(&m_EditPwd, &keyEvent))
                {
                    return true;
                }
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

bool CXSIPasswordBox::event(QEvent * pEvent)
{
    if (NULL != pEvent && isActiveWindow())
    {
        //界面存在lineEdit,不需要进行按键事件的判断
        int nEventType = pEvent->type();

        if (pEvent->type() == QEvent::Show)
        {
            m_EditPwd.setFocus();
        }
        else if (QEvent::KeyPress == nEventType || QEvent::KeyRelease == nEventType)
        {
            return false;
        }
    }

    return CMessageBoxBase::event(pEvent);
}
