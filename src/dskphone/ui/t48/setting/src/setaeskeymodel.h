#ifndef SETTING_SETAESKEYMODEL_H_
#define SETTING_SETAESKEYMODEL_H_

#include <ylstring.h>

namespace NS_SETTING
{
// Class CSetAesKeyModel.
class CSetAesKeyModel
{
public:
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
    CSetAesKeyModel();
    ~CSetAesKeyModel();

    friend class CSetAesKeyController;
};

}  // namespace NS_SETTING

#endif  // SETTING_SETAESKEYMODEL_H_
