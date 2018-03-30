#ifndef __LOCAL_CONTACT_UI_HELPER_H__
#define __LOCAL_CONTACT_UI_HELPER_H__

#include "commonuihelper.h"
#include "localdetaillayout.h"

class CComboBoxListItem;

class CLocalContactUIHelper : public CCommonUIHelper
{
public:
    CLocalContactUIHelper();
    virtual ~CLocalContactUIHelper();

    static CLocalContactUIHelper * GetUIHelper(CDirUIHelperPtr & pUIHelper);

    static void GetGroupDisplayName(int nGroupId, const yl::string & strGroupName,
                                    QString & strDisplayName);

public:
    void SetDetailLayout(CLocalDetailLayoutPtr pDetailLayout);
    void SetInputFocus(const char * pszAttr);
    void OnDetailCtrlFocus(bool bFocus);

    bool GetLocalGroupData(ContactGroupData & groupData);
    bool GetLocalContactData(int & nGroupId, ContactGeneralItemData & itemData);
    bool IsSameContactRing(yl::string & strRing, yl::string & strContactRing);
    bool IsSameContactPhoto(yl::string & strPhoto, yl::string & strContactPhoto);

    virtual bool FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount);
    virtual int GetActionByTitle(int nUIState, int nTitleAction);
    virtual void OnGroupChange();
    virtual void OnAttachFrameList();
    virtual void InitDetailTitle();

    void LoadLocalAdditionData(int nAction, int nDataIndex, int nDataCount);

#if IF_FEATURE_FAVORITE_DIR
    bool IsFromDsskeyAdd();

    bool GetLocalContactFavorite(yl::string & strFavoriteIndex);
#endif

protected:
    virtual bool FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                              SmartPtr & pDetail);

protected:
    void InitSettingTitle();
    void InitCopyLayout();
    bool IsCanMoveOtherGroup(int nGroupId);

    void FillNumberComboBoxList(CComboBoxListItem * pCmbNumberItem, yl::string & strFirstSelectNumber,
                                yl::string & strSecondSelectNumber, yl::string & strThirdSelectNumber);

    void FillNumberComboBoxList(CComboBoxListItem * pCmbNumberItem,
                                YLList<yl::string> & listSelectNumber);

    bool FillContactDetail(int nId, LocalDirContactRecord * pContact, int nDataIndex, int nDataCount);

protected:
    // 详情列表布局
    CLocalDetailLayoutPtr m_pDetailLayout;
    // 联系人详情是否编辑状态
    bool m_bDetailEditing;
};

#endif
