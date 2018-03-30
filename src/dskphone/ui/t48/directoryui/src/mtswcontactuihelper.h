#if IF_FEATURE_METASWITCH_DIRECTORY
#ifndef __MTSW_CONTACT_UI_HELPER_H__
#define __MTSW_CONTACT_UI_HELPER_H__

#include "commonuihelper.h"
#include "mtswdetaillayout.h"
#include "broadsoftsearchlayout.h"
#include "localdetaillayout.h"


class CMTSWContactUIHelper : public CCommonUIHelper
{
public:
    CMTSWContactUIHelper();
    virtual ~CMTSWContactUIHelper();

    bool GetMTSWContactListData(ContactMetaSwitchContactList & itemData);

    static CMTSWContactUIHelper * GetUIHelper(CDirUIHelperPtr & pUIHelper);
    static void GetMetaSwitchDisplayName(int nGroupId, const yl::string & strGroupName,
                                         QString & strDisplayName);

public:
    void SetAdditionLayout(CMTSWDetailLayoutPtr pLayout);
    void SetDetailLayout(CMTSWDetailLayoutPtr pDetailLayout);

    void SetInputFocus(const char * pszAttr);
    void OnDetailCtrlFocus(bool bFocus);

public:
    virtual bool FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount);
    virtual int GetActionByTitle(int nUIState, int nTitleAction);
    virtual void OnGroupChange();
    virtual void OnAttachFrameList();
    virtual void InitDetailTitle();

    void LoadMTSWAdditionData(int nAction, int nDataIndex, int nDataCount);

protected:
    virtual bool RefreshListItemAmount(int nUIState, int nFromIndex, int nTotalCount);
    virtual bool FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                              SmartPtr & pDetail);

    void InitSettingTitle();

    void CheckSettingTitle();

    bool FillContactListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                             ContactMetaSwitchContactList * pContact);
    bool FillExtentionItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                           ContactMetaSwitchExtention * pContact);
    bool FillMLHGItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                      ContactMetaSwitchMLHG * pContact);

    bool FillExtentionDetail(ContactMetaSwitchExtention * pContact);
    bool FillMLHGDetail(ContactMetaSwitchMLHG * pContact);

    bool FillNormalDetail(int nDataIndex, int nTotalCount, VEC_ATTRIBUTE & vecAttr);
    bool FillContactDetail(int nId, ContactMetaSwitchContactList * pDetail, int nDataIndex,
                           int nDataCount);
    void InitCopyLayout();

    virtual void InitUpdateTitle();
protected:
    // 详情列表布局
    CMTSWDetailLayoutPtr m_pDetailLayout;
    // 联系人详情是否编辑状态
    bool m_bDetailEditing;
};

#endif
#endif
