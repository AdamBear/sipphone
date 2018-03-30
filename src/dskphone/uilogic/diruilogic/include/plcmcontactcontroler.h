#ifndef __PLCM_CONTACT_CONTROLER_H__
#define __PLCM_CONTACT_CONTROLER_H__

#include "commondirctrl.h"


#if IF_BUG_PLCMDIR
class CPlcmContactControler : public CCommonDirCtrl
{
public:
    CPlcmContactControler();
    virtual ~CPlcmContactControler();

public:
    static CPlcmContactControler * GetControler(CDirectoryControlerPtr & pControler);
    static ContactRemoteUnit * GetPlcmContact(SmartPtr & pDetail);
    static ContactRemoteMenu * GetPlcmGroup(SmartPtr & pDetail);

public:
    int GetDetailType(int nId);
    bool GetGroupTitle(int nGroupId, yl::string & strTitle);

    void UpdateContacts(int nGroupId);
    bool OnBack();

    virtual int GetBrowseItemAmount();

    virtual bool PreLoadContact(int nGroupId);
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccount = AUTO_ACCOUNT_IDENTIFY);
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool CancelAction(int nAction, int nDataIndex, int nId);
    virtual LRESULT OnMessage(msgObject & refMessage);

protected:
    void OnCopyContact(int nContactId, int nAction, int nTarGroupId);
    void OnDownLoadFinished(msgObject & refMessage);
#ifdef DIR_SUPPORT_BATCH_OPERATE
    bool CopyToLocalContact(int nGroupId, VEC_CONTACT_ID & vecContactId);
#endif

    virtual void PrepareDetailData();

protected:
    bool m_bDownloading;
    bool m_bUpdating;
    int m_nDownLoadGroupId;
};

#endif
#endif
