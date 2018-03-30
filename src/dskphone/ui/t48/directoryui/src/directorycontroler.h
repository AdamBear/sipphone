#ifndef __DIRECTORY_CONTROLER_H__
#define __DIRECTORY_CONTROLER_H__

#include <ylsmartptr.h>
#include "../include/directorycommon.h"

#ifdef FL_SUPPORT
class CDirectoryListDelegate;
#ifndef CDirectoryListDelegatePtr
typedef YLSmartPtr<CDirectoryListDelegate> CDirectoryListDelegatePtr;
#endif
#endif

class CDirUIHelper;
#ifdef DIR_SUPPORT_SINGLE_UIHELPER
#ifndef CDirUIHelperPtr
typedef CDirUIHelper * CDirUIHelperPtr;
#endif
#else
#ifndef CDirUIHelperPtr
typedef YLSmartPtr<CDirUIHelper> CDirUIHelperPtr;
#endif
#endif


class CDirectoryControler
{
public:
    explicit CDirectoryControler(int nType);
    virtual ~CDirectoryControler();

public:
    static bool IsUserLevelShow(int nDirType, int nGroupId = -1);
    static bool IsDirCanEidt(int nDirModType);

public:
    void SetReadOnly(bool bReadOnly);
    bool IsReadOnly() const;

    int GetType() const;

    void SetGroupId(int nGroupId);
    int GetGroupId() const;

#ifdef FL_SUPPORT
    void Init(CDirectoryListDelegatePtr & pDelegate, CDirUIHelperPtr & pUIHelper);
#else
    void Init(CDirUIHelperPtr & pUIHelper);
#endif
    virtual void UnInit();

    CDirUIHelperPtr GetUIHelper();

    bool IsInMenu() const;
    bool IsBrowserState();
    bool IsSearchState();
    bool IsDetailState();
#ifdef DIR_SUPPORT_SETTING
    bool IsSettingState();
#endif
    int GetState();

    const yl::string & GetSearchKey();

    virtual bool IsCanOnlySearch() const;
    virtual void OnEnterSearch();

    virtual int GetParGroupId(int nGroupId);

    virtual void PreLoadContactOnUIReturn(int nGroupId) {}
    virtual bool PreLoadContact(int nGroupId);

    virtual bool PreSearchContact() = 0;
    virtual bool LoadMenuData(int nDataIndex, int nDataCount)           = 0;
    virtual bool LoadListData(int nDataIndex, int nDataCount)           = 0;
    virtual bool LoadDetailData(int nDataIndex, int nDataCount)         = 0;
    virtual bool OnAction(int nAction, int nDataIndex, int nId)         = 0;
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY)    = 0;
    virtual bool GetContactName(yl::string & strName)                    = 0;
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    virtual void GetDetailInfo(int & nId, int & nIndex, int & nCount) const = 0;
    virtual bool GetContactImage(int nId, yl::string & strImage) const   = 0;
    virtual bool SwitchContact(int & nId, int & nIndex)                   = 0;
    virtual bool IsSpecialSearchGroup(int nGroupId)
    {
        return false;
    }
    virtual bool LoadMore()
    {
        return false;
    }
    virtual void RefreshTotalCount();
#endif
    virtual int GetAdjoinGroupId(bool bPrevious, int & nIndex);
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool CancelAction(int nAction, int nDataIndex, int nId);
    virtual LRESULT OnMessage(msgObject & refMessage);

#ifdef DIR_SUPPORT_SWITCH_DETAIL
protected:
    void SetTotalCount(int nTotalCount);
    void SetCurIndex(int nCurIndex);
public:
    int GetTotalCount() const;
    int GetCurIndex() const;
#endif

public:
    bool IsBlindTransfer();
    void SetBlindTransfer(bool bBlindTransfer);

protected:
    // 是否只读状态(通话时，不能修改联系人和群组织)
    static bool s_bReadOnly;
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    static int s_nTotalCount;
    static int s_nCurIndex;
#endif

#ifdef FL_SUPPORT
    CDirectoryListDelegatePtr m_pDelegate;
#endif

    CDirUIHelperPtr m_pUIHelper;

    // 类型
    int m_nType;

    // 当前操作的群组号
    int m_nGroupId;

    // 是否盲转
    bool m_bBlindTransfer;
};

typedef YLSmartPtr<CDirectoryControler> CDirectoryControlerPtr;

#endif
