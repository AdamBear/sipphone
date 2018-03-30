#include "uilogiclanghelper.h"
#include "baselibs/include/commonapi/stringhelperapi.h"


//翻译
yl::string UILogicTranslateText(const char * lpszText)
{
#if 1 //IF_SUPPORT_AUTO_TRANSLATE

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