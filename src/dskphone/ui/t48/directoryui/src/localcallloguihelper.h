#ifndef __LOCAL_CALLLOG_UI_HELPER_H__
#define __LOCAL_CALLLOG_UI_HELPER_H__

#include "commonuihelper.h"


class CLocalCallLogUIHelper : public CCommonUIHelper
{
public:
    CLocalCallLogUIHelper();
    virtual ~CLocalCallLogUIHelper();

    static CLocalCallLogUIHelper * GetUIHelper(CDirUIHelperPtr & pUIHelper);

public:
    virtual bool FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount);
    virtual int GetActionByTitle(int nUIState, int nTitleAction);
    virtual void OnAttachFrameList();
    virtual void InitDetailTitle();

    bool IsSureDelete(int nId);

protected:
    virtual bool FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                              SmartPtr & pDetail);

    void InitSettingTitle();

    bool GetRelationDescribe(CALL_RELATION eRelation,
                             yl::string & strRelationNum, QString & strOutput);
};

#endif
