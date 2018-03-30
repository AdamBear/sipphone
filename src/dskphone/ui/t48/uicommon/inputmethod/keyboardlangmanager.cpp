#include "keyboardlangmanager.h"
#include "keyboardimemanager.h"

#include "dsklog/log.h"

CKeyboardLangManager::CKeyboardLangManager()
{

}

CKeyboardLangManager::~CKeyboardLangManager()
{

}

CKeyboardLangManager& CKeyboardLangManager::GetInstance()
{
    static CKeyboardLangManager myKeyboardLangManager;
    return myKeyboardLangManager;
}

const SKeyBoardLang* CKeyboardLangManager::GetLangByName(const yl::string& strLangName)
{
    if (m_vecKeyboardLang.size() <= 0)
    {
        return NULL;
    }

    vecKeyboardLang::iterator iterLang = m_vecKeyboardLang.begin();

    for (; iterLang != m_vecKeyboardLang.end(); ++iterLang)
    {
        if (strLangName == (*iterLang).m_strLangName)
        {
            return iterLang;
        }
    }

    return NULL;
}

void CKeyboardLangManager::InitKeyboardFromXmlFile()
{
    m_vecKeyboardLang.clear();

    if (g_KeyBoardXmlReader.bIsLangFileInConfigPath())
    {
        LoadXmlFiles(INPUT_FILES_CONFIG_PATH, g_KeyBoardXmlReader.GetLangFIle());
    }
    else
    {
        LoadXmlFiles(INPUT_FILES_FACTORY_PATH, g_KeyBoardXmlReader.GetLangFIle());
    }
}

void CKeyboardLangManager::LoadXmlFiles(const yl::string& strPath, const yl::string& strFile)
{
    INPUTMETHOD_INFO("CKeyboardLangManager::LoadXmlFiles, strFile = %s", strFile.c_str());

    yl::string strFullPath = strPath + strFile;

    if (!pathFileExist(strFullPath.c_str()))
    {
        return;
    }

    xml_document docLangXmlFile;
    if (!docLangXmlFile.load_file(strFullPath.c_str()))
    {
        INPUTMETHOD_INFO("CKeyboardLangManager::LoadXmlFiles, Load File Fail, strFile = %s", strFile.c_str());
        g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_XML_FORMAT);
        return;
    }

    xml_node xmlLang = docLangXmlFile.first_child();
    if (xmlLang.empty())
    {
        INPUTMETHOD_INFO("CKeyboardLangManager::LoadXmlFiles, First Child is Empty, strFile = %s", strFile.c_str());
        g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_FIRST_CHILD_EMPTY);
        return;
    }

    //判断名字
    yl::string strBaseName = xmlLang.name();
    if (0 != strBaseName.compare(INPUT_LANG_NAME))
    {
        INPUTMETHOD_INFO("CKeyboardLangManager::LoadXmlFiles, First Child Name Error, strFile = %s, strBaseName = %s", strFile.c_str(), strBaseName.c_str());
        g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_FIRST_CHILD_NAME);
        return;
    }

    //遍历Lang节点
    xml_node xmlLangData = xmlLang.child(INPUT_XML_NODE_LANG);
    while (!xmlLangData.empty())
    {
        // 检查节点是否正确
        if (strcmp(INPUT_XML_NODE_LANG, xmlLangData.name()) != 0)
        {
            xmlLangData = xmlLangData.next_sibling();
            INPUTMETHOD_INFO("CKeyboardLangManager::LoadXmlFiles, Lang Name Error, strFile = %s", strFile.c_str());
            g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_LANG_NAME_ERROR);
            continue;
        }

        yl::string strValue = "";

        SKeyBoardLang tKeyboardLang;

        //获取Lang名称
        if (DetectAttr(xmlLangData, INPUT_LANG_ATTRI_NAME, strValue))
        {
            if (strValue.empty())
            {
                xmlLangData = xmlLangData.next_sibling();
                INPUTMETHOD_INFO("CKeyboardLangManager::LoadXmlFiles, Lang Name Null, strFile = %s", strFile.c_str());
                g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_LANG_NAME_ERROR);
                continue;
            }

            tKeyboardLang.m_strLangName = strValue;
        }
        else
        {
            xmlLangData = xmlLangData.next_sibling();
            INPUTMETHOD_INFO("CKeyboardLangManager::LoadXmlFiles, Attribution Name Error, strFile = %s", strFile.c_str());
            g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_LANG_NAME_ERROR);
            continue;
        }

        //遍历Lang节点
        xml_node xmlLangIme = xmlLangData.child(INPUT_XML_NODE_IME);
        while (!xmlLangIme.empty())
        {
            // 检查节点是否正确
            if (strcmp(INPUT_XML_NODE_IME, xmlLangIme.name()) != 0)
            {
                xmlLangIme = xmlLangIme.next_sibling();
                INPUTMETHOD_INFO("CKeyboardLangManager::LoadXmlFiles, Ime Name Error, strFile = %s", strFile.c_str());
                continue;
            }

            //获取Ime名称
            if (DetectAttr(xmlLangIme, INPUT_LANG_ATTRI_NAME, strValue))
            {
                int nImeIndex = g_KeyBoardImeManager.FindImeIndexByName(strValue);
                if (KEYBOARD_IME_ERROR_INDEX != nImeIndex)
                {
                    //如果确实存在对应Ime，才将其加入到该Lang列表中
                    tKeyboardLang.m_vecImeIndex.push_back(nImeIndex);
                    //大写键盘在加载的时候是紧接在小写键盘之后的
                    //获取下一个键盘，如果m_strImeName相同，则代表是对应的大写键盘
                    const SKeyboardIme* pNextKeyboardIme = g_KeyBoardImeManager.GetImeByIndex(nImeIndex + 1);
                    if (NULL != pNextKeyboardIme && pNextKeyboardIme->m_strImeName == strValue)
                    {
                        tKeyboardLang.m_vecImeIndex.push_back(nImeIndex + 1);
                    }
                }
                else
                {
                    xmlLangIme = xmlLangIme.next_sibling();
                    INPUTMETHOD_INFO("CKeyboardLangManager::LoadXmlFiles, Ime Not Exist, strFile = %s, Ime = %s", strFile.c_str(), strValue.c_str());
                    g_KeyBoardHintManager.AddHint(strValue.c_str(), KEYBOARD_HINT_ERROR_TYPE_IME_NOT_EXIST);
                    continue;
                }
            }
            else
            {
                xmlLangIme = xmlLangIme.next_sibling();
                INPUTMETHOD_INFO("CKeyboardLangManager::LoadXmlFiles, Value Ime Error, strFile = %s", strFile.c_str());
                g_KeyBoardHintManager.AddHint(strValue.c_str(), KEYBOARD_HINT_ERROR_TYPE_IME_NOT_EXIST);
                continue;
            }

            xmlLangIme = xmlLangIme.next_sibling();
        }
        //获取Lang属性

        //如果组内包含至少一个语言则加入到记录中
        if (tKeyboardLang.m_vecImeIndex.size() > 0)
        {
            m_vecKeyboardLang.push_back(tKeyboardLang);
        }
        xmlLangData = xmlLangData.next_sibling();
    }
}
