#ifndef SETTING_PHONELOCKMODEL_H_
#define SETTING_PHONELOCKMODEL_H_

#include "setting/include/common.h"

namespace NS_SETTING
{
// Class CPhoneLockModel.
class CPhoneLockModel
{
public:
    // Get phone lock type.
    // Return phone lock type if retrieved successfully, otherwise
    // return PHONE_LOCK_TYPE_NONE.
    PhoneLockType GetPhoneLockType() const;
    // Set phone lock type.
    // | ePhoneLockType | is the type to set.
    void SetPhoneLockType(PhoneLockType ePhoneLockType);

private:
    // Ctor and Dtor.
    CPhoneLockModel();
    ~CPhoneLockModel();

    friend class CPhoneLockController;
};
}  // namespace NS_SETTING

#endif  // SETTING_PHONELOCKMODEL_H_
