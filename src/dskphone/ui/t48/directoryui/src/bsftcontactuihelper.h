#ifndef __BSFT_CONTACT_UI_HELPER_H__
#define __BSFT_CONTACT_UI_HELPER_H__

#include "commonuihelper.h"
#include "broadsoftdetaillayout.h"
#include "broadsoftsearchlayout.h"
#include "localdetaillayout.h"


class CBSFTContactUIHelper : public CCommonUIHelper
{
public:
    CBSFTContactUIHelper();
    virtual ~CBSFTContactUIHelper();

    static CBSFTContactUIHelper * GetUIHelper(CDirUIHelperPtr & pUIHelper);
    static void GetGroupDisplayName(int nGroupId, const yl::string & strGroupName,
                                    QString & strDisplayName);
    static void OnPreciseSearchResult();

public:
    void SetAdditionLayout(CLocalDetailLayoutPtr pLayout);
    void SetDetailLayout(CBroadSoftDetailLayoutPtr pDetailLayout);
    void SetSearchLayout(CBroadSoftSearchLayoutPtr pSearchLayout);
    CBroadSoftSearchLayoutPtr GetSearchLayout();
    void SetInputFocus(const char * pszAttr);
    void OnDetailCtrlFocus(bool bFocus);
    void CheckPreciseSearchUI();
    bool LoadBSFTPreciseSearchData(int nDataIndex, int nDataCount);

    bool GetBSFTPersonalData(ContactBroadSoftPersonal & itemData);
    bool GetBSFTPreciseSearchData(int & nGroupId, ContactBroadSoftItemData & itemData);


public:
    virtual bool FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount);
    virtual int GetActionByTitle(int nUIState, int nTitleAction);
    virtual void OnGroupChange();
    virtual void OnAttachFrameList();
    virtual void InitDetailTitle();
    virtual void RefreshSearchTitle(int nTotalCount);
    virtual void RefreshUpdateTitle(int nContactAmount, bool bRefresh = false);

protected:
    virtual bool RefreshListItemAmount(int nUIState, int nFromIndex, int nTotalCount);
    virtual bool FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                              SmartPtr & pDetail);

    void InitSettingTitle();
    void InitPreciseSearchTitle();
    void CheckSettingTitle();

    bool FillCommonItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                        ContactBroadSoftCommon * pContact);
    bool FillPersonalItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                          ContactBroadSoftPersonal * pContact);
    bool FillEnterpriseItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                            ContactBroadSoftEnterprise * pContact);
    bool FillCustomItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                        ContactBroadSoftCustom * pContact);
    bool FillGroupItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                       ContactBroadSoftGroup * pContact);

    bool FillPersonalDetail(ContactBroadSoftPersonal * pContact);
    bool FillCommonDetail(ContactBroadSoftCommon * pContact);
    bool FillEnterpriseDetail(int nDataIndex, int nDataCount, ContactBroadSoftEnterprise * pContact);
    bool FillCustomDetail(int nDataIndex, int nDataCount, ContactBroadSoftCustom * pContact);
    bool FillGroupDetail(int nDataIndex, int nDataCount, ContactBroadSoftGroup * pContact);
    bool FillNormalDetail(int nDataIndex, int nTotalCount, VEC_ATTRIBUTE & vecAttr);

protected:
    // 添加布局
    CLocalDetailLayoutPtr m_pAdditionLayout;
    // 详情列表布局
    CBroadSoftDetailLayoutPtr m_pDetailLayout;
    // 搜索列表布局
    CBroadSoftSearchLayoutPtr m_pSearchLayout;
    // 联系人详情是否编辑状态
    bool m_bDetailEditing;
};

#endif
