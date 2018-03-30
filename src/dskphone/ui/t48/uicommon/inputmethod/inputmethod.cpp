#include <ylstl/ylstringalgorithm.h>

#include "inputmethod.h"
#include "widgetinfomanager.h"
#include "mainwnd/mainwnd.h"
#include "longpressinputmanager.h"
#include "language/common/common_function.h"
#include "uikernel/qpswlineedit.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/languagehelper.h"

#include "configiddefine/configiddefine.h"
#include "configparser/modconfigparser.h"
#include "commonunits/modcommonunits.h"

using namespace yl;

// 内部函数声明
#ifdef _WIN32
void imeSendCommitContent(const yl::string & strText, QWidget * wgtHost = NULL);
void imeSendPreeditContent(const yl::string & strText, QWidget * wgtHost = NULL);
#else
void imeSendCommitContent(const yl::string & strText);
void imeSendPreeditContent(const yl::string & strText);
#endif

//新增接口
void qInstallIME(QWidget* pWidget, QString qstrInputBaseName, QString _qstrFontBaseName)
{
    if (qstrInputBaseName.isNull())
    {
        return ;
    }

    if (qstrInputBaseName.isEmpty())
    {
        return ;
    }

    if (_qstrFontBaseName.isEmpty())
    {
        return ;
    }

    g_inputmanager.qSetKeyBoard(pWidget, false, INPUT_DLGTYPE_EN);
}

void qInstallKeyBoard(QWidget* p, bool bIsAutoShow, INPUT_DLGTYPE enTyep, INPUT_DIALOG_STYLE enStyle/* = INPUT_DIALOG_NORMAL*/)
{
    if (!FullKeyboardEnabled())
    {
        return;
    }
    g_inputmanager.qSetKeyBoard(p, bIsAutoShow, enTyep, enStyle);
}

void qSetKeyBoardType(QWidget* p, INPUT_DLGTYPE enTyep)
{
    g_inputmanager.qSetKeyBoardType(p, enTyep);
}

void qSetEnterType(EnterKeyType eEnterType)
{
    if (!FullKeyboardEnabled())
    {
        return;
    }
    g_inputmanager.qSetEnterType(eEnterType);
}

void qRaiseFullKeyboard()
{
    if (!FullKeyboardEnabled() || !qGetIsShowWnd())
    {
        return;
    }
    g_inputmanager.qRaiseFullKeyboard();
}

void qSetGeometry(QWidget* _pQwidget, QString _qstrInputBaseName, int _iLeft, int _iTop, int _iWidth, int _iHeight)
{
    //CInputMethod::GetInstance()->SetGeometry(_pQwidget, _qstrInputBaseName, _iLeft, _iTop, _iWidth, _iHeight);
}

// 外部调用全键盘隐藏时，不再进行延迟隐藏的处理
void qSetIsShowWnd(const QWidget* _pQwidget, QString _qstrInputBaseName, bool bIsShowWnd)
{
    if (!FullKeyboardEnabled())
    {
        return;
    }
    if (bIsShowWnd)
    {
        g_inputmanager.ShowInputDlg(_pQwidget);
    }
    else
    {
        g_inputmanager.HideInputDlg(_pQwidget);
    }
}

void qSetIsShowWnd(const QWidget* _pQwidget, bool bIsShowWnd)
{
    if (!FullKeyboardEnabled())
    {
        return;
    }

    if (_MainWND.focusWidget() != _pQwidget)
    {
        INPUTMETHOD_INFO("qSetIsShowWnd   _pQwidget is not the _MainWND.focusWidget !!!");
    }
    if (bIsShowWnd)
    {
        g_inputmanager.ShowInputDlg(_pQwidget);
    }
    else
    {
        g_inputmanager.HideInputDlg(_pQwidget);
    }
}

void qSetHideKeyboard()
{
    g_inputmanager.ForceHideInputDlg();
}

void qBlockKeyboard(bool bBlock /* = false */)
{
    g_inputmanager.BlockKeyboard(bBlock);
}

bool qGetIsShowWnd()
{
    if (!FullKeyboardEnabled())
    {
        return false;
    }
    return g_inputmanager.IsShowKeyboard();
}


//新增接口结束
void qInstallIME(QWidget* pWidget, LPCSTR lpszIMEName, const yl::string & strIMEType)
{
    if (FullKeyboardEnabled())
    {
        QString strIme = QString::fromUtf8(lpszIMEName);
        if (strIme.isNull())
        {
            return;
        }

        if (strIme.isEmpty())
        {
            return;
        }
        g_inputmanager.InstallIME(pWidget, lpszIMEName);
    }
    else
    {
        _ImeMagager.InstallIME(pWidget, lpszIMEName, strIMEType);
    }
}

// 为控件卸载输入法
void qUninstallIME(QWidget* pWidget)
{
    if (FullKeyboardEnabled())
    {
        g_inputmanager.ReleaseWidget(pWidget);
    }
    else
    {
        _ImeMagager.UninstallIME(pWidget);
    }
}

QString qGetIMEFirstChar(QString lpszIMEName, int iKeyCode)
{
    if (FullKeyboardEnabled())
    {
        return g_inputmanager.GetIMEFirstChar(lpszIMEName, iKeyCode);
    }
    else
    {
        return toQString(_ImeMagager.GetIMEFirstChar(lpszIMEName.toUtf8().data(), iKeyCode));
    }
}

QString qGetCurrentIMETitle(QWidget* pWidget)
{
    if (FullKeyboardEnabled())
    {
        return g_inputmanager.GetCurrIMETitle(pWidget);
    }
    else
    {
        return toQString(_ImeMagager.GetCurrentIMETitle(pWidget));
    }
}

QString qGetCurrentIMEName(QWidget* pWidget)
{
    if (FullKeyboardEnabled())
    {
        return g_inputmanager.GetCurrIMETitle(pWidget);
    }
    else
    {
        return toQString(_ImeMagager.GetCurrentIMEName(pWidget));
    }
}

void qSetCurrentIME(QWidget * pWidget, QString lpszIMEName, int iCusIndex/* = 0*/, const yl::string & strIMEType)
{
    if (FullKeyboardEnabled())
    {
        g_inputmanager.SetCurrentIME(pWidget, lpszIMEName);
    }
    else
    {
        _ImeMagager.SetCurrentIME(pWidget, lpszIMEName.toStdString().c_str(), iCusIndex, strIMEType);
    }
}

void qSetWidgetIMEType(QWidget * pWidget, const yl::string & strIMEType /* = "" */)
{
    _ImeMagager.SetWidgetIMEType(pWidget, strIMEType);
}

QString qShift2NextIME(QWidget* pWidget)
{
    if (FullKeyboardEnabled())
    {
        return g_inputmanager.ChangeToNextFont(pWidget);
    }
    else
    {
        return toQString(_ImeMagager.Shift2NextIME(pWidget));
    }
}

QString qGetAllIMEName(QWidget* pWidget)
{
    if (FullKeyboardEnabled())
    {
        return g_inputmanager.GetAllIMEName(pWidget);
    }
    else
    {
        return toQString(_ImeMagager.GetAllIMEName(pWidget));
    }
}


bool qSetQTextEditMaxLength(QWidget* pWidget, int maxLen)
{
    if (FullKeyboardEnabled())
    {
        g_inputmanager.SetQTextEditMaxLength(pWidget, maxLen);
        return true;
    }
    else
    {
        return _ImeMagager.setQTextEditMaxLength(pWidget, maxLen);
    }
}
int qGetQTextEditMaxLength(QWidget* pWidget)
{
    if (FullKeyboardEnabled())
    {
        return g_inputmanager.GetQTextEditMaxLength(pWidget);
    }
    else
    {
        return _ImeMagager.getQTextEditMaxLength(pWidget);
    }
}

bool qPassEventToEdit(QObject* pObj, QEvent *pEvent)
{
    if (FullKeyboardEnabled())
    {
        bool v_bResult = false;

        v_bResult = g_inputmanager.eventFilter(pObj, pEvent);

        return v_bResult;
    }
    else
    {
        return _ImeMagager.passEventToEdit(pObj, pEvent);
    }
}

bool pImeHighlighting(imeItem & item)
{
    return item.m_nTimerID != -1;
}

// 点击空白键调用的操作
// pWidget是控件对应的窗体
void qOnBackspaceClicked(QWidget* pWidget)
{
    if (FullKeyboardEnabled())
    {
        g_inputmanager.DealBackspace(pWidget);
    }
    else
    {
        if (NULL == pWidget)
        {
            return;
        }

        // 获得当前的焦点控件
        QWidget * pFocusWidget = pWidget->focusWidget();

        if (NULL == pFocusWidget || !(pFocusWidget->inherits("QLineEdit")
                                      || pFocusWidget->inherits("QTextEdit")))
        {
            return;
        }

        imeItem * pItem = _ImeMagager.findImeItem(pFocusWidget);

        // 判断IME是否处于编辑状态
        if (pItem != NULL && pImeHighlighting(*pItem))
        {
            // 暂停编辑状态
            pItem->stopTimer();
            pItem->m_nTimerID = -1;

            // 清空编辑内容
#ifdef _WIN32
            ::imeSendCommitContent("", pFocusWidget);
#else
            ::imeSendCommitContent("");
#endif

            pItem->m_strPreeditString = "";
            pItem->m_nRepeatCount = 0;
        }
        else
        {
            // 不处于编辑状态
            // 根据控件的类型删除已经输入的一个字符
            QLineEdit * pLineEdit = qobject_cast<QLineEdit *>(pFocusWidget);
            if (pLineEdit != NULL)
            {
                pLineEdit->backspace();
                //删除编辑字符过程中判断剩余字串中是否还有希伯来语，进而判断靠左还是靠右显示
                QString  strText = pLineEdit->text();
                yl::wstring strW = yl::to_utf16(strText.toUtf8().data());
                if (Lang_IsRTL(strW))
                {
                    if (pLineEdit->alignment() == Qt::AlignLeft)
                    {
                        pLineEdit->setAlignment(Qt::AlignRight);
                    }
                }
                else
                {
                    if (pLineEdit->alignment() == Qt::AlignRight)
                    {
                        pLineEdit->setAlignment(Qt::AlignLeft);
                    }
                }
            }
            else
            {
                QTextEdit * pTextEdit = qobject_cast<QTextEdit *>(pFocusWidget);
                if (pTextEdit != NULL)
                {
                    QTextCursor cursor = pTextEdit->textCursor();
                    cursor.deletePreviousChar();

                    QString  strText = pTextEdit->toPlainText();
                    yl::wstring strW = yl::to_utf16(strText.toUtf8().data());
                    if (Lang_IsRTL(strW))
                    {
                        if (pTextEdit->alignment() == Qt::AlignLeft)
                        {
                            pTextEdit->setAlignment(Qt::AlignRight);
                        }
                    }
                    else
                    {
                        if (pTextEdit->alignment() == Qt::AlignRight)
                        {
                            pTextEdit->setAlignment(Qt::AlignLeft);
                        }
                    }
                }
            }
        }
    }
}

// 将预编辑的字符串输入控件，并返回控件上的内容
QString qReturnEdit(QWidget* pWidget)
{
    if (FullKeyboardEnabled())
    {
        //这边是马上让字符输出 一定要
        return g_inputmanager.qReturnEdit(pWidget);
    }
    else
    {
        // 找到响应的IME item
        imeItem * pItem = _ImeMagager.findImeItem(pWidget);

        // 判断是否处于编辑状态
        if (pItem != NULL && pImeHighlighting(*pItem))
        {
            // 将编辑中的内容输入控件
            pItem->commitInput();
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
        }

        // 根据控件的类型返回控件的text
        // 注意qPswLineEdit为LineEdit的子类,所以需要在其前面
        qPswLineEdit * pPswLineEdit = qobject_cast<qPswLineEdit *>(pWidget);
        if (pPswLineEdit != NULL)
        {
            return pPswLineEdit->text();
        }

        QLineEdit * pLineEdit = qobject_cast<QLineEdit *>(pWidget);
        if (pLineEdit != NULL)
        {
            return pLineEdit->text();
        }

        QTextEdit * pTextEdit = qobject_cast<QTextEdit *>(pWidget);
        if (pTextEdit != NULL)
        {
            return pTextEdit->toPlainText();
        }


        return NULL;
    }
}

// 输入预编辑内容
// 返回true表示处于预编辑状态
// 返回false表示不处于预编辑状态
bool InputPreeditContent(QWidget* pWidget)
{
    if (FullKeyboardEnabled())
    {
        return g_inputmanager.InputPreeditContent(pWidget);
    }
    else
    {
        bool bPreedit = false;

        // 找到widget对应的IME item
        imeItem * pItem = _ImeMagager.findImeItem(pWidget);

        // 判断是否处于编辑状态
        if (pItem != NULL && pImeHighlighting(*pItem))
        {
            // 将编辑中的内容输入控件
            pItem->commitInput();
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);

            bPreedit = true;
        }

        return bPreedit;
    }
}

// 判断控件是否处于预编辑状态
bool qIsPreedit(QWidget* pWidget)
{
    if (FullKeyboardEnabled())
    {
        return g_inputmanager.IsPreedit(pWidget);
    }
    else
    {
        // 找到widget对应的IME item
        imeItem * pItem = _ImeMagager.findImeItem(pWidget);

        // 判断是否处于编辑状态
        if (pItem != NULL && pImeHighlighting(*pItem))
        {
            return true;
        }

        return false;
    }
}

//重新从文件 加载输入法
void qReloadIME()
{
    if (FullKeyboardEnabled())
    {
        Lang_ReLoadLang();
        CLanguageHelper::GetInstance().InitializeLangHelper();
        g_inputmanager.ReloadIMEFile();
    }
    else
    {
        _ImeMagager.PreloadIME();
        Lang_ReLoadLang();
        CLanguageHelper::GetInstance().InitializeLangHelper();
        _ImeMagager.ReloadIME();
    }
}

void qSetNameFormat(QWidget* pWidget, bool bNameFormat)
{
    if (FullKeyboardEnabled())
    {
        g_inputmanager.SetNameFormat(pWidget, bNameFormat);
    }
    else
    {
        _ImeMagager.SetNameFormat(pWidget, bNameFormat);
    }
}
static bool s_bEnableFullKeyboard = 0;
bool FullKeyboardEnabled()
{
    return s_bEnableFullKeyboard;
}

void FullKeyboard_Init()
{
    s_bEnableFullKeyboard = (1 == configParser_GetConfigInt(kszPhoneVirtualKeyboardEnable));
}

void InputMethod_Init()
{
    if (s_bEnableFullKeyboard)
    {
        g_inputmanager.Init();
    }
    else
    {
        _ImeMagager;
    }
}

void SetIsPlayKeyboardAnimation(bool bPlay)
{
    g_inputmanager.SetIsPlayKeyboardAnimation(bPlay);
}

void qUseKeyBoardFromIME(QWidget* pWidget, INPUT_DIALOG_STYLE enStyle /*= INPUT_DIALOG_NORMAL */, bool bIsAutoShow /*= false */)
{
    if (!FullKeyboardEnabled())
    {
        return;
    }
    g_inputmanager.UseKeyBoardFromIME(pWidget, enStyle, bIsAutoShow);
}

bool qChangeKeyBoardStyle(const  QWidget* pWidget, INPUT_DIALOG_STYLE enStyle /*= INPUT_DIALOG_NORMAL */)
{
    return g_inputmanager.ChangeKeyBoardStyle(pWidget, enStyle);
}

const QWidget* GetCurrInputWgt()
{
    return g_inputmanager.GetCurrInputWgt();
}

void qStartLongPressInput(QObject* pObj, int iSrcKeyCode, int iDestKeyCode, int iLongPressTime)
{
    g_LongPressInputManager.startLongPress(pObj, iSrcKeyCode, iDestKeyCode, iLongPressTime);
}

void qStopLongPressInput(int iKeyCode)
{
    g_LongPressInputManager.stopLongPress(iKeyCode);
}

bool qIsLongInputWork()
{
    return g_LongPressInputManager.IsOnWork();
}

bool inputMethod_RegisterKeyBoardShowHookProc(KEYBOARD_SHOW_HOOK_PROC keyboardShowHook, KEYBOARD_SHOW_HOOK_MODULE priority)
{
    return g_inputmanager.RegisterKeyBoardShowHookProc(keyboardShowHook, priority);
}

bool inputMethod_UnRegisterKeyBoardShowHookProc(KEYBOARD_SHOW_HOOK_PROC keyboardShowHook)
{
    return g_inputmanager.UnRegisterKeyBoardShowHookProc(keyboardShowHook);
}


//////////////////////////////////////////////////////////////////////////



yl::string GetKeyResult(const yl::wstring & strKeySequence, int nRepeatCount)
{
    // 判断自身的序列是否为空
    if (!strKeySequence.empty())
    {
        // 将nRepeatCount转化为对应Index
        // 可以使递增的nRepeatCount循环的读取字符串序列中的每一个字符
        int iIndex = (nRepeatCount - 1) % strKeySequence.length();

        // 根据Index返回对应的字符
        WCHAR szText[2] = { 0 };
        szText[0] = strKeySequence[iIndex];
        szText[1] = 0;
        return to_utf8(szText);
    }

    return NULL;
}


/************************************************************************/
/*                        about   class  qKeySequence                   */
/************************************************************************/
imeItem::imeItem()
{
    // 指针赋空
    m_pHostWidget = NULL;
    m_pCurrentKeyMap = NULL;

    // 初始化点击次数
    m_nRepeatCount = 0;

    // 初始化计时器ID
    m_nTimerID = -1;

    m_nQTextEditMaxLength = DEFAULT_QTEXTEDIT_MAXLENGTH;          //默认QTextEdit最大长度

    //默认为非名称格式
    m_bNameFormat = false;

    m_pImeSet = NULL;
    m_strIMEType = "";
}

void imeItem::AddKeymaps(mapKeys * pKeyMaps)
{
    // 入口参数检查，指针判空处理
    if (NULL == pKeyMaps)
    {
        return;
    }

    // 添加到输入法链表中
    m_listValidKeyMaps.push_back(pKeyMaps);

    // 判断当前是否有输入法
    if (m_pCurrentKeyMap == NULL)
    {
        // 设置当前的输入法
        SetCurrentkeymaps(pKeyMaps);
    }
}

void imeItem::SetCurrentkeymaps(mapKeys * pKeyMaps)
{
    // 入口参数检查，指针判空处理
    if (NULL != pKeyMaps)
    {
        m_pCurrentKeyMap = pKeyMaps;
    }
    if (_ImeMagager.GetCurrentIMEName(m_pHostWidget) == IME_Abc)
    {
        SetNameFormat(true);
    }
    else
    {
        SetNameFormat(false);
    }
}

void imeItem::SetNameFormat(bool bNameFormat)
{
    if (bNameFormat == m_bNameFormat)
    {
        return;
    }

    m_bNameFormat = bNameFormat;

    if (m_pHostWidget != NULL)
    {
        QTextEdit * pTextEdit = qobject_cast<QTextEdit *>(m_pHostWidget);
        if (pTextEdit)
        {
            if (bNameFormat)
            {
                //QObject::connect(pTextEdit, SIGNAL(textChanged()), this, SLOT(AutoUpper()));
            }
            else
            {
                //disconnect(pTextEdit, SIGNAL(textChanged()), this, SLOT(AutoUpper()));
            }
        }
        else
        {
            QLineEdit * pLineEdit = qobject_cast<QLineEdit *>(m_pHostWidget);
            if (pLineEdit)
            {
                if (bNameFormat)
                {
                    //connect(pLineEdit, SIGNAL(textEdited(const QString &)), this, SLOT(AutoUpper(const QString &)));
                }
                else
                {
                    //disconnect(pLineEdit, SIGNAL(textEdited(const QString &)), this, SLOT(AutoUpper(const QString &)));
                }
            }
        }
    }
}

void imeItem::AutoUpper()
{
    QTextEdit * pTextEdit = qobject_cast<QTextEdit *>(m_pHostWidget);
    if (pTextEdit)
    {
        QString strText = pTextEdit->toPlainText();
        QTextCursor cursor = pTextEdit->textCursor();//先记下当前位置
        pTextEdit->setText(::ToNameFormat(strText));
        pTextEdit->setTextCursor(cursor);//防止删除单词首字母后光标跳至最后
    }
}

void imeItem::AutoUpper(const QString & strText)
{
    QLineEdit * pLineEdit = qobject_cast<QLineEdit *>(m_pHostWidget);
    if (pLineEdit)
    {
        int iPos = pLineEdit->cursorPosition();//先记下当前位置
        pLineEdit->setText(::ToNameFormat(strText));
        pLineEdit->setCursorPosition(iPos);//防止删除单词首字母后光标跳至最后
    }
}

void imeItem::CheckTextDirection()
{
    QTextEdit * pTextEdit = qobject_cast<QTextEdit *>(m_pHostWidget);
    if (pTextEdit)
    {
        QString strText = pTextEdit->toPlainText();
        //输入有希伯来语且现在排列方式是从左往右，则改为从右向左
        //Qt已支持希伯来语等从右往左显示的语言，对于字符串显示不用再做其他操作
        if (Lang_IsRTL(fromQString(strText)))
        {
            if (pTextEdit->alignment() == Qt::AlignLeft)
            {
                pTextEdit->setAlignment(Qt::AlignRight);
            }
        }
        else
        {
            if (pTextEdit->alignment() == Qt::AlignRight)
            {
                pTextEdit->setAlignment(Qt::AlignLeft);
            }
        }
    }
}

void imeItem::CheckTextDirection(const QString & strText)
{
    QLineEdit * pLineEdit = qobject_cast<QLineEdit *>(m_pHostWidget);
    if (pLineEdit)
    {
        if (Lang_IsRTL(fromQString(strText)))
        {
            pLineEdit->setAlignment(Qt::AlignRight);
        }
        else
        {
            if (pLineEdit->alignment() == Qt::AlignRight)
            {
                pLineEdit->setAlignment(Qt::AlignLeft);
            }
        }
    }
}

void imeItem::SetHostWidget(QWidget * pWidget)
{
    if (NULL == pWidget)
    {
        return;
    }

    m_pHostWidget = pWidget;

    //连接检查文字方向的槽函数（目前用于希伯来语）
    if (NULL != pWidget)
    {
        QLineEdit * pLineEdit = qobject_cast<QLineEdit *>(pWidget);
        if (pLineEdit)
        {
            disconnect(pLineEdit, SIGNAL(textChanged(const QString &)), this,
                       SLOT(CheckTextDirection(const QString &)));
            connect(pLineEdit, SIGNAL(textChanged(const QString &)), this,
                    SLOT(CheckTextDirection(const QString &)), Qt::UniqueConnection);
        }
        else
        {
            QTextEdit * pTextEdit = qobject_cast<QTextEdit *>(pWidget);
            if (pTextEdit)
            {
                disconnect(pTextEdit, SIGNAL(textChanged()), this, SLOT(CheckTextDirection()));
                connect(pTextEdit, SIGNAL(textChanged()), this, SLOT(CheckTextDirection()), Qt::UniqueConnection);
            }
        }
    }
}

mapKeys * imeItem::GetNextKeymaps()
{
    if (m_listValidKeyMaps.size() == 0)
    {
        return NULL;
    }

    // 寻找当前输入法
    listKeyMaps::iterator it = m_listValidKeyMaps.begin();
    for (; it != m_listValidKeyMaps.end(); ++it)
    {
        if (*it == m_pCurrentKeyMap)
        {
            break;
        }
    }


    // 判断当前输入法迭代器的正确性
    chASSERT(it != m_listValidKeyMaps.end());
    ++it;

    // 若当前输入法是最后一个输入法，切换到第一个输入法
    return it == m_listValidKeyMaps.end() ? m_listValidKeyMaps.front() : *it;
}

bool imeItem::eventFilter(QObject * pObj, QEvent * pEvent)
{
    // 先判断消息是否是自身所对应的控件
    if (pObj == m_pHostWidget)
    {
        // 处理点击事件
        if (pEvent->type() == QEvent::KeyPress)
        {
            return OnKeyPress(pObj, pEvent);
        }

        // 处理FocusOut事件
#ifndef _WIN32
        if (pEvent->type() == QEvent::FocusOut)
        {
            // 判断是否处在预编辑中
            if (!m_strPreeditString.empty())
            {
                // 判断事件对象的类型
                QLineEdit * pLineEdit = qobject_cast<QLineEdit *>(pObj);
                if (pLineEdit != NULL)
                {
                    // 输入在预编辑中的文字
                    pLineEdit->insert(toQString(m_strPreeditString.c_str()));
                }
                else
                {
                    QTextEdit * pTextEdit = qobject_cast<QTextEdit *>(pObj);
                    if (pTextEdit != NULL)
                    {
                        // 输入在预编辑中的文字
                        pTextEdit->insertPlainText(toQString(m_strPreeditString.c_str()));
                    }
                }
            }

            // 消除预编辑相关的状态
            m_strPreeditString = "";
            m_nRepeatCount = 0;
            stopTimer();
        }
        else if (pEvent->type() == QEvent::Destroy)     //控件摧毁  自动卸载输入法
        {
            qUninstallIME(m_pHostWidget);
        }
#else
        if (pEvent->type() == QEvent::MouseButtonPress)
        {
            if (m_nTimerID >= 0)
            {
                return true;
            }
        }
        if (pEvent->type() == QEvent::FocusOut
                && _MainWND.isActiveWindow())
        {
            // 判断是否处在预编辑中
            if (!m_strPreeditString.empty())
            {
                INPUTMETHOD_INFO("InputMethod FocusOut Commit Repeat Cnt [%d]", m_nRepeatCount);
                commitInput();
            }
            stopTimer();
        }
        else if (pEvent->type() == QEvent::Destroy)     //控件摧毁  自动卸载输入法
        {
            qUninstallIME(m_pHostWidget);
        }
#endif
    }

    return false;
}

void imeItem::timerEvent(QTimerEvent * pEvent)
{
    if (-1 != m_nTimerID
            && m_nTimerID == pEvent->timerId())
    {
        // 输入预编辑状态的字符
        commitInput();

        // 停止计时器
        stopTimer();
    }
}

bool imeItem::OnKeyPress(QObject * pObj, QEvent * pEvent)
{
    // 函数只处理按键点击事件，强转事件类型
    QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
    int nKeyCode = pKeyEvent->key();

    int iIndex = CImeLogic::Key2Index(nKeyCode);

    // 判断当前输入法中是否有对应的按键
    if (iIndex < 0 || iIndex >= KEY_NUM)
    {
        // 判断是否处于预编辑状态
        if (!m_strPreeditString.empty())
        {
            // 输入预编辑状态的字符
            commitInput();

            //按键是方向键，再次丢出按键按下事件，本函数不处理，避免输入字符跟随光标移动
            if (nKeyCode == PHONE_KEY_LEFT || nKeyCode == PHONE_KEY_RIGHT
                    || nKeyCode == PHONE_KEY_UP || nKeyCode == PHONE_KEY_DOWN)
            {
                //处理所有其他事件，主要避免还有IME事件未处理!!!!
                QApplication::processEvents(QEventLoop::ExcludeUserInputEvents |
                                            QEventLoop::ExcludeSocketNotifiers);
            }

            return true;
        }

        // 判断对应的控件何种类型
        QLineEdit * pLineEdit = qobject_cast<QLineEdit *>(pObj);
        if (pLineEdit != NULL)
        {
            // 输入按键是方向键
            if (nKeyCode == PHONE_KEY_LEFT || nKeyCode == PHONE_KEY_RIGHT)
            {
                // 重设光标位置
                // 获得当前的光标位置
                int pos = pLineEdit->cursorPosition();

                // 根据按键对光标位置进行修正
                if (nKeyCode == PHONE_KEY_LEFT)
                {
                    pos--;
                }
                else if (nKeyCode == PHONE_KEY_RIGHT)
                {
                    pos++;
                }

                // 重设光标位置
                pLineEdit->setCursorPosition(pos);
                return true;
            }
        }
        else
        {

            QTextEdit * pTextEdit = qobject_cast<QTextEdit *>(pObj);
            if (pTextEdit != NULL)
            {
                // 输入按键是方向键
                if (nKeyCode == PHONE_KEY_LEFT || nKeyCode == PHONE_KEY_RIGHT
                        || nKeyCode == PHONE_KEY_UP || nKeyCode == PHONE_KEY_DOWN)
                {

                    // 重设光标位置
                    // 获得当前的光标位置
                    QTextCursor cursor = pTextEdit->textCursor();

                    // 根据按键对光标位置进行修正
                    if (nKeyCode == PHONE_KEY_LEFT)
                    {
                        cursor.movePosition(QTextCursor::Left);
                    }
                    else if (nKeyCode == PHONE_KEY_RIGHT)
                    {
                        cursor.movePosition(QTextCursor::Right);
                    }
                    else if (nKeyCode == PHONE_KEY_UP)
                    {
                        cursor.movePosition(QTextCursor::Up);
                    }
                    else if (nKeyCode == PHONE_KEY_DOWN)
                    {
                        cursor.movePosition(QTextCursor::Down);
                    }

                    // 重设光标位置
                    pTextEdit->setTextCursor(cursor);
                    return true;
                }
            }
        }

        return false;
    }

    if (m_pCurrentKeyMap == NULL)
    {
        return false;
    }

    yl::wstring & strKeySequence = m_pCurrentKeyMap->keyMap[iIndex];

    //此处增加对希伯来语的支持
    //若输入有希伯来语
    QLineEdit * pLineEdit = qobject_cast<QLineEdit *>(pObj);
    if (pLineEdit != NULL)
    {
        QString  strText = pLineEdit->text();
        yl::wstring strW = yl::to_utf16(strText.toUtf8().data());
        if (Lang_IsRTL(strW) || Lang_IsRTL(strKeySequence))
        {
            //输入有希伯来语且现在排列方式是从左往右，则改为从右向左
            //Qt已支持希伯来语等从右往左显示的语言，对于字符串显示不用再做其他操作
            if (pLineEdit->alignment() == Qt::AlignLeft)
            {
                pLineEdit->setAlignment(Qt::AlignRight);
            }
        }
        else
        {
            if (pLineEdit->alignment() == Qt::AlignRight)
            {
                pLineEdit->setAlignment(Qt::AlignLeft);
            }
        }
    }
    else
    {
        QTextEdit * pTextEdit = qobject_cast<QTextEdit *>(pObj);
        if (pTextEdit != NULL)
        {
            QString  strText = pTextEdit->toPlainText();
            yl::wstring strW = yl::to_utf16(strText.toUtf8().data());
            if (Lang_IsRTL(strW) || Lang_IsRTL(strKeySequence))
            {
                if (pTextEdit->alignment() == Qt::AlignLeft)
                {
                    pTextEdit->setAlignment(Qt::AlignRight);
                }
            }
            else
            {
                if (pTextEdit->alignment() == Qt::AlignRight)
                {
                    pTextEdit->setAlignment(Qt::AlignLeft);
                }
            }
        }
    }

    // 判断是否处于未编辑状态，或者输入的按键和上次的不同
    if (m_nRepeatCount == 0
            || m_nLastKeyCode != nKeyCode)
    {
        // 结束当前输入状态
        commitInput();

        // 开始新一轮的输入
        m_nLastKeyCode = nKeyCode;
        m_nRepeatCount = 1;
    }
    else
    {
        // 继续编辑
        m_nRepeatCount++;

    }

    // 字符序列为空，不进行下一步处理
    if (strKeySequence.length() == 0)
    {
        return true;
    }

    if (strKeySequence.length() == 1)
    {
        // 字符序列只有一个字符的情况
        // 直接输入到编辑框上，不进入预编辑状态
        sendCommitToEdit(to_utf8(strKeySequence));
        m_nRepeatCount = 0;
    }
    else
    {
        // 字符序列多有个字符的情况
        // 取得相应的字符串输入到输入框
        m_strPreeditString = GetKeyResult(strKeySequence, m_nRepeatCount);
        if (sendPreeditToEdit(m_strPreeditString))
        {
            // 启动代表预编辑时间计时器
            // 时间到则表示编辑结束
            startTimer();
        }
        else
        {
            // 编辑失败,设置预编辑字符串为空
            m_strPreeditString = "";
        }
    }

    return true;
}

void imeItem::ToAbcFormat(yl::string & strPreedit)
{
    if (!m_bNameFormat)
    {
        return;
    }

    QString strLast;
    QString qstrPreedit = toQString(strPreedit);
    QLineEdit * pLineEdit = qobject_cast<QLineEdit *>(m_pHostWidget);
    if (pLineEdit != NULL)
    {
        int iPos = pLineEdit->cursorPosition();//先记下当前位置

        strLast = pLineEdit->text();

        if (iPos == 0 || strLast.at(iPos - 1) == ' ')
        {
            strPreedit = fromQString(qstrPreedit.toUpper());
        }
    }
    else
    {
        QTextEdit * pTextEdit = qobject_cast<QTextEdit *>(m_pHostWidget);
        if (pTextEdit != NULL)
        {
            QTextCursor cursor = pTextEdit->textCursor();//先记下当前位置

            strLast = pTextEdit->toPlainText();

            if (cursor.position() == 0 || strLast.at(cursor.position() - 1) == ' ')
            {
                strPreedit = fromQString(qstrPreedit.toUpper());
            }
        }
    }
}

bool imeItem::commitInput()
{
    // 判断是否存在预编辑字符
    if (!m_strPreeditString.empty())
    {
        ToAbcFormat(m_strPreeditString);

        // 将预编辑字符输出
#ifdef _WIN32
        imeSendCommitContent(m_strPreeditString, m_pHostWidget);
#else
        imeSendCommitContent(m_strPreeditString);
#endif

        // 还原为初始状态
        m_strPreeditString = "";
        m_nRepeatCount = 0;
        return true;
    }

    return false;
}

void imeItem::startTimer()
{
    // 先停止计时器
    stopTimer();

    // 开启计时并记录ID
    m_nTimerID = QObject::startTimer(800);
}

void imeItem::stopTimer()
{
    // 判断计时器是否激活
    if (m_nTimerID != -1)
    {
        QObject::killTimer(m_nTimerID);
        m_nTimerID = -1;
    }
}

BOOL imeItem::isTextOverflow() const
{
    QLineEdit * pLineEdit = qobject_cast<QLineEdit *>(m_pHostWidget);

    // 判断所对应控件的类型
    if (pLineEdit != NULL)
    {
        // 判断是否处于编辑状态
        if (this->m_nTimerID != -1 && m_nRepeatCount == 1)
        {
            // 编辑状态需要将当期内容的size再加1
            return pLineEdit->text().size() + 1 >= pLineEdit->maxLength();
        }
        else
        {
            return pLineEdit->text().size() >= pLineEdit->maxLength();
        }
    }
    else
    {
        QTextEdit * pTextEdit = qobject_cast<QTextEdit *>(m_pHostWidget);
        if (pTextEdit != NULL)
        {
            // 判断是否处于编辑状态
            if (this->m_nTimerID != -1 && m_nRepeatCount == 1)
            {
                // 编辑状态需要将当期内容的size再加1
                return pTextEdit->toPlainText().size() + 1 >= m_nQTextEditMaxLength;
            }
            else
            {
                return pTextEdit->toPlainText().size() >= m_nQTextEditMaxLength;
            }
        }
    }

    return FALSE;
}

BOOL imeItem::sendCommitToEdit(const yl::string & strText) const
{
    // 判断控件是否溢出
    if (isTextOverflow())
    {
        return FALSE;
    }

    // 输出内容到编辑框
#ifdef _WIN32
    ::imeSendCommitContent(strText, m_pHostWidget);
#else
    ::imeSendCommitContent(strText);
#endif

    return TRUE;
}

BOOL imeItem::sendPreeditToEdit(const yl::string & strText) const
{
    // 判断控件是否溢出
    if (isTextOverflow())
    {
        return FALSE;
    }

    // 输出预编辑内容到编辑框
#ifdef _WIN32
    ::imeSendPreeditContent(strText, m_pHostWidget);
#else
    ::imeSendPreeditContent(strText);
#endif

    return TRUE;
}

WidgetImeAttr imeItem::GetAttr()
{
    WidgetImeAttr eAttrItem;
    eAttrItem.pWidget = m_pHostWidget;
    eAttrItem.lpSet = m_pImeSet;
    eAttrItem.strCurrentImeName = m_pCurrentKeyMap->strName;
    return eAttrItem;
}


/************************************************************************/
/*                        about  IME  Manager                           */
/************************************************************************/
void IMEManager::InstallIME(QWidget * pWidget, LPCSTR lpszIMEName, const yl::string & strIMEType)
{
    // 入口参数检查
    if (NULL == pWidget
            || NULL == lpszIMEName)
    {
        INPUTMETHOD_INFO("InstallIME error");
        return;
    }

    // 取得对应的imeItem
    imeItem & objItem = getIMEItem(pWidget);
    objItem.SetIMEType(strIMEType);

    // 当做过滤波器安装到控件上
    pWidget->installEventFilter(&objItem);
    // 防止修改源指针
    LPCSTR lpImeSet = lpszIMEName;
    // 添加输入法
    yl::string strIMEName;
    //没有函数对应
    while (PeekPhraseByCharTokens(lpImeSet, strIMEName, ",") != -1)
    {
        LPCSTR lpTargetImeName = strIMEName.c_str();

        // 自定义输入法特殊处理， 在标志位置插入输入法
        if (strcmp(lpTargetImeName, IME_DIY) == 0)
        {
            int iCustomImeCount = _ImeLogic.GetCustomImeCount();

            for (int i = 0; i < iCustomImeCount; ++i)
            {
                LPCSTR lpCusImeName = _ImeLogic.Ime2Name(IT_STD_Custom, i);

                mapKeys * pMapKeys = _ImeLogic.GetKeyMap(lpCusImeName);
                if (pMapKeys != NULL)
                {
                    objItem.AddKeymaps(pMapKeys);
                }
            }

            continue;
        }

        // 通过输入法的名字寻找输入法
        mapKeys * pMapKeys = _ImeLogic.GetKeyMap(lpTargetImeName);
        if (pMapKeys != NULL)
        {
            objItem.AddKeymaps(pMapKeys);
        }
        else
        {
            INPUTMETHOD_INFO("pMapKeys name=[%s] is Null!", lpTargetImeName);
        }
    }

    if (objItem.GetImeCount() > 1)
    {
        // 如果安装的不是单个输入法的话，记下该序列，当改变语言重新加载时还原该输入法
        objItem.SetImeSet(lpszIMEName);
    }

    IF_MODE_LOG_TRACE("install pWidget=[%s] set=[%s]", pWidget->metaObject()->className(), lpszIMEName);
}

void IMEManager::SetNameFormat(QWidget * pWidget, bool bNameFormat)
{
    // 入口参数检查
    if (NULL == pWidget
            || !(pWidget->inherits("QLineEdit") || pWidget->inherits("QTextEdit")))
    {
        INPUTMETHOD_INFO("SetNameFormat error");
        return;
    }

    // 取得对应的imeItem
    imeItem * pItem = findImeItem(pWidget);
    if (pItem == NULL)
    {
        return;
    }

    pItem->SetNameFormat(bNameFormat);
}

void IMEManager::UninstallIME(QWidget * pWidget)
{
    // 入口参数判断，指针判空处理
    if (NULL == pWidget)
    {
        return;
    }

    // 遍历链表
    listItems::iterator it = listItems::begin();
    while (it != listItems::end())
    {
        imeItem * pItem = *it;
        if (pItem != NULL)
        {
            // 找到控件对应的输入法管理类，删除
            if (pItem->m_pHostWidget == pWidget)
            {
                if (pItem->IsNameFormat())
                {
                    pItem->SetNameFormat(false);
                }
                pItem->stopTimer();
                pWidget->removeEventFilter(pItem);
                pItem->disconnect(pItem, SLOT(CheckTextDirection(const QString &)));
                pItem->disconnect(pItem, SLOT(CheckTextDirection()));
                it = listItems::erase(it);
                delete pItem;
                pItem = NULL;
                continue;
            }
        }
        it++;
    }

    IF_MODE_LOG_TRACE("UninstallIME pWidget=[%s]", pWidget->metaObject()->className());
}

yl::string IMEManager::GetCurrentIMETitle(QWidget * pWidget)
{
    // 入口参数检测
    if (NULL == pWidget)
    {
        return NULL;
    }

    imeItem * pItem = findImeItem(pWidget);
    return pItem != NULL ? keyMaps2Title(pItem->m_pCurrentKeyMap) : NULL;
}

// 获得当前输入法的名称
yl::string IMEManager::GetCurrentIMEName(QWidget * pWidget)
{
    // 入口参数检测
    if (NULL == pWidget)
    {
        return NULL;
    }

    imeItem * pItem = findImeItem(pWidget);
    return pItem != NULL && pItem->m_pCurrentKeyMap ? pItem->m_pCurrentKeyMap->strName : yl::string();
}

// 获取控件当前输入法,对应按键的第一个字符
yl::string IMEManager::GetIMEFirstChar(LPCSTR lpszIMEName, int iKeyCode)
{
    // 入口参数检测
    if (lpszIMEName == NULL)
    {
        return yl::string();
    }

    // 获取当前的输入法
    mapKeys * pMapKey = _ImeLogic.GetKeyMap(lpszIMEName);

    if (NULL != pMapKey)
    {
        int iIndex = CImeLogic::Key2Index(iKeyCode);
        if (iIndex >= 0 && iIndex < KEY_NUM)
        {
            // 获得按键对应的字符序列
            yl::wstring & strKeySequence = pMapKey->keyMap[iIndex];

            // 字符序列为空，不处理
            if (strKeySequence.length() > 0)
            {
                // 取第一个字符
                return GetKeyResult(strKeySequence, 1);
            }
        }
    }

    return yl::string();
}

void IMEManager::SetCurrentIME(QWidget * pWidget, LPCSTR lpszIMEName, int iCusIndex, const yl::string & strIMEType)
{
    // 入口参数判断
    if (NULL == pWidget || NULL == lpszIMEName)
    {
        return;
    }

    IF_MODE_LOG_TRACE("ime=[%s] index=[%d]", lpszIMEName, iCusIndex);

    if (strcmp(lpszIMEName, IME_DIY) == 0)
    {
        lpszIMEName = _ImeLogic.Ime2Name(IT_STD_Custom, iCusIndex);
    }

    // 寻找控件对应的输入法管理类
    imeItem * pItem = findImeItem(pWidget);
    if (pItem != NULL && IsIMEExist(lpszIMEName, pItem))
    {
        // 通过输入法的名字寻找输入法
        mapKeys * pMapKeys = _ImeLogic.GetKeyMap(lpszIMEName);
        if (NULL != pMapKeys)
        {
            // 设置当前输入法为寻找到的输入法
            pItem->SetCurrentkeymaps(pMapKeys);
        }
        pItem->SetIMEType(strIMEType);
    }
}

void IMEManager::SetWidgetIMEType(QWidget * pWidget, const yl::string strIMEType /* = "" */)
{
    // 入口参数判断
    if (NULL == pWidget)
    {
        return;
    }

    // 寻找控件对应的输入法管理类
    imeItem * pItem = findImeItem(pWidget);
    if (pItem != NULL)
    {
        pItem->SetIMEType(strIMEType);
    }
}

yl::string IMEManager::Shift2NextIME(QWidget * pWidget)
{
    // 入口参数检测
    if (NULL == pWidget)
    {
        return NULL;
    }

    // 寻找控件对应的输入法管理类
    imeItem * pItem = findImeItem(pWidget);
    if (pItem == NULL)
    {
        return NULL;
    }

    // 获得当前输入法的下一种输入法
    mapKeys * pMapKeys = pItem->GetNextKeymaps();
    if (NULL != pMapKeys)
    {
        pItem->SetCurrentkeymaps(pMapKeys);

        commonUnits_SetIMEType(pItem->m_strIMEType, keyMaps2Title(pMapKeys));
        return keyMaps2Title(pMapKeys);
    }
    else
    {
        return NULL;
    }
}

bool IMEManager::setQTextEditMaxLength(QWidget * pWidget, int maxLen)
{
    // 寻找输入法对应的管理类
    imeItem * pItem = findImeItem(pWidget);

    // 指针判空处理
    if (pItem == NULL)
    {
        return FALSE;
    }

    pItem->m_nQTextEditMaxLength = maxLen;
    return TRUE;
}

int IMEManager::getQTextEditMaxLength(QWidget * pWidget)
{
    // 寻找输入法对应的管理类
    imeItem * pItem = findImeItem(pWidget);

    // 指针判空处理
    if (pItem == NULL)
    {
        return -1;
    }

    return pItem->m_nQTextEditMaxLength;
}

bool IMEManager::passEventToEdit(QObject * pObj, QEvent * pEvent)
{
    // 入口参数检查
    if (NULL == pObj || NULL == pEvent)
    {
        return false;
    }

    QWidget * pWidget = qobject_cast<QWidget *>(pObj);
    if (NULL == pWidget)
    {
        return false;
    }

    // 寻找输入法对应的管理类
    imeItem * pItem = findImeItem(pWidget);
    if (NULL == pItem)
    {
        return false;
    }

    // 将事件发送给过滤器处理
    return pItem->eventFilter(pObj, pEvent);
}

yl::string IMEManager::keyMaps2Title(mapKeys * pKeyMaps)
{
    if (pKeyMaps == NULL)
    {
        return yl::string();
    }

    yl::string strIME = pKeyMaps->strName;

    // 若strIME中有包含"123"则名字设置为"123"
    // IME_123_Dial、IME_123_IP这两种输入法也显示为"123"
    if (strIME.substr(0, 3) == "123")
    {
        strIME = "123";
    }
    else if (strIME.substr(0, 3) == "2aB")
    {
        strIME = "2aB";
    }
    else if (strIME.substr(0, 3) == "abc")
    {
        strIME = "abc";
    }
    else if (strIME.substr(0, 3) == "ABC")
    {
        strIME = "ABC";
    }
    else if (strIME == IME_hebrew)
    {
        strIME = LANG_TRANSLATE(TRID_HEBREW_INPUT_METHOD).toUtf8().data();
    }
    return strIME;
}

// 判断类中是否有这种输入法
bool IMEManager::IsIMEExist(LPCSTR lpszIMEName, imeItem * pItem)
{
    // 入口参数判断
    if (NULL == lpszIMEName || NULL == pItem)
    {
        return false;
    }

    // 遍历查看是否存在某种输入法
    listKeyMaps::iterator it = pItem->m_listValidKeyMaps.begin();
    for (; it != pItem->m_listValidKeyMaps.end(); ++it)
    {
        if (NULL != *it && (*it)->strName == lpszIMEName)
        {
            return true;
        }
    }

    return false;
}

imeItem * IMEManager::findImeItem(QWidget * pWidget)
{
    // 入口参数检查
    if (NULL == pWidget)
    {
        return NULL;
    }

    // 遍历链表
    listItems::iterator it = listItems::begin();
    for (; it != listItems::end(); ++it)
    {
        imeItem * pItem = *it;

        // 判断是否为对应的输入法管理类
        if (pItem != NULL && pItem->m_pHostWidget == pWidget)
        {
            return pItem;
        }
    }

    return NULL;
}

imeItem & IMEManager::getIMEItem(QWidget * pWidget)
{
    // 查看控件是否已经有对应的IMEItem
    imeItem * pExistItem = findImeItem(pWidget);
    if (pExistItem != NULL)
    {
        return *pExistItem;
    }

    // 新建一个ImeItem对象
    imeItem * pNewItem = new imeItem();
    listItems::push_back(pNewItem);
    pNewItem->SetHostWidget(pWidget);
    return *pNewItem;
}

yl::string IMEManager::GetAllIMEName(QWidget * pWidget)
{
    // 入口参数检测
    if (NULL == pWidget)
    {
        return NULL;
    }

    imeItem * pItem = findImeItem(pWidget);
    if (NULL == pItem)
    {
        return NULL;
    }

    yl::string strIMEName;
    // 保存最后显示的输入法
    yl::string strLast;
    listKeyMaps::iterator it = pItem->m_listValidKeyMaps.begin();
    listKeyMaps::iterator CurImeIt;
    // 获取当前输入法后的输入法
    for (; it != pItem->m_listValidKeyMaps.end(); ++it)
    {
        mapKeys * pMapKey = *it;
        if (NULL != pMapKey)
        {
            if (pMapKey != pItem->m_pCurrentKeyMap)
            {
                strLast = strLast + " " + keyMaps2Title(pMapKey);
            }
            else
            {
                CurImeIt = it;
                break;
            }
        }
    }
    // 获取当前输入法前的输入法，保证切换输入法按顺序切换
    it = CurImeIt;
    for (; it != pItem->m_listValidKeyMaps.end(); ++it)
    {
        mapKeys * pMapKey = *it;
        if (NULL != pMapKey && pMapKey != pItem->m_pCurrentKeyMap)
        {
            strIMEName = strIMEName + " " + keyMaps2Title(pMapKey);
        }
    }
    INPUTMETHOD_INFO("strLast(%s), strIMEName(%s)", strLast.c_str(), strIMEName.c_str());
    strIMEName = strIMEName + strLast;
    return strIMEName;
}

IMEManager::~IMEManager()
{
    ReleaseIme();
}

void IMEManager::ReleaseIme()
{
    listItems::iterator it = listItems::begin();
    while (it != listItems::end())
    {
        if (*it == NULL)
        {
            listItems::erase(it);
        }
        else
        {
            UninstallIME((*it)->GetWidget());
        }

        it = listItems::begin();
    }
}

void IMEManager::ReloadIME()
{
    // 删除输入法
    ReleaseIme();

    // 重新加载输入法
    for (vecAttr::iterator it = m_vecImeAttr.begin(); it != m_vecImeAttr.end(); ++it)
    {
        InstallIME(it->pWidget, it->lpSet);
        SetCurrentIME(it->pWidget, it->strCurrentImeName.c_str());
    }

    m_vecImeAttr.clear();
}

void IMEManager::PreloadIME()
{
    // 先保存原有控件输入法
    for (listItems::iterator it = listItems::begin(); it != listItems::end(); ++it)
    {
        imeItem * pItem = *it;
        if (pItem == NULL)
        {
            continue;
        }

        WidgetImeAttr eAttrItem = pItem->GetAttr();

        if (eAttrItem)
        {
            m_vecImeAttr.push_back(eAttrItem);
        }
    }
}

/************************************************************************/
/*                        about  API                                    */
/************************************************************************/

#if WINDOWS_SYSTEM
// add by tianlei.yealink 2016-12-9 10:49:05
// 将软键盘的字符发送给对应的widget
// windows平台可能要用到QInputContext，先采用这种发消息方式，再研究
class IMFrame
{
public:
    // 发送预编辑的字符给对应的widget
    void sendPreeditContent(const QString & str, QWidget * wgtHost)
    {
        int cursorPosition = 1, selectionLength = 0;
        QString preeditString = str;

        QList<QInputMethodEvent::Attribute> attributes;

        int selPos = cursorPosition;
        if (selectionLength == 0)
        {
            selPos = 0;
        }
        else if (selectionLength < 0)
        {
            selPos += selectionLength;
            selectionLength = -selectionLength;
        }
        if (selPos > 0)
            attributes += QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat, 0, selPos,
                          QVariant(int(QInputContext::PreeditFormat)));

        if (selectionLength)
            attributes += QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat, selPos, selectionLength,
                          QVariant(int(QInputContext::SelectionFormat)));

        if (selPos + selectionLength < preeditString.length())
            attributes += QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat,
                          selPos + selectionLength,
                          preeditString.length() - selPos - selectionLength,
                          QVariant(int(QInputContext::PreeditFormat)));

        attributes += QInputMethodEvent::Attribute(QInputMethodEvent::Cursor, cursorPosition, 0,
                      QVariant());

        QInputMethodEvent ime(preeditString, attributes);
        if (!wgtHost)
        {
            if (QApplication::focusWidget()
                    && (QApplication::focusWidget()->inherits("QLineEdit")
                        || QApplication::focusWidget()->inherits("QTextEdit")))
            {
                qApp->sendEvent(QApplication::focusWidget(), &ime);
            }
        }
        else if (wgtHost->inherits("QLineEdit")
                 || wgtHost->inherits("QTextEdit"))
        {
            qApp->sendEvent(wgtHost, &ime);
        }
    }

    // 发送字符给对应的widget
    void sendCommitContent(const QString & str, QWidget * wgtHost)
    {
        QString commitString = str;
        QInputMethodEvent ime;
        ime.setCommitString(commitString, 0, 0);
        if (!wgtHost)
        {
            if (QApplication::focusWidget()
                    && (QApplication::focusWidget()->inherits("QLineEdit")
                        || QApplication::focusWidget()->inherits("QTextEdit")))
            {
                qApp->sendEvent(QApplication::focusWidget(), &ime);
            }
        }
        else if (wgtHost->inherits("QLineEdit")
                 || wgtHost->inherits("QTextEdit"))
        {
            qApp->sendEvent(wgtHost, &ime);
        }
    }

public:
    IMFrame()
    {
    }

    ~IMFrame()
    {
    }
};
#else

// 将软键盘的字符发送给对应的widget
class IMFrame : public QWSInputMethod
{
public:
    // 发送预编辑的字符给对应的widget
    void sendPreeditContent(const QString & str)
    {
        QWSInputMethod::sendPreeditString(str, 1);
    }

    // 发送字符给对应的widget
    void sendCommitContent(const QString & str)
    {
        QWSInputMethod::sendCommitString(str);
    }

public:
    IMFrame()
    {
        QWSServer::setCurrentInputMethod(this);
    }

    ~IMFrame()
    {
        QWSServer::resetInputMethod();
    }
};
#endif

// 获得唯一的IMFrame对象
IMFrame & getInstanceIME()
{
    static IMFrame g_InstanceIME;
    return g_InstanceIME;
}

#ifdef _WIN32
void imeSendCommitContent(const yl::string & strText, QWidget * wgtHost)
{
    // 在控制台输出以方便调试
    INPUTMETHOD_INFO("Commit(%s)", strText.c_str());

    getInstanceIME().sendCommitContent(toQString(strText.c_str()), wgtHost);
}

// 发送预编辑的字符给对应的widget
void imeSendPreeditContent(const yl::string & strText, QWidget * wgtHost)
{
    // 在控制台输出以方便调试
    INPUTMETHOD_INFO("Preedit(%s)", strText.c_str());
    getInstanceIME().sendPreeditContent(toQString(strText.c_str()), wgtHost);
}
#else
// 发送字符给对应的widget
void imeSendCommitContent(const yl::string & strText)
{
    getInstanceIME().sendCommitContent(toQString(strText.c_str()));
}

// 发送预编辑的字符给对应的widget
void imeSendPreeditContent(const yl::string & strText)
{
    getInstanceIME().sendPreeditContent(toQString(strText.c_str()));
}
#endif

void qOnIMEButtonClicked(QDialog * pDialog, qSoftButton * pSoftKeyBtn)
{
    // 获得当前焦点控件
    QWidget * pWidget = pDialog->focusWidget();

    // 获得下一种输入法的名字
    QString strIMEName = qShift2NextIME(pWidget);

    // 强制转换为对应类型
    qSoftButton * pBar = dynamic_cast<qSoftButton *>(pSoftKeyBtn);

    // 判断传入的控件类型是否正确
    if (NULL != pBar && pBar->m_strButtonName == "IME")
    {
        // 设置对应的控件显示输入法名
        pBar->SetTitle(strIMEName);
    }
}

QString ToNameFormat(QString strInPut)
{
    if (strInPut.length() < 1)
    {
        return strInPut;
    }

    QString strText = strInPut;
    strText = strText.toLower();
    for (int i = 0; i < strInPut.length(); ++i)
    {
        if (i == 0 || strText[i - 1] == QChar(' '))
        {
            strText[i] = strText[i].toUpper();
        }
    }

    return strText;
}

void qIMEInit()
{
    _ImeMagager;
}
