#include "soundcontroller.h"

#include "ETLLib.hpp"
#include "commonunits/modcommonunits.h"
#include "settinguilogic/include/common.h"
#include "voice/include/modvoice.h"

#include "resource/modresource.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>

#include "account/include/account_datadefine.h"
#include <ylstl/yllist.h>
#include <ylstl/ylsmartptr.h>

namespace
{
typedef YLList<YLSmartPtr<BaseResItem> >   LIST_RES_ITEM_SMART_PTR;
}

//////////////////////////////////////////////////////////////////////////
// Class CSoundController
CSoundController * CSoundController::m_pSoundController = NULL;

CSoundController * CSoundController::GetInstance()
{
    // Instantiate the singleton instance if not existed.
    if (NULL == m_pSoundController)
    {
        m_pSoundController = new CSoundController();
    }
    return m_pSoundController;
}

void CSoundController::ReleaseInstance()
{
    // Release the singleton instance if exists.
    if (NULL != m_pSoundController)
    {
        delete m_pSoundController;
        m_pSoundController = NULL;
    }
}

bool CSoundController::IsKeyPressSound()
{
    // Read key press sound status from config file.
    return CONFIG_ON == StrictCheck(configParser_GetConfigInt(kszKeypressSound));
}

void CSoundController::SetKeyPressSound(bool bOn, bool bSaveLocalConfig /*= true*/)
{
    // Saves the value to config file.
    configParser_SetConfigInt(kszKeypressSound, bOn ? 1 : 0,
                              bSaveLocalConfig ? CONFIG_LEVEL_PHONE : CONFIG_LEVEL_IGNORE);
}

bool CSoundController::GetRingFileList(RingFileListData * pData) const
{
    if (NULL != pData)
    {
        LIST_RES_ITEM_SMART_PTR lsRingTone;
        if (0 == res_getResItemsByResType(lsRingTone, RING_TYPE))
        {
            LIST_RES_ITEM_SMART_PTR::iterator iter = lsRingTone.begin();
            RingFileData  ringData;

            for (int i = 0; iter != lsRingTone.end(); iter++, ++i)
            {
                YLSmartPtr<BaseResItem> & pItem = *iter;
                if (pItem.IsEmpty())
                {
                    continue;
                }

                ringData.m_strRingDisplayName = pItem->resItemDispLabel;
                ringData.m_strRingPath        = pItem->resPath;
                pData->m_listRing.push_back(ringData);
            }
            return true;
        }
    }
    return false;
}

const yl::string CSoundController::GetCurrentRingPath(int iAccountID/* = -1*/) const
{
    // Get current ring path.
    yl::string strPath = "";
    if (-1 == iAccountID)
    {
        strPath = configParser_GetConfigString(kszRingtype);
    }
#ifdef IF_FEATURE_PSTN
    else if (iAccountID >= PSTN_LINE_BEGIN_ID && iAccountID <= PSTN_LINE_END_ID)
    {
        strPath = configParser_GetCfgItemStringValue(kszPSTNAccountRing, iAccountID - PSTN_LINE_BEGIN_ID);
    }
#endif // IF_FEATURE_PSTN
    else
    {
        strPath = configParser_GetCfgItemStringValue(kszAccountRing, iAccountID);
    }

    commonUnits_Hide2Real(strPath, PHFILE_TYPE_RING_FILE);
    return strPath;
}

bool CSoundController::SetCurrentRingPath(const yl::string & strRingPath, int iAccountID)
{
    bool bRet = false;
    yl::string strPath(strRingPath);
    commonUnits_Real2Hide(strPath, PHFILE_TYPE_RING_FILE);

    if (-1 == iAccountID)
    {
        bRet = configParser_SetConfigString(kszRingtype, strPath.c_str(), CONFIG_LEVEL_PHONE);
    }
#ifdef IF_FEATURE_PSTN
    if (iAccountID >= PSTN_LINE_BEGIN_ID && iAccountID <= PSTN_LINE_END_ID)
    {
        bRet = configParser_SetCfgItemStringValue(kszPSTNAccountRing, iAccountID - PSTN_LINE_BEGIN_ID,
                strPath.c_str(), CONFIG_LEVEL_IGNORE);
    }
#endif // IF_FEATURE_PSTN
    else
    {
        bRet = configParser_SetCfgItemStringValue(kszAccountRing, iAccountID, strPath.c_str(),
                CONFIG_LEVEL_PHONE);
    }

    return bRet;
}

const yl::string CSoundController::GetDefaultRingPath() const
{
    // Get default ring path.
    YLSmartPtr<BaseResItem> pItem = res_getDefaultItem(RING_TYPE);
    return pItem->resPath;
}

void CSoundController::PlayRing(const yl::string & strRingPath)
{
    voice_PlayRing(strRingPath.c_str());
}

void CSoundController::StopRing()
{
    voice_StopRingInMenu();
}

#if IF_SUPPORT_BLF_RING
const yl::string CSoundController::GetCurrentBlfRingPath() const
{
    yl::string strPath = configParser_GetConfigString(kszBlfRingType);
    commonUnits_Hide2Real(strPath, PHFILE_TYPE_RING_FILE);
    return strPath;
}

const yl::string CSoundController::GetDefaultBlfRingPath() const
{
    yl::string strPath = configParser_GetConfigString(kszBlfRingType, CONFIG_LEVEL_FACTORY);
    commonUnits_Hide2Real(strPath, PHFILE_TYPE_RING_FILE);
    return strPath;
}

bool CSoundController::SetCureentBlfRingPath(const yl::string& strRingPath)
{
    bool bRet = false;
    yl::string strPath(strRingPath);
    commonUnits_Real2Hide(strPath, PHFILE_TYPE_RING_FILE);
    return configParser_SetConfigString(kszBlfRingType, strPath.c_str(), CONFIG_LEVEL_PHONE);
}
#endif

CSoundController::CSoundController()
{

}

CSoundController::~CSoundController()
{

}

const yl::string CSoundController::GetCommonRingPath() const
{
    // Get Common ring path.
    yl::string strPath = configParser_GetConfigString(kszRingtype);

    commonUnits_Hide2Real(strPath, PHFILE_TYPE_RING_FILE);
    return strPath;
}

