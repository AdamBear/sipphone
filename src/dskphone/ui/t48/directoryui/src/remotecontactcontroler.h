#ifndef __REMOTE_CONTACT_CONTROLER_H__
#define __REMOTE_CONTACT_CONTROLER_H__

#include "commondirctrl.h"
#ifndef DIR_SUPPORT_SINGLE_UIHELPER
class CRemoteUIHelper;
#endif


class CRemoteContactControler : public CCommonDirCtrl
{
public:
    CRemoteContactControler();
    virtual ~CRemoteContactControler();

public:
    static CRemoteContactControler * GetControler(CDirectoryControlerPtr & pControler);
    static ContactRemoteUnit * GetRemoteContact(SmartPtr & pDetail);
    static ContactRemoteMenu * GetRemoteGroup(SmartPtr & pDetail);
    static bool IsServerSearchGroup(int nGroupId);

public:
    int GetHotKeyGroupId(int nKeyCode);
    int GetDetailType(int nId);
    int GetSubMenuIndexbyId(int nMenuId);
    bool GetGroupTitle(int nGroupId, yl::string & strTitle);
    bool IsHadContactInGroup(int nGroupId);

    void UpdateContacts(int nGroupId);
    bool OnBack();

    virtual bool PreLoadContact(int nGroupId);
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccount = AUTO_ACCOUNT_IDENTIFY);
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool CancelAction(int nAction, int nDataIndex, int nId);
    virtual LRESULT OnMessage(msgObject & refMessage);

    CRemoteUIHelper * GetRemoteUIHelper();

protected:
    void GetNumberAttr(ContactRemoteUnit * pRecord, LIST_ATTRIBUTE & lsNumber, bool bCopy);
    void OnCopyContact(int nAction, int nId);
    void OnDownLoadFinished(msgObject & refMessage);
#ifdef DIR_SUPPORT_BATCH_OPERATE
    bool CopyToLocalContact(int nGroupId, VEC_CONTACT_ID & vecContactId);
#endif

protected:
    bool m_bDownloading;
    bool m_bUpdating;
    int m_nHotKeyId;
    int m_nDownLoadGroupId;
    yl::string m_strHotKey;
};

#endif
