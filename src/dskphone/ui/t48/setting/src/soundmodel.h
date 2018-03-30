#ifndef SETTING_SOUNDMODEL_H_
#define SETTING_SOUNDMODEL_H_

#include <list>
#include <ylstring.h>
#include "commondefine.h"

struct KeypressSoundData;
struct RingFileData;
struct RingFileListData;

namespace NS_SETTING
{
// Class CSoundModel
class CSoundModel
{
public:
    // Ring file list data.
    typedef std::list<RingFileData *> ListRing;

    const yl::string GetCurrentRingPath(int iAccountID = -1) const;

    const yl::string GetCommonRingPath() const;
    // Set current ring file(full path).
    // | strRingPath | is the ring path to set.
    // Return true if set successfully, otherwise return false.
    bool SetCurrentRingPath(const yl::string & strRingPath, int iAccountID = -1);

    // If or not the key press sound is on.
    // Returns true if call waiting is on, otherwise returns false.
    bool IsKeypressOn() const;
    // Set press sound on or off.
    // | isOn | is the value to set, it is true to set press sound on,
    // otherwise set press sound off.
    void SetKeypressSound(bool isOn, bool bSaveLocalConfig = true);

    const yl::string GetCurrentBlfRingPath() const;
    const yl::string GetDefaultBlfRingPath() const;
    bool SetCurrentBlfRingPath(const yl::string & strRingPath);


private:
    // Ctor and Dtor.
    CSoundModel();
    ~CSoundModel();

private:
    friend class CSoundController;
};

}   // namespace NS_SETTING

#endif  // SETTING_SOUNDMODEL_H_
