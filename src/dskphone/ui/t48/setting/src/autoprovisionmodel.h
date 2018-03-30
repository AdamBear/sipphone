#ifndef SETTING_AUTOPROVISION_H_
#define SETTING_AUTOPROVISION_H_

#include <ylstring.h>

namespace NS_SETTING
{
// Class CAutoProvisionModel.
class CAutoProvisionModel
{
public:
    // Get auto provision URL.
    // Return URL if retrieved successfully, otherwise return
    // empty string.
    const yl::string GetUrl() const;
    // Set auto provision URL.
    // | strUrl | is the URL to set.
    void SetUrl(const yl::string & strUrl);

    // Get auto provision user name.
    const yl::string GetPUserName() const;
    // Set auto provision user name.
    // | strUserName | is the username to set.
    void SetUserName(const yl::string & strUserName);

    // Get auto provision password.
    // Return the password if retrieved successfully, otherwise return
    // empty string.
    const yl::string GetPassword() const;
    // Set auto provision password.
    // | strPassword | is the password to set.
    void SetPassword(const yl::string & strPassword);

    const yl::string GetAuthenticationUserName() const;

    const yl::string GetAuthenticationassword() const;

    void StartCurrentConnect(yl::string & strName, yl::string & strPassword);

    // Get AES key.
    // Returns AES key if retrieved successfully, otherwise return false.
    const yl::string GetAesKey() const;
    // Set AES key.
    // | strAesKey | is the key to set.
    void SetAesKey(const yl::string & strAesKey);

    // Get AES MAC key.
    // Return AES MAC key if retrieved successfully, otherwise return false.
    const yl::string GetAesMacKey() const;
    // Set AES MAC key.
    // | strAesMacKey | is the MAC key to set.
    void SetAesMacKey(const yl::string & strAesMacKey);

private:
    // Ctor and Dtor.
    CAutoProvisionModel();
    ~CAutoProvisionModel();

    friend class CAutoProvisionController;
};

}  // namespace NS_SETTING

#endif  // SETTING_AUTOPROVISION_H_
