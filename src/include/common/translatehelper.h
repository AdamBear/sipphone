#ifndef _TRANSLATE_HELPER_
#define _TRANSLATE_HELPER_

#include <ylstring.h>
#include "commonapi/stringhelperapi.h"
#include "dsklog/log.h"

#define LANGUAGE_FLAG           "?`"
static yl::string LogicTranslateText(const char * lpszText)
{
#if IF_SUPPORT_AUTO_TRANSLATE

    if (lpszText == NULL
            || strcmp(lpszText, "") == 0)
    {
        //字符串为空，返回空
        return "";
    }

    yl::string strTran(lpszText);
    if (NULL == strstr(lpszText, LANGUAGE_FLAG))
    {
        strTran.clear();
        strTran = commonAPI_FormatString("%s%.2d%s", LANGUAGE_FLAG, strlen(lpszText), lpszText);
    }

    return strTran;
#else
    return yl::string(lpszText);
#endif
}

static yl::string LogicTranslateText(yl::string strText)
{
#if IF_SUPPORT_AUTO_TRANSLATE

    if (strText.empty())
    {
        //字符串为空，返回空
        return "";
    }

    yl::string strTran = strText;
    if (NULL == strstr(strText.c_str(), LANGUAGE_FLAG))
    {
        strTran.clear();
        strTran = commonAPI_FormatString("%s%.2d%s", LANGUAGE_FLAG, strText.length(), strText.c_str());
    }

    return strTran;
#else
    return strText;
#endif
}

#define _LOGIC_LANG_TRAN(lpszText)      LogicTranslateText(lpszText)    //逻辑层翻译宏

#endif
