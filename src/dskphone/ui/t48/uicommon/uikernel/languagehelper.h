#ifndef LANGUAGEHELPER_H
#define LANGUAGEHELPER_H
#include <ETLLib/ETLLib.hpp>
#include "ylstl/yllist.h"
#include "ylstl/ylvector.h"
#include "translateiddefine.h"
#include "language/common/singletonbase.h"

///////////////////////////////////////////////////////////////////////////////
//定义翻译语言的数据结构
class CLangInfo
{
public:
    //源翻译内容
    yl::string strOrigName;
    //翻译后输出的内容
    yl::string strTranName;
    //语言翻译文件名称
    yl::string strLangFile;
    //使用的字体名称
    yl::string strFontName;
};

//定义翻译时要使用的数组类型
typedef YLVector<CLangInfo> VectorLang;

class QString;

class CLanguageHelper
    : public SingletonBase<CLanguageHelper>
{
public:
    CLanguageHelper();

    bool InitializeLangHelper(const char * szLangName = NULL);

    static bool GetAllLangInfo(VectorLang & objLangArray);

    QString TranslateText(const QString & strText);
    QString TranslateText(LPCTSTR lpszText);

    //通过语言类型取得字体
    LPCTSTR GetFontByLanguage(const char * lpszLangName = NULL);

    QString ParseLangText(const QString & strText);

    LPCSTR Translate(LPCSTR lpszText);

private:
    yl::string m_strActiveFont;
};

//翻译宏返回值类型：QString
//输入参数为：要翻译的字符串
#define LANG_TRANSLATE(lpszText)    CLanguageHelper::GetInstance().TranslateText(lpszText)      //进行字符组合的宏
#define LANG_PARSE(lpszText)        CLanguageHelper::GetInstance().ParseLangText(lpszText)      //进行字符解析的宏
#define LANG_EXP(lpszText)          CLanguageHelper::GetInstance().Translate(lpszText)

#ifdef IF_ENABLE_TESTABILITY
#ifdef __cplusplus
extern "C" QString __Language_ParseText(const QString & text);
#endif
#endif

#endif
