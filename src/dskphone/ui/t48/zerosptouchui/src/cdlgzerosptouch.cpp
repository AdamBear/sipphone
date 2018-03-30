#include "config.h"

#include "cdlgzerosptouch.h"
#include "uicommon.h"
#include "keymap.h"

#include "uimanager/uimanager_inc.h"
#include "uimanager/uimanager_header.h"
#include "uimanager/rectdefine.h"
#include "mainwnd/mainwnd.h"

#include "uikernel/qsoftkeybar.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qcomboboxutility.h"
#include "uikernel/qwidgetutility.h"
#include "inputmethod/inputmethod.h"
#include "qtcommon/qmisc.h"
#include "baseui/modbaseui.h"
#include "baseui/fontcolor.h"
#include "baseui/t4xpicpath.h"

#include "messagebox/modmessagebox.h"
#include "adapterbox/include/modadapterbox.h"

#include "setting/include/common.h"
#include "setting/include/modzero.h"
#include "setting/include/zero_common.h"
#include "modzerosptouch.h"
#include "settingui/src/networksettingmanager.h"

#include "configparser/modconfigparser.h"
#include <configiddefine.h>

#include "lamp/backlight/include/modbacklight.h"
#include "inputmethod/inputmethod.h"
#include <QComboBox>

#include "dlgzeronetwork.h"
#include "dlgzeroautoprovision.h"
#include "autopcontrol/include/modautopcontrol.h"
#include "imagemanager/modimagemanager.h"
#include "commonunits/modcommonunits.h"
#include "setting/src/autoprovisioncontroller.h"

// 界面Actionc初始化
#define ACTION_OPEN_STATUS  ZERO_STATUS
#define ACTION_ZERO_QUIT         "Quit"
#define ACTION_ZERO_NEXT_PAGE    ZERO_NEXT
#define ACTION_CANCEL       "Cancel"

namespace
{
#define FONT_SIZE                   24
#define FONT_WEIGHT                 75

// Rect of ui widget
const QRect RECT_LAB_TEXT(170, 30, 480, 180);//(0, 37, 480, 180);

const QRect RECT_LAB_USER_NAME(20, 42, 141, 21);
const QRect RECT_EDIT_USER_NAME(150, 37, 228, 34);
const QRect RECT_LAB_PASSWORD(20, 77, 141, 31);
const QRect RECT_EDIT_PASSWORD(150, 77, 228, 34);

// action
#define PAGE_ACTION_submit          "submit"
#define PAGE_ACTION_exit            "exit"
#define PAGE_ACTION_back            "back"
// count interval time
#define COUNT_DOWN_INTERVAL_TIME    1000
// popup dialog ID
#define MSGBOX_ID_REBOOT            13

// test for RPS
//#define TEST_RPS
}

/************************************************************************/
/*                       about CDlgZeroSPTouch                          */
/************************************************************************/
CDlgZeroSPTouch::CDlgZeroSPTouch()
    : CBaseDialog()
    , m_pRemainTimer(NULL)
    , m_ePageType(ZERO_PAGE_MAIN)
    , m_bShowTime(true)
    , m_pDlgZeroNetwork(NULL)
    , m_pDlgAutoP(NULL)
    , m_pDelayTimer(NULL)
    , m_eDelayType(DOT_NONE)
    , m_bRejectToIdle(false)
    , m_bExitAutoPWhenExitDlg(true)
{
    ui.setupUi(this);
    SetWindowType(eWT_ZeroSPTouch);
    //设置为顶层窗体属性
    SetTopWnd();
    InitData();
}

CDlgZeroSPTouch::~CDlgZeroSPTouch()
{
    qUninstallIME(ui.editUserName);
    qUninstallIME(ui.editPassword);
}

//取得实例
CBaseDialog * CDlgZeroSPTouch::CreateInstance()
{
    return new CDlgZeroSPTouch();
}

// 初始化
void CDlgZeroSPTouch::Init()
{
    // 关闭背光灯变化
    backlight_OpenBacklightEvent(false);

    m_bShowTime = true;

    m_bRejectToIdle = configParser_GetConfigInt(kszZeroSimplification) != 1;
}

// 释放数据,将窗口清空回复到初始化状态
void CDlgZeroSPTouch::Uninit()
{
    // 开启背光灯变化
    backlight_OpenBacklightEvent(true);

    m_bShowTime = true;
    m_bRejectToIdle = false;

    if (NULL != m_pDelayTimer)
    {
        m_pDelayTimer->stop();
    }
}

void CDlgZeroSPTouch::InitData()
{
    SetDlgGeometry(RECT_ZERO);
    //初始化计时器
    m_pRemainTimer = new QTimer(this);
    if (NULL != m_pRemainTimer)
    {
        //连接超时处理函数
        connect(m_pRemainTimer, SIGNAL(timeout()), this, SLOT(OnRemainTimeOut()));
    }

    m_pDelayTimer = new QTimer(this);
    if (NULL != m_pDelayTimer)
    {
        connect(m_pDelayTimer, SIGNAL(timeout()), this, SLOT(OnDelayOperator()));
    }

    // 设置字体大小
    QFont font;
    font.setPointSize(FONT_SIZE);
    font.setWeight(FONT_WEIGHT);
    font.setBold(true);
    ui.labText->setFont(font);
    ui.labText->setGeometry(RECT_LAB_TEXT);

//  ui.labUserName->setGeometry(RECT_LAB_USER_NAME);
//  ui.editUserName->setGeometry(RECT_EDIT_USER_NAME);
//  ui.labPassword->setGeometry(RECT_LAB_PASSWORD);
//  ui.editPassword->setGeometry(RECT_EDIT_PASSWORD);

    // 设置文字居中
    ui.labText->setAlignment(Qt::AlignCenter);

    // 设置字体颜色
    SetWgtFontColor(ui.labText, FONT_TYPE_TEXT);

    //以下是重写向鉴权界面的设置

    ui.labUserName->setText(LANG_TRANSLATE(TRID_USER_NAME));
    SetWgtFontColor(ui.labUserName, FONT_TYPE_TEXT);

    ui.labPassword->setText(LANG_TRANSLATE(TRID_PASSWORD));
    SetWgtFontColor(ui.labPassword, FONT_TYPE_TEXT);

    //设置Redirector界面效果
#if IF_FEATURE_AUTOP_IME
    qInstallIME(ui.editUserName, IME_Name);
    qUseKeyBoardFromIME(ui.editUserName, INPUT_DIALOG_NEWLINE, false);
#else
    qInstallIME(ui.editUserName, IME_DIAL);
    qUseKeyBoardFromIME(ui.editUserName, INPUT_DIALOG_NEWLINE, false);
#endif
    qInstallIME(ui.editPassword, IME_PWD);
    qUseKeyBoardFromIME(ui.editPassword, INPUT_DIALOG_NEWLINE, false);
    qLineEditUtility::SetMaxLength(ui.editPassword, EDIT_TYPE_PASSWORD);

    ui.editUserName->installEventFilter(this);
    ui.editPassword->installEventFilter(this);

    this->installEventFilter(this);

#if IF_FEATURE_AUTOP_IME
    yl::string strUserIME = commonUnits_GetIMEByType(kszZeroUserIME);
    if (!strUserIME.empty())
    {
        qSetCurrentIME(ui.editUserName, strUserIME.c_str(), 0, kszZeroUserIME);
    }
    else
    {
        qLineEditUtility::SetCurrentInputMethod(ui.editUserName, EDIT_TYPE_NAME);
    }

    yl::string strPasswordIME = commonUnits_GetIMEByType(kszZeroPasswordIME);
    if (!strPasswordIME.empty())
    {
        qSetCurrentIME(ui.editPassword, strPasswordIME.c_str(), 0, kszZeroPasswordIME);
    }
    else
    {
        qLineEditUtility::SetCurrentInputMethod(ui.editPassword, EDIT_TYPE_NAME_PWD);
    }
#else
    qLineEditUtility::SetCurrentInputMethod(ui.editUserName, EDIT_TYPE_NAME);
    qLineEditUtility::SetCurrentInputMethod(ui.editPassword, EDIT_TYPE_PASSWORD);
#endif

    ui.editPassword->setEchoMode(QLineEdit::Password);

    ui.frmRedirector->hide();

    // set background
    m_pDlgZeroNetwork = new CDlgZeroNetwork(this);
    if (NULL != m_pDlgZeroNetwork)
    {
        m_pDlgZeroNetwork->setGeometry(100, 0, width() - 200, height());
        m_pDlgZeroNetwork->hide();
        connect(m_pDlgZeroNetwork, SIGNAL(RefreshSoftkey()), this, SLOT(SetSoftkeyData()));
    }

    m_pDlgAutoP = new DlgZeroAutoProvision(this);
    if (NULL != m_pDlgAutoP)
    {
        m_pDlgAutoP->setGeometry(100, 0, width() - 200, height());
        m_pDlgAutoP->hide();
    }
}

// 设置小窗口数据
void CDlgZeroSPTouch::SetData(void * pData/* = NULL*/)
{
    //更新标题
    UpdateTitle();

    //设置softkey
    SetSoftkeyData();
}

// 延时进行某些操作
void CDlgZeroSPTouch::DelayOperator(CDlgZeroSPTouch::DELAY_OPERATOR_TYPE eType)
{
    if (NULL == m_pDelayTimer)
    {
        return;
    }

    m_eDelayType = eType;
    m_pDelayTimer->stop();
    m_pDelayTimer->start(WAITTING_EXIT_TIME);
}

// 延时操作后的释放资源等操作
void CDlgZeroSPTouch::OnDelayOperator()
{
    if (NULL != m_pDelayTimer)
    {
        m_pDelayTimer->stop();
    }

    switch (m_eDelayType)
    {
    case DOT_NETWORK_FAIL:
        {
            OnNetworkFailPageTimeout();
        }
        break;
    case DOT_UPDATE_FAIL:
        {
            OnUpdataFailPageTimeout();
        }
        break;
    case DOT_UPDATE_FINISH:
        {
            OnUpdateFinishPageTimeout();
        }
        break;
#if IF_FEATURE_METASWITCH
    case DOT_AUTH_FAIL:
        {
            OnAuthFailPageTimeout();
        }
        break;
#endif
    }
}

bool CDlgZeroSPTouch::IsRpsOrZerotouch()
{
    int nAutoPType = Autop_GetUpdateType();

    return (AUT_ZERO == nAutoPType || AUT_RPS == nAutoPType);
}


//更新标题
void CDlgZeroSPTouch::UpdateTitle()
{
    // 设置Title
    if (ZERO_PAGE_REDIRECTOR_SUBMIT == m_ePageType
            || ZERO_PAGE_REDIRECTOR_FAILED == m_ePageType
            || ZERO_PAGE_REDIRECTOR_FINISH == m_ePageType)
    {
        m_strTitle = LANG_TRANSLATE(TRID_REDIRCTOR).toUtf8().data();
    }
    else if (ZERO_PAGE_REDIRECTOR_NOTE == m_ePageType)
    {
        m_strTitle = "";
    }
    else if (ZERO_PAGE_NETWORK_SETTING == m_ePageType)
    {
        m_strTitle = LANG_TRANSLATE(TRID_NETWORK).toUtf8().data();
    }
    else if (ZERO_PAGE_AUTOP == m_ePageType
#if IF_FEATURE_METASWITCH
             || ZERO_PAGE_AUTOP_DIRECT == m_ePageType
#endif
            )
    {
        m_strTitle = LANG_TRANSLATE(TRID_AUTO_PROVISION).toUtf8().data();
    }
    else if (ZERO_PAGE_AUTHENTICATION == m_ePageType)
    {
        m_strTitle = LANG_TRANSLATE(TRID_INVAILD_PROVISION).toUtf8().data();
    }
    else
    {
        if (AUT_RPS == Autop_GetUpdateType())
        {
            m_strTitle = LANG_TRANSLATE(TRID_REDIRCTOR).toUtf8().data();
        }
        else
        {
            m_strTitle = LANG_TRANSLATE(TRID_ZERO_TOUCH).toUtf8().data();
            ui.frmRedirector->hide();
            ui.labText->show();
        }
    }
}

// 获取页面的Softkey
void CDlgZeroSPTouch::SetSoftkeyData()
{
    InitSoftKeyData();

    switch (m_ePageType)
    {
    case ZERO_PAGE_AUTOP:
#if IF_FEATURE_METASWITCH
    case ZERO_PAGE_AUTOP_DIRECT:
#endif
        {
            m_SoftkeyData[1].m_strSoftkeyAction = IME_ACTION;

            m_SoftkeyData[2].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_DELETE);
            m_SoftkeyData[2].m_strSoftkeyAction = IME_DEL_ACTION;
            m_SoftkeyData[2].m_strIconPath = PIC_SOFTKEY_DELETE;

#if IF_BUG_30885
            m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_NEXT);
#else
            m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_OK);
#endif
            m_SoftkeyData[3].m_strSoftkeyAction = ACTION_ZERO_NEXT_PAGE;
            m_SoftkeyData[3].m_strIconPath = PIC_SOFTKEY_NEXT;
        }
        break;
    case ZERO_PAGE_AUTHENTICATION:
        {
            m_SoftkeyData[1].m_strSoftkeyAction = IME_ACTION;

            m_SoftkeyData[2].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_DELETE);
            m_SoftkeyData[2].m_strSoftkeyAction = IME_DEL_ACTION;
            m_SoftkeyData[2].m_strIconPath = PIC_SOFTKEY_DELETE;

            m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_OK);
            m_SoftkeyData[3].m_strSoftkeyAction = ACTION_ZERO_NEXT_PAGE;
            m_SoftkeyData[3].m_strIconPath = PIC_SOFTKEY_NEXT;
        }
        break;
    case ZERO_PAGE_MAIN:
        {
#if IF_BUG_30885
            m_SoftkeyData[0].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_STATUS);
            m_SoftkeyData[0].m_strSoftkeyAction = ACTION_OPEN_STATUS;
            m_SoftkeyData[0].m_strIconPath = PIC_SOFTKEY_STATUS;
#else
            m_SoftkeyData[1].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_STATUS);
            m_SoftkeyData[1].m_strSoftkeyAction = ACTION_OPEN_STATUS;
            m_SoftkeyData[1].m_strIconPath = PIC_SOFTKEY_STATUS;

            m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_OK);
            m_SoftkeyData[3].m_strSoftkeyAction = ACTION_ZERO_NEXT_PAGE;
            m_SoftkeyData[3].m_strIconPath = PIC_SOFTKEY_NEXT;
#endif
        }
        break;
    case ZERO_PAGE_NETWORK_SETTING:
        {
            QWidget * pWgt = focusWidget();
            if (NULL != pWgt && (pWgt->inherits("QLineEdit") || pWgt->inherits("QTextEdit")))
            {
                m_SoftkeyData[1].m_strSoftkeyAction = IME_ACTION;

                m_SoftkeyData[2].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_DELETE);
                m_SoftkeyData[2].m_strSoftkeyAction = IME_DEL_ACTION;
                m_SoftkeyData[2].m_strIconPath = PIC_SOFTKEY_DELETE;
            }

#if IF_BUG_30885
            m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_NEXT);
#else
            m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_OK);
#endif
            m_SoftkeyData[3].m_strSoftkeyAction = ACTION_ZERO_NEXT_PAGE;
            m_SoftkeyData[3].m_strIconPath = PIC_SOFTKEY_NEXT;
        }
        break;
    case ZERO_PAGE_NOTE_UPDAT_FAIL:
    case  ZERO_PAGE_NOTE_UPTADE_FINISH:
    case ZERO_PAGE_REDIRECTOR_FINISH:
        {
            m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_OK);
            m_SoftkeyData[3].m_strSoftkeyAction = ACTION_ZERO_QUIT;
            m_SoftkeyData[3].m_strIconPath = PIC_SOFTKEY_NEXT;
        }
        break;
    case ZERO_PAGE_REDIRECTOR_NOTE:
        {
#if IF_BUG_32779
            if ("Dutch" == configParser_GetConfigString(kszCurrentLang))
            {
                m_SoftkeyData[3].m_strSoftkeyTitle  = "Cancel";
            }
            else
#endif
            {
                m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_CANCEL);
            }
            m_SoftkeyData[3].m_strSoftkeyAction = ACTION_CANCEL;
            m_SoftkeyData[3].m_strIconPath = PIC_SOFTKEY_CANCEL;
        }
        break;
    case ZERO_PAGE_NOTE_NETWORK_CHECK:
    case ZERO_PAGE_NOTE_NETWORK_FAIL:
#if IF_FEATURE_METASWITCH
    case ZERO_PAGE_NOTE_AUTH_FAIL:
#endif
        {
            //此处三个提示界面SoftKey为空
        }
        break;
    case ZERO_PAGE_REDIRECTOR_SUBMIT:
        {
            m_SoftkeyData[1].m_strSoftkeyAction = IME_ACTION;

            m_SoftkeyData[2].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_DELETE);
            m_SoftkeyData[2].m_strSoftkeyAction = IME_DEL_ACTION;
            m_SoftkeyData[2].m_strIconPath = PIC_SOFTKEY_DELETE;

            m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_OK);
            m_SoftkeyData[3].m_strSoftkeyAction = PAGE_ACTION_submit;
            m_SoftkeyData[3].m_strIconPath = PIC_SOFTKEY_SUBMIT;
        }
        break;
    case ZERO_PAGE_REDIRECTOR_FAILED:
        {
            //不做设置
            m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_EXIT);
            m_SoftkeyData[3].m_strSoftkeyAction = PAGE_ACTION_exit;
            m_SoftkeyData[3].m_strIconPath = PIC_SOFTKEY_EXIT;
        }
        break;
    default:
        {
            // default none
        }
        break;
    }

    // 刷新softkey
    _MainWND.DrawSoftkey(this);
    _MainWND.DisplayIMESoftkey(focusWidget());
}

bool CDlgZeroSPTouch::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL == pObject || NULL == pEvent)
    {
        return false;
    }

    if (QEvent::Show == pEvent->type())
    {
        if (ZERO_PAGE_REDIRECTOR_SUBMIT == m_ePageType)
        {
            qSetEnterType(ENTER_TYPE_NEXT);
            ui.editUserName->setFocus();
            QWidget::setTabOrder(ui.editUserName, ui.editPassword);
            qSetIsShowWnd(ui.editUserName, true);
            return false;
        }
        if (ZERO_PAGE_NETWORK_SETTING == m_ePageType)
        {
            QWidget * pWgt = focusWidget();
            if (pWgt != NULL)
            {
                _MainWND.DisplayIMESoftkey(pWgt);
            }
        }
    }
    else if (QEvent::KeyPress == pEvent->type())
    {
        QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;

        if (PHONE_KEY_CANCEL == pKeyEvent->key())
        {
            // 处理有下拉框时X键
            QWidget * pwgt = focusWidget();
            if (NULL != pwgt && pwgt->inherits("QComboBox")
                    && pwgt != pObject)
            {
                QComboBox * pCmbBox = (QComboBox *)pwgt;
                pCmbBox->hidePopup();
                return true;
            }

            if (ZERO_PAGE_MAIN == m_ePageType || ZERO_PAGE_REDIRECTOR_SUBMIT == m_ePageType
                    || ZERO_PAGE_REDIRECTOR_NOTE == m_ePageType)
            {
                OnAction(ACTION_ZERO_QUIT);
            }
            else if (ZERO_PAGE_NETWORK_SETTING == m_ePageType
                     || ZERO_PAGE_AUTOP == m_ePageType
                     || ZERO_PAGE_AUTHENTICATION == m_ePageType
                     || ZERO_PAGE_STATUS == m_ePageType)
            {
                OnAction(PAGE_ACTION_back);
            }
            else if (ZERO_PAGE_REDIRECTOR_SUBMIT == m_ePageType
#if IF_FEATURE_METASWITCH
                     || ZERO_PAGE_AUTOP_DIRECT == m_ePageType
#endif
                    )
            {
                OnAction(PAGE_ACTION_exit);
            }
            return true;
        }

        if (ZERO_PAGE_NETWORK_SETTING == m_ePageType)
        {
            if (PHONE_KEY_OK != pKeyEvent->key()
                    && PHONE_KEY_CANCEL != pKeyEvent->key())
            {
                return m_pDlgZeroNetwork->eventFilter(pObject, pEvent);
            }
        }

        if (ZERO_PAGE_AUTOP == m_ePageType
                || ZERO_PAGE_AUTHENTICATION == m_ePageType
#if IF_FEATURE_METASWITCH
                || ZERO_PAGE_AUTOP_DIRECT == m_ePageType
#endif
           )
        {
            if (PHONE_KEY_OK != pKeyEvent->key()
                    && PHONE_KEY_CANCEL != pKeyEvent->key())
            {
                return m_pDlgAutoP->eventFilter(pObject, pEvent);
            }
            if (FullKeyboardEnabled())
            {
                if (PHONE_KEY_OK == pKeyEvent->key())
                {
                    if (m_pDlgAutoP->eventFilter(pObject, pEvent))
                    {
                        return true;
                    }
                }
            }
        }

        if (PHONE_KEY_OK == pKeyEvent->key())
        {
            //相当于点击next
            //点击ok效果与softKey 4保持一致
            switch (m_ePageType)
            {
            case ZERO_PAGE_MAIN:
            case ZERO_PAGE_NETWORK_SETTING:
            case ZERO_PAGE_AUTOP:
            case ZERO_PAGE_AUTHENTICATION:
#if IF_FEATURE_METASWITCH
            case ZERO_PAGE_AUTOP_DIRECT:
#endif
                {
                    qSetHideKeyboard();
                    OnAction(ACTION_ZERO_NEXT_PAGE);
                }
                break;
            case ZERO_PAGE_REDIRECTOR_SUBMIT:
                {
                    if (ui.editPassword->hasFocus())
                    {
                        OnAction(PAGE_ACTION_submit);
                        qSetHideKeyboard();
                    }
                    else
                    {
                        ui.editPassword->setFocus();
                        ui.editPassword->selectAll();
                        qSetEnterType(ENTER_TYPE_DONE);
                        qSetIsShowWnd(ui.editPassword, true);
                    }
                }
                break;
            case ZERO_PAGE_NOTE_UPDAT_FAIL:
            case ZERO_PAGE_NOTE_UPTADE_FINISH:
            case ZERO_PAGE_REDIRECTOR_FINISH:
                {
                    OnAction(ACTION_ZERO_QUIT);
                }
                break;
            case ZERO_PAGE_STATUS:
                {
                    OnAction(PAGE_ACTION_back);
                }
                break;
            default:
                break;
            }
        }
        else if (PHONE_KEY_UP == pKeyEvent->key()
                 || PHONE_KEY_DOWN == pKeyEvent->key())
        {
            if (ZERO_PAGE_REDIRECTOR_SUBMIT == m_ePageType)
            {
                if (ui.editUserName->hasFocus())
                {
                    ui.editPassword->setFocus();
                    ui.editPassword->selectAll();
                    qSetEnterType(ENTER_TYPE_DONE);
                    qSetIsShowWnd(ui.editPassword, true);
                }
                else
                {
                    ui.editUserName->setFocus();
                    ui.editUserName->selectAll();
                    qSetEnterType(ENTER_TYPE_NEXT);
                    qSetIsShowWnd(ui.editUserName, true);
                }
            }
            return false;
        }
        else if (PHONE_KEY_LEFT == pKeyEvent->key()
                 || PHONE_KEY_RIGHT == pKeyEvent->key())
        {
            if (ZERO_PAGE_REDIRECTOR_SUBMIT == m_ePageType)
            {
                QWidget * pFocusWidget = focusWidget();

                if (NULL != pFocusWidget
                        && pFocusWidget->inherits("QLineEdit"))
                {
                    QLineEdit * pLineEdit = (QLineEdit *)pFocusWidget;

                    // 判断是否选中所有文字
                    if (pLineEdit->selectedText() == pLineEdit->text())
                    {
                        if (PHONE_KEY_LEFT == pKeyEvent->key())
                        {
                            // 设置光标到最前方
                            pLineEdit->setCursorPosition(0);
                        }
                        else
                        {
                            // 设置光标到末尾
                            pLineEdit->end(false);
                        }
                        return true;
                    }
                }
            }
        }
    }
    else if (QEvent::FocusIn == pEvent->type()
             || QEvent::FocusOut == pEvent->type())
    {
        if (ZERO_PAGE_NETWORK_SETTING == m_ePageType)
        {
            return m_pDlgZeroNetwork->eventFilter(pObject, pEvent);
        }

        if (FullKeyboardEnabled() && QEvent::FocusOut == pEvent->type())
        {
            if (pObject->inherits("QLineEdit"))
            {
                QLineEdit *pEdit = static_cast<QLineEdit *>(pObject);
                qReturnEdit(pEdit);
            }
        }

        QWidget * pWgt = focusWidget();

        //如果焦点移动到QLineEdit上设置其光标
        if (NULL != pWgt && pWgt->inherits("QLineEdit"))
        {
            QLineEdit * pLineEdit = (QLineEdit *)pWgt;
            pLineEdit->end(false);            //设置光标在末尾
            //pLineEdit->setCursor(Qt::ArrowCursor); //设置光标
        }
        if (ZERO_PAGE_REDIRECTOR_SUBMIT == m_ePageType)
        {
            if (pObject == ui.editUserName)
            {
                qSetEnterType(ENTER_TYPE_NEXT);
                qSetIsShowWnd(ui.editUserName, true);
            }
            else if (pObject == ui.editPassword)
            {
                qSetEnterType(ENTER_TYPE_DONE);
                qSetIsShowWnd(ui.editPassword, true);
            }
        }

        // 设置softkey的IME信息
        _MainWND.DisplayIMESoftkey(pWgt);
    }
    else if (FullKeyboardEnabled()
             && (pEvent->type() == QEvent::MouseButtonPress
                 || pEvent->type() == QEvent::MouseButtonRelease))
    {
        if (ZERO_PAGE_NETWORK_SETTING == m_ePageType)
        {
            return m_pDlgZeroNetwork->eventFilter(pObject, pEvent);
        }
        else if (ZERO_PAGE_AUTOP == m_ePageType
                 || ZERO_PAGE_AUTHENTICATION == m_ePageType
#if IF_FEATURE_METASWITCH
                 || ZERO_PAGE_AUTOP_DIRECT == m_ePageType
#endif
                )
        {
            return m_pDlgAutoP->eventFilter(pObject, pEvent);
        }
    }

    return CBaseDialog::eventFilter(pObject, pEvent);
}

// 处理界面操作
bool CDlgZeroSPTouch::OnAction(const QString & strAction)
{
    ZEROUI_INFO("CDlgZeroSPTouch::OnAction(%s), pageType[%d]"
                , strAction.toUtf8().data(), m_ePageType);
    m_bShowTime = false;

    if (ACTION_BACK == strAction)
    {
        if (ZERO_PAGE_MAIN == m_ePageType || ZERO_PAGE_REDIRECTOR_SUBMIT == m_ePageType)
        {
            OnAction(ACTION_ZERO_QUIT);
        }
        else
        {
            OnAction(PAGE_ACTION_back);
        }
    }
    else if (ACTION_ZERO_QUIT == strAction || PAGE_ACTION_exit == strAction
             || ACTION_CANCEL == strAction)
    {
        StopRemain();

        m_bRejectToIdle = false;

        //http://10.2.1.199/Bug.php?BugID=123019
        //RPS后台继续执行更新
        //if (Autop_GetUpdateType() == AUT_RPS
        //  && ZERO_PAGE_REDIRECTOR_NOTE == m_ePageType)
        //{
        //  m_bExitAutoPWhenExitDlg = false;
        //}

        // 退出界面
        ZeroSPTouch_Destroy();
    }
    else if (ACTION_ZERO_NEXT_PAGE == strAction)
    {
        // 开启背光灯
        backlight_OpenBacklightEvent(true);

        StopRemain();

#if IF_BUG_30885
        m_bRejectToIdle = true;
#else
        m_bRejectToIdle = false;
#endif

        if (ZERO_PAGE_NETWORK_SETTING == m_ePageType)
        {
            //检查内容合法性
            if (!m_pDlgZeroNetwork->SaveNetworkData())
            {
                return true;
            }
            m_pDlgZeroNetwork->Uninit();
        }
        else if (ZERO_PAGE_AUTOP == m_ePageType
#if IF_FEATURE_METASWITCH
                 || ZERO_PAGE_AUTOP_DIRECT == m_ePageType
#endif
                )
        {
            if (!m_pDlgAutoP->SavePageData())
            {
                return true;
            }
        }
#if IF_BUG_30885
        else if (m_ePageType == ZERO_PAGE_MAIN)
        {
            zero_BeginAutop();
            return true;
        }
#endif

        // 鉴权进入下一个界面由Logic层控制
        if (ZERO_PAGE_AUTHENTICATION == m_ePageType)
        {
            if (!m_pDlgAutoP->SavePageData())
            {
                return true;
            }

            if (!IsRpsOrZerotouch())
            {
                m_bExitAutoPWhenExitDlg = false;

                ZeroSPTouch_Destroy();

                return true;
            }
        }
        else
        {
            zero_EnterNextStage(m_strPageAction, strAction.toUtf8().data());
        }
    }
    else if (ACTION_OPEN_STATUS == strAction)
    {
        // 开启背光灯
        backlight_OpenBacklightEvent(true);

        StopRemain();

        m_bRejectToIdle = false;

        ui.labText->setText(LANG_TRANSLATE(TRID_IF_UPDATE_NOW));

        zero_EnterNextStage(m_strPageAction, strAction.toUtf8().data());
    }
    else if (PAGE_ACTION_submit == strAction)
    {
        //提交数据
        AutoProvisionData currentPageData;

        //处理预输入问题
        InputPreeditContent(ui.editUserName);
        InputPreeditContent(ui.editPassword);

        currentPageData.m_strUsername =
            ui.editUserName->text().trimmed().toUtf8().data();
        currentPageData.m_strPassword =
            ui.editPassword->text().trimmed().toUtf8().data();

        //zero_SetRedirectData(currentPageData.m_strUsername,currentPageData.m_strPassword);

        // 检查网络,进行重定向操作
        if (zero_CheckingNetwork())
        {
            Autop_StartConnect(Autop_GetUpdateType(), currentPageData.m_strUsername,
                               currentPageData.m_strPassword);
        }
    }
    else if (PAGE_ACTION_back == strAction)
    {
        if (ZERO_PAGE_NETWORK_SETTING == m_ePageType)
        {
            m_pDlgZeroNetwork->ClearFocusMark();
        }
        else if (ZERO_PAGE_AUTHENTICATION == m_ePageType)
        {
            ZeroSPTouch_Destroy();

            return true;
        }

        if (ZERO_PAGE_AUTOP == m_ePageType
                && _NetworkChangeFlagManager.GetNetworkChangeFlag())  //判断网络改变标记
        {
            //弹出重启提示后，无论点击是否，都将标记重置
            _NetworkChangeFlagManager.ClearChangeFlag();

            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_NETWORK_CHANGED_IF_TAKE_EFFECT_AFTER_RESTART),
                                        MESSAGEBOX_OkCancel, -1,  "", "", MSGBOX_ID_REBOOT);
        }
        else if (m_ePageType == ZERO_PAGE_AUTOP
                 && configParser_GetConfigInt(kszZeroSimplification) == 1)
        {
            ZeroSPTouch_Destroy();
        }
        else
        {
            zero_EnterNextStage(m_strPageAction, strAction.toUtf8().data());
        }
    }
    else if (IME_ACTION == strAction)
    {
        //输入法切换
        _MainWND.SwitchNextIMEName(this->focusWidget());
    }
    else if (IME_DEL_ACTION == strAction)
    {
        //删除字符
        qOnBackspaceClicked(this);
    }
    else
    {
        chWARNING2(false, "CDlgZeroSPTouch::OnUIAction strAction error!");
    }

    return true;
}

// 将相应的页面Action转换为Type
ZeroPageType CDlgZeroSPTouch::ActionToType(const yl::string & strPageAction)
{
    ZeroPageType eType = ZERO_PAGE_NONE;

    ZEROUI_DEBUG("CDlgZeroSPTouch::ActionToType Action(%s)", strPageAction.c_str());

    if (ZERO_MAIN_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_MAIN;
    }
    else if (ZERO_NETWORK_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_NETWORK_SETTING;
    }
    else if (ZERO_STATUS_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_STATUS;
    }
    else if (ZERO_UPDATING_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_NOTE_UPDATING;
    }
    else if (ZERO_UPDATEFINISH_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_NOTE_UPTADE_FINISH;
    }
    else if (ZERO_UPDATEFAIL_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_NOTE_UPDAT_FAIL;
    }
    else if (ZERO_NETWORK_CHECK_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_NOTE_NETWORK_CHECK;
    }
    else if (ZERO_NETWORK_FAIL_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_NOTE_NETWORK_FAIL;
    }
    else if (ZERO_REBOOTING_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_NOTE_REBOOTING;
    }
    else if (ZERO_REDIRECTOR_NOTE == strPageAction)
    {
        eType = ZERO_PAGE_REDIRECTOR_NOTE;
    }
    else if (ZERO_REDIRECTOR_CONFIG == strPageAction)
    {
        eType = ZERO_PAGE_REDIRECTOR_SUBMIT;
    }
    else if (ZERO_REDIRECTOR_FAILED == strPageAction)
    {
        eType = ZERO_PAGE_REDIRECTOR_FAILED;
    }
    else if (ZERO_REDIRECTOR_FINISH == strPageAction)
    {
        eType = ZERO_PAGE_REDIRECTOR_FINISH;
    }
    else if (ZERO_AUTOP_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_AUTOP;
    }
    else if (strPageAction == ZERO_AUTHENTICATION)
    {
        eType = ZERO_PAGE_AUTHENTICATION;
    }
#if IF_BUG_30885
    else if (strPageAction == ZERO_UNAUTHORIZED_PAGE)
    {
        eType = ZERO_PAGE_UNAUTHORIZED;
    }
#endif
#if IF_FEATURE_METASWITCH
    else if (ZERO_AUTHFAIL_PAGE == strPageAction)
    {
        eType = ZERO_PAGE_NOTE_AUTH_FAIL;
    }
    else if (strPageAction == ZERO_AUTOP_PAGE_DIRECT)
    {
        eType = ZERO_PAGE_AUTOP_DIRECT;
    }
#endif

    ZEROUI_DEBUG("CDlgZeroSPTouch::ActionToType eType(%d)", eType);
    return eType;
}

// 设置页面的类型
void CDlgZeroSPTouch::SetPageType(ZeroPageType ePageType)
{
    // 设置类型
    m_ePageType = ePageType;
    // 停止倒计时
    StopRemain();

    ResetDelayOperatorTimer();

#if IF_FEATURE_METASWITCH
    if (ZERO_PAGE_AUTOP_DIRECT == m_ePageType)
    {
        m_bRejectToIdle = false;
    }
#endif

    if (IsAuthPage())
    {
        StartAuthTime();
    }

    // 刷新界面
    RefreshPage();
}

void CDlgZeroSPTouch::ResetDelayOperatorTimer()
{
    if (m_pDelayTimer != NULL && m_pDelayTimer->isActive())
    {
        m_eDelayType = DOT_NONE;
        m_pDelayTimer->stop();
    }
}

void CDlgZeroSPTouch::OnReturnToIdle()
{
    StopRemain();
    zero_Exit(m_bExitAutoPWhenExitDlg);
}

// 设置页面的类型
void CDlgZeroSPTouch::SetPageType(const yl::string & strPageAction)
{
    m_strPageAction = strPageAction;
    SetPageType(ActionToType(strPageAction));
}

//子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
QPixmap CDlgZeroSPTouch::GetCustomBackGround()
{
    return THEME_GET_BMP(PIC_GENERAL_DLG_NORMAL_BACKGROUND);
}

// 刷新界面
void CDlgZeroSPTouch::RefreshPage()
{
    ZEROUI_INFO("CDlgZeroSPTouch::RefreshPage()");
    //默认隐藏m_pDlgZeroNetwork和m_pDlgZeroStatus
    m_pDlgZeroNetwork->hide();
    m_pDlgAutoP->hide();
    ui.frmRedirector->hide();

    switch (m_ePageType)
    {
    case ZERO_PAGE_MAIN:
        {
            ui.labText->show();

            // 开始倒计时
            if (m_bShowTime)
            {
                StartRemain();
            }
            else
            {
                ui.labText->setText(LANG_TRANSLATE(TRID_IF_UPDATE_NOW));
            }
        }
        break;
    case ZERO_PAGE_NETWORK_SETTING:
        {
            ui.labText->hide();
            m_pDlgZeroNetwork->raise();
            m_pDlgZeroNetwork->show();
            m_pDlgZeroNetwork->Init();
        }
        break;
    case ZERO_PAGE_AUTOP:
#if IF_FEATURE_METASWITCH
    case ZERO_PAGE_AUTOP_DIRECT:
#endif
        {
            ui.labText->hide();
            m_pDlgAutoP->SetPageType(AUTOP_PAGE_NORMAL);
            m_pDlgAutoP->LoadPageData();
            m_pDlgAutoP->show();
        }
        break;
    case ZERO_PAGE_AUTHENTICATION:
        {
            ui.labText->hide();
            m_pDlgAutoP->SetPageType(AUTOP_PAGE_AUTHENTICATION);
            m_pDlgAutoP->LoadPageData();
            m_pDlgAutoP->show();
        }
        break;
    case ZERO_PAGE_NOTE_UPDATING:
        {
            ui.labText->show();
            ui.labText->setText(LANG_TRANSLATE(TRID_CONFIG_UPDATING));
        }
        break;
    case  ZERO_PAGE_NOTE_UPTADE_FINISH:
    case ZERO_PAGE_REDIRECTOR_FINISH:
        {
            ui.labText->show();
            ui.labText->setText(LANG_TRANSLATE(TRID_UPDATE_FINISH));

            DelayOperator(DOT_UPDATE_FINISH);
        }
        break;
    case  ZERO_PAGE_NOTE_REBOOTING:
        {
            ui.labText->show();
            ui.labText->setText(LANG_TRANSLATE(TRID_NETWORK_CHANGED_AND_REBOOTING));
        }
        break;
    case  ZERO_PAGE_NOTE_UPDAT_FAIL:
        {
            ui.labText->show();
            ui.labText->setText(LANG_TRANSLATE(TRID_UPDATE_SKIPPED));

            DelayOperator(DOT_UPDATE_FAIL);
        }
        break;
#if IF_FEATURE_METASWITCH
    case  ZERO_PAGE_NOTE_AUTH_FAIL:
        {
            ui.labText->setWordWrap(true);
            ui.labText->show();
            ui.labText->setText(LANG_TRANSLATE(TRID_AUTOP_AUTH_ERR));

            DelayOperator(DOT_AUTH_FAIL);
        }
        break;
#endif
    case ZERO_PAGE_NOTE_NETWORK_CHECK:
        {
            ui.labText->show();
            ui.labText->setText(LANG_TRANSLATE(TRID_NETWORK_CHECKING));
        }
        break;
    case  ZERO_PAGE_NOTE_NETWORK_FAIL:
        {
            ui.labText->show();
            ui.labText->setText(LANG_TRANSLATE(TRID_NETWORK_UNAVAILABLE));

            DelayOperator(DOT_NETWORK_FAIL);
        }
        break;
    case ZERO_PAGE_REDIRECTOR_NOTE:
        {
            ui.labText->setText(LANG_TRANSLATE(TRID_SENDING_PROVISION_REQUEST));
            ui.labText->show();
            ui.frmRedirector->hide();
        }
        break;
    case ZERO_PAGE_REDIRECTOR_SUBMIT:
        {
            ui.labText->hide();
            ui.frmRedirector->show();

            ui.editUserName->setFocusPolicy(Qt::StrongFocus);
            ui.editPassword->setFocusPolicy(Qt::StrongFocus);

            ui.editUserName->clear();
            ui.editPassword->clear();

            // V82 新需求：http://10.2.1.199/Bug.php?BugID=117433
            AutoProvisionData stAutoProvisionData;
            if (g_pAutoProvisionController->GetAuthenticationInfo(&stAutoProvisionData))
            {
                ui.editUserName->setText(toQString(stAutoProvisionData.m_strUsername));
            }

            //设置焦点在用户名输入框上
            ui.editUserName->setFocus();
        }
        break;
    case ZERO_PAGE_REDIRECTOR_FAILED:
        {
            ui.labText->setText(LANG_TRANSLATE(TRID_REDIRCT_UPDATING_FAILED));
            ui.labText->show();
            ui.frmRedirector->hide();
            _MainWND.update();

            //3秒后退出提示
            DelayOperator(DOT_UPDATE_FAIL);
        }
        break;
#if IF_BUG_30885
    case ZERO_PAGE_UNAUTHORIZED:
        {
            ui.labText->setWordWrap(true);
            ui.labText->setText(LANG_TRANSLATE(TRID_AUTHORIZED_INCORRECT));
            ui.labText->show();
            ui.frmRedirector->hide();
            _MainWND.update();

            //3秒后退出提示
            QTimer::singleShot(WAITTING_EXIT_TIME, this, SLOT(OnAuthorizedIncorrectTimeout()));
        }
        break;
#endif
    default:
        {
            ui.labText->setText("");
        }
        break;
    }

    //更新标题
    UpdateTitle();

    //设置softkey
    SetSoftkeyData();

    // 刷新softkey
    _MainWND.DrawSoftkey(this);
    _MainWND.DisplayIMESoftkey(focusWidget());
}

// 开启倒计时
void CDlgZeroSPTouch::StartRemain()
{
    //  读取配置表设置总时间
    m_nRemainTime = configParser_GetConfigInt(kszZeroWaitTime);
    if (m_nRemainTime <= 0)
    {
        m_nRemainTime = 10;
    }

    if (NULL != m_pRemainTimer)
    {
        // 设置lab的文字
        ui.labText->setText(LANG_TRANSLATE(TRID_IF_UPDATE_NOW) + " " + QString::number(
                                m_nRemainTime) + LANG_TRANSLATE(TRID_ZERO_S));

        m_pRemainTimer->start(COUNT_DOWN_INTERVAL_TIME);
    }
}

// 关闭倒计时
void CDlgZeroSPTouch::StopRemain()
{
    if (NULL != m_pRemainTimer)
    {
        m_pRemainTimer->stop();
    }
}

void CDlgZeroSPTouch::OnRemainTimeOut()
{
    //每次超时,时间减少1s,形成倒时计效果
    if (m_nRemainTime > 1
            && ZERO_PAGE_MAIN == m_ePageType)
    {
        m_nRemainTime --;
        ui.labText->setText(LANG_TRANSLATE(TRID_IF_UPDATE_NOW) + " " + QString::number(
                                m_nRemainTime) + LANG_TRANSLATE(TRID_ZERO_S));
    }
    else
    {
        // 停止计时器
        if (NULL != m_pRemainTimer)
        {
            m_pRemainTimer->stop();
        }

        m_bRejectToIdle = false;

#if IF_BUG_30885
        bool bReturnToIdle = configParser_GetConfigInt(kszZeroDirectUpdate) == 0;
        if (!bReturnToIdle)
        {
            // 倒计时进入下一个页面
            OnAction(ACTION_ZERO_NEXT_PAGE);
            m_bRejectToIdle = true;
        }
        else
#endif
        {
            //倒时计结束,退出界面
            ZeroSPTouch_Destroy();
        }
    }
}

void CDlgZeroSPTouch::OnNetworkFailPageTimeout()
{
    if (Autop_GetUpdateType() == AUT_RPS)
    {
        ZeroSPTouch_OpenDlg(ZERO_NETWORK_PAGE);
    }
    else if (configParser_GetConfigInt(kszZeroSimplification) == 1)
    {
        // 网络检查失败后，跳到Autop界面
        ZeroSPTouch_OpenDlg(ZERO_AUTOP_PAGE);
    }
    else if (1 == configParser_GetConfigInt(kszZeroNetworkAndURLEnable))
    {
#if IF_BUG_30885
        ZeroSPTouch_OpenDlg(ZERO_UPDATEFAIL_PAGE);
#else
        // 网络检查失败后，跳到网络配置界面
        ZeroSPTouch_OpenDlg(ZERO_NETWORK_PAGE);
#endif
    }
    else
    {
        ZeroSPTouch_OpenDlg(ZERO_MAIN_PAGE);
    }
}

void CDlgZeroSPTouch::OnUpdateFinishPageTimeout()
{
    ZEROUI_INFO("CDlgZeroSPTouch::OnUpdateFinishPageTimeout");

    m_bRejectToIdle = false;
    ZeroSPTouch_Destroy();
}

void CDlgZeroSPTouch::OnUpdataFailPageTimeout()
{
    ZEROUI_DEBUG("CDlgZeroSPTouch::OnUpdataFailPageTimeout");

    m_bRejectToIdle = false;
    // 退出Zero的模块
    ZeroSPTouch_Destroy();
}

void CDlgZeroSPTouch::OnAuthFailPageTimeout()
{
#if IF_FEATURE_METASWITCH
    ZEROUI_DEBUG("CDlgZeroSPTouch::OnAuthFailPageTimeout");
    m_bRejectToIdle = false;
    if (Autop_GetUpdateType() == AUT_MTSW_HTTP_DIGEST)
    {
        ZeroSPTouch_OpenDlg(ZERO_AUTOP_PAGE_DIRECT);
    }
    else if (Autop_GetUpdateType() == AUT_ZERO)
    {
        ZeroSPTouch_OpenDlg(ZERO_AUTHENTICATION);
    }
    else
    {
        // 退出Zero的模块
        ZeroSPTouch_Destroy();
    }
#endif
}

void CDlgZeroSPTouch::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int nResult = pMessageBox->GetResult();
    switch (pMessageBox->GetID())
    {
    case MSGBOX_ID_REBOOT:
        {
            if (MessageBox_OK == nResult)
            {
                AdapterBox_Reboot(0);
            }
            else
            {
                zero_EnterNextStage(m_strPageAction, PAGE_ACTION_back);
            }
        }
        break;

    default:
        break;
    }
}

bool CDlgZeroSPTouch::IsShowBackButton()
{
    if (ZERO_UPDATING_PAGE == m_strPageAction
            || ZERO_UPDATEFAIL_PAGE == m_strPageAction
            || ZERO_UPDATEFINISH_PAGE == m_strPageAction
            || ZERO_REBOOTING_PAGE == m_strPageAction
            || ZERO_NETWORK_CHECK_PAGE == m_strPageAction
            || ZERO_NETWORK_FAIL_PAGE == m_strPageAction
            //|| ZERO_REDIRECTOR_CONFIG == m_strPageAction
#if IF_BUG_30885
            || ZERO_MAIN_PAGE == m_strPageAction
            || ZERO_UNAUTHORIZED_PAGE == m_strPageAction
#endif
            || ZERO_REDIRECTOR_FAILED == m_strPageAction
            || ZERO_REDIRECTOR_FINISH == m_strPageAction
            || ZERO_REDIRECTOR_NOTE == m_strPageAction
#if IF_FEATURE_METASWITCH
            || ZERO_AUTHFAIL_PAGE == m_strPageAction
#endif
       )
    {
        return false;
    }
    else
    {
        return true;
    }
}

// 鉴权失败的提示
void CDlgZeroSPTouch::OnAuthorizedIncorrectTimeout()
{
#if IF_BUG_30885
    ZeroSPTouch_OpenDlg(ZERO_AUTOP_PAGE);
#endif
}

void CDlgZeroSPTouch::OnKeyboardShow()
{
    if (NULL != m_pDlgZeroNetwork)
    {
        m_pDlgZeroNetwork->OnKeyboardShow();
    }
}

void CDlgZeroSPTouch::OnKeyboardHide()
{
    if (NULL != m_pDlgZeroNetwork)
    {
        m_pDlgZeroNetwork->OnKeyboardHide();
    }
}

void CDlgZeroSPTouch::StartAuthTime()
{
    m_bRejectToIdle = true;
    StopRemain();

    int iAuthTime = configParser_GetConfigInt(kszAuthExpiredTime);
    if (iAuthTime == 0)
    {
        return;
    }
    else if (iAuthTime < 10)
    {
        iAuthTime = 10;
    }
    else if (iAuthTime > 86400)
    {
        iAuthTime = 86400;
    }

    if (NULL != m_pRemainTimer)
    {
        m_pRemainTimer->start(iAuthTime * 1000);
    }
}

bool CDlgZeroSPTouch::IsAuthPage()
{
    if (ZERO_PAGE_AUTHENTICATION == m_ePageType)
    {
        return true;
    }
    return false;
}

bool CDlgZeroSPTouch::IsResultPage()
{
    if (ZERO_PAGE_NOTE_UPDAT_FAIL == m_ePageType
            || ZERO_PAGE_NOTE_UPTADE_FINISH == m_ePageType)
    {
        return true;
    }
    return false;
}
