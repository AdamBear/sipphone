#ifndef __MOD_LANGTRANSLATE_H__
#define __MOD_LANGTRANSLATE_H__
#include <ETLLib/ETLLib.hpp>
#include <ylstring.h>
#include "languagedef.h"

class xWorkStation;
yl::string DECLARE_EXPORT lang_AppenLangFlage(const char* lpszText);
yl::string DECLARE_EXPORT lang_AppenLangFlage(const yl::string& strText);
yl::string DECLARE_EXPORT lang_Translate(const char* lpszText);
yl::string DECLARE_EXPORT lang_Translate(const yl::string& strText);
#define _LANG(str) lang_AppenLangFlage(str)
#define _TRAN(str) lang_Translate(str)
bool DECLARE_EXPORT lang_Init();
bool DECLARE_EXPORT lang_UnInit();
bool DECLARE_EXPORT lang_Register(xWorkStation* pWorkStation);
bool DECLARE_EXPORT lang_UnRegister(xWorkStation* pWorkStation);
#endif

