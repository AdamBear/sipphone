#ifndef __I_BTUC_UI_HELPER_H__
#define __I_BTUC_UI_HELPER_H__

#include <diruilogic/include/diruilogicdefine.h>

#if DIR_UI_SUPPORT_LOAD_DATA_LIST && FEATURE_BROADTOUCH_UC
#include "directorycommon.h"

class IBTUCUIHelper
{
public:
    IBTUCUIHelper() {}
    virtual ~IBTUCUIHelper() {}

public:
    virtual bool FillGroupList(GroupList & listGroup, int nDataIndex, int nDataCount)    = 0;
    virtual bool FillContactList(ContactBuddyListData & listContact)                     = 0;
    virtual bool FillContact(PresenceInfo & info, int nDataIndex, int nDataCount)        = 0;
};
#endif

#endif
