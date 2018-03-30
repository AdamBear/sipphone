#ifndef SETTING_INTERCOM_H_
#define SETTING_INTERCOM_H_

namespace NS_SETTING
{

// Class CIntercomModel.
class CIntercomModel
{
public:
    // Get allowance.
    // Returns true if intercom is allowed, otherwise returns false.
    bool IsAllowed() const;
    // Set allowance.
    // | isAllowed | is the flag to set, set it true to allow intercom, otherwise
    // disallow intercom.
    void SetAllowance(bool isAllowed);

    // Whether or not the intercom mute is on.
    // Returns current intercom mute status. True indicates intercom mute
    // is on, otherwise intercom mute is off.
    bool IsMuteOn() const;
    // Set intercom mute status.
    // | isOn | is the flag to set, set it true to open intercom mute,
    // otherwise close intercom mute.
    void SetMute(bool isOn);

    // Whether or not the intercom tone is on.
    // Returns true if current intercom tone is on, otherwise returns false.
    bool IsToneOn() const;
    // Set intercom tone status.
    // | isOn | is the flag to set, set it true to open intercom tone,
    // otherwise close intercom tone.
    void SetTone(bool isOn);

    // Whether or not the intercom barge is on.
    // Returns true if current intercom barge is on, otherwise returns false.
    bool IsBargeOn() const;
    // Set intercom barge status.
    // | isOn | is the flag to set, set it true to open intercom barge,
    // otherwise close intercom barge.
    void SetBarge(bool isOn);

private:
    // Ctor and Dtor.
    CIntercomModel();
    ~CIntercomModel();

private:
    friend class CIntercomController;
};

}  // namespace NS_SETTING

#endif
