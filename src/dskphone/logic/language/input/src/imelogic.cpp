#include "input_include.h"

#ifdef IF_TEST_MODE
#define DebugPrint(vec, level)  PrintIme(vec, level, __FUNCTION__, __LINE__)
#else
#define DebugPrint(vec, level)  void(0)
#endif

using namespace pugi;
using namespace yl;


ImeKeyMap & ImeKeyMap::operator=(const ImeKeyMap & other)
{
    if (this != &other)
    {
        strName = other.strName;
        for (int i = 0; i < KEY_NUM; ++i)
        {
            keyMap[i] = other.keyMap[i];
        }
    }

    return *this;
}

// 当前标准输入法
static const mapItem STD_IME_MAP[] =
{
    { IT_STD_2aB,       IME_2aB         },
    { IT_STD_ABC,       IME_ABC         },
    { IT_STD_abc,       IME_abc         },
    { IT_STD_Abc,       IME_Abc         },
    { IT_STD_123,       IME_123         },
    { IT_STD_IP,        IME_123_IP      },
    { IT_STD_Dial,      IME_123_Dial    },
    { IT_STD_Pwd_2aB,   IME_2aB_Pwd     },
    { IT_STD_Pwd_abc,   IME_abc_Pwd     },
    { IT_STD_Pwd_ABC,   IME_ABC_Pwd     },
    { IT_STD_Pwd_123,   IME_123_Pwd     },
    { IT_STD_Custom,    IME_DIY         },
};

int CImeLogic::Key2Index(int iKeyCode)
{
    int iIndex = -1;
    switch (iKeyCode)
    {
    case PHONE_KEY_1:
    case PHONE_KEY_2:
    case PHONE_KEY_3:
    case PHONE_KEY_4:
    case PHONE_KEY_5:
    case PHONE_KEY_6:
    case PHONE_KEY_7:
    case PHONE_KEY_8:
    case PHONE_KEY_9:
        {
            iIndex = iKeyCode - PHONE_KEY_1;
        }
        break;
    case PHONE_KEY_0:
        {
            iIndex = 9;
        }
        break;
    case PHONE_KEY_STAR:
        {
            iIndex = 10;
        }
        break;
    case PHONE_KEY_POUND:
        {
            iIndex = 11;
        }
        break;
    default:
        break;
    }

    return iIndex;
}

void CImeLogic::Reload()
{
    IF_TimeTrace;

    vecDIYIme.clear();
    vecBasic.clear();

    vecImeType vecExtern;
    vecImeType vecCustom;

    _LangAttrManager.CreatDefaultLocation();

    // 加载INI格式的自定义输入法
    LoadCustomClassicalIme(vecCustom);
    // 加载XML格式的自定义输入法
    LoadCustomXmlIme(vecCustom);
    // 加载Basic节点
    LoadBasicIme(vecExtern);
    // 创建输入法逻辑层
    CreatImeImp(vecCustom, vecExtern);
}

yl::string CImeLogic::GetLatticeIMEFile()
{
    yl::string strCurrentLang = configParser_GetConfigString(kszCurrentLang);
    //先检查用户上传的路径
    yl::string strTargetFile = commonAPI_FormatString("%s/ime/%s_ime.txt", CONFIG_PATH,
                               strCurrentLang.c_str());
    LPCSTR lpImePath = strTargetFile.c_str();

    if (!pathFileExist(lpImePath))
    {
        //再检查出厂配置
        strTargetFile = commonAPI_FormatString("%s/ime/%s_ime.txt", FACTORY_PATH, strCurrentLang.c_str());
        lpImePath = strTargetFile.c_str();

        if (!pathFileExist(lpImePath))
        {
            if (pathFileExist(szCustomIMEFile))//优先使用config下的ime
            {
                lpImePath = szCustomIMEFile;
            }
            else
            {
                lpImePath = szIMEFile;
            }
        }

        if (!pathFileExist(lpImePath))
        {
            return yl::string();
        }
    }

    return lpImePath;
}

bool CImeLogic::LoadCustomClassicalIme(vecImeType & vec)
{
    IF_TimeTrace;

    yl::string strImeFile = GetLatticeIMEFile();

    // 检查文件是否可访问
    if (strImeFile.empty())
    {
        return false;
    }

    APP_INFO("LoadCustomClassicalIme file [%s]", strImeFile.c_str());

    yl::string strLangTextA = to_utf8(dump_utf16_file(strImeFile));
    LPCSTR lpszText = strLangTextA.c_str();

    yl::string strLineText;

    vecImeType vecItem;
    ImeKeyMap item;

    while (PeekPhraseByCharTokens(lpszText, strLineText, "\n") == 0)
    {
        trim_left(strLineText, " ");

        //判断iImeType
        LPCSTR szUtf8 = strLineText.c_str();
        char szName[128] = {0};

        if (strLineText.find('[') == 0)
        {
            item.strName.clear();

            if (sscanf(szUtf8, "[%[^]]", szName) == 1)
            {
                item.strName = szName;

                // 除话机自带输入法外，其它输入法不区分大小写
                if (!IsStdIME(item.strName))
                {
                    item.strName.to_lower();
                }

                continue;
            }
        }

        if (item.strName.empty())
        {
            continue;
        }

        int iIndex = (int)strLineText[0];

        yl::string::size_type lpos = strLineText.find('"');
        yl::string::size_type rpos = strLineText.rfind('"');

        if (lpos == yl::string::npos || rpos == yl::string::npos || rpos <= lpos)
        {
            continue;
        }

        int iLogicIndex = Key2Index(iIndex);
        if (iLogicIndex >= 0 && iLogicIndex < KEY_NUM)
        {
            yl::wstring str = to_utf16(strLineText.substr(lpos + 1, rpos - lpos - 1));

            item.keyMap[iLogicIndex] = str;
            vecItem.push_back(item);
        }
    }

    for (vecImeType::iterator it = vecItem.begin(); it != vecItem.end(); ++it)
    {
        if (IsVaild(&(*it)))
        {
            vec.push_back(*it);
        }
    }
    return true;
}

bool CImeLogic::LoadBasicIme(vecImeType & vec)
{
    IF_TimeTrace;

    LPCSTR lpImePath = 0;
    if (pathFileExist(szXmlFacIMEFile))
    {
        lpImePath = szXmlFacIMEFile;
    }

    if (textEMPTY(lpImePath))
    {
        return false;
    }

    APP_INFO("LoadBasicIme file [%s]", lpImePath);

    xml_document doc;
    xml_parse_result objResult = doc.load_file(lpImePath);
    if (objResult.status != status_ok)
    {
        return false;
    }

    xml_node nodeRoot = doc.child("YealinkIPPhoneIME");
    if (nodeRoot.empty())
    {
        return false;
    }

    bool bRet = false;
    xml_node xmlChild = nodeRoot.child("basic");
    if (xmlChild)
    {
        bRet |= ParseSubItem(xmlChild, vecBasic);
    }

    xmlChild = nodeRoot.child("extend");
    if (xmlChild)
    {
        LPCSTR lpName = _LangAttrManager.GetName();
        if (!textEMPTY(lpName))
        {
            // 未加載輸入法時，加載字符集輸入法
            if (ParseSubItem(xmlChild, vec, lpName))
            {
                return true;
            }
            else
            {
                lpName = _LangAttrManager.GetSetName();
                if (!textEMPTY(lpName))
                {
                    bRet |= ParseSubItem(xmlChild, vec, lpName);
                }
            }
        }
    }

    return bRet;
}

bool CImeLogic::LoadCustomXmlIme(vecImeType & vec)
{
    IF_TimeTrace;
    yl::string strImeFile = GetLatticeIMEFile();
    if (!strImeFile.empty() && strImeFile != szIMEFile && vec.size() > 0)
    {
        return false;
    }

    LPCSTR lpImePath = szXmlCfgIMEFile;
    if (!pathFileExist(szXmlCfgIMEFile))
    {
        return false;
    }

    xml_document doc;
    xml_parse_result objResult = doc.load_file(lpImePath);
    if (objResult.status != status_ok)
    {
        return false;
    }

    xml_node nodeRoot = doc.child("YealinkIPPhoneIME");
    if (nodeRoot.empty())
    {
        return false;
    }

    return ParseSubItem(nodeRoot, vec);
}

bool CImeLogic::LoadDefaultIme(LPCSTR strIme/* = NULL*/)
{
    if (textEMPTY(strIme))
    {
        return false;
    }

    ImeKeyMap * pKeyMap = NULL;
    for (vecImeType::iterator it = vecBasic.begin(); it != vecBasic.end(); ++it)
    {
        if (it->strName == strIme)
        {
            // 已经合法不再继续
            if (IsVaild(strIme))
            {
                return false;
            }

            pKeyMap = &(*it);
            break;
        }
    }

    APP_INFO("LoadDefaultIme ime=[%s] keymap=[%p]", strIme, pKeyMap);

    ImeKeyMap iItem;
    // 在输入法中没有找到
    if (pKeyMap == NULL)
    {
        pKeyMap = &iItem;
    }

    // 应产品部要求*键的输入
    char szDataEnStar[128] = { 42, 46, 44, 39, 63, 92, 45, 40, 41, 64, 47, 58, 95, 59, 43, \
                               38, 37, 61, 60, 62, 239, 191, 161, 36, 239, 191, 165, 194, 164, 91, 93, 123, 125, 126, \
                               94, 194, 161, 194, 191, 194, 167, 35, 0, 0, 0
                             };

    yl::wstring strStarW = to_utf16(szDataEnStar);

    if (strcmp(strIme, IME_123) == 0
            || strcmp(strIme, IME_123_IP) == 0
            || strcmp(strIme, IME_123_Dial) == 0
            || strcmp(strIme, IME_123_Pwd) == 0)
    {
        pKeyMap->strName = strIme;
        pKeyMap->keyMap[0] = L"1";
        pKeyMap->keyMap[1] = L"2";
        pKeyMap->keyMap[2] = L"3";
        pKeyMap->keyMap[3] = L"4";
        pKeyMap->keyMap[4] = L"5";
        pKeyMap->keyMap[5] = L"6";
        pKeyMap->keyMap[6] = L"7";
        pKeyMap->keyMap[7] = L"8";
        pKeyMap->keyMap[8] = L"9";
        pKeyMap->keyMap[9] = L"0";
        pKeyMap->keyMap[11] = L"#";

        if (strcmp(strIme, IME_123_Dial) == 0)
        {
            pKeyMap->keyMap[10] = L"*.:@";
        }
        else if (strcmp(strIme, IME_123_Pwd) == 0)
        {
            pKeyMap->keyMap[10] = strStarW;
        }
        else
        {
            pKeyMap->keyMap[10] = L".*:/@[]";
        }
    }
    else if (strcmp(strIme, IME_abc) == 0
             || strcmp(strIme, IME_abc_Pwd) == 0
             || strcmp(strIme, IME_Abc) == 0)
    {
        pKeyMap->strName = strIme;
        pKeyMap->keyMap[0] = L"";
        pKeyMap->keyMap[1] = L"abc";
        pKeyMap->keyMap[2] = L"def";
        pKeyMap->keyMap[3] = L"ghi";
        pKeyMap->keyMap[4] = L"jkl";
        pKeyMap->keyMap[5] = L"mno";
        pKeyMap->keyMap[6] = L"pqrs";
        pKeyMap->keyMap[7] = L"tuv";
        pKeyMap->keyMap[8] = L"wxyz";
        pKeyMap->keyMap[9] = L" ";
        pKeyMap->keyMap[10] = strStarW;
        pKeyMap->keyMap[11] = L"#";
    }
    else if (strcmp(strIme, IME_ABC) == 0)
    {
        pKeyMap->strName = strIme;
        pKeyMap->keyMap[0] = L"";
        pKeyMap->keyMap[1] = L"ABC";
        pKeyMap->keyMap[2] = L"DEF";
        pKeyMap->keyMap[3] = L"GHI";
        pKeyMap->keyMap[4] = L"JKL";
        pKeyMap->keyMap[5] = L"MNO";
        pKeyMap->keyMap[6] = L"PQRS";
        pKeyMap->keyMap[7] = L"TUV";
        pKeyMap->keyMap[8] = L"WXYZ";
        pKeyMap->keyMap[9] = L" ";
        pKeyMap->keyMap[10] = strStarW;
        pKeyMap->keyMap[11] = L"#";
    }
    else if (strcmp(strIme, IME_2aB) == 0)
    {
        pKeyMap->strName = strIme;
        pKeyMap->keyMap[0] = L"1";
        pKeyMap->keyMap[1] = L"2abcABC";
        pKeyMap->keyMap[2] = L"3defDEF";
        pKeyMap->keyMap[3] = L"4ghiGHI";
        pKeyMap->keyMap[4] = L"5jklJKL";
        pKeyMap->keyMap[5] = L"6mnoMNO";
        pKeyMap->keyMap[6] = L"7pqrsPQRS";
        pKeyMap->keyMap[7] = L"8tuvTUV";
        pKeyMap->keyMap[8] = L"9wxyzWXYZ";
        pKeyMap->keyMap[9] = L"0";
        pKeyMap->keyMap[10] = strStarW;
        pKeyMap->keyMap[11] = L"#";
    }
    else
    {
        return false;
    }

    if (pKeyMap == &iItem)
    {
        vecBasic.push_back(iItem);
    }

    return true;
}

bool CImeLogic::ParseSubItem(const xml_node & nodeRoot, vecImeType & vec, LPCTSTR lpNode/* = NULL*/)
{
    if (nodeRoot.empty())
    {
        return false;
    }

    APP_INFO("ParseSubItem node [%s]", lpNode);

    IF_TimeTrace;

    bool bFindNode = false;

    xml_node xmlSubRoot = nodeRoot.child("encode");
    while (xmlSubRoot)
    {
        xml_node xmlSetNode = xmlSubRoot;
        xmlSubRoot = xmlSubRoot.next_sibling("encode");

        yl::string strType = xmlSetNode.attribute("type").as_string();
        strType.to_lower();

        // 跳过非目标节点
        if (!textEMPTY(lpNode) && strType != lpNode)
        {
            continue;
        }

        xml_node xmlMethod = xmlSetNode.child("method");
        while (xmlMethod)
        {
            xml_node xmlImeNode = xmlMethod;
            xmlMethod = xmlMethod.next_sibling("method");

            yl::string strName = xmlImeNode.attribute("type").as_string();
            // 跳过非目标节点
            if (strName.empty())
            {
                continue;
            }

            if (!IsStdIME(strName))
            {
                strName.to_lower();
            }

            ImeKeyMap iItem;
            bool bAdd = false;

            // refer的处理
            LPCSTR LPRef = xmlImeNode.attribute("refer").as_string();
            if (!textEMPTY(LPRef))
            {
                for (vecImeType::iterator it = vec.begin(); it != vec.end(); ++it)
                {
                    if (it->strName == LPRef)
                    {
                        iItem = *it;
                        bAdd = true;
                        break;
                    }
                }
            }

            iItem.strName = strName;

            xml_node xmlItem = xmlImeNode.child("item");
            while (xmlItem)
            {
                LPCSTR pKey = xmlItem.attribute("key").as_string();
                if (textEMPTY(pKey))
                {
                    break;
                }

                int iKey = Key2Index(*pKey);
                yl::string strValue = xmlItem.attribute("value").as_string();
                yl::wstring strValueW = to_utf16(strValue);

                if (iKey >= 0 && iKey < KEY_NUM)
                {
                    bAdd = true;
                    iItem.keyMap[iKey] = strValueW;
                }

                xmlItem = xmlItem.next_sibling("item");
            }

            if (bAdd && IsVaild(&iItem))
            {
                bFindNode = true;
                vec.push_back(iItem);
            }
        }
    }

    return bFindNode;
}

bool CImeLogic::CreatImeImp(vecImeType & vecCustom, vecImeType & vecExtern)
{
    APP_INFO("basic=[%d] extern=[%d] custom=[%d]", vecBasic.size(), vecExtern.size(), vecCustom.size());
    // 先加载Baisc输入法
    vecImeType & vecIncr = vecCustom.size() > 0 ? vecCustom : vecExtern;

    DebugPrint(vecBasic, 0);
    DebugPrint(vecCustom, 0);
    DebugPrint(vecExtern, 0);

    if (vecIncr.size() > 0)
    {
        for (vecImeType::iterator it = vecIncr.begin(); it != vecIncr.end(); ++it)
        {
            bool bAdd = true;
            for (vecImeType::iterator itImp = vecBasic.begin(); itImp != vecBasic.end(); ++itImp)
            {
                // 同名覆盖
                if (it->strName == itImp->strName)
                {
                    IF_MODE_LOG_TRACE("overwrite ime name [%s]", it->strName.c_str());
                    bAdd = false;
                    *itImp = *it;
                    break;
                }
            }

            if (bAdd)
            {
                IF_MODE_LOG_TRACE("add ime name[%s]", it->strName.c_str());
                // 非同名添加
                vecBasic.push_back(*it);
            }
        }
    }

    // 占用一个输入法位置
    if (_LangAttrManager.GetLangAttr() & IA_IME_BUILDIN)
    {
        ImeKeyMap keyMay;
        LPCSTR lpName = _LangAttrManager.GetName();

        if (!textEMPTY(lpName))
        {
            keyMay.strName = lpName;
            vecBasic.push_back(keyMay);
        }
    }

    // 将自定义输入法提取出来，相对STD_IME_MAP的输入法而言，其它都是自定义
    for (vecImeType::iterator it = vecBasic.begin(); it != vecBasic.end(); ++it)
    {
        if (!IsStdIME(it->strName))
        {
            IF_MODE_LOG_TRACE("custom ime is [%s]", it->strName.c_str());
            vecDIYIme.push_back(&(*it));
        }
    }

#if 1
    // 默认输入法
    for (int i = 0; i < sizeof(STD_IME_MAP) / sizeof(STD_IME_MAP[0]); ++i)
    {
        LoadDefaultIme(STD_IME_MAP[i].strName);
    }
#endif

    APP_INFO("basic=[%d] custom=[%d]", vecBasic.size(), vecDIYIme.size());

    DebugPrint(vecBasic, 1);
    // 最终输入法存在basic里面
    return vecBasic.size();
}

bool CImeLogic::IsStdIME(const yl::string & strName)
{
    if (strName.empty())
    {
        return false;
    }

    return Str2KeyEx(strName, STD_IME_MAP) != IT_STD_NONE;
}


LPCTSTR CImeLogic::Ime2Name(IME_ITEM_TYPE eType, UINT iIndex/* = 0*/)
{
    IF_MODE_LOG_TRACE("type=[%d] index=[%d] size=[%d]", eType, iIndex, vecDIYIme.size());

    if (eType == IT_STD_Custom)
    {
        if (vecDIYIme.size() > iIndex)
        {
            return vecDIYIme[iIndex]->strName.c_str();
        }

        return NULL;
    }

    return Key2StrEx(eType, STD_IME_MAP);
}

IME_ITEM_TYPE CImeLogic::Name2Ime(const yl::string & strIme)
{
    if (strIme.empty())
    {
        return IT_STD_NONE;
    }

    IME_ITEM_TYPE eType = (IME_ITEM_TYPE)Str2KeyEx(strIme, STD_IME_MAP);
    if (eType == IT_STD_NONE)
    {
        yl::string strLowerName(strIme);
        strLowerName.to_lower();

        for (vecImeItemPt::iterator it = vecDIYIme.begin(); it != vecDIYIme.end(); ++it)
        {
            if (strLowerName == (*it)->strName)
            {
                return IT_STD_Custom;
            }
        }
    }

    return eType;
}

LPCWSTR CImeLogic::IMEFromKey(int iKeyCode, IME_ITEM_TYPE eType, UINT iCusIndex/* = 0*/)
{
    int iIndex = Key2Index(iKeyCode);

    IF_MODE_LOG_TRACE("index=[%d] type=[%d] name=[%s] size=[%d]",
                      iIndex, eType, Ime2Name(eType, iCusIndex), vecBasic.size());

    if (iIndex < 0 || iIndex >= KEY_NUM)
    {
        return NULL;
    }

    if (eType == IT_STD_Custom)
    {
        if (vecDIYIme.size() > iCusIndex)
        {
            return vecDIYIme[iCusIndex]->keyMap[iIndex].c_str();
        }
    }
    else
    {
        LPCSTR lpImeName = Ime2Name(eType, iCusIndex);
        if (!textEMPTY(lpImeName))
        {
            for (vecImeType::iterator it = vecBasic.begin(); it != vecBasic.end(); ++it)
            {
                if (it->strName == lpImeName)
                {
                    return it->keyMap[iIndex].c_str();
                }
            }
        }
    }

    return NULL;
}

ImeKeyMap * CImeLogic::GetKeyMap(LPCSTR lpszIMEName)
{
    if (textEMPTY(lpszIMEName))
    {
        return NULL;
    }

    for (vecImeType::iterator it = vecBasic.begin(); it != vecBasic.end(); ++it)
    {
        if (it->strName == lpszIMEName)
        {
            return &(*it);
        }
    }

    return NULL;
}

bool CImeLogic::IsVaild(LPCTSTR lpImeName)
{
    return IsVaild(GetKeyMap(lpImeName));
}

bool CImeLogic::IsVaild(IME_ITEM_TYPE eType, UINT iIndex /*= 0*/)
{
    if (eType == IT_STD_Custom)
    {
        return vecDIYIme.size() > iIndex;
    }

    return strlen(Key2StrEx(eType, STD_IME_MAP)) > 0;
}

bool CImeLogic::IsVaild(ImeKeyMap * pKeyMap)
{
    if (pKeyMap == NULL)
    {
        return false;
    }

    // 参照旧版本，检测3、5两个节点
    return !pKeyMap->keyMap[3].empty() && !pKeyMap->keyMap[5].empty();
}

#ifdef IF_TEST_MODE
void CImeLogic::PrintIme(const vecImeType & vec, int iLevel /*= 1*/, LPCSTR strFun, int iLine)
{
    IF_MODE_LOG_TRACE("callbyfun=[%s] position=[%d] size=[%d]", strFun, iLine, vec.size());
    for (int i = 0; i < vec.size(); ++i)
    {
        PrintImeKeyMap(vec[i], i, iLevel);
    }
}

void CImeLogic::PrintImeKeyMap(const ImeKeyMap & keyMapPair, int iIndex, int iLevel)
{
    IF_MODE_LOG_TRACE(" -- index=[%d] type=[%s]", iIndex + 1, keyMapPair.strName.c_str());
    if (iLevel > 0)
    {
        for (int k = 0; k < KEY_NUM; ++k)
        {
            IF_MODE_LOG_TRACE(" -- -- key=[%d] value=[%s]", k, to_utf8(keyMapPair.keyMap[k]).c_str());
        }
    }
}
#endif





