#include "soundcontroller.h"

#include "ETLLib.hpp"
#include "commonunits/modcommonunits.h"
#include "setting/include/common.h"
#include "voice/include/modvoice.h"

#include "basemethod.h"
#include "soundmodel.h"
#include "resource/modresource.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CSoundController
CSoundController * CSoundController::m_pSoundController = NULL;

CSoundController * CSoundController::GetInstance()
{
    // Instantiate the singleton instance if not existed.
    if (m_pSoundController == NULL)
    {
        m_pSoundController = new CSoundController();
        if (m_pSoundController != NULL)
        {
            // Do initialization.
            m_pSoundController->MyInit();
        }
    }
    return m_pSoundController;
}

void CSoundController::ReleaseInstance()
{
    // Release the singleton instance if exists.
    if (m_pSoundController != NULL)
    {
        // Do finalization.
        m_pSoundController->MyFinal();

        delete m_pSoundController;
        m_pSoundController = NULL;
    }
}

bool CSoundController::GetPageMenuListByClickAction(
    MenuItemListData * pData, const yl::string & strClickAction) const
{
    // Check the click action.
    if (strClickAction.empty() || pData == NULL)
    {
        return false;
    }

    // Call the base method.
    return NS_SETTING::GetPageMenuListByClickAction(pData, strClickAction);
}

const yl::string CSoundController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // Call the base method.
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

bool CSoundController::GetGeneralData(SoundGeneralData * pData)
{
    if (pData == NULL || m_pSoundModel == NULL)
    {
        return false;
    }

    // Get data from model.
    pData->m_isKeypressSoundOn = m_pSoundModel->IsKeypressOn();
    return true;
}

void CSoundController::SetGeneralData(const SoundGeneralData & refData,
                                      bool bSaveLocalConfig/* = true*/)
{
    if (m_pSoundModel != NULL)
    {
        // Set data to model.
        m_pSoundModel->SetKeypressSound(refData.m_isKeypressSoundOn, bSaveLocalConfig);
    }
}

bool CSoundController::GetRingFileList(RingFileListData * pData) const
{
    if (pData != NULL)
    {
        YLList<YLSmartPtr<BaseResItem> > lsRingTone;
        if (0 == res_getResItemsByResType(lsRingTone, RING_TYPE))
        {
            YLList<YLSmartPtr<BaseResItem> >::iterator iter = lsRingTone.begin();
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
    return m_pSoundModel != NULL ? m_pSoundModel->GetCurrentRingPath(iAccountID) : "";
}

bool CSoundController::SetCurrentRingPath(const yl::string & strRingPath, int iAccountID)
{
    // Set current ring path.
    if (m_pSoundModel == NULL)
    {
        return false;
    }

    return m_pSoundModel->SetCurrentRingPath(strRingPath, iAccountID);
}

const yl::string CSoundController::GetCurrentBlfRingPath() const
{
    return m_pSoundModel != NULL ? m_pSoundModel->GetCurrentBlfRingPath() : "";
}

const yl::string CSoundController::GetDefaultBlfRingPath() const
{
    return m_pSoundModel != NULL ? m_pSoundModel->GetDefaultBlfRingPath() : "";
}

bool CSoundController::SetCureentBlfRingPath(const yl::string & strRingPath)
{
    if (m_pSoundModel == NULL)
    {
        return false;
    }

    return m_pSoundModel->SetCurrentBlfRingPath(strRingPath);
}

const yl::string CSoundController::GetDefaultRingPath() const
{
    // Get default ring path.
    YLSmartPtr<BaseResItem> pItem = res_getDefaultItem(RING_TYPE);
    return pItem->resPath;
}

int CSoundController::RemoveStringByPath(const yl::string & strRingPath)
{
    return 0;
//#warning "TODO:CommonUnits_RemoveRing"
//  return CommonUnits_RemoveRing(strRingPath);
}

void CSoundController::PlayRing(const yl::string & strRingPath)
{
    voice_PlayRing(strRingPath.c_str());
}

void CSoundController::StopRing()
{
    voice_StopRingInMenu();
}

// Get the volume data.
bool CSoundController::GetVolumeData(VolumeData * pData)
{
    if (m_pSoundModel == NULL || pData == NULL)
    {
        return false;
    }

    pData->m_nVolumeValue = voice_GetVolume();
    return true;
}

// Set the volume value.
void CSoundController::SetVolumeData(const VolumeData & refData)
{
    // Call the functions of voice channel.
    voice_SetVolume(refData.m_nVolumeValue);
}

CSoundController::CSoundController()
    : m_pSoundModel(NULL)
{

}

CSoundController::~CSoundController()
{

}

// Initialize.
bool CSoundController::MyInit()
{
    // Initialize the language model.
    if (m_pSoundModel == NULL)
    {
        m_pSoundModel = new CSoundModel();
    }
    return m_pSoundModel != NULL;
}

// Finalize.
bool CSoundController::MyFinal()
{
    // Release the language model.
    if (m_pSoundModel != NULL)
    {
        delete m_pSoundModel;
        m_pSoundModel = NULL;
    }
    return m_pSoundModel == NULL;
}

const yl::string CSoundController::GetCommonRingPath() const
{
    // Get Common ring path.
    return m_pSoundModel != NULL ? m_pSoundModel->GetCommonRingPath() : "";
}

}   // namespace NS_SETTING
