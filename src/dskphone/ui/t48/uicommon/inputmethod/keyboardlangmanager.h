#ifndef CKEYBOARDLANGMANAGER_H
#define CKEYBOARDLANGMANAGER_H

#include "keyboardmanagerbase.h"

class CKeyboardLangManager : public CKeyboardManagerBase
{
private:
    CKeyboardLangManager();
    virtual ~CKeyboardLangManager();

public:
    static CKeyboardLangManager& GetInstance();

    virtual void InitKeyboardFromXmlFile();

    const vecKeyboardLang& GetKeyboardLang()
    {
        return m_vecKeyboardLang;
    }
    const SKeyBoardLang* GetLangByName(const yl::string& strLangName);

protected:
    virtual void LoadXmlFiles(const yl::string& strPath, const yl::string& strFile);

private:

private:
    vecKeyboardLang             m_vecKeyboardLang;
};

#define g_KeyBoardLangManager (CKeyboardLangManager::GetInstance())

#endif // CKEYBOARDLANGMANAGER_H
