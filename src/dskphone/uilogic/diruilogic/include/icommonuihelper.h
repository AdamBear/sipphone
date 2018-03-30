#ifndef __I_COMMON_UI_HELPER_H__
#define __I_COMMON_UI_HELPER_H__

#include <diruilogic/include/diruilogicdefine.h>

#if DIR_UI_SUPPORT_LOAD_DATA_LIST
#include "directorycommon.h"

class ICommonUIHelper
{
public:
    ICommonUIHelper() {}
    virtual ~ICommonUIHelper {}

public:
    virtual bool FillData(ContactListData & listData)                                        = 0;
};
#endif

#endif
