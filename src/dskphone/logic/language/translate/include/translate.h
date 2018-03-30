#pragma once

#include "language/common/singletonbase.h"
#include "language/common/lang_common.h"

typedef YLVector<yl::string>    VectorLangInfo;

typedef YLHashMap<yl::string, yl::string>       mapLanguageText;

///////////////////////////////////////////////////////////////////////////////
class CTransLogic
    : public mapLanguageText, public SingletonBase<CTransLogic>
{
public:
    CTransLogic(void) {}

    void Reload();

    //根据语言名获取文件路径
    static bool GetFilePathByLanguage(yl::string & strFilePath, const yl::string & strLangName);

    //获取所有语言名
    static bool GetAllLangInfo(VectorLangInfo & objLangArray);

    //翻译实现函数，输入参数类型为char*类型
    LPCSTR Translate(LPCSTR lpszText);
    //初始化
    bool InitializeLangHelper(const yl::string & strText);

private:
    typedef YLList<yl::string> ListLangFileName; // 翻译文件名列表

    //通过输入文件名称加载翻译文件内容
    bool LoadLanguage(LPCSTR szLangFile);

    // 根据翻译文件名解析语言名
    static bool GetLangNameFromFileName(const yl::string & strFileName, yl::string & strLangName);

    static bool Add2Vector(VectorLangInfo & listLangNames, const yl::string & strName);

#ifdef IF_SUPPORT_XML_TRANS
    static void GetLangFileNameFromXml(const yl::string & strLangFile,
                                       ListLangFileName & listLangFileName);

    void LoadXmlLang(LPCSTR lpszLangFile, LPCSTR lpLangName);
#endif
};

#define _TransLogic GET_SINGLETON_INSTANCE(CTransLogic)

