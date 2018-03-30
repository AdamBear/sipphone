#include "setaeskeymodel.h"

#include "configparser/modconfigparser.h"
#include <configiddefine.h>

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CSetAesKeyModel.
const yl::string CSetAesKeyModel::GetAesKey() const
{
    // Call configParser method.
    return configParser_GetConfigString(kszAesKey);
}

void CSetAesKeyModel::SetAesKey(const yl::string & strAesKey)
{
    // Set value to config file.
    configParser_SetConfigString(kszAesKey, strAesKey.c_str(), CONFIG_LEVEL_PHONE);
}

const yl::string CSetAesKeyModel::GetAesMacKey() const
{
    // Get value from config.
    return configParser_GetConfigString(kszAesMacKey);
}

void CSetAesKeyModel::SetAesMacKey(const yl::string & strAesMacKey)
{
    // Set value into config.
    configParser_SetConfigString(kszAesMacKey, strAesMacKey.c_str(), CONFIG_LEVEL_PHONE);
}

CSetAesKeyModel::CSetAesKeyModel()
{

}

CSetAesKeyModel::~CSetAesKeyModel()
{

}

}  // namespace NS_SETTING

