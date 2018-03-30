#include "soundmodel.h"
#include "ETLLib.hpp"
// #include "CommonAPI/CommonAPI.h"
#include "commonunits/modcommonunits.h"
#include <configiddefine.h>
#include "setting/include/common.h"
#include "path_define.h"
#include "configparser/modconfigparser.h"

namespace NS_SETTING
{
////////////////////////////////////////////////////////////////////////
// Class CSoundModel
CSoundModel::CSoundModel()
{

}

CSoundModel::~CSoundModel()
{

}

const yl::string CSoundModel::GetCurrentRingPath(int iAccountID/* = -1*/) const
{
    // Get current ring path.
    yl::string strPath = "";
    if (iAccountID == -1)
    {
        strPath = configParser_GetConfigString(kszRingtype);
    }
    else
    {
        strPath = configParser_GetCfgItemStringValue(kszAccountRing, iAccountID);
    }

    commonUnits_Hide2Real(strPath, PHFILE_TYPE_RING_FILE);
    return strPath;
}

bool CSoundModel::SetCurrentRingPath(const yl::string & strRingPath, int iAccountID/* = -1*/)
{
    bool bRet = false;
    yl::string strPath(strRingPath);
    commonUnits_Real2Hide(strPath, PHFILE_TYPE_RING_FILE);

    if (iAccountID == -1)
    {
        bRet = configParser_SetConfigString(kszRingtype, strPath.c_str(), CONFIG_LEVEL_PHONE);
    }
    else
    {
        bRet = configParser_SetCfgItemStringValue(kszAccountRing, iAccountID, strPath.c_str(),
                CONFIG_LEVEL_PHONE);
    }

    return bRet;
}

bool CSoundModel::IsKeypressOn() const
{
    // Read key press sound status from config file.
    return StrictCheck(configParser_GetConfigInt(kszKeypressSound)) == CONFIG_ON;
}

// Set press sound on or off.
void CSoundModel::SetKeypressSound(bool isOn, bool bSaveLocalConfig/* = true*/)
{
    // Saves the value to config file.
    configParser_SetConfigInt(kszKeypressSound, isOn ? 1 : 0,
                              bSaveLocalConfig ? CONFIG_LEVEL_PHONE : CONFIG_LEVEL_DEFAULT);
}

const yl::string CSoundModel::GetCommonRingPath() const
{
    yl::string strPath = configParser_GetConfigString(kszRingtype);

    commonUnits_Hide2Real(strPath, PHFILE_TYPE_RING_FILE);
    return strPath;
}

const yl::string CSoundModel::GetCurrentBlfRingPath() const
{
    yl::string strPath = configParser_GetConfigString(kszBlfRingType);

    commonUnits_Hide2Real(strPath, PHFILE_TYPE_RING_FILE);
    return strPath;
}

const yl::string CSoundModel::GetDefaultBlfRingPath() const
{
    yl::string strPath = configParser_GetConfigString(kszBlfRingType, CONFIG_LEVEL_FACTORY);

    commonUnits_Hide2Real(strPath, PHFILE_TYPE_RING_FILE);
    return strPath;
}

bool CSoundModel::SetCurrentBlfRingPath(const yl::string & strRingPath)
{
    bool bRet = false;
    yl::string strPath(strRingPath);
    commonUnits_Real2Hide(strPath, PHFILE_TYPE_RING_FILE);

    return configParser_SetConfigString(kszBlfRingType, strPath.c_str(), CONFIG_LEVEL_PHONE);
}

}  // namespace NS_SETTING

