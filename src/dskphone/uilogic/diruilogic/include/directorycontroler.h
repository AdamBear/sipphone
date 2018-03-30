#ifndef __DIRECTORY_CONTROLER_H__
#define __DIRECTORY_CONTROLER_H__

#include <ylsmartptr.h>
#include "directorycommon.h"
#include <diruilogic/include/diruilogicdefine.h>
#include "idiruihelper.h"


class CDirectoryControler
{
public:
    explicit CDirectoryControler(int nType);
    virtual ~CDirectoryControler();

public:
    static bool IsUserLevelShow(int nDirType, int nGroupId = -1);
    static bool IsDirCanEdit(int nDirType, int nGroupId);
    static bool IsDirCanEidt(int nDirModType);

    static void SetAttributeList(int nDirType, LIST_ATTRIBUTE & lsAtrr);
    static int GetAttributeType();
    static int GetAttributeSize();
    static int GetAttributeCount(const char * pszName);
    static bool GetAttributeByIndex(int nIndex, yl::string & strName, yl::string & strValue);
    static bool GetAttributeByName(int nIndex, const char * pszName, yl::string & strValue);

public:
    void Init(IDirUIHelperPtr pUIHelper);
    virtual void UnInit();

    IDirUIHelperPtr GetUIHelper();

    int GetType() const;

    virtual void SetGroupId(int nGroupId);
    int GetGroupId() const;
    bool IsInMenu() const;

    virtual void EnterState(int nState);
    virtual void LeaveState(int nState);
    virtual void OnEnterMenu();

    bool IsBrowseState();
    bool IsSearchState();
    bool IsDetailState();
#ifdef DIR_SUPPORT_SETTING
    bool IsSettingState();
#endif
    int GetState();
    int GetDetailState();

    int GetDetailItemAmount();
    bool GetDetailData(int nIndex, ST_DETAIL_DATA & stDetail);

    const yl::string & GetSearchKey();

    virtual bool IsCanOnlySearch() const;
    virtual bool IsManualSearch() const;
    virtual bool IsEmptyKeySearch() const;
    virtual bool IsCanEnterSearchByKey(int nKey) const;

    virtual int GetSearchDirType(int nGroupId);
    virtual int GetParGroupId(int nGroupId);
    virtual int GetDialAction();

    virtual bool EnterGroupByHotKey(int nKey);
    virtual bool IsCanEnterGroupByKey(int nKey);
    virtual bool PreLoadContact(int nGroupId);
    virtual bool PreSearchContact()                                     = 0;
#if DIR_UI_SUPPORT_LOAD_DATA_LIST
    virtual bool LoadMenuData(int nDataIndex, int nDataCount)           = 0;
    virtual bool LoadListData(int nDataIndex, int nDataCount)           = 0;
#endif
    virtual bool OnAction(int nAction, int nDataIndex, int nId)         = 0;
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY)    = 0;
    virtual bool GetContactShowName(yl::string & strName)                = 0;
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    virtual void GetDetailInfo(int & nId, int & nIndex, int & nCount) const = 0;
    virtual bool GetContactImage(int nId, yl::string & strImage) const   = 0;
    virtual bool SwitchContact(int & nId, int & nIndex)                   = 0;
    virtual void RefreshTotalCount();
#endif
    virtual int GetBrowseItemAmount()                                   = 0;
    virtual int GetSearchItemAmount()                                   = 0;
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool CancelAction(int nAction, int nDataIndex, int nId);
    virtual LRESULT OnMessage(msgObject & refMessage);
    bool IsBlindTransfer();
    void SetBlindTransfer(bool bBlindTransfer);

protected:
    bool Dial(int nAccountId, const yl::string & strName, LIST_ATTRIBUTE & lsAttr, int nDialAction);
    void RefreshUI(int nType, int nParam);

#ifdef DIR_SUPPORT_SWITCH_DETAIL
protected:
    void SetTotalCount(int nTotalCount);
    void SetCurIndex(int nCurIndex);
public:
    int GetTotalCount() const;
    int GetCurIndex() const;
#endif

protected:
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    static int s_nTotalCount;
    static int s_nCurIndex;
#endif

    // 联系人拷贝的数据、拔号号码列表
    static int s_nAttrType;
    static LIST_ATTRIBUTE s_listAtrr;

    IDirUIHelperPtr m_pUIHelper;

    // 类型
    int m_nType;

    // 当前操作的群组号
    int m_nGroupId;

    // 当前状态
    int m_nState;

    int m_nDetailStatus;

    bool m_bBlindTransfer;

    LIST_DIR_UI_STATE m_listState;
    VEC_DETAIL_DATA m_vecDetail;
};

typedef YLSmartPtr<CDirectoryControler> CDirectoryControlerPtr;

#endif
