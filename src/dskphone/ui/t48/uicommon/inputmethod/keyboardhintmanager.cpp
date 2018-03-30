#include "keyboardhintmanager.h"

#define KEYBOARD_HINT_START                                 "Keyboard File \""
#define KEYBOARD_HINT_LINE_END                          "\" \n"
#define KEYBOARD_HINT_LANG_FILE_START               "In \"keyboard_lang.xml\", Ime File \n \""
#define KEYBOARD_HINT_LANG_FILE_END                 "\" \nFormat Error or Not Exist!"

#define KEYBOARD_HINT_XML_FORMAT_ERROR      "Xml Format Error!"
#define KEYBOARD_HINT_FIRST_CHILD_EMPTY         "Root Node Not Exist!"
#define KEYBOARD_HINT_FIRST_CHILD_NAME          "Root Node Name Error!"
#define KEYBOARD_HINT_WITHOUT_LAYOUT            "Without Layout Attribution!"
#define KEYBOARD_HINT_LAYOUT_NOT_EXIST          "Target Layout File Not Exist!"
#define KEYBOARD_HINT_WITHOUT_IME_TYPE          "Without ImeType Attribution!"
#define KEYBOARD_HINT_IME_TYPE_ERROR                "Error ImeType Attribution!"
#define KEYBOARD_HINT_NO_DISPLAYNAME            "Without DisplayName Attribution!"
#define KEYBOARD_HINT_NO_CAPITALNAME            "Without CapitalName Attribution!"
#define KEYBOARD_HINT_LANG_NAME_ERROR           "Without Lang Name!"
#define KEYBOARD_HINT_IME_NOT_EXIST                 "Target Ime File Not Exist!"

CKeyboardHintManager::CKeyboardHintManager()
    : m_bAllowAddHint(true)
{
    m_bAllowAddHint = true;

    m_mapHint.clear();
    m_listHint.clear();

    m_mapHint.insert(KEYBOARD_HINT_ERROR_TYPE_XML_FORMAT, KEYBOARD_HINT_XML_FORMAT_ERROR);
    m_mapHint.insert(KEYBOARD_HINT_ERROR_TYPE_FIRST_CHILD_EMPTY, KEYBOARD_HINT_FIRST_CHILD_EMPTY);
    m_mapHint.insert(KEYBOARD_HINT_ERROR_TYPE_FIRST_CHILD_NAME, KEYBOARD_HINT_FIRST_CHILD_NAME);
    m_mapHint.insert(KEYBOARD_HINT_ERROR_TYPE_WITHOUT_LAYOUT, KEYBOARD_HINT_WITHOUT_LAYOUT);
    m_mapHint.insert(KEYBOARD_HINT_ERROR_TYPE_LAYOUT_NOT_EXIST, KEYBOARD_HINT_LAYOUT_NOT_EXIST);
    m_mapHint.insert(KEYBOARD_HINT_ERROR_TYPE_WITHOUT_IME_TYPE, KEYBOARD_HINT_WITHOUT_IME_TYPE);
    m_mapHint.insert(KEYBOARD_HINT_ERROR_TYPE_IME_TYPE_ERROR, KEYBOARD_HINT_IME_TYPE_ERROR);
    m_mapHint.insert(KEYBOARD_HINT_ERROR_TYPE_NO_DISPLAYNAME, KEYBOARD_HINT_NO_DISPLAYNAME);
    m_mapHint.insert(KEYBOARD_HINT_ERROR_TYPE_NO_CAPITALNAME, KEYBOARD_HINT_NO_CAPITALNAME);
    m_mapHint.insert(KEYBOARD_HINT_ERROR_TYPE_LANG_NAME_ERROR, KEYBOARD_HINT_LANG_NAME_ERROR);
    m_mapHint.insert(KEYBOARD_HINT_ERROR_TYPE_IME_NOT_EXIST, KEYBOARD_HINT_IME_NOT_EXIST);
}

CKeyboardHintManager::~CKeyboardHintManager()
{

}

CKeyboardHintManager& CKeyboardHintManager::GetInstance()
{
    static CKeyboardHintManager myKeyboardHintManager;
    return myKeyboardHintManager;
}

void CKeyboardHintManager::AddHint(const yl::string& strFile, KEYBOARD_HINT_ERROR_TYPE eErrorType)
{
    if (!m_bAllowAddHint)
    {
        return;
    }

    if (KEYBOARD_HINT_ERROR_TYPE_IME_NOT_EXIST == eErrorType)
    {
        yl::string strHint = KEYBOARD_HINT_LANG_FILE_START;
        strHint += strFile;
        strHint += KEYBOARD_HINT_LANG_FILE_END;

        m_listHint.push_back(strHint);
    }
    else
    {
        yl::string strHint = KEYBOARD_HINT_START;
        strHint += strFile;
        strHint += KEYBOARD_HINT_LINE_END;

        strHint += m_mapHint.find(eErrorType).value();
        m_listHint.push_back(strHint);
    }
}

void CKeyboardHintManager::ShowNextHint()
{
    if (m_iterlistHint != m_listHint.end())
    {
        MessageBox_ShowCommonMsgBox(this, (*m_iterlistHint).c_str(), MESSAGEBOX_Cancel);
    }
}

void CKeyboardHintManager::ShowHints()
{
    if (m_listHint.size() <= 0)
    {
        return;
    }

    m_bAllowAddHint = false;
    m_iterlistHint = m_listHint.begin();
    ShowNextHint();
}

void CKeyboardHintManager::MessageBoxCallBack(CMessageBoxBase* pMessageBox)
{
    //手动关闭上一个弹框的情况下，才显示下一个提示
    if (NULL == pMessageBox || MessageBox_Cancel != pMessageBox->GetResult())
    {
        return;
    }

    ++m_iterlistHint;
    ShowNextHint();
}
