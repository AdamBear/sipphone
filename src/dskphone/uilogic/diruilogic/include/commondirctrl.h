#ifndef __COMMON_DIR_CTRL_H__
#define __COMMON_DIR_CTRL_H__

#include "directorycontroler.h"
#include "directorycommon.h"


class CCommonDirCtrl : public CDirectoryControler
{
public:
    explicit CCommonDirCtrl(int nDirType, DIR_TYPE eLogicType);
    virtual ~CCommonDirCtrl();

    static CCommonDirCtrl * GetCommonControler(CDirectoryControlerPtr & pControler);
    static int GetGroupIdByIndex(DIR_TYPE eLogicType, int nGroupIndex);
    static DIR_TYPE GetLogicType(int nDirType);
    static bool IsDirEnable(int nDirType);
    static bool GetDisplayName(DIR_TYPE eLogicType, int nId, yl::string & strName);
    static DirGroupRecord * GetGroupRecord(SmartPtr & pDetail);

    static bool GetErrMsg(int nError, yl::string & strErrMsg);
    static void ShowErrorMsg(int nError);

public:
// Group
    virtual int GetParGroupId(int nGroupId);
    virtual int GetResultGroupId();
    virtual bool IsGroupData(SmartPtr & pDetail);
    virtual bool GetBrowseGroupList(VEC_CONTACT_ID & vecId);
    bool IsGroupId(int nId);

    bool GetMenuGroupId(VEC_CONTACT_ID & vecId);
    int GetGroupIndexById(int nGroupId);
    bool GetGroupName(int nGroupId, yl::string & strGroupName);

// Contact
    void ResetContactId();

    int GetContactId() const;
    SmartPtr GetDetail(int nId);
    SmartPtr GetCurrentDetail() const;
    SmartPtr GetDetailByIndex(int & nId, int nIndex);
    int GetContactAmount(int nGroupId);
    virtual bool GetContactShowName(yl::string & strName);

#ifdef DIR_SUPPORT_SETTING
    bool GetGroupContactId(VEC_CONTACT_ID & vecContactId);
#endif

// Browser
    virtual int GetBrowseItemAmount();

// Search
    virtual bool PreSearchContact();
    virtual int GetSearchItemAmount();

// LoadData
#if DIR_UI_SUPPORT_LOAD_DATA_LIST
    virtual bool LoadMenuData(int nDataIndex, int nDataCount);
    virtual bool LoadListData(int nDataIndex, int nDataCount);
#endif

    virtual bool OnAction(int nAction, int nDataIndex, int nId);

// Detail
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    virtual void GetDetailInfo(int & nId, int & nIndex, int & nCount) const;
    virtual bool GetContactImage(int nId, yl::string & strImage) const;
    virtual bool SwitchContact(int & nId, int & nIndex);
#endif

// Indicator
#if DIR_SUPPORT_INDICATOR
    bool IsHadContactList();
    bool GetIndicatorList(FastIndexList & listIndicator);
#endif
    virtual int GetMenuIdByIndex(int nIndex);

protected:
// Group
    int GetGroupChildrenAmount(int nGroupId, int nGroupType);
    int GetSearchGroupId() const;

// Contact
#if DIR_UI_SUPPORT_LOAD_DATA_LIST
    bool LoadSearchListData(int nDataIndex, int nDataCount);
    bool LoadGroupListData(int nDataIndex, int nDataCount);
#endif

    void EnterDetail(int nContactId, int nDataIndex, int nAction);

    virtual void PrepareDetailData();

// Search
    void OnSearchResult(msgObject & refMessage);

protected:
    // 逻辑层类型
    DIR_TYPE m_eLogicType;
    int m_nContactId;
};

#endif
