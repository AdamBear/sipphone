#ifndef __REMOTE_UI_HELPER_H__
#define __REMOTE_UI_HELPER_H__

#include "commonuihelper.h"

struct ContactRemoteUnit;
struct ContactRemoteMenu;

class CRemoteUIHelper : public CCommonUIHelper
{
public:
    CRemoteUIHelper();
    virtual ~CRemoteUIHelper();

    static CRemoteUIHelper * GetUIHelper(CDirUIHelperPtr & pUIHelper);

public:
    virtual bool FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount);
    virtual int GetActionByTitle(int nUIState, int nTitleAction);
    virtual void OnGroupChange();
    virtual void OnAttachFrameList();
    virtual void InitDetailTitle();

    void RefreshTitle();

protected:
    virtual bool RefreshListItemAmount(int nUIState, int nFromIndex, int nTotalCount);

    virtual bool FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                              SmartPtr & pDetail);

    void CheckSettingContactTitle();
    bool FillRemoteContact(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                           ContactRemoteUnit * pContact);

    void InitSettingTitle();
};

#endif
