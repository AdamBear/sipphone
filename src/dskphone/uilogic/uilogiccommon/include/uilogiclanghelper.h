#ifndef _UILOGIC_LANG_HELPER
#define _UILOGIC_LANG_HELPER

#include <ylstl/ylstring.h>

#define LANGUAGE_FLAG           "?`"

//翻译
yl::string UILogicTranslateText(const char * lpszText);


//翻译宏
//#define _UILOGIC_LANG_TRAN(strText)       UILogicTranslateText( (yl::string(strText)).c_str())    //逻辑层翻译宏

#define _UILOGIC_LANG_TRAN(lpszText)        UILogicTranslateText(lpszText)  //逻辑层翻译宏

#endif

