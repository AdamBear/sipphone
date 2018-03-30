#include "fontsmanager.h"
#include "fontnormal.h"
#include "fontnormalshift.h"
#include "font123.h"
#include "fontabc.h"
#include "fonta_bc.h"
#include "fonta_b_c_.h"
#include "font2ab_.h"
#include "font123dial.h"
#include "font123diallync.h"
#include "font123staronlydial.h"
#include "font123ip.h"
#include "fontsymbol.h"
#include "widgetinfomanager.h"
#include <QLineEdit>
#include <QTextEdit>

#ifdef CUSTOME_KEYBOARD
#include "fontcustom.h"
#endif

CFontsManager::CFontsManager(void)
{
    Init();
    m_pCurrFont    = NULL;
    m_iLastKeyCode = 0;
    m_nTimerID     = 0;
}

CFontsManager::~CFontsManager(void)
{
    m_mapFonts.clear();
    m_pCurrFont = NULL;
}

void CFontsManager::Init()
{
    m_mapFonts.clear();
    InitFontsObj();
}

void CFontsManager::Release()
{

}

void CFontsManager::ReloadIMEFile()
{
    _IMEPROXY.InitIMEText();

    std::map<std::string, CFontBase*>::iterator it;
    for (it = m_mapFonts.begin(); it != m_mapFonts.end() ; ++it)
    {
        CFontBase* FontBase = it->second;
        if (NULL != FontBase)
        {
            FontBase->LoadIMEFile();
        }
    }
}

bool CFontsManager::InputPreeditContent(QWidget* pWidget)
{
    if (NULL == pWidget || pWidget != g_WidgetInfoManager.GetCurrentWidget())
    {
        return false;
    }

    stopTimer();

    return m_FontOutput.SubmitCurrentCommitData(pWidget);
}

bool CFontsManager::IsPreedit(QWidget* pWidget)
{
    if (NULL == pWidget || pWidget != g_WidgetInfoManager.GetCurrentWidget())
    {
        return false;
    }

    return (!m_FontOutput.IsPreeditEmpty());
}

void CFontsManager::InitFontsObj()
{
    CFontBase* v_pFontBase                      = NULL;

    {
        v_pFontBase = new CFont123();
        if (NULL != v_pFontBase)
        {
            v_pFontBase->InitData();
            v_pFontBase->LoadIMEFile();
            m_mapFonts.insert(std::make_pair(IME_123, v_pFontBase));
        }
    }

    {
        v_pFontBase = new CFontabc();
        if (NULL != v_pFontBase)
        {
            v_pFontBase->InitData();
            v_pFontBase->LoadIMEFile();
            m_mapFonts.insert(std::make_pair(IME_abc, v_pFontBase));
        }
    }

    {
        v_pFontBase = new CFonta_b_c_();
        if (NULL != v_pFontBase)
        {
            v_pFontBase->InitData();
            v_pFontBase->LoadIMEFile();
            m_mapFonts.insert(std::make_pair(IME_ABC, v_pFontBase));
        }
    }

    {
        v_pFontBase = new CFont2ab_();
        if (NULL != v_pFontBase)
        {
            v_pFontBase->InitData();
            v_pFontBase->LoadIMEFile();
            m_mapFonts.insert(std::make_pair(IME_2aB, v_pFontBase));
        }
    }

    {
        v_pFontBase = new CFont_123_Dial();
        if (NULL != v_pFontBase)
        {
            v_pFontBase->InitData();
            v_pFontBase->LoadIMEFile();
            m_mapFonts.insert(std::make_pair(IME_123_Dial, v_pFontBase));
        }
    }

    {
        v_pFontBase = new CFont_123_Dail_Lync();
        if (NULL != v_pFontBase)
        {
            v_pFontBase->InitData();
            v_pFontBase->LoadIMEFile();
            m_mapFonts.insert(std::make_pair(IME_123_Dial_Lync, v_pFontBase));
        }
    }

    {
        v_pFontBase = new CFont_123_Star_Only_Dial();
        if (NULL != v_pFontBase)
        {
            v_pFontBase->InitData();
            v_pFontBase->LoadIMEFile();
            m_mapFonts.insert(std::make_pair(IME_123_Star_Only_Dial, v_pFontBase));
        }
    }

    {
        v_pFontBase = new CFont_123_IP();
        if (NULL != v_pFontBase)
        {
            v_pFontBase->InitData();
            v_pFontBase->LoadIMEFile();
            m_mapFonts.insert(std::make_pair(IME_123_IP, v_pFontBase));
        }
    }

    {
        v_pFontBase = new CFontNormal();
        if (NULL != v_pFontBase)
        {
            v_pFontBase->InitData();
            v_pFontBase->LoadIMEFile();
            m_mapFonts.insert(std::make_pair(FONT_NORMALS, v_pFontBase));
        }
    }

    {
        v_pFontBase = new CFontNormal_Shift();
        if (NULL != v_pFontBase)
        {
            v_pFontBase->InitData();
            v_pFontBase->LoadIMEFile();
            m_mapFonts.insert(std::make_pair(FONT_NORMAL_SHIFT, v_pFontBase));
        }
    }

    {
        v_pFontBase = new CFonta_bc();
        if (NULL != v_pFontBase)
        {
            v_pFontBase->InitData();
            v_pFontBase->LoadIMEFile();
            m_mapFonts.insert(std::make_pair(IME_Abc, v_pFontBase));
        }
    }

    {
        v_pFontBase = new CFontSymbol();
        if (NULL != v_pFontBase)
        {
            v_pFontBase->InitData();
            v_pFontBase->LoadIMEFile();
            m_mapFonts.insert(std::make_pair(IME_symbol, v_pFontBase));
        }
    }

#ifdef CUSTOME_KEYBOARD
    {
        v_pFontBase = new CFontCustom();
        if (NULL != v_pFontBase)
        {
            v_pFontBase->InitData();
            v_pFontBase->LoadIMEFile();
            m_mapFonts.insert(std::make_pair(FONT_CUSTOM_KEYBOARD, v_pFontBase));
        }
    }
#endif
}

CFontBase*  CFontsManager::CreatFontObjByType(string strFont)
{
    CFontBase* v_pFontBase                      = NULL;

    if (IME_123 == strFont)
    {
        v_pFontBase = new CFont123();
    }
    else if (IME_abc == strFont)
    {
        v_pFontBase = new CFontabc();
    }
    else if (IME_ABC == strFont)
    {
        v_pFontBase = new CFonta_b_c_();
    }
    else if (IME_2aB == strFont)
    {
        v_pFontBase = new CFont2ab_();
    }
    else if (IME_123_Dial == strFont)
    {
        v_pFontBase = new CFont_123_Dial();
    }
    else if (IME_123_Dial_Lync == strFont)
    {
        v_pFontBase = new CFont_123_Dail_Lync();
    }
    else if (IME_123_Star_Only_Dial == strFont)
    {
        v_pFontBase = new CFont_123_Star_Only_Dial();
    }
    else if (IME_123_IP == strFont)
    {
        v_pFontBase = new CFont_123_IP();
    }
    else if (FONT_NORMALS == strFont)
    {
        v_pFontBase = new CFontNormal();
    }
    else if (FONT_NORMAL_SHIFT == strFont)
    {
        v_pFontBase = new CFontNormal_Shift();
    }
    else if (IME_Abc == strFont)
    {
        v_pFontBase = new CFonta_bc();
    }
    else if (IME_symbol == strFont)
    {
        v_pFontBase = new CFontSymbol();
    }
#ifdef CUSTOME_KEYBOARD
    else if (FONT_CUSTOM_KEYBOARD == strFont)
    {
        v_pFontBase = new CFontCustom();
    }
    if (v_pFontBase == NULL)
    {
        v_pFontBase = new CFontCustom();
    }
#endif
    if (NULL != v_pFontBase)
    {
        v_pFontBase->InitData();
        v_pFontBase->LoadIMEFile();
        m_mapFonts.insert(std::make_pair(strFont, v_pFontBase));
    }
    return v_pFontBase;
}

bool CFontsManager::SetFontByType(string strFont)
{
    if ("" == strFont)
    {
        m_pCurrFont = NULL;
        return true;
    }
    if (m_pCurrFont != NULL && strFont == m_pCurrFont->GetName().toUtf8().data())
    {
        return true;
    }
    std::map<std::string, CFontBase*>::iterator it;
    it = m_mapFonts.find(strFont);
    if (it != m_mapFonts.end())
    {
        m_pCurrFont = it->second;
        return true;
    }
    else
    {
        m_pCurrFont = CreatFontObjByType(strFont);
        if (m_pCurrFont != NULL)
        {
            return true;
        }
        return false;
    }
}

QString CFontsManager::GetCurrFontName()
{
    if (NULL != m_pCurrFont)
    {
        return m_pCurrFont->GetName();
    }
    return "";
}

QString CFontsManager::GetCurrFontTitle()
{
    if (NULL != m_pCurrFont)
    {
        return m_pCurrFont->GetTitle();
    }
    return "";
}

bool CFontsManager::DealKeyRelease(QObject *pObj, int iKeyCode)
{
    if (NULL == m_pCurrFont)
    {
        return false;
    }
    return m_pCurrFont->DealKeyRelease(pObj, iKeyCode);
}

bool CFontsManager::DealKeyPress(QObject *pObj, int iKeyCode)
{
    if (NULL == m_pCurrFont)
    {
        return false;
    }
    if (Qt::Key_Backspace == iKeyCode)
    {
        //重置IME的点击计数
        CFontPreeditBase* Font = (CFontPreeditBase*)m_pCurrFont;
        if (NULL != Font)
        {
            Font->ResetPressTimes();
        }
        return m_FontOutput.DealBackspace(g_WidgetInfoManager.GetCurrentWidget());
    }

    bool bResult = true;
    //if ( g_WidgetInfoManager.IsTextOverflow() )
    //{
    //  return false;
    //}
    if (m_iLastKeyCode != iKeyCode)
    {
        m_pCurrFont->Init();
    }
    QString strFont = m_pCurrFont->GetFontData(NULL, iKeyCode, 0);
    QWidget* pCurrentWidget = g_WidgetInfoManager.GetCurrentWidget();
    INPUTMETHOD_INFO("CFontsManager::DealKeyPress %s", strFont.toLocal8Bit().constData());
    if (g_WidgetInfoManager.IsPerInput() || m_pCurrFont->IsPerInput(iKeyCode))
    {
        if (strFont.isNull() || strFont.isEmpty())
        {
            m_iLastKeyCode = 0;
            if (!m_FontOutput.IsPreeditEmpty())
            {
                stopTimer();
                m_FontOutput.SubmitCurrentCommitData(pCurrentWidget);
            }
            bResult = DealDirectionKey(pObj, iKeyCode);
        }
        else
        {
            bResult = true;
            if (m_iLastKeyCode != iKeyCode
                    || PHONE_KEY_INPUTMETHOD_CUSTOM_KEYBOARD == iKeyCode)
            {
                stopTimer();
                m_FontOutput.SubmitCurrentCommitData(pCurrentWidget);
                m_iLastKeyCode = iKeyCode;
            }
            else
            {
                if (m_FontOutput.IsOutStrEqual(strFont))
                {
                    stopTimer();
                    m_FontOutput.SubmitCurrentCommitData(pCurrentWidget);
                }
            }
            if (!g_WidgetInfoManager.IsTextOverflow())
            {
                m_FontOutput.SubmitCurrentPreeditData(pCurrentWidget, strFont);
                startTimer();
            }
        }
    }
    else
    {
        if (!m_FontOutput.IsPreeditEmpty())
        {
            stopTimer();
            m_FontOutput.SubmitCurrentCommitData(pCurrentWidget);
        }
        if (strFont.isNull() || strFont.isEmpty())
        {
            return DealDirectionKey(pObj, iKeyCode);
        }
        else
        {
            m_FontOutput.SubmitCommitData(pCurrentWidget, strFont);
        }
    }
    return bResult;
}

bool CFontsManager::DealDirectionKey(QObject *pObj, int iKeyCode)
{
    QWidget* pWidget            = (QWidget*)pObj;
    if (NULL == pWidget)
    {
        return false;
    }
    if (iKeyCode == PHONE_KEY_LEFT || iKeyCode == PHONE_KEY_RIGHT
            || iKeyCode == PHONE_KEY_UP || iKeyCode == PHONE_KEY_DOWN)
    {
        if (pWidget->inherits("QLineEdit"))
        {
            QLineEdit* pLineEdit = (QLineEdit*)(pWidget);
            if (pLineEdit != NULL)
            {
                if (iKeyCode == PHONE_KEY_LEFT || iKeyCode == PHONE_KEY_RIGHT)
                {
                    // 重设光标位置
                    // 获得当前的光标位置
                    int pos = pLineEdit->cursorPosition();

                    // 根据按键对光标位置进行修正
                    if (iKeyCode == PHONE_KEY_LEFT)
                    {
                        pos--;
                    }
                    else if (iKeyCode == PHONE_KEY_RIGHT)
                    {
                        pos++;
                    }

                    // 重设光标位置
                    pLineEdit->setCursorPosition(pos);
                    return true;
                }
            }
        }
        else if (pWidget->inherits("QTextEdit"))
        {
            QTextEdit* pTextEdit = (QTextEdit*)(pWidget);
            if (pTextEdit != NULL)
            {
                // 重设光标位置
                // 获得当前的光标位置
                QTextCursor cursor = pTextEdit->textCursor();

                // 根据按键对光标位置进行修正
                if (iKeyCode == PHONE_KEY_LEFT)
                {
                    cursor.movePosition(QTextCursor::Left);
                }
                else if (iKeyCode == PHONE_KEY_RIGHT)
                {
                    cursor.movePosition(QTextCursor::Right);
                }
                else if (iKeyCode == PHONE_KEY_UP)
                {
                    cursor.movePosition(QTextCursor::Up);
                }
                else if (iKeyCode == PHONE_KEY_DOWN)
                {
                    cursor.movePosition(QTextCursor::Down);
                }

                // 重设光标位置
                pTextEdit->setTextCursor(cursor);
                return true;
            }
        }
        return false;
    }
    return false;
}

void CFontsManager::OnBackspaceClicked(QWidget* pWidget)
{
    if (NULL != pWidget)
    {
        if (g_WidgetInfoManager.IsCurrWidget(pWidget) ||
                g_WidgetInfoManager.IsCurrWidget(pWidget->focusWidget()))
        {
            if (!DealKeyPress(pWidget, Qt::Key_Backspace))
            {
                if (!DeleteEditData(pWidget))
                {
                    DeleteEditData(pWidget->focusWidget());
                }
            }
            return;
        }
    }
    // 不处于编辑状态
    // 根据控件的类型删除已经输入的一个字符
    //DeleteEditData(pWidget);
}


bool CFontsManager::DeleteEditData(QWidget* pWidget)
{
    if (pWidget->inherits("QLineEdit"))
    {
        QLineEdit* pLineEdit = (QLineEdit*)(pWidget);
        if (pLineEdit != NULL)
        {
            pLineEdit->backspace();
            return true;
        }
    }
    else if (pWidget->inherits("QTextEdit"))
    {
        QTextEdit* pTextEdit = (QTextEdit*)(pWidget);
        if (pTextEdit != NULL)
        {
            QTextCursor cursor = pTextEdit->textCursor();
            cursor.deletePreviousChar();
            return true;
        }
    }
    return false;
}

void CFontsManager::EnterEditData(QWidget* pWidget)
{
    stopTimer();
    m_FontOutput.SubmitCurrentCommitData(pWidget);
}

void CFontsManager::startTimer()
{
    // 先停止计时器
    stopTimer();
    // 开启计时并记录ID
    m_nTimerID = QObject::startTimer(800);
}

void CFontsManager::stopTimer()
{
    // 判断计时器是否激活
    if (m_nTimerID != 0)
    {
        QObject::killTimer(m_nTimerID);
        m_nTimerID = 0;
    }
}

void CFontsManager::timerEvent(QTimerEvent* pEvent)
{
    // 输入预编辑状态的字符
    // 停止计时器
    stopTimer();
    m_FontOutput.SubmitCurrentCommitData(g_WidgetInfoManager.GetCurrentWidget());
    m_iLastKeyCode = 0;
}

void CFontsManager::DealFocusIn()
{
    //stopTimer();
    // 重置点击次数
    // ^……

    if (m_pCurrFont != NULL)
    {
        m_pCurrFont->Init();
    }
    m_iLastKeyCode              = 0;
}

void CFontsManager::DealFocusOut()
{
    //stopTimer();
    // 重置点击次数
    // ^……
    if (m_pCurrFont != NULL)
    {
        m_pCurrFont->Release();
        m_pCurrFont = NULL;
    }
    m_iLastKeyCode              = 0;
}

void CFontsManager::DealMousePress()
{
    stopTimer();
    m_FontOutput.SubmitCurrentCommitData(g_WidgetInfoManager.GetCurrentWidget());
}

QString CFontsManager::GetIMEFirstChar(QString lpszIMEName, int iKeyCode)
{
    std::map<std::string, CFontBase*>::iterator it;
    it = m_mapFonts.find(lpszIMEName.toStdString());
    if (it != m_mapFonts.end())
    {
        CFontBase* pFontBase = it->second;
        return pFontBase->GetFirstChar(iKeyCode);
    }
    return "";
}

QString CFontsManager::GetFontTitleByName(std::string strName)
{
    if (IME_123 == strName)
    {
        return IME_TITLE_123;
    }
    else if (IME_abc == strName)
    {
        return IME_abc;
    }
    else if (IME_ABC == strName)
    {
        return IME_ABC;
    }
    else if (IME_2aB == strName)
    {
        return IME_2aB;
    }
    else if (IME_123_Dial == strName)
    {
        return IME_TITLE_123;
    }
    else if (IME_123_Dial_Lync == strName)
    {
        return IME_TITLE_123;
    }
    else if (IME_123_Star_Only_Dial == strName)
    {
        return IME_TITLE_123;
    }
    else if (IME_123_IP == strName)
    {
        return IME_TITLE_123;
    }
    else if (IME_Abc == strName)
    {
        return IME_Abc;
    }
    else if (FONT_NORMALS == strName)
    {
        return FONT_NORMALS;
    }
    else if (FONT_NORMAL_SHIFT == strName)
    {
        return FONT_NORMAL_SHIFT;
    }
    else if (IME_symbol == strName)
    {
        return IME_TITLE_symbol;
    }
#ifdef CUSTOME_KEYBOARD
    else if (FONT_CUSTOM_KEYBOARD == strName)
    {
        return FONT_CUSTOM_KEYBOARD;
    }
#endif
    return "";
}
