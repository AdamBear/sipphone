#ifndef SETTING_CALLWAITINGMODEL_H_
#define SETTING_CALLWAITINGMODEL_H_
#include <ylstring.h>

struct CallWaitingData;

namespace NS_SETTING
{
// Class CCallWaitingModel.
class CCallWaitingModel
{
public:
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
    yl::string GetOnCode();

    //get callwaitting offcode from config file
    yl::string GetOffCode();

    //set callwaitting oncode and offcode to config file
    //if set config successed ,return true
    bool SetCode(yl::string strOnCode, yl::string strOffCode);

private:
    // Ctor and Dtor.
    CCallWaitingModel();
    ~CCallWaitingModel();

private:
    friend class CCallWaitingController;
};

}  // namespace NS_SETTING

#endif  // SETTING_CALLWAITINGMODEL_H_
