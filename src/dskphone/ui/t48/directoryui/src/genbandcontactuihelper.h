#ifndef __GB_CONTACT_UI_HELPER_H__
#define __GB_CONTACT_UI_HELPER_H__

#include "commonuihelper.h"
#include "genbandpabdetaillayout.h"
#include "genbandsearchlayout.h"
#include "localdetaillayout.h"

#if IF_FEATURE_GBBOOK
class CGenbandContactUIHelper : public CCommonUIHelper
{
public:
    CGenbandContactUIHelper();
    virtual ~CGenbandContactUIHelper();

    static CGenbandContactUIHelper * GetUIHelper(CDirUIHelperPtr & pUIHelper);



    static void GetGroupDisplayName(int nGroupId, const yl::string & strGroupName,
                                    QString & strDisplayName);


public:
    void SetDetailLayout(CGenbandDetailLayoutPtr pDetailLayout);

    void SetSearchLayout(CGenbandSearchLayoutPtr pSearchLayout);

    void SetInputFocus(const char * pszAttr);


    void OnDetailCtrlFocus(bool bFocus);

    void CheckPreciseSearchUI();
    bool LoadGBPreciseSearchData(int nDataIndex, int nDataCount);

    bool GetPABContactData(ContactGBBookUnit & itemData);
    bool GetGenbandGroupData(yl::string strGroupName);

    GBBOOK_OPERATION_TYPE GetGABPreciseSearchType();
    bool GetGABPreciseSearchData(yl::string & strKey);

    void SetGABLastSearchData();

    void LoadGenbandAdditionData(int nAction, int nDataIndex, int nDataCount);

    void SetGABGroupIsSelect();
public:
    virtual bool FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount);
    virtual int GetActionByTitle(int nUIState, int nTitleAction);
    virtual void OnGroupChange();
    virtual void OnAttachFrameList();
    virtual void InitDetailTitle();

protected:
    virtual bool RefreshListItemAmount(int nUIState, int nFromIndex, int nTotalCount);
    virtual bool FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                              SmartPtr & pDetail);

    void InitSettingTitle();
    void InitPreciseSearchTitle();
    void CheckSettingTitle();

    bool FillGenbandItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                         ContactGBBookUnit * pContact);

    bool FillNormalDetail(int nDataIndex, int nTotalCount, VEC_ATTRIBUTE & vecAttr);

    bool FillGenbandDetail(int nDataIndex, int nDataCount, ContactGBBookUnit * pContact,
                           bool bIsPab = true);

    bool FillGenbandEdit(int nId, ContactGBBookUnit * pDetail, int nDataIndex, int nDataCount);
    void InitCopyLayout();

protected:
    // 详情列表布局
    CGenbandDetailLayoutPtr m_pDetailLayout;
    // 搜索列表布局
    CGenbandSearchLayoutPtr m_pSearchLayout;
    // 联系人详情是否编辑状态
    bool m_bDetailEditing;
};
#endif

#endif
