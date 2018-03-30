#ifndef __BLUETOOTH_CONTACT_CONTROLER_H__
#define __BLUETOOTH_CONTACT_CONTROLER_H__

#include "commondirctrl.h"

#ifndef DIR_SUPPORT_SINGLE_UIHELPER
class CBluetoothUIHelper;
#endif

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

    virtual bool PreLoadContact(int nGroupId);
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool CancelAction(int nAction, int nDataIndex, int nId);
    virtual LRESULT OnMessage(msgObject & refMessage);

public:
    CBluetoothUIHelper * GetBluetoothUIHelper();

protected:
    void GetNumberAttr(LocalDirContactRecord * pRecord, LIST_ATTRIBUTE & lsNumber, bool bCopy);
    void OnCopyContact(int nAction, int nId);
    void OnDownLoadFinished(msgObject & refMessage);
#ifdef DIR_SUPPORT_BATCH_OPERATE
    bool CopyToLocalContact(int nGroupId, VEC_CONTACT_ID & vecContactId);
#endif

protected:
    bool m_bDownloading;
    bool m_bUpdating;
};

#endif
