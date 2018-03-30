#ifndef SETTING_CALLWAITINGCONTROLLER_H_
#define SETTING_CALLWAITINGCONTROLLER_H_

#include <ylstring.h>

#include "commondefine.h"

struct CallWaitingData;
struct MenuItemListData;

class CCallWaitingModel;

// Class CCallWaitingController
class CCallWaitingController
{
public:
    // Get the singleton instance, it's a lazy singleton.
    static CCallWaitingController * GetInstance();
    // Release the singleton instance.
    static void ReleaseInstance();

    // Get call waiting data, including call waiting status and play-tone.
    // | pCallWaitingData | is the data retrieved.
    // Returns true if retrieved successfully, otherwise returns false.
    bool GetCallWaitingData(CallWaitingData * pCallWaitData) const;
    // Set call waiting data.
    // | refData | is the data to set.
    void SetCallWaitingData(const CallWaitingData & refData);

    // If or not the current call waiting is on.
    // Returns true if call waiting is on, otherwise returns false.
    bool IsOn() const;
    // Set call waiting on or off.
    // | isOn | is the value to set, it is true to set call waiting on,
    // otherwise set call waiting off.
    void SetStatus(bool isOn);

    // If or not call waiting will play tone.
    // Returns true if call waiting will play tone, otherwise returns false.
    bool IsPlayTone() const;
    // Set call waiting play tone or not.
    // | isPlay | is the value to set, it is true to set call waiting to
    // play tone, otherwise not play tone.
    void SetPlayTone(bool isPlay);

    //get callwaitting oncode from config file
    yl::string GetOnCode() const;

    //get callwaitting offcode from config file
    yl::string GetOffCode() const;

    //set callwaitting oncode and offcode to config file
    //if set config successed ,return true
    bool SetCode(yl::string strOnCode, yl::string strOffCode);

private:
    // Ctor and Dtor.
    CCallWaitingController();
    ~CCallWaitingController();

private:
    static CCallWaitingController * m_pCallWaitController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CCallWaitingController);
};

#define g_pCallWaitingController (CCallWaitingController::GetInstance())

#endif  // SETTING_CALLWAITINGCONTROLLER_H_
