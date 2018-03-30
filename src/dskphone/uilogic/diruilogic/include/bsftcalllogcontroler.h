#ifndef __BSFT_CALL_LOG_CONTROLER_H__
#define __BSFT_CALL_LOG_CONTROLER_H__

#include "commondirctrl.h"


class CBSFTCallLogControler : public CCommonDirCtrl
{
public:
    CBSFTCallLogControler();
    virtual ~CBSFTCallLogControler();

public:
    static CBSFTCallLogControler * GetControler(CDirectoryControlerPtr & pControler);
    static BSFTCallLogRecord * GetBSFTCallLog(SmartPtr & pDetail);
#if IF_FEATURE_MUTI_CALLLOG
    static bool IsMutiAccountChooseGroup(int nGroupId);
#endif
public:
    virtual void SetGroupId(int nGroupId);
    virtual bool IsGroupData(SmartPtr & pDetail);

    virtual void OnEnterMenu();

    virtual bool IsCanEnterSearchByKey(int nKey) const;

    bool DialCallLog(int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    bool EditCallLog();
    bool IsPersonalCalllog();

    virtual bool PreLoadContact(int nGroupId);
#if DIR_UI_SUPPORT_LOAD_DATA_LIST
    virtual bool LoadMenuData(int nDataIndex, int nDataCount);
#endif
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool CancelAction(int nAction, int nDataIndex, int nId);
    virtual LRESULT OnMessage(msgObject & refMessage);

    virtual int GetBrowseItemAmount();

#if IF_FEATURE_MUTI_CALLLOG
    virtual bool GetBrowseGroupList(VEC_CONTACT_ID & vecId);
    virtual void UnInit();
    void SetCallType(CALL_TYPE eDirType);
    int GetAccountID();
#endif
#if IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST && IF_FEATURE_MUTI_CALLLOG
    virtual int GetMenuIdByIndex(int nIndex);
#endif

protected:
    void OnCopyContact(int nContactId);

    virtual void PrepareDetailData();

#if DIR_SUPPORT_NET_CALLLOG_DELETE
    bool OnDeleteGroup(int nAction, int nGroupId);
    bool CheckDeleteCallLog(int nRetCode, int nAction = DIR_ACTION_NONE);
#endif

protected:
    bool m_bDownloading;
#if IF_FEATURE_MUTI_CALLLOG
    CALL_TYPE m_eDirType;
#endif
#if IF_FEATURE_MUTI_CALLLOG
    int m_nAccountID;
#endif
};

#endif
