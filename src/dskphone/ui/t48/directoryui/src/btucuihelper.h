#ifndef __BTUC_UI_HELPER_H__
#define __BTUC_UI_HELPER_H__

#include "diruihelper.h"
#include "presence/include/modpresence.h"


class CBTUCUIHelper : public CDirUIHelper
{
public:
    CBTUCUIHelper();
    virtual ~CBTUCUIHelper();

    static CBTUCUIHelper * GetUIHelper(CDirUIHelperPtr & pUIHelper);

public:
    virtual void OnAttachFrameList();
    virtual void InitDetailTitle();

#if FEATURE_BROADTOUCH_UC
    bool FillGroupList(GroupList & listGroup, int nDataIndex, int nDataCount);
    bool FillContactList(ContactBuddyListData & listContact);
    bool FillContact(PresenceInfo & info, int nDataIndex, int nDataCount);

protected:
    bool RefreshContactItem(int nItemIndex, int nDataIndex, int nDataCount, PresenceInfo & info);
#endif
};
typedef CBTUCUIHelper * CBTUCUIHelperPtr;

#endif
