#ifndef SETTING_STATUSMODEL_H_
#define SETTING_STATUSMODEL_H_

#include <ylstring.h>

struct PhoneStatusData;

namespace NS_SETTING
{

// Class CStatusModel, records phone status info, such as phone version,
// Mac etc. It does not record phone network and account info, since they
// are manager by other class.
class CStatusModel
{
public:
    // Get phone model value.
    const yl::string GetModel() const;
    // Set phone model value.
    void SetModel(const yl::string & strModel);

    // Get phone hardware value.
    const yl::string GetHardWare() const;

    // Get phone firmware value.
    const yl::string GetFirmware() const;

    // Get phone product Id value.
    const yl::string GetProductId() const;
    // Set phone product Id value.
    void SetProductId(const yl::string & strProductId);

private:
    // Ctor and Dtor.
    CStatusModel();
    ~CStatusModel();

private:
    friend class CStatusController;
};

}  // namespace NS_SETTING

#endif  // SETTING_STATUSMODEL_H_
