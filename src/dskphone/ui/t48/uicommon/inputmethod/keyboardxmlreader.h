#ifndef CKEYBOARDXMLREADER_H
#define CKEYBOARDXMLREADER_H

#include "inputmanagerdefine.h"

class CKeyboardXmlReader
{
private:
    CKeyboardXmlReader();
    ~CKeyboardXmlReader();

public:
    static CKeyboardXmlReader& GetInstance();

    //获取配置文件入口
    void LoadKeyboardXmlFiles();

    //获取Factory/Config目录下的Layout文件
    listString& GetFactoryLayoutList()
    {
        return m_listFactoryLayoutFile;
    }
    listString& GetConfigLayoutList()
    {
        return m_listConfigLayoutFile;
    }

    //获取Factory/Config目录下的Ime文件
    listString& GetFactoryImeList()
    {
        return m_listFactoryImeFile;
    }
    listString& GetConfigImeList()
    {
        return m_listConfigImeFile;
    }

    //获取Lang文件
    //Lang文件只能使用一个，即如果Config下存在Lang文件则直接使用Config下的，而不使用Factory下的
    bool bIsLangFileInConfigPath()
    {
        return m_bIsLangFileInConfigPath;
    }
    const yl::string& GetLangFIle()
    {
        return m_strLangFile;
    }

private:
    listString m_listFactoryLayoutFile;
    listString m_listConfigLayoutFile;

    listString m_listFactoryImeFile;
    listString m_listConfigImeFile;

    bool m_bIsLangFileInConfigPath;
    yl::string  m_strLangFile;

};

#define g_KeyBoardXmlReader (CKeyboardXmlReader::GetInstance())

#endif // CKEYBOARDXMLREADER_H
