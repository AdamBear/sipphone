#ifndef SETTING_ANONYMOUSCALLMODEL_H_
#define SETTING_ANONYMOUSCALLMODEL_H_

#include <ylstring.h>

struct AnonymousCallData;

namespace NS_SETTING
{
// Class CAnonymousCallModel
class CAnonymousCallModel
{
public:
    // Max anonymous call count.
    static const int m_knMaxAnonymousCallCount = 17;

    // Whether or not anonymous call is on.
    // | nLineId | is the line Id.
    // Return true if anonymous call is on, otherwise return false.
    bool IsAnonymousOn(int nLineId) const;

    // Set anonymous call status, ON or OFF.
    // | nLineId | is the line Id.
    // | isOn | is true to set anonymous call on, false to set anonymous call off.
    void SetAnonymousStatus(int nLineId, bool isOn);

    // Get Send anonymous call on code or off code.
    // | nLineId | is the line Id.
    // Return true if send anonymous call on code, otherwise return false.
    bool GetAnonymousSendCode(int nLineId) const;

    // Set Send anonymous call on code or off code.
    // | nLineId | is the line Id.
    // | isOn | is true to set on code, false to set off code.
    void SetAnonymousSendCode(int nLineId, bool isOn) const;

    // Get Send anonymous reject on code or off code.
    // | nLineId | is the line Id.
    // Return true if send anonymous reject on code, otherwise return false.
    bool GetAnonymousRejectSendCode(int nLineId) const;

    // Set Send anonymous reject on code or off code.
    // | nLineId | is the line Id.
    // | isOn | is true to set on code, false to set off code.
    void SetAnonymousRejectSendCode(int nLineId, bool isOn) const;

    // Get anonymous call on code.
    // | nLineId | is the line Id.
    // Return the on code if retrieved successfully, otherwise return empty string.
    const yl::string GetAnonymousOnCode(int nLineId) const;
    // Set anonymous call on code.
    // | nLineId | is the line Id.
    // | strOnCode | is the code to set.
    void SetAnonymousOnCode(int nLineId, const yl::string & strOnCode);

    // Get anonymous call off code.
    // | nLineId | is the line Id.
    // Return the off code if retrieved successfully, otherwise return empty.
    const yl::string GetAnonymousOffCode(int nLineId) const;
    // Set anonymous call on code.
    // | nLineId | is the line Id.
    // | strOffCode | is the code to set.
    void SetAnonymousOffCode(int nLineId, const yl::string & strOffCode);

    // Whether or not rejection is on.
    // | nLineId | is the line Id.
    // Return true if rejection is on, otherwise return false.
    bool IsRejectionOn(int nLineId) const;

    // Set rejection status, ON or OFF.
    // | nLineId | is the line Id.
    // | isOn | is true to set rejection on, false to set rejection off.
    void SetRejectionStatus(int nLineId, bool isOn);

    // Get rejection on code.
    // | nLineId | is the line Id.
    // Return the on code if retrieved successfully, otherwise return empty string.
    const yl::string GetRejectionOnCode(int nLineId) const;
    // Set rejection on code.
    // | nLineId | is the line Id.
    // | strOnCode | is the code to set.
    void SetRejectionOnCode(int nLineId, const yl::string & strOnCode);

    // Get rejection off code.
    // | nLineId | is the line Id.
    // Return the off code if retrieved successfully, otherwise return empty.
    const yl::string GetRejectionOffCode(int nLineId) const;
    // Set rejection on code.
    // | nLineId | is the line Id.
    // | strOffCode | is the code to set.
    void SetRejectionOffCode(int nLineId, const yl::string & strOffCode);

    bool IsAdministratorHasSet(int nAccount);

private:
    // Ctor and Dtor.
    CAnonymousCallModel();
    ~CAnonymousCallModel();

private:
    friend class CAnonymousCallController;
};
}

#endif  // SETTING_ANONYMOUSCALLMODEL_H_
