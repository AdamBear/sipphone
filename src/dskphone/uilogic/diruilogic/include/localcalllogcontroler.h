#ifndef __LOCAL_CALL_LOG_CONTROLER_H__
#define __LOCAL_CALL_LOG_CONTROLER_H__

#include "commondirctrl.h"


class CLocalCallLogControler : public CCommonDirCtrl
{
public:
    CLocalCallLogControler();
    virtual ~CLocalCallLogControler();

public:
    static CLocalCallLogControler * GetControler(CDirectoryControlerPtr & pControler);
    static LocalLogItemRecord * GetLocalCallLog(SmartPtr & pDetail);

public:
    bool DeleteCallLog(int nCallLogId);
    bool ClearAllCallLog();

    bool DialCallLog(int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    bool EditCallLog();

public:
    virtual void SetGroupId(int nGroupId);

    virtual bool IsCanEnterSearchByKey(int nKey) const;

    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool GetContactName(yl::string & strName);
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    virtual bool GetContactImage(int nId, yl::string & strImage) const;
#endif

    virtual int GetBrowseItemAmount();
    virtual int GetDialAction();

#if IF_BUG_27560
    bool GetContactNameToUI(yl::string & strName);
#endif
    bool IsChooseNumberToSend();
    bool OnNumberChooseResult(int nAccountId, const yl::string& strNumber);

protected:
    void OnCopyContact(int nContactId, int nAction, int nGroupId);

    virtual void PrepareDetailData();
    void GetDisplayNameByRelation(CALL_RELATION& eRelation,
                                  yl::string& strRelationNum, yl::string& strOutput);
    bool IsEnterNumberChoose(const yl::string& strRelationNum);
    void OnEnterNumberChoose(int nAccountId, const yl::string& strNumber,
                             const yl::string& strName, const yl::string& strRelationNum);
protected:
    bool m_bChooseNumberToSend;
};

#endif
