#ifndef SETTING_SOUNDCONTROLLER_H_
#define SETTING_SOUNDCONTROLLER_H_

#include <ylstring.h>

#include "commondefine.h"
#include "settinguilogic/include/settingdefine.h"

struct RingFileListData;

class CSoundModel;

// Class CSoundController
class CSoundController
{
public:
    // Get the singleton instance, it's a lazy singleton.
    static CSoundController * GetInstance();
    // Release the singleton instance.
    static void ReleaseInstance();

    // Get sound general data.
    // | pData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool IsKeyPressSound();
    // Set sound general data.
    // | refData | is the data to set.
    void SetKeyPressSound(bool bOn, bool bSaveLocalConfig = true);

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

    // return empty.
    const yl::string GetDefaultRingPath() const;

    //get global ring path
    const yl::string GetCommonRingPath() const;

    // Play ring file.
    // | strRingPath | is the ring path(full path).
    void PlayRing(const yl::string & strRingPath);
    // Stop playing ring.
    void StopRing();

#if IF_SUPPORT_BLF_RING
    const yl::string GetCurrentBlfRingPath() const;
    const yl::string GetDefaultBlfRingPath() const;
    bool SetCureentBlfRingPath(const yl::string& strRingPath);
#endif

private:
    // Ctor and Dtor.
    CSoundController();
    ~CSoundController();

private:
    static CSoundController * m_pSoundController;   // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CSoundController);
};

#define g_pSoundController (CSoundController::GetInstance())

#endif  // SETTING_SOUNDCONTROLLER_H_
