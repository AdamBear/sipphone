#include "lang_include.h"
#include "input/include/imelogic.h"
#include "input/include/langattrmanager.h"
#include "adapterbox/include/modadapterbox.h"
#include "devicelib/phonedevice.h"
#include "commonunits/t2xpath.h"

#define BMP_HASH_SIZE       128
#define LAN_PATH_MAX        512

using namespace pugi;
using namespace yl;


#ifdef IF_SUPPORT_XML_TRANS
bool StrEndWith(const yl::string & strHost, const yl::string & strMask, bool bCaseSensitive = true)
{
    if (strHost.length() < strMask.length())
    {
        return false;
    }
    // 两者都为空的情况
    else if (strHost.empty() && strMask.empty())
    {
        return true;
    }

    const yl::string & strHostEx = bCaseSensitive ? strHost : yl::string(strHost).to_lower();
    const yl::string & strMaskEx = bCaseSensitive ? strMask : yl::string(strMask).to_lower();

    yl::string::size_type iPos = strHostEx.find(strMaskEx);
    if (iPos != yl::string::npos)
    {
        return iPos == strHostEx.length() - strMaskEx.length();
    }

    return false;
}

//输入参数：“源内容”字符串
//返回值：通过特定算法取得的hash索引值
static inline UINT item_Hash(LPCSTR lpszText)
{
#if 0
    UINT nHash = 0;

    //以遇到'\0'标识循环结束
    while (*lpszText != '\0')
    {
        nHash = (nHash << 5) + nHash + *lpszText++;
    }
    //链表空间大小设置为BMP_HASH_SIZE，因此通过取模使得返回值在0到(BMP_HASH_SIZE-1)范围内
    return nHash % BMP_HASH_SIZE;
#else
    // JS Hash 目前集速度和离散综合表现最好的算法，替换以上实现 By liany @2015.10.10
    size_t hash = 1315423911;

    while (*lpszText)
    {
        hash ^= ((hash << 5) + (*lpszText++) + (hash >> 2));
    }

    return (hash & 0x7FFFFFFF) % BMP_HASH_SIZE;
#endif
}
#endif


bool CTransLogic::InitializeLangHelper(const yl::string & strText)
{
    IF_TimeTrace;

    //定义翻译文件路径
    yl::string strFilePath;
    //取得翻译文件路径
    if (!GetFilePathByLanguage(strFilePath, strText))
    {
        return false;
    }

    IF_MODE_LOG_TRACE("file=[%s]", strFilePath.c_str());

    //加载翻译文件内容到类定义的数据结构
#ifdef IF_SUPPORT_XML_TRANS
    if (StrEndWith(strFilePath, ".xml"))
    {
        LoadXmlLang(strFilePath.c_str(), strText.c_str());
    }
    else
#endif
    {
        LoadLanguage(strFilePath.c_str());
    }

    return true;
}

bool CTransLogic::LoadLanguage(LPCSTR szLangFile)
{
    if (textEMPTY(szLangFile))
    {
        return false;
    }

    //如果文件不存在，返回false
    if (!pathFileExist(szLangFile))
    {
        APP_WARN("Language File Name(%s) is inexist!", szLangFile);
        return false;
    }

    yl::string strLangTextA;
#ifdef IF_SUPPORT_UTF8_TRANS
    if (IsUTF8Encode(szLangFile))
    {
        strLangTextA = dump_file(szLangFile);
    }
    else
#endif
    {
        yl::wstring strLangTextw = dump_utf16_file(szLangFile);
        strLangTextA = to_utf8(strLangTextw);
    }
    if (strLangTextA.back() != '\n')
    {
        // 如果文件末尾没有新行，会导致PeekPhraseByCharTokens无法识别最后一行，因此需要添加"\n"
        APP_INFO("No new line at end of language file '%s', append it.", szLangFile);
        strLangTextA.append("\n");
    }
    LPCSTR lpszText = strLangTextA.c_str();
    yl::string strLineText;
    yl::string strKey;
    yl::string strValue;
    yl::string strNono;

    while (PeekPhraseByCharTokens(lpszText, strLineText, "\n") == 0)
    {
        LPCSTR lpszLineText = strLineText.c_str();

        if (*lpszLineText == '"')
        {
            lpszLineText = PeekContentFromQuotaString(lpszLineText + 1, strKey);
            lpszLineText = SkipWhiteSpace(lpszLineText);
            if (*lpszLineText != '=')
            {
                continue;
            }
            ++lpszLineText;
        }
        else if (PeekPhraseByCharTokens(lpszLineText, strNono, "=") != 0)
        {
            continue;
        }

        lpszLineText = SkipWhiteSpace(lpszLineText);
        if (*lpszLineText == '"')
        {
            lpszLineText = PeekContentFromQuotaString(lpszLineText + 1, strValue);
        }
        else
        {
            PeekPhraseByCharTokens(lpszLineText, strValue, "\n");
        }

        if (strKey.empty())
        {
            continue;
        }

        operator[](strKey.to_lower()) = strValue;

        _LangAttrManager.CalRefCount(strValue);
    }

    return true;
}

bool CTransLogic::GetLangNameFromFileName(const yl::string & strFileName, yl::string & strLangName)
{
    char szLang[LAN_PATH_MAX] = { 0 };
    if (sscanf(strFileName.c_str(), "%*d.GUI.%[^.]", szLang) == 1
            || sscanf(strFileName.c_str(), "lang%*c%[^.]", szLang) == 1
            || sscanf(strFileName.c_str(), "%[^.].txt", szLang) == 1)
    {
        strLangName = szLang;
        return true;
    }
    return false;
}

//翻译实现接口
//输入参数类型：char*类型
LPCSTR CTransLogic::Translate(LPCSTR lpszText)
{
    //指针判空
    if (textEMPTY(lpszText))
    {
        return lpszText;
    }

    yl::string strKey = lpszText;
    strKey.to_lower();

    iterator it = find(strKey);
    if (it == end())
    {
        //源内容没有添加到翻译文件列表，那么返回“源字符串”
        return lpszText;
    }

    return it->second.c_str();
}

bool CTransLogic::GetFilePathByLanguage(yl::string & strFilePath, const yl::string & strLangName)
{
    if (strLangName.empty())
    {
        return false;
    }

    //优先使用用户上传的文件
    yl::string strLanguagePath = res_getPathByName(RESOURCE_PATH_LANG);
    LPCSTR strPathArr[] = {USER_LANG_FILE_PATH, strLanguagePath.c_str()};
    for (int i = 0; i < sizeof(strPathArr) / sizeof(strPathArr[0]); i++)
    {
        ListLangFileName listLangFile;
        LPCSTR strLangPath = strPathArr[i];

        if (!commonAPI_getFilesInDirectoryByExt(&listLangFile, strLangPath, "lang,txt,xml"))
        {
            //WARNING("Can't find lang file,%s!", PHONE_RESOURCE_PATH "language");
            continue;
        }

        for (ListLangFileName::iterator it = listLangFile.begin(); it != listLangFile.end(); ++it)
        {
            yl::string & strLangFile = *it;

            if (strLangFile.empty())
            {
                continue;
            }

#ifdef IF_SUPPORT_XML_TRANS
            if (StrEndWith(strLangFile, ".xml"))
            {
                ListLangFileName listXmlLangFileName;
                GetLangFileNameFromXml(strLangPath + strLangFile, listXmlLangFileName);
                for (ListLangFileName::iterator it = listXmlLangFileName.begin(); it != listXmlLangFileName.end();
                        ++it)
                {
                    yl::string strCurLangName;
                    if (GetLangNameFromFileName(*it, strCurLangName) && strLangName == strCurLangName)
                    {
                        strFilePath = strLangPath + strLangFile;
                        return true;
                    }
                }
            }
            else
#endif
            {
                yl::string strCurLangName;
                if (GetLangNameFromFileName(strLangFile, strCurLangName) && strLangName == strCurLangName)
                {
                    strFilePath = strLangPath + strLangFile;
                    return true;
                }
            }
        }
    }

    return false;
}

bool CTransLogic::GetAllLangInfo(VectorLangInfo & objLangArray)
{
    IF_TimeTrace;
    APP_INFO("CTransLogic::GetAllLangInfo");

    ListLangFileName listLangFile;

    objLangArray.clear();

    yl::string strLanguagePath = res_getPathByName(RESOURCE_PATH_LANG);
    LPCSTR pathArr[] = {USER_LANG_FILE_PATH, strLanguagePath.c_str()};

    for (int i = 0; i < sizeof(pathArr) / sizeof(pathArr[0]); i++)
    {
        ListLangFileName listLangFileInCurrentDirectory;
        if (!commonAPI_getFilesInDirectoryByExt(&listLangFileInCurrentDirectory, pathArr[i],
                                                "lang,txt,xml"))
        {
            APP_WARN("Can't find lang file in %s", pathArr[i]);
            continue;
        }

        for (ListLangFileName::iterator it = listLangFileInCurrentDirectory.begin();
                it != listLangFileInCurrentDirectory.end(); ++it)
        {
            yl::string & strLangFileName = *it;
#ifdef IF_SUPPORT_XML_TRANS
            if (StrEndWith(strLangFileName, ".xml"))
            {
                // 获取xml文件中的语言
                GetLangFileNameFromXml((pathArr[i] + strLangFileName).c_str(), listLangFile);
            }
            else
#endif
            {
                listLangFile.push_back(strLangFileName);
            }
        }
    }

    // 先加載後排序，可確保排序
    SortStringList(listLangFile);

    for (ListLangFileName::iterator it = listLangFile.begin(); it != listLangFile.end(); it++)
    {
        yl::string strLangName;
        if (GetLangNameFromFileName(*it, strLangName))
        {
            Add2Vector(objLangArray, strLangName);
        }
        else
        {
            APP_WARN("Init lang Parameters from filename [%s] error, igored!", (*it).c_str());
        }
    }

    return objLangArray.size();
}

bool CTransLogic::Add2Vector(VectorLangInfo & listLangNames, const yl::string & strName)
{
    if (yl::find(listLangNames.begin(), listLangNames.end(), strName) == listLangNames.end())
    {
        listLangNames.push_back(strName);
        return true;
    }
    else
    {
        APP_ERR("duplication lang name=[%s]", strName.c_str());
        return false;
    }
}

void CTransLogic::Reload()
{
    // 清理当前翻译
    clear();
    // 加载新翻译
    InitializeLangHelper(configParser_GetConfigString(kszCurrentLang));
}

#ifdef IF_SUPPORT_XML_TRANS
void CTransLogic::GetLangFileNameFromXml(const yl::string & strLangFile,
        ListLangFileName & listLangFileName)
{
    APP_INFO("CTransLogic::GetLangNameFromXml(%s, vector)", strLangFile.c_str());
    if (strLangFile.empty())
    {
        return;
    }

    xml_document docLang;
    if (!docLang.load_file(strLangFile.c_str()))
    {
        APP_ERR("Load xml:%s Failed!", strLangFile.c_str());
        return;
    }

    yl::string strKey, strValue;
    xml_node ItemNode = docLang.child("Lang");
    if (ItemNode)
    {
        xml_node RootNode = docLang.child("Lang");
        for (xml_attribute_iterator it = RootNode.attributes_begin(); it != RootNode.attributes_end(); ++it)
        {
            listLangFileName.push_back(it->value());
        }
    }
}

void CTransLogic::LoadXmlLang(LPCSTR lpszLangFile, LPCSTR lpLangName)
{
    APP_INFO("CTransLogic::LoadXmlLang(%s, %s)", lpszLangFile, lpLangName);
    if (textEMPTY(lpLangName) || textEMPTY(lpszLangFile))
    {
        return;
    }

    yl::string strPath = lpszLangFile;
    if (!pathFileExist(strPath.c_str()))
    {
        strPath = res_getPathByName(RESOURCE_PATH_LANG);
        strPath += "lang.xml";
    }

    // 读取xml类型的翻译
    xml_document docLang;
    if (!docLang.load_file(strPath.c_str()))
    {
        return;
    }
    xml_node root = docLang.child("Lang");
    if (!root)
    {
        return;
    }

    // 遍历查找语言对应的缩写(属性名)
    xml_attribute_iterator it = root.attributes_begin();
    for (; it != root.attributes_end(); ++it)
    {
        yl::string strLangName;
        if (GetLangNameFromFileName(it->value(), strLangName) && strLangName == lpLangName)
        {
            break;
        }
    }
    if (it == root.attributes_end())
    {
        return;
    }
    yl::string strLangAbbr = it->name();
    yl::string strKey, strValue;
    for (xml_node ItemNode = root.child("Item"); ItemNode; ItemNode = ItemNode.next_sibling())
    {
        strKey = ItemNode.attribute("Sor").value();
        if (strKey.empty())
        {
            continue;
        }

        xml_attribute attr = ItemNode.attribute(strLangAbbr.c_str());
        if (attr.empty())
        {
            //APP_WARN("No translate of '%s' for key '%s'!", strLangAbbr.c_str(), strKey.c_str());
            continue;
        }
        strValue = attr.value();
        //APP_INFO("Translate: key=%s, value=%s", strKey.c_str(), strValue.c_str());

        operator[](strKey.to_lower()) = strValue;

        _LangAttrManager.CalRefCount(strValue);
    }
}
#endif
