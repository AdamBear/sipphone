#include "keyboardimemanager.h"
#include "keyboardlayoutmanager.h"

#include "dsklog/log.h"
#include "qtcommon/qmisc.h"

CKeyboardImeManager::CKeyboardImeManager()
    : m_nImeCount(0)
    , m_bWithCaptial(false)
    , m_strSelectedChar("")
{

}

CKeyboardImeManager::~CKeyboardImeManager()
{

}

CKeyboardImeManager& CKeyboardImeManager::GetInstance()
{
    static CKeyboardImeManager myKeyboardImeManager;
    return myKeyboardImeManager;
}

const SKeyboardIme* CKeyboardImeManager::GetImeByIndex(int nIndex)
{
    if (m_vecKeyboardIme.size() <= 0 || nIndex < 0 || m_vecKeyboardIme.size() <= nIndex)
    {
        return NULL;
    }

    vecKeyboardIme::iterator iterIme = m_vecKeyboardIme.begin();

    for (; iterIme != m_vecKeyboardIme.end(); ++iterIme)
    {
        if (nIndex == (*iterIme).m_nImeIndex)
        {
            return iterIme;
        }
    }

    return NULL;
}

int CKeyboardImeManager::FindImeIndexByName(const yl::string& strImeName)
{
    if (m_vecKeyboardIme.size() <= 0)
    {
        return KEYBOARD_IME_ERROR_INDEX;
    }

    vecKeyboardIme::iterator iterIme = m_vecKeyboardIme.begin();

    for (; iterIme != m_vecKeyboardIme.end(); ++iterIme)
    {
        if (strImeName == (*iterIme).m_strImeName)
        {
            return (*iterIme).m_nImeIndex;
        }
    }

    return KEYBOARD_IME_ERROR_INDEX;
}

void CKeyboardImeManager::InitKeyboardFromXmlFile()
{
    m_vecKeyboardIme.clear();
    m_nImeCount = 0;

    CKeyboardManagerBase::LoadXmlFiles(g_KeyBoardXmlReader.GetFactoryImeList(), INPUT_FILES_FACTORY_PATH);
    CKeyboardManagerBase::LoadXmlFiles(g_KeyBoardXmlReader.GetConfigImeList(), INPUT_FILES_CONFIG_PATH);
}

void CKeyboardImeManager::LoadXmlFiles(const yl::string& strPath, const yl::string& strFile)
{
    INPUTMETHOD_INFO("CKeyboardImeManager::LoadXmlFiles, strFile = %s", strFile.c_str());

    //每次要加载一个文件前，将该变量置false
    m_bWithCaptial = false;

    yl::string strFullPath = strPath + strFile;

    if (!pathFileExist(strFullPath.c_str()))
    {
        return;
    }

    xml_document docImeXmlFile;
    if (!docImeXmlFile.load_file(strFullPath.c_str()))
    {
        INPUTMETHOD_INFO("CKeyboardImeManager::LoadXmlFiles, Load File Fail, strFile = %s", strFile.c_str());
        g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_XML_FORMAT);
        return;
    }

    xml_node xmlImeData = docImeXmlFile.first_child();
    if (xmlImeData.empty())
    {
        INPUTMETHOD_INFO("CKeyboardImeManager::LoadXmlFiles, First Child is Empty, strFile = %s", strFile.c_str());
        g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_FIRST_CHILD_EMPTY);
        return;
    }

    //判断名字
    yl::string strBaseName = xmlImeData.name();
    if (0 != strBaseName.compare(INPUT_IME_NAME))
    {
        INPUTMETHOD_INFO("CKeyboardImeManager::LoadXmlFiles, First Child Name Error, strFile = %s, strBaseName = %s", strFile.c_str(), strBaseName.c_str());
        g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_FIRST_CHILD_NAME);
        return;
    }

    yl::string strValue = "";

    SKeyboardIme tKeyboardIme;
    SKeyboardIme tKeyboardImeCapital;
    tKeyboardIme.m_strImeName = strFile;
    tKeyboardImeCapital.m_strImeName = strFile;

    //获取对应的Layout名字
    if (DetectAttr(xmlImeData, INPUT_IME_ATTRI_LAYOUT, strValue))
    {
        if (NULL == g_KeyBoardLayoutManager.GetLayoutByName(strValue))
        {
            INPUTMETHOD_INFO("CKeyboardImeManager::LoadXmlFiles, Layout File not exits, strFile = %s, Layout Name = %s", strFile.c_str(), strValue.c_str());
            g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_LAYOUT_NOT_EXIST);
            return;
        }

        tKeyboardIme.m_strLayoutName = strValue;
        tKeyboardImeCapital.m_strLayoutName = strValue;
    }
    else
    {
        INPUTMETHOD_INFO("CKeyboardImeManager::LoadXmlFiles, Without Layout File, strFile = %s", strFile.c_str());
        g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_WITHOUT_LAYOUT);
        return;
    }

    //获取键盘类型（Char、Symbol）
    if (DetectAttr(xmlImeData, INPUT_IME_ATTRI_IMETYPE, strValue))
    {
        if (KEYBOARD_IME_TYPE_CHAR == strValue)
        {
            tKeyboardIme.m_eImeType = IME_TYPE_CHAR;
            tKeyboardImeCapital.m_eImeType = IME_TYPE_CAPITAL;
        }
        else if (KEYBOARD_IME_TYPE_SYMBOL == strValue)
        {
            tKeyboardIme.m_eImeType = IME_TYPE_SYMBOL;
            tKeyboardImeCapital.m_eImeType = IME_TYPE_SYMBOL;
        }
        else
        {
            INPUTMETHOD_INFO("CKeyboardImeManager::LoadXmlFiles, Attribution ImeType Error, strFile = %s", strFile.c_str());
            g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_IME_TYPE_ERROR);
            return;
        }
    }
    else
    {
        INPUTMETHOD_INFO("CKeyboardImeManager::LoadXmlFiles, Without Ime Type, strFile = %s", strFile.c_str());
        g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_WITHOUT_IME_TYPE);
        return;
    }

    //获取普通键盘显示名
    if (DetectAttr(xmlImeData, INPUT_IME_ATTRI_DISPLAYNAME, strValue))
    {
        if (strValue.empty())
        {
            INPUTMETHOD_INFO("CKeyboardImeManager::LoadXmlFiles, Ime Name Null, strFile = %s, Layout Name = %s", strFile.c_str(), strValue.c_str());
            g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_NO_DISPLAYNAME);
            return;
        }

        tKeyboardIme.m_strDisplayName = strValue;
    }
    else
    {
        INPUTMETHOD_INFO("CKeyboardImeManager::LoadXmlFiles, Attribution DisplayName Error, strFile = %s", strFile.c_str());
        g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_NO_DISPLAYNAME);
        return;
    }

    //获取大写键盘显示名，Symbol类型不需要大写显示名
    if (IME_TYPE_CAPITAL == tKeyboardImeCapital.m_eImeType)
    {
        //如果需要但是又缺少，则认为格式有误
        if (DetectAttr(xmlImeData, INPUT_IME_ATTRI_CAPITALNAME, strValue))
        {
            if (strValue.empty())
            {
                INPUTMETHOD_INFO("CKeyboardImeManager::LoadXmlFiles, Ime CapitalName Null, strFile = %s, Layout Name = %s", strFile.c_str(), strValue.c_str());
                g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_NO_CAPITALNAME);
                return;
            }

            tKeyboardImeCapital.m_strDisplayName = strValue;
        }
        else
        {
            INPUTMETHOD_INFO("CKeyboardImeManager::LoadXmlFiles, Attribution CapitalName Error, strFile = %s", strFile.c_str());
            g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_NO_CAPITALNAME);
            return;
        }
    }
    else
    {
        tKeyboardImeCapital.m_strDisplayName = tKeyboardIme.m_strDisplayName;
    }

    //遍历Line节点
    xml_node xmlImeLine = xmlImeData.child(INPUT_XML_NODE_LINE);
    while (!xmlImeLine.empty())
    {
        // 检查节点是否正确
        if (strcmp(INPUT_XML_NODE_LINE, xmlImeLine.name()) != 0)
        {
            xmlImeLine = xmlImeLine.next_sibling();
            INPUTMETHOD_INFO("CKeyboardImeManager::LoadXmlFiles, Line Name Error, strFile = %s", strFile.c_str());
            continue;
        }

        vecKeyboardImeKeyLine tImeLine;
        vecKeyboardImeKeyLine tImeLineCapital;

        //遍历Key节点
        xml_node xmlImeKey = xmlImeLine.child(INPUT_XML_NODE_KEY);
        while (!xmlImeKey.empty())
        {
            // 检查节点是否正确
            if (strcmp(INPUT_XML_NODE_KEY, xmlImeKey.name()) != 0)
            {
                xmlImeKey = xmlImeKey.next_sibling();
                INPUTMETHOD_INFO("CKeyboardImeManager::LoadXmlFiles, Key Name Error, strFile = %s", strFile.c_str());
                continue;
            }

            SKeyBoardImeKey tImeKey;
            SKeyBoardImeKey tImeKeyCapital;

            //获取Key属性
            GetImeInfoByNode(xmlImeKey, tImeKey, tImeKeyCapital);

            tImeLine.push_back(tImeKey);
            tImeLineCapital.push_back(tImeKeyCapital);
            xmlImeKey = xmlImeKey.next_sibling();
        }

        tKeyboardIme.m_vecImeKey.push_back(tImeLine);
        tKeyboardImeCapital.m_vecImeKey.push_back(tImeLineCapital);
        xmlImeLine = xmlImeLine.next_sibling();
    }

    tKeyboardIme.m_nImeIndex = m_nImeCount;
    ++m_nImeCount;
    m_vecKeyboardIme.push_back(tKeyboardIme);

    //符号键盘不存在大写键盘
    //只要存在至少一个大写按钮，就认为存在大写键盘
    if (tKeyboardIme.m_eImeType == IME_TYPE_CHAR && m_bWithCaptial)
    {
        tKeyboardImeCapital.m_nImeIndex = m_nImeCount;
        ++m_nImeCount;
        m_vecKeyboardIme.push_back(tKeyboardImeCapital);
    }
}

void CKeyboardImeManager::GetImeInfoByNode(const xml_node& xmlNode, SKeyBoardImeKey& tImeKey, SKeyBoardImeKey& tImeKeyCapital)
{
    yl::string strValue = "";

    if (DetectAttr(xmlNode, INPUT_IME_ATTRI_DISPLAY, strValue))
    {
        tImeKey.m_strDisplayName = strValue;
        tImeKeyCapital.m_strDisplayName = strValue;
    }
    else
    {
        tImeKey.m_strDisplayName = "";
        tImeKeyCapital.m_strDisplayName = "";
    }
    if (DetectAttr(xmlNode, INPUT_IME_ATTRI_FUCTION, strValue))
    {
        tImeKey.m_strFunction = strValue;
        tImeKeyCapital.m_strFunction = strValue;
    }
    else
    {
        tImeKey.m_strFunction = "";
        tImeKeyCapital.m_strFunction = "";
    }
    if (DetectAttr(xmlNode, INPUT_IME_ATTRI_IME_NORMAL, strValue))
    {
        tImeKey.m_strImeList = strValue;
    }
    else
    {
        tImeKey.m_strImeList = "";
    }
    if (DetectAttr(xmlNode, INPUT_IME_ATTRI_IME_CAPITAL, strValue))
    {
        //某个按钮有大写字符，则认为存在大写键盘
        m_bWithCaptial = true;
        tImeKeyCapital.m_strImeList = strValue;
    }
    else
    {
        tImeKeyCapital.m_strImeList = "";
    }
}

void CKeyboardImeManager::SelectChar(int nImeIndex, int nButtonLIne, int nButtonID, int nInputboxIndex)
{
    INPUTMETHOD_INFO("CKeyboardImeManager::SelectChar, nImeIndex = %d, nButtonLIne = %d, nButtonID = %d, nInputboxIndex = %d", nImeIndex, nButtonLIne, nButtonID, nInputboxIndex);

    if (nImeIndex <= KEYBOARD_IME_ERROR_INDEX
            || nImeIndex >= m_nImeCount
            || nImeIndex >= m_vecKeyboardIme.size()
            || nInputboxIndex < KEYBOARD_INPUTBOX_ERROR_INDEX)
    {
        m_strSelectedChar = "";
        return;
    }
    //nInputboxIndex == KEYBOARD_INPUTBOX_ERROR_INDEX的情况即是没有弹出备选框的情况
    if (nInputboxIndex == KEYBOARD_INPUTBOX_ERROR_INDEX)
    {
        nInputboxIndex = 0;
    }
    const vecKeyboardImeKeyTable& tKeyboardImeTable = m_vecKeyboardIme[nImeIndex].m_vecImeKey;
    if (nButtonLIne < 0 || nButtonLIne >= tKeyboardImeTable.size())
    {
        m_strSelectedChar = "";
        return;
    }
    const vecKeyboardImeKeyLine& tKeyboardImeLine = tKeyboardImeTable[nButtonLIne];
    if (nButtonID < 0 || nButtonID >= tKeyboardImeLine.size())
    {
        m_strSelectedChar = "";
        return;
    }
    const SKeyBoardImeKey& tKeyboardImeKey = tKeyboardImeLine[nButtonID];
    if ("" != tKeyboardImeKey.m_strFunction)
    {
        m_strSelectedChar = "";
        return;
    }
    const yl::string& strImeList = tKeyboardImeKey.m_strImeList;
    QString strShowString = toQString(strImeList);
    int nLength = strShowString.size();
    if (nInputboxIndex < 0 || nLength < 1 || nInputboxIndex >= nLength)
    {
        m_strSelectedChar = "";
        return;
    }
    m_strSelectedChar = strShowString.at(nInputboxIndex);
}
