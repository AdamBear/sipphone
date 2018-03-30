#ifndef __COMMON_DIR_CTRL_H__
#define __COMMON_DIR_CTRL_H__

#include "directorycontroler.h"
#include "directorycommon.h"

#ifdef DIR_SUPPORT_SINGLE_UIHELPER
#include "diruihelper.h"
#else
class CCommonUIHelper;
#endif


class CCommonDirCtrl : public CDirectoryControler
{
public:
    explicit CCommonDirCtrl(int nDirType, DIR_TYPE eLogicType);
    virtual ~CCommonDirCtrl();

    static CCommonDirCtrl * GetCommonControler(CDirectoryControlerPtr & pControler);
    static int GetGroupIdByIndex(DIR_TYPE eLogicType, int nGroupIndex);
    static bool IsGroupId(DIR_TYPE eLogicType, int nId);
    static DIR_TYPE GetLogicType(int nDirType);
    static bool IsDirEnable(int nDirType);
    static bool GetDisplayName(DIR_TYPE eLogicType, int nId, yl::string & strName);
    static DirGroupRecord * GetGroupRecord(SmartPtr & pDetail);

public:
    virtual int GetParGroupId(int nGroupId);
    virtual bool PreSearchContact();
    virtual bool LoadMenuData(int nDataIndex, int nDataCount);
    virtual bool LoadListData(int nDataIndex, int nDataCount);
    virtual bool LoadDetailData(int nDataIndex, int nDataCount);
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool GetContactName(yl::string & strName);
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    virtual void GetDetailInfo(int & nId, int & nIndex, int & nCount) const;
    virtual bool GetContactImage(int nId, yl::string & strImage) const;
    virtual bool SwitchContact(int & nId, int & nIndex);
    virtual bool IsSpecialSearchGroup(int nGroupId);
#endif
    virtual int GetAdjoinGroupId(bool bPrevious, int & nIndex);

    int GetContactId() const;
    bool GetMenuGroupId(VEC_CONTACT_ID & vecId);

    SmartPtr GetDetail(int nId);
    SmartPtr GetCurrentDetail() const;

    bool IsGroupId(int nId);
    int GetGroupIndexById(int nGroupId);
    int GetGroupIdByIndex(int nGroupIndex);
    bool GetGroupName(int nGroupId, yl::string & strGroupName);
    int GetGroupIdByName(const yl::string & strGroupName);
    int GetContactAmount(int nGroupId);
#ifdef DIR_SUPPORT_SETTING
    bool GetGroupContactId(VEC_CONTACT_ID & vecContactId);
#endif

    static bool GetErrMsg(int nError, yl::string & strErrMsg);

protected:
    CCommonUIHelper * GetCommonUIHelper();
    bool LoadSearchListData(int nDataIndex, int nDataCount);
    bool LoadGroupListData(int nDataIndex, int nDataCount);
    void OnSearchResult(msgObject & refMessage);
    int GetSearchGroupId() const;
    virtual int GetLogicListGroupId();

protected:
    // 逻辑层类型
    DIR_TYPE m_eLogicType;
    int m_nContactId;
    int m_nSearchType;
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    int m_nDetailStatus;
#endif
};

#endif
