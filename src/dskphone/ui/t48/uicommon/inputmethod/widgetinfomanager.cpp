#include "widgetinfomanager.h"
#include <QLineEdit>
#include <QTextEdit>

#include "keyboardswitchmanager.h"

CWidgetInfoManager::CWidgetInfoManager(void)
{
    m_pCurrWidgetInfo = NULL;
}

CWidgetInfoManager::~CWidgetInfoManager(void)
{
}

CWidgetInfoManager& CWidgetInfoManager::GetInstance()
{
    static CWidgetInfoManager WidgetInfoManager;
    return WidgetInfoManager;
}

void CWidgetInfoManager::SaveKeyBoard(QWidget* p, bool bIsAutoShow, INPUT_DLGTYPE enTyep, INPUT_DIALOG_STYLE enStyle /*= INPUT_DIALOG_NORMAL */)
{
    if (NULL == p)
    {
        return;
    }
    std::list<CWidgetInfo>::iterator it;
    for (it = m_lstWidgetInfo.begin() ; it != m_lstWidgetInfo.end();)
    {
        if (NULL != it->m_pwidget)
        {
            if (it->m_pwidget == p)
            {
                it->m_KeyBoardInfo.Init();
                it->m_KeyBoardInfo.m_enShowStyle = enStyle;
                it->m_KeyBoardInfo.m_lstInputDlgType.push_back(enTyep);
                it->m_KeyBoardInfo.m_bAutoShow = bIsAutoShow;
                it->m_KeyBoardInfo.m_enInputDlgType = enTyep;
                return;
            }
            ++it;
        }
        else
        {
            it = m_lstWidgetInfo.erase(it);
        }
    }
    CWidgetInfo WidgetInfo;
    WidgetInfo.m_pwidget = p;
    WidgetInfo.m_KeyBoardInfo.m_lstInputDlgType.push_back(enTyep);
    WidgetInfo.m_KeyBoardInfo.m_enShowStyle = enStyle;
    WidgetInfo.m_KeyBoardInfo.m_bAutoShow = bIsAutoShow;
    WidgetInfo.m_KeyBoardInfo.m_enInputDlgType = enTyep;
    m_lstWidgetInfo.push_front(WidgetInfo);
}

void CWidgetInfoManager::SetKeyBoardType(QWidget* p, INPUT_DLGTYPE enTyep)
{
    if (NULL == p)
    {
        return;
    }
    std::list<CWidgetInfo>::iterator it;
    for (it = m_lstWidgetInfo.begin() ; it != m_lstWidgetInfo.end();)
    {
        if (NULL != it->m_pwidget)
        {
            if (it->m_pwidget == p)
            {
                it->m_KeyBoardInfo.m_enInputDlgType = enTyep;
                return;
            }
            ++it;
        }
        else
        {
            it = m_lstWidgetInfo.erase(it);
        }
    }
}

INPUT_DLGTYPE CWidgetInfoManager::GetKeyBoardTypeToUse(const QWidget* p)
{
    if (NULL == p)
    {
        return INPUT_DLGTYPE_NONE;
    }
    SetCurrWidget(p);
    if (NULL == m_pCurrWidgetInfo)
    {
        return INPUT_DLGTYPE_NONE;
    }
    return m_pCurrWidgetInfo->m_KeyBoardInfo.m_enInputDlgType;
}

INPUT_DLGTYPE CWidgetInfoManager::GetKeyBoardType(const QWidget* p)
{
    if (NULL == p)
    {
        return INPUT_DLGTYPE_NONE;
    }
    if (NULL != m_pCurrWidgetInfo && m_pCurrWidgetInfo->m_pwidget == p)
    {
        return m_pCurrWidgetInfo->m_KeyBoardInfo.m_enInputDlgType;
    }
    std::list<CWidgetInfo>::iterator it;
    for (it = m_lstWidgetInfo.begin() ; it != m_lstWidgetInfo.end();)
    {
        if (NULL == it->m_pwidget)
        {
            it = m_lstWidgetInfo.erase(it);
        }
        else if (p == it->m_pwidget)
        {
            return it->m_KeyBoardInfo.m_enInputDlgType;
        }
        else
        {
            ++it;
        }
    }
    return INPUT_DLGTYPE_NONE;
}

INPUT_DIALOG_STYLE CWidgetInfoManager::GetKeyBoardStyleToUse(const QWidget* p)
{
    if (NULL == p)
    {
        return INPUT_DIALOG_NORMAL;
    }
    SetCurrWidget(p);
    if (NULL == m_pCurrWidgetInfo)
    {
        return INPUT_DIALOG_NORMAL;
    }
    return m_pCurrWidgetInfo->m_KeyBoardInfo.m_enShowStyle;
}

bool CWidgetInfoManager::IsKeyBoardAutoShow(const QWidget* p)
{
    if (NULL == p)
    {
        return false;
    }
    SetCurrWidget(p);
    if (NULL == m_pCurrWidgetInfo)
    {
        return false;
    }
    return m_pCurrWidgetInfo->m_KeyBoardInfo.m_bAutoShow;
}

void CWidgetInfoManager::SetCurrWidget(const QWidget* p)
{
    if (m_pCurrWidgetInfo != NULL && m_pCurrWidgetInfo->m_pwidget == p)
    {
        return;
    }
    if (NULL == p)
    {
        return;
    }

    std::list<CWidgetInfo>::iterator it;
    for (it = m_lstWidgetInfo.begin() ; it != m_lstWidgetInfo.end();)
    {
        if (NULL == it->m_pwidget)
        {
            it = m_lstWidgetInfo.erase(it);
        }
        else if (p == it->m_pwidget)
        {
            // m_pCurrWidgetInfo->reset();
            m_pCurrWidgetInfo = &(*it);
            return;
        }
        else
        {
            ++it;
        }
    }
    m_pCurrWidgetInfo = NULL;
    return;
}

QWidget* CWidgetInfoManager::GetCurrentWidget() const
{
    if (NULL == m_pCurrWidgetInfo)
    {
        return NULL;
    }

    return m_pCurrWidgetInfo->m_pwidget;
}

bool CWidgetInfoManager::IsCurrWidget(const QWidget* p)
{
    if (p == NULL || NULL == m_pCurrWidgetInfo)
    {
        return false;
    }
    else
    {
        if (p != m_pCurrWidgetInfo->m_pwidget)
        {
            return false;
        }
    }
    return true;
}

void CWidgetInfoManager::DealFocusOut()
{
    m_pCurrWidgetInfo = NULL;
}

// 返回值：true 表示控件p没有在缓存里，需要外部绑定事件过滤器
// false 表示已经存在，不需要绑定事件过滤器
bool CWidgetInfoManager::SaveIME(QWidget* p, QString strFonts)
{
    if (NULL == p)
    {
        return false;
    }
    if (strFonts.size() == 0)
    {
        return false;
    }
    QStringList list1 = strFonts.split(",");
    std::list<CWidgetInfo>::iterator it;
    for (it = m_lstWidgetInfo.begin() ; it != m_lstWidgetInfo.end();)
    {
        if (NULL != it->m_pwidget)
        {
            if (it->m_pwidget == p)
            {
                for (int i = 0; i < list1.size(); i++)
                {
                    it->m_FontsInfo.m_lstFonts.push_back(list1[i].toStdString());
                }
                return false;
            }
            ++it;
        }
        else
        {
            it = m_lstWidgetInfo.erase(it);
        }
    }
    CWidgetInfo WidgetInfo;
    WidgetInfo.m_pwidget = p;
    for (int i = 0; i < list1.size(); i++)
    {
        WidgetInfo.m_FontsInfo.m_lstFonts.push_back(list1[i].toStdString());
    }
    WidgetInfo.m_FontsInfo.m_strCurrFontName = list1[0].toStdString();
    m_lstWidgetInfo.push_front(WidgetInfo);
    return true;
}

void CWidgetInfoManager::SetKeyBoardType(const QWidget* p, INPUT_DLGTYPE enTyep)
{
    if (NULL != p && NULL != m_pCurrWidgetInfo)
    {
        if (p == m_pCurrWidgetInfo->m_pwidget)
        {
            m_pCurrWidgetInfo->m_KeyBoardInfo.m_enInputDlgType = enTyep;
        }
    }
}

void CWidgetInfoManager::SetFontType(const QWidget* p, string strFont)
{
    if (NULL == m_pCurrWidgetInfo)
    {
        return;
    }
    if (p != m_pCurrWidgetInfo->m_pwidget)
    {
        return;
    }
    if (strFont != m_pCurrWidgetInfo->m_FontsInfo.m_strCurrFontName)
    {
        std::list<std::string>::iterator it;
        for (it = m_pCurrWidgetInfo->m_FontsInfo.m_lstFonts.begin();
                it != m_pCurrWidgetInfo->m_FontsInfo.m_lstFonts.end() ; ++it)
        {
            if (strFont == *it)
            {
                m_pCurrWidgetInfo->m_FontsInfo.m_strCurrFontName = strFont;
            }
        }
        return;
    }
}

// 扩展使用，用于获取下一个全键盘类型
INPUT_DLGTYPE CWidgetInfoManager::GetNextKeyBoardType(INPUT_DLGTYPE enTyep)
{
    return INPUT_DLGTYPE_EN;
}
// 扩展使用，用于判断是否可以切换到此类型的全键盘
bool CWidgetInfoManager::IsCanTransTo(INPUT_DLGTYPE enTyep)
{

    return true;
}

std::string CWidgetInfoManager::GetCurrFontType(const QWidget* p)
{
    if (NULL == p)
    {
        return "";
    }
    if (NULL != m_pCurrWidgetInfo && p == m_pCurrWidgetInfo->m_pwidget)
    {
        return m_pCurrWidgetInfo->m_FontsInfo.m_strCurrFontName;
    }
    SetCurrWidget(p);
    if (NULL != m_pCurrWidgetInfo)
    {
        return m_pCurrWidgetInfo->m_FontsInfo.m_strCurrFontName;
    }
    return "";
}

std::string CWidgetInfoManager::GetNextFontsType(const QWidget* p)
{
    if (NULL == p || NULL == m_pCurrWidgetInfo || m_pCurrWidgetInfo->m_pwidget != p)
    {
        return "";
    }

    if (m_pCurrWidgetInfo->m_FontsInfo.m_lstFonts.size() == 0)
    {
        return "";
    }
    std::list<string>::iterator it;
    for (it = m_pCurrWidgetInfo->m_FontsInfo.m_lstFonts.begin();
            it != m_pCurrWidgetInfo->m_FontsInfo.m_lstFonts.end();)
    {
        if (m_pCurrWidgetInfo->m_FontsInfo.m_strCurrFontName == *it)
        {
            ++it;
            if (it == m_pCurrWidgetInfo->m_FontsInfo.m_lstFonts.end())
            {
                return m_pCurrWidgetInfo->m_FontsInfo.m_lstFonts.front();
            }
            return *it;
        }
        ++it;
    }
    return m_pCurrWidgetInfo->m_FontsInfo.m_lstFonts.front();
}

void CWidgetInfoManager::SetCurrWidgetFont(const QWidget* p, string strFont)
{
    if (NULL == p || NULL == m_pCurrWidgetInfo || m_pCurrWidgetInfo->m_pwidget != p)
    {
        return;
    }
    m_pCurrWidgetInfo->m_FontsInfo.m_strCurrFontName = strFont;
    return;
}

bool CWidgetInfoManager::IsPerInput()
{
    if (NULL == m_pCurrWidgetInfo || NULL == m_pCurrWidgetInfo->m_pwidget)
    {
        return false;
    }
    QLineEdit* v_pLineEdit          = qobject_cast<QLineEdit*>(m_pCurrWidgetInfo->m_pwidget);
    if (NULL != v_pLineEdit)
    {
        if (QLineEdit::Password == v_pLineEdit->echoMode())
        {
            return true;
        }
    }
    return false;
}

bool CWidgetInfoManager::IsTextOverflow()
{
    if (NULL == m_pCurrWidgetInfo || NULL == m_pCurrWidgetInfo->m_pwidget)
    {
        return false;
    }

    if (m_pCurrWidgetInfo->m_pwidget->inherits("QLineEdit"))
    {
        QLineEdit* pLineEdit = (QLineEdit*)m_pCurrWidgetInfo->m_pwidget;

        // 判断所对应控件的类型
        if (pLineEdit != NULL)
        {
            return pLineEdit->text().size() >= pLineEdit->maxLength();
        }
    }
    else if (m_pCurrWidgetInfo->m_pwidget->inherits("QTextEdit"))
    {
        QTextEdit* pTextEdit = (QTextEdit*)(m_pCurrWidgetInfo->m_pwidget);
        if (pTextEdit != NULL)
        {
            return pTextEdit->toPlainText().size() >= m_pCurrWidgetInfo->m_iMaxLen;
        }
    }
    return false;
}

// 删除控件p中的strFont类型字库，并返回删除后的下一个字库类型
std::string CWidgetInfoManager::DeleteFontType(const QWidget* p, std::string strFont)
{
    if (NULL == p || NULL == m_pCurrWidgetInfo || p != m_pCurrWidgetInfo->m_pwidget)
    {
        return "";
    }
    std::list<string>::iterator it;
    for (it = m_pCurrWidgetInfo->m_FontsInfo.m_lstFonts.begin();
            it != m_pCurrWidgetInfo->m_FontsInfo.m_lstFonts.end();)
    {
        if (strFont == *it)
        {
            it = m_pCurrWidgetInfo->m_FontsInfo.m_lstFonts.erase(it);
            if (m_pCurrWidgetInfo->m_FontsInfo.m_lstFonts.size() == 0)
            {
                return "";
            }
            if (it == m_pCurrWidgetInfo->m_FontsInfo.m_lstFonts.end())
            {
                return m_pCurrWidgetInfo->m_FontsInfo.m_lstFonts.front();
            }
            return *it;
        }
        ++it;
    }
    return "";
}

void CWidgetInfoManager::DeleteWidget(QWidget* p)
{
    if (NULL == p)
    {
        return;
    }
    if (m_pCurrWidgetInfo != NULL && m_pCurrWidgetInfo->m_pwidget == p)
    {
        m_pCurrWidgetInfo = NULL;
    }
    std::list<CWidgetInfo>::iterator it;
    for (it = m_lstWidgetInfo.begin(); it != m_lstWidgetInfo.end(); ++it)
    {
        if (p == it->m_pwidget)
        {
            m_lstWidgetInfo.erase(it);
            return;
        }
    }
}

void CWidgetInfoManager::SetCurrentIME(const QWidget* p, std::string strIMEName)
{
    std::list<CWidgetInfo>::iterator it;
    for (it = m_lstWidgetInfo.begin(); it != m_lstWidgetInfo.end(); ++it)
    {
        if (p == it->m_pwidget)
        {
            CFontsInfo& FontsInfo = it->m_FontsInfo;
            std::list<std::string>::iterator iter;
            for (iter = FontsInfo.m_lstFonts.begin() ; iter != FontsInfo.m_lstFonts.end() ; ++iter)
            {
                if (strIMEName == *iter)
                {
                    FontsInfo.m_strCurrFontName = strIMEName;
                    return;
                }
            }
            return;
        }
    }
}
// 获取控件当前的输入法名称
std::string CWidgetInfoManager::GetWidgetCurrName(const QWidget* p)
{
    if (NULL == p)
    {
        return "";
    }
    std::list<CWidgetInfo>::iterator it;
    for (it = m_lstWidgetInfo.begin(); it != m_lstWidgetInfo.end(); ++it)
    {
        if (p == it->m_pwidget)
        {
            CFontsInfo FontsInfo = it->m_FontsInfo;
            return FontsInfo.m_strCurrFontName;
        }
    }
    return "";
}

std::string CWidgetInfoManager::GetWidgetFontsName(const QWidget* p)
{
    if (NULL == p)
    {
        return "";
    }
    std::string strFontsName = "";
    std::list<CWidgetInfo>::iterator it;
    for (it = m_lstWidgetInfo.begin(); it != m_lstWidgetInfo.end(); ++it)
    {
        if (p == it->m_pwidget)
        {
            CFontsInfo FontsInfo = it->m_FontsInfo;
            std::list<std::string>::iterator iter;
            for (iter = FontsInfo.m_lstFonts.begin() ;;)
            {
                std::string Fontname = *iter;
                if ("" != Fontname)
                {
                    strFontsName += Fontname;
                }
                ++iter;
                if (iter != FontsInfo.m_lstFonts.end())
                {
                    strFontsName += ",";
                }
                else
                {
                    break;
                }
            }
            break;
        }
    }
    return strFontsName;
}

void CWidgetInfoManager::SetQTextEditMaxLen(const QWidget* p, int iMaxLen)
{
    if (NULL == p || iMaxLen < 0)
    {
        return;
    }
    std::list<CWidgetInfo>::iterator it;
    for (it = m_lstWidgetInfo.begin(); it != m_lstWidgetInfo.end(); ++it)
    {
        if (p == it->m_pwidget)
        {
            CWidgetInfo& WidgetInfo = *it;
            WidgetInfo.m_iMaxLen = iMaxLen;
            return;
        }
    }
}

int CWidgetInfoManager::GetQTextEditMaxLen(const QWidget* p)
{
    if (NULL == p)
    {
        return 0;
    }
    std::list<CWidgetInfo>::iterator it;
    for (it = m_lstWidgetInfo.begin(); it != m_lstWidgetInfo.end(); ++it)
    {
        if (p == it->m_pwidget)
        {
            return it->m_iMaxLen;
        }
    }
    return 0;
}

void CWidgetInfoManager::SetNameFormat(QWidget* pWidget, bool bNameFormat)
{
    if (NULL == pWidget)
    {
        return;
    }
    std::list<CWidgetInfo>::iterator it;
    for (it = m_lstWidgetInfo.begin(); it != m_lstWidgetInfo.end(); ++it)
    {
        if (pWidget == it->m_pwidget)
        {
            if (bNameFormat == it->m_bNameFormat)
            {
                return;
            }
            CWidgetInfo& WidgetInfo = *it;

            WidgetInfo.m_bNameFormat = bNameFormat;

            if (pWidget != NULL)
            {
                QTextEdit* pTextEdit = qobject_cast<QTextEdit*>(pWidget);
                if (pTextEdit)
                {
                    if (bNameFormat)
                    {
                        QObject::connect(pTextEdit, SIGNAL(textChanged()), this, SLOT(AutoUpper()));
                    }
                    else
                    {
                        disconnect(pTextEdit, SIGNAL(textChanged()), this, SLOT(AutoUpper()));
                    }
                }
                else
                {


                    QLineEdit* pLineEdit = qobject_cast<QLineEdit*>(pWidget);
                    if (pLineEdit)
                    {
                        if (bNameFormat)
                        {
                            connect(pLineEdit, SIGNAL(textEdited(const QString &)), this, SLOT(AutoUpper(const QString &)));
                        }
                        else
                        {
                            disconnect(pLineEdit, SIGNAL(textEdited(const QString &)), this, SLOT(AutoUpper(const QString &)));
                        }
                    }
                }
            }
            break;
        }
    }


}

void CWidgetInfoManager::AutoUpper()
{
    if (NULL == m_pCurrWidgetInfo || NULL == m_pCurrWidgetInfo->m_pwidget)
    {
        return ;
    }

    QTextEdit* pTextEdit = qobject_cast<QTextEdit*>(m_pCurrWidgetInfo->m_pwidget);
    if (pTextEdit)
    {
        QString strText = pTextEdit->toPlainText();
        QTextCursor cursor = pTextEdit->textCursor();//先记下当前位置
        pTextEdit->setText(ToNameFormat(strText));
        pTextEdit->setTextCursor(cursor);//防止删除单词首字母后光标跳至最后
    }

}

void CWidgetInfoManager::AutoUpper(const QString & strText)
{
    if (NULL == m_pCurrWidgetInfo || NULL == m_pCurrWidgetInfo->m_pwidget)
    {
        return ;
    }

    QLineEdit* pLineEdit = qobject_cast<QLineEdit*>(m_pCurrWidgetInfo->m_pwidget);
    if (pLineEdit)
    {
        int iPos = pLineEdit->cursorPosition();//先记下当前位置
        pLineEdit->setText(ToNameFormat(strText));
        pLineEdit->setCursorPosition(iPos);//防止删除单词首字母后光标跳至最后
    }
}

QString CWidgetInfoManager::ToNameFormat(QString strInPut)
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

bool CWidgetInfoManager::ChangeKeyBoardStyle(const QWidget* pWidget, INPUT_DIALOG_STYLE enStyle /*= INPUT_DIALOG_NORMAL */)
{
    if (NULL == pWidget)
    {
        return false;
    }
    std::list<CWidgetInfo>::iterator it;
    for (it = m_lstWidgetInfo.begin() ; it != m_lstWidgetInfo.end();)
    {
        if (NULL != it->m_pwidget)
        {
            if (it->m_pwidget == pWidget)
            {
                it->m_KeyBoardInfo.m_enShowStyle = enStyle;
                return true;
            }
            ++it;
        }
        else
        {
            it = m_lstWidgetInfo.erase(it);
        }
    }
    return false;
}

bool CWidgetInfoManager::IsWidgetBind(QWidget* pWidget)
{
    if (NULL == pWidget)
    {
        return false;
    }
    std::list<CWidgetInfo>::iterator it;
    for (it = m_lstWidgetInfo.begin() ; it != m_lstWidgetInfo.end();)
    {
        if (NULL != it->m_pwidget)
        {
            if (it->m_pwidget == pWidget)
            {
                return true;
            }
            ++it;
        }
        else
        {
            it = m_lstWidgetInfo.erase(it);
        }
    }
    return false;
}

void CWidgetInfoManager::ResetAllDlgTypeByLanguage()
{
    std::list<CWidgetInfo>::iterator it;
    for (it = m_lstWidgetInfo.begin() ; it != m_lstWidgetInfo.end(); ++it)
    {
        if (NULL != it->m_pwidget)
        {
            it->m_KeyBoardInfo.m_enInputDlgType = g_KeyBoardSwitchManager.GetNewTypeWhenLangChange(it->m_KeyBoardInfo.m_enInputDlgType);
        }
    }
}
