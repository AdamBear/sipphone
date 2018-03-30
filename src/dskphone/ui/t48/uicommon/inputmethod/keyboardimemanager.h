#ifndef CKEYBOARDIMEMANAGER_H
#define CKEYBOARDIMEMANAGER_H

#include "keyboardmanagerbase.h"

class CKeyboardImeManager : public CKeyboardManagerBase
{
private:
    CKeyboardImeManager();
    virtual ~CKeyboardImeManager();

public:
    static CKeyboardImeManager& GetInstance();

    virtual void InitKeyboardFromXmlFile();

    const vecKeyboardIme& GetKeyboardIme()
    {
        return m_vecKeyboardIme;
    }
    const SKeyboardIme* GetImeByIndex(int nIndex);
    int FindImeIndexByName(const yl::string& strImeName);

    int GetImeCount()
    {
        return m_nImeCount;
    }

    void SelectChar(int nImeIndex, int nButtonLIne, int nButtonID, int nInputboxIndex);
    const QString& GetSelectedChar()
    {
        return m_strSelectedChar;
    }
    void ResetSelectedChar()
    {
        m_strSelectedChar = "";
    }

protected:
    virtual void LoadXmlFiles(const yl::string& strPath, const yl::string& strFile);

private:
    //根据节点获取属性值
    void GetImeInfoByNode(const xml_node& xmlNode
                          , SKeyBoardImeKey& tImeKey
                          , SKeyBoardImeKey& tImeKeyCapital);

private:
    //记录目前已经有几个Ime
    int                                 m_nImeCount;
    //保存所有Ime
    vecKeyboardIme              m_vecKeyboardIme;

    //记录当前Ime是否包含大写键盘
    bool                                    m_bWithCaptial;

    //备选框选中的字符
    QString                         m_strSelectedChar;
};

#define g_KeyBoardImeManager (CKeyboardImeManager::GetInstance())

#endif // CKEYBOARDIMEMANAGER_H
