#ifndef __BLUETOOTH_CONTACT_CONTROLER_H__
#define __BLUETOOTH_CONTACT_CONTROLER_H__

#include <diruilogic/include/diruilogicdefine.h>

#ifdef IF_SUPPORT_BLUETOOTH_CONTACT
#include "commondirctrl.h"


class CBluetoothContactControler : public CCommonDirCtrl
{
public:
    CBluetoothContactControler();
    virtual ~CBluetoothContactControler();

public:
    static CBluetoothContactControler * GetControler(CDirectoryControlerPtr & pControler);
    static LocalDirContactRecord * GetLocalContact(SmartPtr & pDetail);

public:
    void UpdateContacts();

    virtual bool IsGroupData(SmartPtr & pDetail);

    virtual bool PreLoadContact(int nGroupId);
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool CancelAction(int nAction, int nDataIndex, int nId);
    virtual LRESULT OnMessage(msgObject & refMessage);

protected:
    void GetNumberAttr(LocalDirContactRecord * pRecord, LIST_ATTRIBUTE & lsNumber, bool bCopy);
    void GetAttributeByName(LocalDirContactRecord * pRecord, LIST_ATTRIBUTE & lsNumber, bool bCopy
                            , const yl::string& strName, const yl::string& strShow);
    void OnCopyContact(int nContactId, int nAction, int nGroupId);
    void OnDownLoadFinished(msgObject & refMessage);
#ifdef DIR_SUPPORT_BATCH_OPERATE
    bool CopyToLocalContact(int nGroupId, VEC_CONTACT_ID & vecContactId);
#endif
    virtual void PrepareDetailData();
    void AddDetailDataByName(LocalDirContactRecord * pRecord, const yl::string& strName, const yl::string& strShow);

protected:
    bool m_bDownloading;
    bool m_bUpdating;
};
#endif

#endif
