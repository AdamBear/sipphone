#include "passwordmessagebox.h"
#include "modmessagebox.h"
#include "uikernel/qwidgetutility.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "inputmethod/inputmethod.h"
#include "mainwnd/mainwnd.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "commonapi/systemhelperapi.h"
#include "uicommon.h"
#include "dsklog/log.h"
#include "keyguard/include/modkeyguard.h"
#include "keymap.h"
#include "qtcommon/qmisc.h"
#include "baseui/t4xpicpath.h"
#include "feature/include/modfeature.h"
#include "mainwndhelp/modmainwndhelp.h"

#define MESSAGEBOX_RECT_LEFT 171
#define MESSAGEBOX_RECT_TOP 160
#define MESSAGEBOX_RECT_WIDTH 458
#define MESSAGEBOX_RECT_HEIGHT 160

#define MESSAGEBOX_NOTE_LEFT 4
#define MESSAGEBOX_NOTE_TOP 35
#define MESSAGEBOX_NOTE_WIDTH 158
#define MESSAGEBOX_NOTE_HEIGHT 36

#define MESSAGEBOX_LINEEDIT_LEFT 163
#define MESSAGEBOX_LINEEDIT_TOP 35
#define MESSAGEBOX_LINEEDIT_WIDTH 245
#define MESSAGEBOX_LINEEDIT_HEIGHT 36

#define MESSAGEBOX_BTN_SET_LEFT_MARGIN 8
#define MESSAGEBOX_BTN_SET_RIGHT_MARGIN 7
#define MESSAGEBOX_BTN_SET_BOTTOM_MARGIN 8

#define MESSAGEBOX_BTN_HEIGHT 53

CPasswordMessageBox::CPasswordMessageBox(CMaskFrame * pMaskFrame,
        const QString & strNode,
        const QString & strWaringText,
        const QString & strPassWord,
        const QString & strUserLevel,
        int nMaxInput/* = -1*/)
    : CMessageBoxBase(pMaskFrame)
    , m_strUserLevel(strUserLevel)
    , m_strWaringText(strWaringText)
    , m_strPassWord(strPassWord)
    , m_nMaxInput(nMaxInput)
{
    m_labNote.setText(strNode);

    m_labNote.setParent(this);
    m_lineEditPassword.setParent(this);

    Init();
}

CPasswordMessageBox::CPasswordMessageBox(CMaskFrame * pMaskFrame
        , const QString & strNode
        , int nMaxInput)
    : CMessageBoxBase(pMaskFrame)
    , m_nMaxInput(nMaxInput)
{
    m_labNote.setText(strNode);

    m_labNote.setParent(this);
    m_lineEditPassword.setParent(this);

    Init();
}

CPasswordMessageBox::~CPasswordMessageBox()
{
    qUninstallIME(&m_lineEditPassword);
}

void CPasswordMessageBox::Init()
{
    ////---- T48 位置重调
    QRect rect = QRect(MESSAGEBOX_RECT_LEFT, MESSAGEBOX_RECT_TOP, MESSAGEBOX_RECT_WIDTH,
                       MESSAGEBOX_RECT_HEIGHT);
    this->setGeometry(rect);
    setAutoFillBackground(true);
    QPalette palette;
    QPixmap pix(THEME_GET_BMP(PIC_MSGBOX_BG1));
    pix.scaled(rect.size());
    palette.setBrush(QPalette::Window, QBrush(pix));
    setPalette(palette);

    UpdateSoftkeyData();

    UpdateEditLineData();

    this->installEventFilter(this);         //为本窗口安装过滤器
}

void CPasswordMessageBox::UpdateEditLineData()
{
    SetWgtFontColor(&m_labNote, FONT_TYPE_TITLE, Qt::white);
    m_labNote.setObjectName("labNote");
    m_labNote.setAlignment(Qt::AlignCenter);
    m_labNote.setGeometry(MESSAGEBOX_NOTE_LEFT,
                          MESSAGEBOX_NOTE_TOP,
                          MESSAGEBOX_NOTE_WIDTH,
                          MESSAGEBOX_NOTE_HEIGHT);

    m_lineEditPassword.setObjectName("lineEditPassword");
    m_lineEditPassword.setGeometry(MESSAGEBOX_LINEEDIT_LEFT,
                                   MESSAGEBOX_LINEEDIT_TOP,
                                   MESSAGEBOX_LINEEDIT_WIDTH,
                                   MESSAGEBOX_LINEEDIT_HEIGHT);

    //设置样式和输入法
    m_lineEditPassword.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_lineEditPassword.setEchoMode(QLineEdit::Password);
    qInstallIME(&m_lineEditPassword, IME_PWD);
    qLineEditUtility::SetCurrentInputMethod(&m_lineEditPassword, EDIT_TYPE_PASSWORD);
    qLineEditUtility::SetMaxLength(&m_lineEditPassword, EDIT_TYPE_PASSWORD);

    //该接口为edk使用
    //通过平常的接口调用的弹出框不会受到这个的影响
    if (0 <= m_nMaxInput)
    {
        m_lineEditPassword.setMaxLength(m_nMaxInput);
    }

    m_lineEditPassword.installEventFilter(this);
    m_lineEditPassword.setFocus();
}

void CPasswordMessageBox::SetIme(LPCSTR lpszImeName)
{
    qUninstallIME(&m_lineEditPassword);
    qInstallIME(&m_lineEditPassword, lpszImeName);
}

void CPasswordMessageBox::PrintMsgBoxInfo(bool bAdd)
{
    if (bAdd)
    {
        POPUPBOX_INFO("Add CPasswordMessageBox::PrintMsgBoxInfo: %s", m_labNote.text().toUtf8().data());
    }
    else
    {
        POPUPBOX_INFO("Del CPasswordMessageBox::PrintMsgBoxInfo: %s", m_labNote.text().toUtf8().data());
    }
}

QLineEdit * CPasswordMessageBox::GetLineEdit()
{
    return &m_lineEditPassword;
}

// 更新softkey数据
void CPasswordMessageBox::UpdateSoftkeyData()
{
    m_BtnSet.setGeometry(MESSAGEBOX_BTN_SET_LEFT_MARGIN,
                         height() - MESSAGEBOX_BTN_SET_BOTTOM_MARGIN - MESSAGEBOX_BTN_HEIGHT,
                         width() - (MESSAGEBOX_BTN_SET_LEFT_MARGIN + MESSAGEBOX_BTN_SET_RIGHT_MARGIN),
                         MESSAGEBOX_BTN_HEIGHT);
    m_BtnSet.show();

    m_pBtnCancel = new qUIPushButton();
    if (m_pBtnCancel)
    {
        m_pBtnCancel->setObjectName("BtnCancel");
        m_pBtnCancel->set3StatePic("", PIC_BTN_MSG_PRESS, "");
        m_pBtnCancel->setText(LANG_TRANSLATE(TRID_CANCEL));
        //绑定槽
        connect(m_pBtnCancel, SIGNAL(clicked()), this, SLOT(OnBtnCancelClicked()));
        m_BtnSet.AddButton(m_pBtnCancel);
    }


    //创建输入法按钮
    if (!FullKeyboardEnabled())
    {
        m_pBtnIME = new qUIPushButton();
        if (m_pBtnIME)
        {
            m_pBtnIME->setObjectName("BtnIME");
            m_pBtnIME->set3StatePic("", PIC_BTN_MSG_PRESS, "");

            //绑定槽
            connect(m_pBtnIME, SIGNAL(clicked()), this, SLOT(OnBtnIMEClicked()));
            m_BtnSet.AddButton(m_pBtnIME);
        }
    }

    if (!FullKeyboardEnabled())
    {
        m_pBtnDelete = new qUIPushButton();
        if (m_pBtnDelete)
        {
            m_pBtnDelete->setObjectName("BtnDelete");
            m_pBtnDelete->set3StatePic("", PIC_BTN_MSG_PRESS, "");

            m_pBtnDelete->setText(LANG_TRANSLATE(TRID_DELETE));
            //绑定槽
            connect(m_pBtnDelete, SIGNAL(clicked()), this, SLOT(OnBtnDeleteClicked()));

            m_BtnSet.AddButton(m_pBtnDelete);
        }
    }

    //创建OK和Cancel按钮
    m_pBtnOk = new qUIPushButton();
    if (m_pBtnOk)
    {
        m_pBtnOk->setObjectName("BtnOk");
        m_pBtnOk->set3StatePic("", PIC_BTN_MSG_PRESS, "");
        m_pBtnOk->setText(LANG_TRANSLATE(TRID_OK));
        //绑定槽
        connect(m_pBtnOk, SIGNAL(clicked()), this, SLOT(OnBtnOKClicked()));
        m_BtnSet.AddButton(m_pBtnOk);

        m_bAllowOkButton = true;
    }
}

bool CPasswordMessageBox::CheckPassword(bool & bOpenByUserPassword)
{
    // 获取用户输入内容
    QString strAllEdit = qReturnEdit(&m_lineEditPassword);
    if (FullKeyboardEnabled())
    {
        strAllEdit = m_lineEditPassword.text();
    }
    // 验证输入密码的正确性
    if (GetType() == MESSAGEBOX_UNLOCK)
    {
        PinType ptResult = keyGuard_GetPinType(strAllEdit.toUtf8().data());

        switch (ptResult)
        {
        case PIN_CORRECT_LOCK:
        {
            return true;
        }
        break;
        case PIN_CORRECT_ADMIN:
        {
            bOpenByUserPassword = true;
            return true;
        }
        break;
        default:
        {
            return false;
        }
        break;
        }

    }
    else if (GetType() ==
             MESSAGEBOX_GUEST_UNLOCK) //在打开窗口的时候初始化m_strPassWord，与输入框内容比较
    {
        if (m_strPassWord == strAllEdit)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (GetType() == MESSAGEBOX_PWD)
    {
        bool bPasswdCorrent = commonAPI_CheckPassword(m_strUserLevel.toUtf8().data(),
                              strAllEdit.toUtf8().data());
        if (bPasswdCorrent && configParser_GetConfigInt(kszUserPwdLimit) != 0
                && feature_UserModeGetCurrType() != UT_ADMIN)
        {
            if (m_strUserLevel == strAllEdit)
            {
                m_strWaringText = LANG_TRANSLATE(TRID_NEED_TO_CHANG_PASSWD);
                return false;
            }
        }

        return bPasswdCorrent;
    }
    else
    {
        POPUPBOX_WARN("CPasswordMessageBox::CheckPassword Type error!");
        return false;
    }

    return false;
}

void CPasswordMessageBox::OnBtnOKClicked()
{
    ///////////////EDK///////////////
    if (MESSAGEBOX_EDK == GetType())
    {
        // 获取用户输入内容
        //设置到ExtraData里
        structEDKCallBack structCallBack = GetExtraData().value<structEDKCallBack>();
        structCallBack.strInput = qReturnEdit(&m_lineEditPassword);
        if (FullKeyboardEnabled())
        {
            structCallBack.strInput = m_lineEditPassword.text();
        }

        SetExtraData(QVariant::fromValue(structCallBack));
        //返回确认的结果
        ExitMessageBox(MessageBox_OK);
    }
    /////////////////////////////////
    else if (MESSAGEBOX_WIFIDETAIL == GetType())
    {
        // 获取用户输入内容
        QString strAllEdit = qReturnEdit(&m_lineEditPassword);
        if (FullKeyboardEnabled())
        {
            strAllEdit = m_lineEditPassword.text();
        }

        //设置到ExtraData里
        SetExtraData(QVariant(strAllEdit));
        //返回确认的结果
        ExitMessageBox(MessageBox_OK);
    }

    // 只有此弹出框有确认按钮时，按ok才有效果
    bool bOpenByUserPassword = false;
    if (CheckPassword(bOpenByUserPassword))
    {
        qSetIsShowWnd(&m_lineEditPassword, false);

        bOpenByUserPassword ? ExitMessageBox(MessageBox_Admin) : ExitMessageBox(MessageBox_OK);
    }
    else
    {
        MessageBox_ShowCommonMsgBox(this,
                                    m_strWaringText,
                                    MESSAGEBOX_Cancel,
                                    2000);

        m_lineEditPassword.clear();
    }
}

void CPasswordMessageBox::OnBtnCancelClicked()
{
    ExitMessageBox(MessageBox_Cancel);
}

//响应点击Delete按钮事件
void CPasswordMessageBox::OnBtnDeleteClicked()
{
    qOnBackspaceClicked(this);
}

//响应点击IME按钮事件
void CPasswordMessageBox::OnBtnIMEClicked()
{
    QString strIMEName = qShift2NextIME(&m_lineEditPassword);
    if (NULL != m_pBtnIME)
    {
        m_pBtnIME->setText(strIMEName);
    }
}

//设置密码类型
void CPasswordMessageBox::SetPassWordType(const QString & strUserLevel)
{
    m_strUserLevel = strUserLevel;
}

// 设置Label
void CPasswordMessageBox::SetNote(const QString & strNote)
{
    //设置标题的文字与字体
    m_labNote.setText(strNote.toUtf8().data());

    // 刷新密码框
    update();
}

// 设置设置自动退出时间
void CPasswordMessageBox::MySetExitTime(int iTimeToExit)
{
    SetExitTime(iTimeToExit);
}

// 设置密码错误时显示的文字
void CPasswordMessageBox::SetWaringText(const QString & strWaringText)
{
    m_strWaringText = strWaringText;
}

// 设置密码框
void CPasswordMessageBox::SetPassWord(const QString & strPassWord)
{
    m_strPassWord = strPassWord;
}

// 设置默认的输入法
void CPasswordMessageBox::SetDefaultIME(LPCSTR lpszIME, const yl::string & strIMEType)
{
    if (FullKeyboardEnabled())
    {
        qSetCurrentIME(&m_lineEditPassword, lpszIME);
        QString strIMEName = toQString(lpszIME);

        qUseKeyBoardFromIME(&m_lineEditPassword, INPUT_DIALOG_ENTER, true);
    }
    else
    {
        qSetCurrentIME(&m_lineEditPassword, lpszIME, 0, strIMEType);

        QString strIMEName = qGetCurrentIMETitle(&m_lineEditPassword);

        if (m_pBtnIME != NULL)
        {
            m_pBtnIME->setText(strIMEName);
        }
    }
}

void CPasswordMessageBox::SetFeedBackShowType(bool bIsMaskFeedback)
{
    if (bIsMaskFeedback)
    {
        m_lineEditPassword.setEchoMode(QLineEdit::Password);
    }
    else
    {
        m_lineEditPassword.setEchoMode(QLineEdit::Normal);
    }
}

bool CPasswordMessageBox::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL != pObject
            && pObject == &m_lineEditPassword
            && NULL != pEvent
            && pEvent->type() == QEvent::FocusOut)
    {
        ExitMessageBox(MessageBox_Destory);
    }

//  return false;
    return CMessageBoxBase::eventFilter(pObject, pEvent);

}

bool CPasswordMessageBox::ProcessKeyEvent(int nKeyCode, bool bPress)
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
            if (qPassEventToEdit(&m_lineEditPassword, &keyEvent))
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

    return false;
}

bool CPasswordMessageBox::event(QEvent * pEvent)
{
    if (pEvent != NULL && isActiveWindow())
    {
        if (pEvent->type() == QEvent::Show)
        {
            m_lineEditPassword.setFocus();
        }
        else if (pEvent->type() == QEvent::KeyPress
                 || pEvent->type() == QEvent::KeyRelease)
        {
            return false;
        }
    }

    return CMessageBoxBase::event(pEvent);
}
