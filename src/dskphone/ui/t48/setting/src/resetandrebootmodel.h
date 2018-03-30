#ifndef SETTING_RESETANDREBOOTMODEL_H_
#define SETTING_RESETANDREBOOTMODEL_H_

namespace NS_SETTING
{
// Class CResetAndRebootModel.
class CResetAndRebootModel
{
public:
    // Get reset option.
    // Return reset option(enum ResetOptionType) if retrieved successfully,
    // otherwise return false.
    int GetResetOption() const;
    // Set reset option.
    // | nOption | is the reset option, it is an enum ResetOptionType.
    void SetResetOption(int nOption);

private:
    // Ctor and Dtor.
    CResetAndRebootModel();
    ~CResetAndRebootModel();

    friend class CResetAndRebootController;
};

}

#endif  // SETTING_RESETANDREBOOTMODEL_H_
