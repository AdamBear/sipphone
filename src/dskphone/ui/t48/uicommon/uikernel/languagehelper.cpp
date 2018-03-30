#include "languagehelper.h"
#include <QtGui>
#include "qtcommon/qmisc.h"
#include <path_define.h>
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "features-define.h"
#include "commonapi/systemhelperapi.h"
#include "dsklog/log.h"
#include "ylstl/ylstringalgorithm.h"
#include "commonapi/stringhelperapi.h"

#include "language/common/common_function.h"
#include "language/translate/include/unicode_detect.h"
#include "language/translate/include/translate.h"

#include "resource/modresource.h"
#include "commonunits/t2xpath.h"
//是否支持自定义字体
#define IF_SUPPORT_CUSTOMER_FONT    1

#define LANG_FLAG           "?`"
#define LANG_STRING_LEN_FLAG    2   //用于存储语言字符串长度的位数
#define LAN_PATH_MAX        512

using namespace yl;

CLanguageHelper::CLanguageHelper()
{
    InitializeLangHelper();
}

//翻译实例对象数据初始化
//输入参数：要翻译语言名称
bool CLanguageHelper::InitializeLangHelper(const char * szLangName)
{
#if IF_SUPPORT_CUSTOMER_FONT
    //通过语言名称取得翻译语言字体名称
    //切换语言后，重新设置一次字体
    QFont font;
    font.setPointSize(18);
    font.setWeight(75);
    font.setBold(true);
    font.setFamily(GetFontByLanguage(szLangName));
    QApplication::setFont(font);
    //  GetFontByLanguage(szLangName);
#endif
    return true;
}

QString CLanguageHelper::ParseLangText(const QString & strText)
{
#if IF_SUPPORT_AUTO_TRANSLATE
    if (strText.isEmpty())
    {
        return strText;
    }

    yl::string strTran = strText.toUtf8().data();

    while (1)
    {
        //保存翻译标记的位置
        int iPos = strTran.find(LANG_FLAG, 0);
        if (iPos == yl::string::npos)
        {
            break;
        }

        //取得第一个要翻译的长度
        int iLength = atoi(strTran.substr(iPos + strlen(LANG_FLAG), LANG_STRING_LEN_FLAG).c_str());


        //提取第一段
        yl::string strFirst = strTran.substr(0, iPos);


        if (iPos + iLength + strlen(LANG_FLAG) + LANG_STRING_LEN_FLAG > strTran.length())
        {
            //容错
            return strText;
        }
        //提取最后一段
        yl::string strThird = strTran.substr(iPos + iLength + strlen(LANG_FLAG) + LANG_STRING_LEN_FLAG);

        //翻译中间段，生成新的中间段
        if (iPos + strlen(LANG_FLAG) + LANG_STRING_LEN_FLAG >= strTran.length()
                || iPos + strlen(LANG_FLAG) + LANG_STRING_LEN_FLAG + iLength > strTran.length())
        {
            //容错
            return strText;
        }
        yl::string strToTrans = strTran.substr(iPos + strlen(LANG_FLAG) + LANG_STRING_LEN_FLAG, iLength);
        // #if TEST_TRAN_LEN
        //      strTran = strFirst + strToTrans + strThird;
        // #else
        yl::string strSecond = _TransLogic.Translate(strToTrans.c_str());

        //重新组合三段
        strTran = strFirst + strSecond + strThird;
//#endif
    }

    //没有翻译字段时，返回处理完成的字符串
    return toQString(strTran.c_str());
#else
    return strText;
#endif
}

QString CLanguageHelper::TranslateText(const QString & strText)
{
    return TranslateText(fromQString(strText).c_str());
}

QString CLanguageHelper::TranslateText(LPCTSTR lpszText)
{
#if IF_SUPPORT_AUTO_TRANSLATE
    if (lpszText == NULL
            || strcmp(lpszText, "") == 0)
    {
        //字符串为空，返回空
        return "";
    }

    QString strTran = lpszText;
    if (!strTran.contains(LANG_FLAG))
    {
        strTran.clear();
        strTran.sprintf("%s%.2d%s", LANG_FLAG, strlen(lpszText), lpszText);
    }

    return strTran;
#else
    return toQString(_TransLogic.Translate(lpszText));
#endif
}

LPCTSTR CLanguageHelper::GetFontByLanguage(const char * lpszLangName /*= NULL*/)
{
    yl::string strLangName = lpszLangName;

    //字符串判空
    if (strLangName.empty())
    {
        strLangName = configParser_GetConfigString(kszCurrentLang);
    }

    if (m_strActiveFont.empty())
    {
        //定义一个存放CLangInfo类型的数据链表
        VectorLang objLangArray;
        //CLangInfo类型的取得链表数据
        if (GetAllLangInfo(objLangArray))
        {
            //通过语言名称遍历查找出字体类型
            for (int nIndex = 0; nIndex < objLangArray.size(); nIndex++)
            {
                if (objLangArray[nIndex].strOrigName == strLangName)
                {
                    //找到相应语言字体然后存字体信息到成员变量m_strActiveFont
                    m_strActiveFont = objLangArray[nIndex].strFontName;
                    break;
                }
            }
        }

        //取得字体失败，默认为：“Tahoma”字体
        if (m_strActiveFont.empty())
        {
            m_strActiveFont = "Tahoma";
        }

        //定义存放字体的数据链表
        YLList<yl::string> listFontFile;
        bool bFind = false;
        //在相应的目录下加载字体信息到字体链表当中
        if (!commonAPI_getFilesInDirectoryByExt(&listFontFile,
                                                res_getPathByName(RESOURCE_PATH_FONTS).c_str(), "ttf"))
        {
            YLList<yl::string>::iterator it = listFontFile.begin();
            //遍历查找
            for (; it != listFontFile.end(); ++it)
            {
                yl::string & strFontPath = *it;
                //如果m_strActiveFont为strFontPath子串那么返回非NULL,表示存在该字体名称
                if (strFontPath.find(m_strActiveFont.c_str()) != yl::string::npos)
                {
                    bFind = true;
                    break;
                }
            }
        }

        //给定的字体在字体文件列表当中不存在，设置默认字体类型为："Tahoma"
        if (!bFind)
        {
            m_strActiveFont = "Tahoma";
        }
    }

    return m_strActiveFont.c_str();
}

bool CLanguageHelper::GetAllLangInfo(VectorLang & objLangArray)
{
    YLList<yl::string> listLangFile;
    CLangInfo   langInfo;
    LPCSTR strPath;
    bool bHaveLanFile = false;

    objLangArray.clear();
    yl::string strResPath = res_getPathByName(RESOURCE_PATH_LANG);
    LPCSTR pathArr[2] = {USER_LANG_FILE_PATH, strResPath.c_str()};
    for (int i = 0; i < sizeof(pathArr) / sizeof(pathArr[0]); i++)
    {
        LPCSTR strPath = pathArr[i];
        listLangFile.clear();

        if (!commonAPI_getFilesInDirectoryByExt(&listLangFile, strPath, "lang,txt"))
        {
            //WARNING("Can't find lang file,%s!", PHONE_RESOURCE_PATH "language");
            continue;
        }
        bHaveLanFile = true;

        SortStringList(listLangFile);

        YLList<yl::string>::iterator it = listLangFile.begin();
        for (; it != listLangFile.end(); ++it)
        {
            yl::string & strLangPath = *it;

            if (!strLangPath.empty())
            {
                //mark to NOT OK
                bool done = false;
                int  idLang;
                char szOrigLang[LAN_PATH_MAX] = {0};
                char ch;

                langInfo.strLangFile = strPath;
                langInfo.strLangFile += strLangPath;

                string strLangTextA;
#ifdef IF_SUPPORT_UTF8_TRANS
                if (IsUTF8Encode(langInfo.strLangFile.c_str()))
                {
                    strLangTextA = dump_file(langInfo.strLangFile);
                }
                else
#endif
                {
                    wstring strLangTextw = dump_utf16_file(langInfo.strLangFile);
                    strLangTextA = to_utf8(strLangTextw);
                }

                if (!strLangTextA.empty())
                {
                    if (sscanf(strLangPath.c_str(), "%d.GUI.%[^.]", &idLang, szOrigLang) != 2
                            && sscanf(strLangPath.c_str(), "lang%c%[^.]", &ch, szOrigLang) != 2
                            && sscanf(strLangPath.c_str(), "%[^.]txt", szOrigLang) != 1)
                    {
                        APP_WARN("Init lang Parameters from filename [%s] error, igored!", strLangPath.c_str());
                        continue;
                    }

                    langInfo.strOrigName = szOrigLang;

                    int nPosName = string::npos;
                    int nPosFont = string::npos;
                    LPCSTR lpszText = strLangTextA.c_str();
                    string strLineText;

                    while (PeekPhraseByCharTokens(lpszText, strLineText, "\n") == 0)
                    {
                        if (nPosName == string::npos)
                        {
                            if ((nPosName = strLineText.find("Name=")) != string::npos)
                            {
                                //set original name
                                langInfo.strTranName = commonAPI_TrimString(strLineText.substr(strLineText.find('=') + 1), " ");
                                done = true;
                            }
                        }

                        if (nPosFont == string::npos)
                        {
                            if ((nPosFont = strLineText.find("FONT=")) != string::npos)
                            {
                                langInfo.strFontName = commonAPI_TrimString(strLineText.substr(strLineText.find('=') + 1), " ");
                            }
                        }

                        if (nPosName != string::npos
                                && nPosFont != string::npos)
                        {
                            //find name and font
                            break;
                        }
                    }

                    if (!done)
                    {
                        //如果文件中 "Name ="字段为空，显示名赋值为文件名中的语言标示
                        langInfo.strTranName = langInfo.strOrigName;
                    }

                    bool bExist = false;
                    //如果用户上传的语言跟系统有重复，那么使用用户上传的文件代替系统文件
                    //http://192.168.1.99/Bug.php?BugID=23104
                    for (int j = 0; j < objLangArray.size(); ++j)
                    {
                        CLangInfo & refLang = objLangArray[j];
                        if (refLang.strOrigName.compare(szOrigLang) == 0)
                        {
                            //标记已存在
                            bExist = true;

                            //判断重复的对象是系统的还是用户文件之间的重复，只有重复系统文件才覆盖
                            if (strstr(refLang.strLangFile.c_str(), (USER_LANG_FILE_PATH)) == NULL
                                    && strstr(refLang.strLangFile.c_str(), (res_getPathByName(RESOURCE_PATH_LANG).c_str())) != NULL)
                            {
                                objLangArray[j].strLangFile = langInfo.strLangFile;
                                objLangArray[j].strTranName = langInfo.strTranName;
                                objLangArray[j].strFontName = langInfo.strFontName;
                            }
                            break;
                        }
                    }

                    //不重复则添加
                    if (!bExist)
                    {
                        objLangArray.push_back(langInfo);
                    }
                }
            }
        }
    }

    return bHaveLanFile;
}

LPCSTR CLanguageHelper::Translate(LPCSTR lpszText)
{
    return _TransLogic.Translate(lpszText);
}

#ifdef IF_ENABLE_TESTABILITY
#ifdef __cplusplus
extern "C" QString __Language_ParseText(const QString & text)
{
    return LANG_PARSE(text);
}
#endif
#endif
