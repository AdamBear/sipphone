#ifndef SETTING_ADVANCEDCALLFEATUREMODEL_H_
#define SETTING_ADVANCEDCALLFEATUREMODEL_H_

#include <ylstring.h>

namespace NS_SETTING
{
// Class CAdvancedCallFeature
class CAdvancedCallFeatureModel
{
public:
    // Get key as send.
    // Returns the key as send.
    int GetKeyAsSend() const;
    // Set key as send.
    // | nType | is the type to set.
    void SetKeyAsSend(int nType);

    // Get hotline number.
    // Return the hotline number retrieved.
    const yl::string GetHotlineNumber() const;
    // Set hotline number.
    // | strHotlineNumber | is the hotline number to set.
    void SetHotlineNumber(const yl::string & strHotlineNumber);

    // Get hotline delay time.
    // Return hotline delay time retrieved.
    int GetHotlineDelay() const;
    // Set hotline delay time.
    // | nHotlineDelay | is the delay time to set.
    void SetHotlineDelay(int nHotlineDelay);

    // Get auto redial interval.
    // Return auto redial interval.
    int GetAutoRedialInterval() const;
    // Set auto redial interval.
    // | nAutoRedialInterval | is the auto redial interval to set.
    void SetAutoRedialInterval(int nAutoRedialInterval);

    // Get auto redial times.
    // Return auto redial times retrieved.
    int GetAutoRedialTimes() const;
    // Set auto redial times.
    // | nAutoRedialTimes | is the auto redial times to set.
    void SetAutoRedialTimes(int nAutoRedialTimes);

    // Whether or not history should be saved.
    // Return true if history should be saved, otherwise not be saved.
    bool IsHistorySaved() const;
    // Set history to saved or not.
    // | isSave | is true to save history, otherwise not to save history.
    void SetHistorySaved(bool isSave);

    // Whether or not auto redial on.
    // Return true if auto redial is turned on, false auto redial is turned off.
    bool IsAutoRedialOn() const;
    // Set auto redial.
    // | isOn | is true to set auto redial on, otherwise set auto redial off.
    void SetAutoRedial(bool isOn);

    // Whether or not call completion is on.
    // Return true if call completion is on, false call completion is off.
    bool IsCallCompletionOn() const;
    // Set call completion.
    // | isOn | is true to set call completion on, otherwise set call completion off.
    void SetCallCompletion(bool isOn);

private:
    // Ctor and Dtor.
    CAdvancedCallFeatureModel();
    ~CAdvancedCallFeatureModel();

    friend class CAdvancedCallFeatureController;
};

}  // namespace NS_SETTING

#endif  // SETTING_ADVANCEDCALLFEATUREMODEL_H_
