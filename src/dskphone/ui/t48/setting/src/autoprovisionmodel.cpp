#include "autoprovisionmodel.h"

#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "autopcontrol/include/modautopcontrol.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CAutoProvisionModel.
const yl::string CAutoProvisionModel::GetUrl() const
{
    // Read auto provision URL from config file.
    return configParser_GetConfigString(kszAutoProvisionUrl);
}

void CAutoProvisionModel::SetUrl(const yl::string & strUrl)
{
    // Set auto provision URL to config file.
    configParser_SetConfigString(kszAutoProvisionUrl, strUrl.c_str(), CONFIG_LEVEL_PHONE);
}

const yl::string CAutoProvisionModel::GetPUserName() const
{
    // Read auto provision username from config file.
    return configParser_GetConfigString(kszAutoProvisionUsername);
}

void CAutoProvisionModel::SetUserName(const yl::string & strUserName)
{
    // Set auto provision username to config file.
    configParser_SetConfigString(kszAutoProvisionUsername, strUserName.c_str(), CONFIG_LEVEL_PHONE);
}

const yl::string CAutoProvisionModel::GetPassword() const
{
    // Read auto provision password from config file.
    return configParser_GetConfigString(kszAutoProvisionPassword);
}

void CAutoProvisionModel::SetPassword(const yl::string & strPassword)
{
    // Set auto provision password to config file.
    configParser_SetConfigString(kszAutoProvisionPassword, strPassword.c_str(), CONFIG_LEVEL_PHONE);
}

const yl::string CAutoProvisionModel::GetAuthenticationUserName() const
{
    return Autop_GetCurrentUserName();
}

const yl::string CAutoProvisionModel::GetAuthenticationassword() const
{
    return Autop_GetCurrentPassword();
}

void CAutoProvisionModel::StartCurrentConnect(yl::string & strName, yl::string & strPassword)
{
    Autop_StartConnect(Autop_GetUpdateType(), strName, strPassword);
}

const yl::string CAutoProvisionModel::GetAesKey() const
{
    // Call configParser method.
    return configParser_GetConfigString(kszAesKey);
}

void CAutoProvisionModel::SetAesKey(const yl::string & strAesKey)
{
    // Set value to config file.
    configParser_SetConfigString(kszAesKey, strAesKey.c_str(), CONFIG_LEVEL_PHONE);
}

const yl::string CAutoProvisionModel::GetAesMacKey() const
{
    // Get value from config.
    return configParser_GetConfigString(kszAesMacKey);
}

void CAutoProvisionModel::SetAesMacKey(const yl::string & strAesMacKey)
{
    // Set value into config.
    configParser_SetConfigString(kszAesMacKey, strAesMacKey.c_str(), CONFIG_LEVEL_PHONE);
}

CAutoProvisionModel::CAutoProvisionModel()
{

}

CAutoProvisionModel::~CAutoProvisionModel()
{

}

}  // namespace NS_SETTING

