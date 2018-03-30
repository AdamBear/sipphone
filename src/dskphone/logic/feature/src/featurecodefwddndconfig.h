#ifndef __FEATURE_CODE_FWD_DND_CONFIG_H__
#define __FEATURE_CODE_FWD_DND_CONFIG_H__

#include <ylstring.h>
#include "talk/fwd_dnd/include/fwddndtypes.h"

class CFeatureCodeFwdDndConfig
{
public:
    static yl::string GetDndCode(int iAccountId, bool bEnable);
    static yl::string GetFwdCode(int iAccountId, FWD_TYPE eType, bool bEnable);

private:
    CFeatureCodeFwdDndConfig();
};

#endif // !__FEATURE_CODE_FWD_DND_CONFIG_H__
