#include "inputmanager.h"
#include "widgetinfomanager.h"
#include <QtGui/QApplication>
#include <QLineEdit>
#include <QTextEdit>
#include "mainwnd/mainwnd.h"
#include "qtcommon/qmisc.h"
#include "globalinputhook/modinputhook.h"

CInputManager::CInputManager(void)
    : m_bBlockKeyboard(false)
{

}

CInputManager::~CInputManager(void)
{
}

CInputManager& CInputManager::GetInstance()
{
    static CInputManager* pInputManager = NULL;
    if (NULL == pInputManager)
    {
        pInputManager = new CInputManager();
        //pInputManager->Init();
    }
    return *pInputManager;
}

void CInputManager::Init()
{
    m_inputdlgmanager.Init();
    connect(&m_inputdlgmanager, SIGNAL(SignChangeDlg(INPUT_DLGTYPE)), this, SLOT(OnDlaTypeChange(INPUT_DLGTYPE)));
    connect(&m_inputdlgmanager, SIGNAL(SignKeyboardShow()), this, SIGNAL(SignKeyboardShow()));
    connect(&m_inputdlgmanager, SIGNAL(SignKeyboardHide()), this, SIGNAL(SignKeyboardHide()));
    connect(&m_inputdlgmanager, SIGNAL(SignKeyboardBeforeShow()), this, SIGNAL(SignKeyboardBeforeShow()));
    connect(&m_inputdlgmanager, SIGNAL(SignKeyboardBeforeHide()), this, SIGNAL(SignKeyboardBeforeHide()));
    connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(OnFocusChanged(QWidget*, QWidget*)));

    inputHook_RegisterKeyPadHookProc(CInputManager::InputDlgKeyHookCallbackFun, HOOK_MODULE_KEYBOARD);
    inputHook_RegisterMouseHookProc(CInputManager::InputDlgMouseHookCallbackFun, HOOK_MODULE_KEYBOARD);
    //UIManager_RegisterTopWndChangeCallBack(CInputManager::InputDlgOnTopWndChange);
}

bool CInputManager::InputDlgKeyHookCallbackFun(int nKeyCode, bool bPress)
{
    return g_inputmanager.ProcessKeyEvent(nKeyCode, bPress);
    /*if ((NULL == pObject) || (NULL == pEvent))
    {
        return false;
    }

    if ((pObject->inherits("QLineEdit")
        || pObject->inherits("QTextEdit"))
        && qPassEventToEdit(pObject, pEvent))
    {
        return true;
    }

    return false;*/
}

bool CInputManager::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    return m_inputdlgmanager.ProcessKeyEvent(nKeyCode, bPress);
}

bool CInputManager::InputDlgMouseHookCallbackFun(QObject* pObject, QMouseEvent *pEvent)
{
    return g_inputmanager.ProcessMouseEvent(pObject, pEvent);
}

bool CInputManager::ProcessMouseEvent(QObject* pObject, QMouseEvent *pEvent)
{
    return m_inputdlgmanager.ProcessMouseEvent(pObject, pEvent);
}

bool CInputManager::RegisterKeyBoardShowHookProc(KEYBOARD_SHOW_HOOK_PROC keyboardShowHook, KEYBOARD_SHOW_HOOK_MODULE priority)
{
    return m_inputdlgmanager.RegisterKeyBoardShowHookProc(keyboardShowHook, priority);
}

bool CInputManager::UnRegisterKeyBoardShowHookProc(KEYBOARD_SHOW_HOOK_PROC keyboardShowHook)
{
    return m_inputdlgmanager.UnRegisterKeyBoardShowHookProc(keyboardShowHook);
}

//void CInputManager::InputDlgOnTopWndChange(const CTopWndChangeInfo& objInfo)
//{
//  g_inputmanager.ProcessOnTopWndChange(objInfo);
//}
//
//void CInputManager::ProcessOnTopWndChange(const CTopWndChangeInfo& objInfo)
//{
//  if (objInfo.m_bBeforeTopWndChange && IsShowKeyboard() && NULL != objInfo.m_dlgPreTop)
//  {
//      HideInputDlg(objInfo.m_dlgPreTop->focusWidget());
//  }
//}

void CInputManager::SetIsPlayKeyboardAnimation(bool bPlay)
{
    m_inputdlgmanager.SetIsPlayKeyboardAnimation(bPlay);
}

void CInputManager::qSetKeyBoard(QWidget* p, bool bIsAutoShow, INPUT_DLGTYPE enTyep,
                                 INPUT_DIALOG_STYLE enStyle /*= INPUT_DIALOG_NORMAL */)
{
    if (NULL == p)
    {
        return;
    }
    INPUTMETHOD_INFO("CInputManager::qSetKeyBoard--------wdg--[%p]", p);

    g_WidgetInfoManager.SaveKeyBoard(p, bIsAutoShow, enTyep, enStyle);

#ifdef CUSTOME_KEYBOARD
    QString strFontsName = IME_KEYBOARD_CUSTOM;
#else
    // 这里添加全键盘对应的字库
    // 目前外部不需要设置全键盘的字库，直接保存大、小写字库
    QString strFontsName = IME_KEYBOARD_ALL;
#endif

    g_WidgetInfoManager.SaveIME(p, strFontsName);

    if (bIsAutoShow)
    {
        p->installEventFilter(this);
    }
    else
    {
        connect(p, SIGNAL(destroyed(QObject*)), this, SLOT(ReleaseWidget(QObject*)));
    }
}

void CInputManager::qSetKeyBoardType(QWidget* p, INPUT_DLGTYPE enType)
{
    if (NULL == p)
    {
        return;
    }
    g_WidgetInfoManager.SetKeyBoardType(p, enType);

    // 切换字库
    // 大写和小写的字库，都包含有数字键盘的字库
    QString strFontName = "";
    if (INPUT_DLGTYPE_EN == enType)
    {
        strFontName = FONT_NORMALS;
    }
    else
    {
        strFontName = FONT_NORMAL_SHIFT;
    }
    g_WidgetInfoManager.SetFontType(p, strFontName.toStdString());
    m_FontsManager.SetFontByType(strFontName.toStdString());
}

void CInputManager::qSetEnterType(EnterKeyType eEnterType)
{
    m_inputdlgmanager.SetEnterType(eEnterType);
}

void CInputManager::qRaiseFullKeyboard()
{
    m_inputdlgmanager.RaiseFullKeyboard();
}

void CInputManager::InstallIME(QWidget* p, QString strFonts)
{
    if (strFonts.size() == 0)
    {
        return ;
    }
    if (NULL == p)
    {
        return;
    }
    INPUTMETHOD_INFO("CInputManager::InstallIME--------wdg--[%p][%s]", p, strFonts.toUtf8().constData());
    if (g_WidgetInfoManager.SaveIME(p, strFonts))
    {
        p->installEventFilter(this);
        //connect( p ,SIGNAL(destroyed(QWidget*)) ,this ,SLOT(ReleaseWidget(QWidget*)));
    }
}

INPUT_DLGTYPE CInputManager::SetKeyBoardTypeAndStyle(const QWidget* p)
{
    INPUT_DLGTYPE enType = g_WidgetInfoManager.GetKeyBoardTypeToUse(p);
    if (INPUT_DLGTYPE_NONE == enType)
    {
        m_inputdlgmanager.HideInputDlgImmediately();
        return INPUT_DLGTYPE_NONE;
    }

    m_inputdlgmanager.SetInputDlgByType(enType);
    m_inputdlgmanager.SetInputDlgStyle(g_WidgetInfoManager.GetKeyBoardStyleToUse(p));
    return enType;
}

void CInputManager::ShowInputDlg(const QWidget* _pQwidget)
{
    INPUTMETHOD_INFO("----------CInputManager::ShowInputDlg-----[%p]--------", _pQwidget);
    //QWidget* pwgt = _MainWND.focusWidget();
    if (NULL == _pQwidget)
    {
        return;
    }

    INPUT_DLGTYPE enType = SetKeyBoardTypeAndStyle(_pQwidget);
    if (INPUT_DLGTYPE_NONE != enType)
    {
        m_inputdlgmanager.ShowInputDlg(enType);
    }

#ifdef CUSTOME_KEYBOARD
    string strFontName = FONT_CUSTOM_KEYBOARD;
#else
    string strFontName = g_WidgetInfoManager.GetCurrFontType(_pQwidget);
    if (strFontName == "")
    {
        return;
    }
#endif

    m_FontsManager.DealFocusIn();
    m_FontsManager.SetFontByType(strFontName);
}

void CInputManager::HideInputDlg(const QWidget* _pQwidget)
{
    if (g_WidgetInfoManager.IsCurrWidget(_pQwidget))
    {
        m_inputdlgmanager.HideInputDlgImmediately();
    }
}

//强制让全键盘隐藏  与qSetIsShowWnd 接口不同是  这边不用参数 直接隐藏
//目前主要给 t49-premise 会议弹框使用
void CInputManager::ForceHideInputDlg()
{
    if (!m_inputdlgmanager.IsDlgShow())
    {
        return ;
    }

    m_inputdlgmanager.HideInputDlgImmediately();
}

void CInputManager::BlockKeyboard(bool bBlock /* = false */)
{
    m_bBlockKeyboard = bBlock;
}

void CInputManager::OnDlaTypeChange(INPUT_DLGTYPE enType)
{
    // 切换全键盘
    g_WidgetInfoManager.SetKeyBoardType(_MainWND.focusWidget(), enType);

    QString strFontName = "";

#ifdef CUSTOME_KEYBOARD
    strFontName = FONT_CUSTOM_KEYBOARD;
#else
    if (INPUT_DLGTYPE_NUM == enType)
    {
        // 数字键盘，目前情况下，可以不切换字库
        return;
    }

    // 切换字库
    // 大写和小写的字库，都包含有数字键盘的字库

    if (INPUT_DLGTYPE_EN == enType)
    {
        strFontName = FONT_NORMALS;
    }
    else
    {
        strFontName = FONT_NORMAL_SHIFT;
    }
#endif
    g_WidgetInfoManager.SetFontType(_MainWND.focusWidget(), strFontName.toStdString());
    m_FontsManager.SetFontByType(strFontName.toStdString());
}

QString CInputManager::ChangeToNextFont(QWidget* p)
{
    /*if ( IME_Abc == g_WidgetInfoManager.GetCurrFontType(p) )
    {
    g_WidgetInfoManager.SetNameFormat(p, false);
    }*/
    std::string strFontName = g_WidgetInfoManager.GetNextFontsType(p);
    if (strFontName == "")
    {
        return m_FontsManager.GetCurrFontTitle();
    }
    while (!m_FontsManager.SetFontByType(strFontName))
    {
        strFontName = g_WidgetInfoManager.DeleteFontType(p, strFontName);
        if (strFontName == "")
        {
            return m_FontsManager.GetCurrFontTitle();
        }
    }
    g_WidgetInfoManager.SetCurrWidgetFont(p, strFontName);
    /*if ( strFontName == IME_Abc )
    {
        g_WidgetInfoManager.SetNameFormat(p, true);
    }*/
    return m_FontsManager.GetCurrFontTitle();
}

void CInputManager::DealBackspace(QWidget* pWidget)
{
    m_FontsManager.OnBackspaceClicked(pWidget);
    //OnKeyPress(pWidget, Qt::Key_Backspace);
}


bool CInputManager::eventFilter(QObject *pObj, QEvent *pEvent)
{
    if (pObj == NULL || pEvent == NULL)
    {
        return false;
    }
    if (pEvent->type() == QEvent::KeyPress)
    {
        QKeyEvent* pKeyEvent = (QKeyEvent*)pEvent;
        if (NULL != pKeyEvent)
        {
            int nKeyCode = pKeyEvent->key();
            return OnKeyPress(pObj, nKeyCode);
        }
    }
    else if (pEvent->type() == QEvent::KeyRelease)
    {
        QKeyEvent* pKeyEvent = (QKeyEvent*)pEvent;
        if (NULL != pKeyEvent)
        {
            int nKeyCode = pKeyEvent->key();
            return OnKeyRelease(pObj, nKeyCode);
        }
    }
    else if (pEvent->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* pMouseEvent = static_cast<QMouseEvent*>(pEvent);
        if (NULL != pMouseEvent)
        {
            OnMousePress(pObj);
        }
    }
    else if (QEvent::Show == pEvent->type())
    {
        QWidget* v_pWidget          = (QWidget*)pObj;
        if (NULL == v_pWidget)
        {
            return false;
        }
        if (v_pWidget->hasFocus())
        {
            OnFocusIn(v_pWidget);
        }
    }
    else if (QEvent::Hide == pEvent->type())
    {
        OnEventHide(pObj);
    }
    else if (pEvent->type() == QEvent::Destroy)
    {
        QWidget* v_pWidget          = (QWidget*)pObj;
        if (NULL == v_pWidget)
        {
            return false;
        }
        ReleaseWidget(v_pWidget);
    }

    return false;
}

bool CInputManager::ReleaseWidget(QObject *pObj)
{
    QWidget* pWidget = (QWidget*)pObj;
    if (NULL == pWidget)
    {
        return false;
    }
    INPUTMETHOD_INFO("CInputManager::ReleaseWidget--------wdg--[%p]", pWidget);
    // 2017年8月18日 防止控件已经destroy掉，父界面析构时又调用卸载输入法
    // 如 zerotouch界面析构时会卸载控件输入法
    if (!g_WidgetInfoManager.IsWidgetBind(pWidget))
    {
        return true;
    }
    const QMetaObject * pmetaboj = pWidget->metaObject();
    if (pmetaboj != NULL)
    {
        // 打印信息
        INPUTMETHOD_INFO("objectName is %s, classname is %s.", pWidget->objectName().toUtf8().data(), pmetaboj->className());
    }
    else
    {
        // 打印信息
        INPUTMETHOD_INFO("objectName is %s", pWidget->objectName().toUtf8().data());
    }
    pWidget->removeEventFilter(this);
    //m_FontsManager.EnterEditData(pWidget);
    OnEventHide(pWidget);
    g_WidgetInfoManager.DeleteWidget(pWidget);
    return true;
}

void CInputManager::OnFocusChanged(QWidget* pOld, QWidget* pNow)
{
    if (g_WidgetInfoManager.IsWidgetBind(pOld))
    {
        OnFocusOut(pOld);
    }
    if (g_WidgetInfoManager.IsWidgetBind(pNow))
    {
        OnFocusIn(pNow);
    }
}

bool CInputManager::OnFocusIn(QWidget* pWidget)
{
    INPUTMETHOD_INFO("CInputManager::OnFocusIn--------wdg--[%p]", pWidget);

    if (NULL == pWidget || pWidget->isHidden()
            || !pWidget->isEnabled()
            || ((!pWidget->inherits("QLineEdit")) && (!pWidget->inherits("QTextEdit"))))
    {
        return false;
    }

    INPUT_DLGTYPE enType = SetKeyBoardTypeAndStyle(pWidget);
    if (INPUT_DLGTYPE_NONE != enType)
    {
        if (g_WidgetInfoManager.IsKeyBoardAutoShow(pWidget) && !m_bBlockKeyboard)
        {
            m_inputdlgmanager.ShowInputDlg(enType);
        }
    }
    string strFontName = g_WidgetInfoManager.GetCurrFontType(pWidget);
    if (strFontName == "")
    {
        return false;
    }
    m_FontsManager.DealFocusIn();
    m_FontsManager.SetFontByType(strFontName);
    return true;
}

bool CInputManager::OnFocusOut(QWidget* pWidget)
{
    INPUTMETHOD_INFO("CInputManager::OnFocusOut--------wdg--[%p]", pWidget);

    if (NULL == pWidget || ((!pWidget->inherits("QLineEdit")) && (!pWidget->inherits("QTextEdit"))))
    {
        return false;
    }

    if (g_WidgetInfoManager.IsCurrWidget(pWidget))
    {
        // todo
        // ……
        // 因为需要支持界面划动，先屏蔽掉FocusOut时全键盘自动隐藏功能，FocusOut时需要外部自行调用隐藏全键盘的操作
        m_FontsManager.EnterEditData(pWidget);
        if (g_WidgetInfoManager.IsKeyBoardAutoShow(pWidget))
        {
            INPUTMETHOD_INFO("CInputManager::OnFocusOut hide");
            m_inputdlgmanager.HideInputDlgAfter();
            m_FontsManager.DealFocusOut();
            g_WidgetInfoManager.DealFocusOut();
        }
        return true;
    }
    return false;
}

bool CInputManager::OnMousePress(const QObject *pObj)
{
    QWidget* v_pWidget          = (QWidget*)pObj;
    if (NULL == v_pWidget || !v_pWidget->isEnabled())
    {
        return false;
    }
    INPUTMETHOD_INFO("CInputManager::OnMousePress--------wdg--[%p]", v_pWidget);
    m_FontsManager.DealMousePress();
    INPUT_DLGTYPE enType = SetKeyBoardTypeAndStyle(v_pWidget);
    if (INPUT_DLGTYPE_NONE != enType)
    {
        if (g_WidgetInfoManager.IsKeyBoardAutoShow(v_pWidget))
        {
            m_inputdlgmanager.ShowInputDlg(enType);
        }
    }
    return true;
}

bool CInputManager::OnKeyPress(QObject *pObj, int iKeyCode)
{
    QWidget* v_pWidget          = (QWidget*)pObj;
    if (NULL == v_pWidget)
    {
        return false;
    }
    INPUTMETHOD_INFO("CInputManager::OnKeyPress--------wdg--[%p]----keycode---[%d]---", v_pWidget, iKeyCode);
    if (PHONE_KEY_INPUTMETHOD_HIDEFULLKEYBOARD == iKeyCode)
    {
        m_inputdlgmanager.HideInputDlgImmediately();
        return true;
    }

    if (Qt::Key_Backspace == iKeyCode)
    {
        DealBackspace(v_pWidget);
        return true;
    }
    return m_FontsManager.DealKeyPress(pObj, iKeyCode);
}

bool CInputManager::OnKeyRelease(QObject *pObj, int iKeyCode)
{
    QWidget* v_pWidget          = (QWidget*)pObj;
    if (NULL == v_pWidget)
    {
        return false;
    }

    return m_FontsManager.DealKeyRelease(pObj, iKeyCode);
}

bool CInputManager::OnEventHide(const QObject *pObj)
{
    QWidget* v_pWidget          = (QWidget*)pObj;
    if (NULL == v_pWidget)
    {
        return false;
    }

    INPUTMETHOD_INFO("CInputManager::OnEventHide--------wdg--[%p]", v_pWidget);
    if (g_WidgetInfoManager.IsCurrWidget(v_pWidget))
    {
        m_FontsManager.EnterEditData(v_pWidget);
        m_inputdlgmanager.HideInputDlgAfter();
        m_FontsManager.DealFocusOut();
        g_WidgetInfoManager.DealFocusOut();
        return true;
    }
    return false;
}

// 将预编辑的字符串输入控件，并返回控件上的内容
QString CInputManager::qReturnEdit(QWidget* pWidget)
{
    if (NULL == pWidget || !g_WidgetInfoManager.IsCurrWidget(pWidget))
    {
        return "";
    }
    INPUTMETHOD_INFO("CInputManager::qReturnEdit---wdg-[%p]---", pWidget);
    m_FontsManager.EnterEditData(pWidget);
    if (pWidget->inherits("QLineEdit"))
    {
        QLineEdit* pLineEdit = (QLineEdit*)(pWidget);
        if (pLineEdit != NULL)
        {
            yl::string str = fromQString(pLineEdit->text());
            return pLineEdit->text();
        }
    }
    else if (pWidget->inherits("QTextEdit"))
    {
        QTextEdit* pTextEdit = (QTextEdit*)(pWidget);
        if (pTextEdit != NULL)
        {
            return pTextEdit->toPlainText();
        }
    }
    return "";
}



// 输入预编辑内容
// 返回true表示处于预编辑状态
// 返回false表示不处于预编辑状态
bool CInputManager::InputPreeditContent(QWidget* pWidget)
{
    return m_FontsManager.InputPreeditContent(pWidget);
}

bool CInputManager::IsPreedit(QWidget* pWidget)
{
    return m_FontsManager.IsPreedit(pWidget);
}

bool CInputManager::IsShowKeyboard()
{
    return m_inputdlgmanager.IsDlgShow();
}

QString CInputManager::GetIMEFirstChar(QString lpszIMEName, int iKeyCode)
{
    return m_FontsManager.GetIMEFirstChar(lpszIMEName, iKeyCode);
}

// 获取控件当前输入法的Title
QString CInputManager::GetCurrIMETitle(const QWidget* p)
{
    return m_FontsManager.GetFontTitleByName(g_WidgetInfoManager.GetWidgetCurrName(p));
}

// 获取控件当前输入法的名称
QString CInputManager::GetCurrIMEName(const QWidget* p)
{
    return toQString(g_WidgetInfoManager.GetWidgetCurrName(p).c_str());
}

void CInputManager::ReloadIMEFile()
{
    m_FontsManager.ReloadIMEFile();
}

void CInputManager::SetCurrentIME(QWidget* p, QString lpszIMEName)
{
    /*if ( IME_Abc != lpszIMEName )
    {
    g_WidgetInfoManager.SetNameFormat(p, false);
    }*/

    g_WidgetInfoManager.SetCurrentIME(p, lpszIMEName.toUtf8().data());

    if (g_WidgetInfoManager.IsCurrWidget(p))
    {
        m_FontsManager.SetFontByType(lpszIMEName.toUtf8().data());
    }

    /*if ( lpszIMEName == IME_Abc )
    {
        g_WidgetInfoManager.SetNameFormat(p, true);
    }*/
}

void CInputManager::SetCurrentIMEByName(const QWidget* p, QString lpszIMEName)
{
    g_WidgetInfoManager.SetCurrentIME(p, lpszIMEName.toUtf8().data());
    m_FontsManager.SetFontByType(lpszIMEName.toUtf8().data());
}

QString CInputManager::GetAllIMEName(const QWidget* p)
{
    return toQString(g_WidgetInfoManager.GetWidgetFontsName(p).c_str());
}

void CInputManager::SetQTextEditMaxLength(const QWidget* pWidget, int imaxLen)
{
    g_WidgetInfoManager.SetQTextEditMaxLen(pWidget, imaxLen);
}

int CInputManager::GetQTextEditMaxLength(const QWidget* pWidget)
{
    return g_WidgetInfoManager.GetQTextEditMaxLen(pWidget);
}
void CInputManager::SetNameFormat(QWidget* pWidget, bool bNameFormat)
{
    g_WidgetInfoManager.SetNameFormat(pWidget, bNameFormat);
}

void CInputManager::UseKeyBoardFromIME(QWidget* pWidget, INPUT_DIALOG_STYLE enStyle /*= INPUT_DIALOG_NORMAL */, bool bIsAutoShow /*= false */)
{
    if (NULL == pWidget)
    {
        return;
    }
    if (INPUT_DLGTYPE_NONE != g_WidgetInfoManager.GetKeyBoardType(pWidget))
    {
        return;
    }
    std::string strFontName = g_WidgetInfoManager.GetWidgetCurrName(pWidget);
    if ("" != strFontName)
    {
        INPUT_DLGTYPE enKeyBoardType = GetKeyBoardTypeByFont(strFontName);

        if (INPUT_DLGTYPE_NONE == enKeyBoardType)
        {
            return;
        }
        ReleaseWidget(pWidget);
        qSetKeyBoard(pWidget, bIsAutoShow, enKeyBoardType, enStyle);
    }
}

INPUT_DLGTYPE CInputManager::GetKeyBoardTypeByFont(const std::string& strFontName)
{
    if (IME_123 == strFontName ||
            IME_123_Pwd == strFontName ||
            IME_123_Dial == strFontName ||
            IME_123_Star_Only_Dial == strFontName ||
            IME_123_IP == strFontName ||
            IME_symbol == strFontName ||
            FONT_NUMBER == strFontName)
    {
        return INPUT_DLGTYPE_NUM;
    }
    else if (IME_abc == strFontName || IME_abc_Pwd == strFontName || IME_Abc == strFontName
             ||  FONT_NORMALS == strFontName || IME_2aB == strFontName)
    {
        return INPUT_DLGTYPE_EN;
    }
    else if (IME_ABC == strFontName || IME_ABC_Pwd == strFontName || FONT_NORMAL_SHIFT == strFontName)
    {
        return INPUT_DLGTYPE_ENCAPITAL;
    }

    INPUTMETHOD_INFO("CInputManager::GetKeyBoardTypeByFont, No Default Ime Judged");
    return INPUT_DLGTYPE_EN;
}


bool CInputManager::ChangeKeyBoardStyle(const  QWidget* pWidget, INPUT_DIALOG_STYLE enStyle /*= INPUT_DIALOG_NORMAL */)
{
    return g_WidgetInfoManager.ChangeKeyBoardStyle(pWidget, enStyle);
}

const QWidget* CInputManager::GetCurrInputWgt()
{
    return g_WidgetInfoManager.GetCurrentWidget();
}



