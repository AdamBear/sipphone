#ifndef SETTING_SOUNDCONTROLLER_H_
#define SETTING_SOUNDCONTROLLER_H_

#include <ylstring.h>

#include "commondefine.h"


struct MenuItemListData;
struct RingFileListData;
struct SoundGeneralData;
struct VolumeData;

namespace NS_SETTING
{
class CSoundModel;

// Class CSoundController
class CSoundController
{
public:
    // Get the singleton instance, it's a lazy singleton.
    static CSoundController * GetInstance();
    // Release the singleton instance.
    static void ReleaseInstance();

    // Get page menu list by click action.
    // | pData | [out] is the data retrieved.
    // | strClickAction | is the click action, it must begin with OpenPage.
    // Such as "OpenPage("account_list.xml")".
    // Returns true if successfully, otherwise return false.
    bool GetPageMenuListByClickAction(MenuItemListData * pData,
                                      const yl::string & strClickAction) const;

    // Get title by click action.
    // | strClickAction | is the click action.
    // Return the title if Retrieved successfully, otherwise return empty.
    const yl::string GetTitleByClickAction(
        const yl::string & strClickAction) const;

    // Get sound general data.
    // | pData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetGeneralData(SoundGeneralData * pData);
    // Set sound general data.
    // | refData | is the data to set.
    void SetGeneralData(const SoundGeneralData & refData, bool bSaveLocalConfig = true);

    // Get Sound File Data.
    // | pData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetRingFileList(RingFileListData * pData) const;
    // Get current ring file(full path).
    // Return current ring file path if retrieved successfully, otherwise
    // return empty.
    const yl::string GetCurrentRingPath(int iAccountID = -1) const;
    // Set current ring file(full path).
    // | strRingPath | is the ring path to set.
    // Return true if set successfully, otherwise return false.
    bool SetCurrentRingPath(const yl::string & strRingPath, int iAccountID = -1);
    // Get default ring path(full path).
    // Return default ring path if retrieved successfully, otherwise

    const yl::string GetCurrentBlfRingPath() const;
    const yl::string GetDefaultBlfRingPath() const;
    bool SetCureentBlfRingPath(const yl::string & strRingPath);

    // return empty.
    const yl::string GetDefaultRingPath() const;

    //get global ring path
    const yl::string GetCommonRingPath() const;
    // Remove ring by full path.
    // | strRingPath | is the ring path.
    // Return RRT_OK if removed successfully, otherwise return error enum.
    int RemoveStringByPath(const yl::string & strRingPath);
    // Play ring file.
    // | strRingPath | is the ring path(full path).
    void PlayRing(const yl::string & strRingPath);
    // Stop playing ring.
    void StopRing();

    // Get the volume data.
    bool GetVolumeData(VolumeData * pData);
    // Set the volume value.
    void SetVolumeData(const VolumeData & refData);

private:
    // Ctor and Dtor.
    CSoundController();
    ~CSoundController();

    // Initialize.
    // Returns true if initialized successfully, otherwise returns false.
    bool MyInit();
    // Finalize.
    // Returns true if finalized successfully, otherwise returns false.
    bool MyFinal();

private:
    CSoundModel * m_pSoundModel; // The Sound model instance.
    static CSoundController * m_pSoundController;   // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CSoundController);
};

}  // namespace NS_SETTING

#define g_pSoundController (NS_SETTING::CSoundController::GetInstance())

#endif  // SETTING_SOUNDCONTROLLER_H_
