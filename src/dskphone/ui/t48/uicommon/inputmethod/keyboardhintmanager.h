#ifndef CKEYBOARDHINTMANAGER_H
#define CKEYBOARDHINTMANAGER_H

#include "inputmanagerdefine.h"

#include "messagebox/modmessagebox.h"

enum KEYBOARD_HINT_ERROR_TYPE
{
    KEYBOARD_HINT_ERROR_TYPE_ERROR = 0,
    KEYBOARD_HINT_ERROR_TYPE_XML_FORMAT,
    KEYBOARD_HINT_ERROR_TYPE_FIRST_CHILD_EMPTY,
    KEYBOARD_HINT_ERROR_TYPE_FIRST_CHILD_NAME,
    KEYBOARD_HINT_ERROR_TYPE_WITHOUT_LAYOUT,
    KEYBOARD_HINT_ERROR_TYPE_LAYOUT_NOT_EXIST,
    KEYBOARD_HINT_ERROR_TYPE_WITHOUT_IME_TYPE,
    KEYBOARD_HINT_ERROR_TYPE_IME_TYPE_ERROR,
    KEYBOARD_HINT_ERROR_TYPE_NO_DISPLAYNAME,
    KEYBOARD_HINT_ERROR_TYPE_NO_CAPITALNAME,
    KEYBOARD_HINT_ERROR_TYPE_LANG_NAME_ERROR,
    KEYBOARD_HINT_ERROR_TYPE_IME_NOT_EXIST,
};

typedef QMap<KEYBOARD_HINT_ERROR_TYPE, yl::string> KEYBOARD_HINT_MAP;

class CKeyboardHintManager : public CMsgBoxCallBackAction
{
private:
    CKeyboardHintManager();
    virtual ~CKeyboardHintManager();

public:
    static CKeyboardHintManager& GetInstance();

    void AddHint(const yl::string& strFile, KEYBOARD_HINT_ERROR_TYPE eErrorType);

    void ShowHints();

    bool ExistErrorHints()
    {
        return m_listHint.size() > 0;
    }

    // 处理弹出框回调
    virtual void MessageBoxCallBack(CMessageBoxBase* pMessageBox);

private:
    void ShowNextHint();

private:
    //一开始允许添加提示，一旦开始显示提示，之后就再不能添加提示了
    bool                                        m_bAllowAddHint;
    KEYBOARD_HINT_MAP       m_mapHint;

    listString                              m_listHint;
    listString::iterator                m_iterlistHint;

};

#define g_KeyBoardHintManager (CKeyboardHintManager::GetInstance())

#endif // CKEYBOARDHINTMANAGER_H
