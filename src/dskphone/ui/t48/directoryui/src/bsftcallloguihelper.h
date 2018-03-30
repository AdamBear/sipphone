#ifndef __BSFT_CALLLOG_UI_HELPER_H__
#define __BSFT_CALLLOG_UI_HELPER_H__

#include "commonuihelper.h"


class CBSFTCallLogUIHelper : public CCommonUIHelper
{
public:
    CBSFTCallLogUIHelper();
    virtual ~CBSFTCallLogUIHelper();

    static CBSFTCallLogUIHelper * GetUIHelper(CDirUIHelperPtr & pUIHelper);

public:
    virtual bool FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount);
    virtual int GetActionByTitle(int nUIState, int nTitleAction);
    virtual void OnAttachFrameList();
    virtual void InitDetailTitle();

#if IF_FEATURE_MUTI_CALLLOG
    void RefreshCallLogGroup(int nGroupID, bool bRefresh = true);
#endif

protected:
    virtual bool FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                              SmartPtr & pDetail);

    void InitSettingTitle();
};

#endif
