#include "featurecodefwddndconfig.h"

#include <configiddefine.h>
#include <modconfigparser.h>

#include <dsklog/log.h>

yl::string CFeatureCodeFwdDndConfig::GetDndCode(int iAccountId, bool bEnable)
{
  if (iAccountId == FWDDND_ACCOUNT_GLOBAL)
  {
    return configParser_GetConfigString(bEnable ? kszDndOnCode : kszDndOffCode);
  }
  else
  {
    return configParser_GetCfgItemStringValue(
             bEnable ? kszAccountDNDOnCode : kszAccountDNDOffCode,
             iAccountId);
  }
}

yl::string CFeatureCodeFwdDndConfig::GetFwdCode(int iAccountId, FWD_TYPE eType, bool bEnable)
{
  if (iAccountId == FWDDND_ACCOUNT_GLOBAL)
  {
    switch (eType)
    {
    case FWD_TYPE_ALWAYS:
      return configParser_GetConfigString(
               bEnable ? kszForwardAlwaysOnCode : kszForwardAlwaysOffCode);
      break;
    case FWD_TYPE_BUSY:
      return configParser_GetConfigString(
               bEnable ? kszForwardBusyOnCode : kszForwardBusyOffCode);
      break;
    case FWD_TYPE_NO_ANSWER:
      return configParser_GetConfigString(
               bEnable ? kszForwardNoAnswerOnCode : kszForwardNoAnswerOffCode);
      break;
    default:
      break;
    }
  }
  else
  {
    switch (eType)
    {
    case FWD_TYPE_ALWAYS:
      return configParser_GetCfgItemStringValue(
               bEnable ? kszAccountAlwaysFWDOnCode : kszAccountAlwaysFWDOffCode,
               iAccountId);
      break;
    case FWD_TYPE_BUSY:
      return configParser_GetCfgItemStringValue(
               bEnable ? kszAccountBusyFWDOnCode : kszAccountBusyFWDOffCode,
               iAccountId);
      break;
    case FWD_TYPE_NO_ANSWER:
      return configParser_GetCfgItemStringValue(
               bEnable ? kszAccountNoASWFWDOnCode : kszAccountNoASWFWDOffCode,
               iAccountId);
      break;
    default:
      break;
    }
  }
  return yl::string();
}

CFeatureCodeFwdDndConfig::CFeatureCodeFwdDndConfig()
{
}
