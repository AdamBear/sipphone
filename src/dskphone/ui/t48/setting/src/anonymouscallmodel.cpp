#include "anonymouscallmodel.h"

#include <assert.h>

#include "configparser/modconfigparser.h"
#include "setting/include/common.h"
#include <configiddefine.h>

#include "commondefine.h"

namespace NS_SETTING
{
// Max file path length.
static const int knMaxPathLen = 260;

//////////////////////////////////////////////////////////////////////////
// Class CAnonymousCallModel
bool CAnonymousCallModel::IsAnonymousOn(int nLineId) const
{
    // Read anonymous call switch value.
    return configParser_GetCfgItemIntValue(kszAnonymousCallSwitch, nLineId) == CONFIG_ON;
}

void CAnonymousCallModel::SetAnonymousStatus(int nLineId, bool isOn)
{
    // Set anonymous call switch value.
    configParser_SetCfgItemIntValue(kszAnonymousCallSwitch, nLineId, isOn ? CONFIG_ON : CONFIG_OFF,
                                    CONFIG_LEVEL_PHONE);
}

const yl::string CAnonymousCallModel::GetAnonymousOnCode(int nLineId) const
{
    return configParser_GetCfgItemStringValue(kszAnonymousCallOnCode, nLineId);
}

void CAnonymousCallModel::SetAnonymousOnCode(
    int nLineId, const yl::string & strOnCode)
{
    // Read anonymous call on code.
    configParser_SetCfgItemStringValue(kszAnonymousCallOnCode, nLineId, strOnCode, CONFIG_LEVEL_PHONE);
}

const yl::string CAnonymousCallModel::GetAnonymousOffCode(int nLineId) const
{
    // Read anonymous call off code.
    return configParser_GetCfgItemStringValue(kszAnonymousCallOffCode, nLineId);
}

void CAnonymousCallModel::SetAnonymousOffCode(
    int nLineId, const yl::string & strOffCode)
{
    // Read anonymous call off code.
    configParser_SetCfgItemStringValue(kszAnonymousCallOffCode, nLineId, strOffCode,
                                       CONFIG_LEVEL_PHONE);
}

bool CAnonymousCallModel::IsRejectionOn(int nLineId) const
{
    // Read rejection call switch value.
    return configParser_GetCfgItemIntValue(kszRejectAnonymousCallSwitch, nLineId) == CONFIG_ON;
}

void CAnonymousCallModel::SetRejectionStatus(int nLineId, bool isOn)
{
    // Set rejection switch value.
    configParser_SetCfgItemIntValue(kszRejectAnonymousCallSwitch, nLineId,
                                    isOn ? CONFIG_ON : CONFIG_OFF, CONFIG_LEVEL_PHONE);
}

const yl::string CAnonymousCallModel::GetRejectionOnCode(int nLineId) const
{
    // Get rejection on code.
    return configParser_GetCfgItemStringValue(kszRejectAnonymousCallOnCode, nLineId);
}

void CAnonymousCallModel::SetRejectionOnCode(
    int nLineId, const yl::string & strOnCode)
{
    // Read rejection on code.
    configParser_SetCfgItemStringValue(kszRejectAnonymousCallOnCode, nLineId, strOnCode,
                                       CONFIG_LEVEL_PHONE);
}

const yl::string CAnonymousCallModel::GetRejectionOffCode(int nLineId) const
{
    // Read rejection off code.
    return configParser_GetCfgItemStringValue(kszRejectAnonymousCallOffCode, nLineId);
}

void CAnonymousCallModel::SetRejectionOffCode(
    int nLineId, const yl::string & strOffCode)
{
    // Read rejection off code.
    configParser_SetCfgItemStringValue(kszRejectAnonymousCallOffCode, nLineId, strOffCode,
                                       CONFIG_LEVEL_PHONE);
}

bool CAnonymousCallModel::IsAdministratorHasSet(int nAccount)
{
    return !configParser_GetCfgItemStringValue(kszAnonymousCallServerBaseOnly, nAccount).empty();
}

CAnonymousCallModel::CAnonymousCallModel()
{

}

CAnonymousCallModel::~CAnonymousCallModel()
{

}

bool CAnonymousCallModel::GetAnonymousSendCode(int nLineId) const
{
    return configParser_GetCfgItemIntValue(kszSendAnonymousCode, nLineId) == CONFIG_ON;
}

void CAnonymousCallModel::SetAnonymousSendCode(int nLineId, bool isOn) const
{
    configParser_SetCfgItemIntValue(kszSendAnonymousCode, nLineId, isOn ? CONFIG_ON : CONFIG_OFF,
                                    CONFIG_LEVEL_PHONE);
}

bool CAnonymousCallModel::GetAnonymousRejectSendCode(int nLineId) const
{
    return configParser_GetCfgItemIntValue(kszAccountSendAnonymousRejection, nLineId) == CONFIG_ON;
}

void CAnonymousCallModel::SetAnonymousRejectSendCode(int nLineId, bool isOn) const
{
    configParser_SetCfgItemIntValue(kszAccountSendAnonymousRejection, nLineId,
                                    isOn ? CONFIG_ON : CONFIG_OFF, CONFIG_LEVEL_PHONE);
}

}  // namespace NS_SETTING
