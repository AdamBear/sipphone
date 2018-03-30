#include "languagemodel.h"

#include "ETLLib/ETLLib.hpp"
// #include "CommonAPI/CommonAPI.h"
#include "configparser/modconfigparser.h"
#include "setting/include/common.h"
#include <configiddefine.h>
#include "path_define.h"
// #include "Define/LanguageMessageDefine.h"
#include "etlmsghandler/modetlmsghandler.h"

namespace NS_SETTING
{
////////////////////////////////////////////////////////////////////
// Class CLanguageModel
CLanguageModel::CLanguageModel()
{

}

CLanguageModel::~CLanguageModel()
{

}

const yl::string CLanguageModel::GetCurrentLanguage() const
{
    // Read current language.
    return configParser_GetConfigString(kszCurrentLang);
}

bool CLanguageModel::SetCurrentLanguage(const yl::string & strLanguage)
{
    // Set current language.
    return configParser_SetConfigString(kszCurrentLang, strLanguage.c_str(), CONFIG_LEVEL_PHONE);
}

}   // namespace NS_SETTING
