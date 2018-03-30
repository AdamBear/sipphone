#if IF_FEATURE_METASWITCH_CALLLOG
#ifndef __MTSW_CALLLOG_UI_HELPER_H__
#define __MTSW_CALLLOG_UI_HELPER_H__

#include "commonuihelper.h"


class CMTSWCallLogUIHelper : public CCommonUIHelper
{
public:
    CMTSWCallLogUIHelper();
    virtual ~CMTSWCallLogUIHelper();

    static CMTSWCallLogUIHelper * GetUIHelper(CDirUIHelperPtr & pUIHelper);

public:
    virtual bool FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount);
    virtual int GetActionByTitle(int nUIState, int nTitleAction);
    virtual void OnAttachFrameList();
    virtual void InitDetailTitle();

protected:
    virtual bool FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                              SmartPtr & pDetail);

    void InitSettingTitle();
};

#endif
#endif //IF_FEATURE_METASWITCH_CALLLOG
