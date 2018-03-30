#ifndef __PLCM_UI_HELPER_H__
#define __PLCM_UI_HELPER_H__

#include "commonuihelper.h"

struct ContactRemoteUnit;
struct ContactRemoteMenu;
#if IF_BUG_PLCMDIR
class CPlcmUIHelper : public CCommonUIHelper
{
public:
    CPlcmUIHelper();
    virtual ~CPlcmUIHelper();

    static CPlcmUIHelper * GetUIHelper(CDirUIHelperPtr & pUIHelper);

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
    bool FillPlcmContact(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                         ContactRemoteUnit * pContact);

    void InitSettingTitle();

    virtual void RefreshSearchTitle(int nTotalCount);

};
#endif
#endif
