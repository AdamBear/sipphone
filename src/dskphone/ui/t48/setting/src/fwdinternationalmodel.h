#ifndef  SETTING_FWDINTERNATIONALMODEL_H_
#define  SETTING_FWDINTERNATIONALMODEL_H_

#include <ylstring.h>

namespace NS_SETTING
{
// Class CFWDInternationalModel.
class CFWDInternationalModel
{
public:
    // 获取FWDInternational开关
    bool GetFWDInternationalEnabled() const;

    // 设置FWDInternational开关
    void SetFWDInternationalEnabled(bool bEnabled);

private:
    // 构造和析构
    CFWDInternationalModel();
    ~CFWDInternationalModel();

    friend class CFWDInternationalController;
};

}  // namespace NS_SETTING

#endif  //  SETTING_FWDINTERNATIONALMODEL_H_
