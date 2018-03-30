#if IF_FEATURE_METASWITCH_DIRECTORY
#ifndef __MTSW_CONTACT_CONTROLER_H__
#define __MTSW_CONTACT_CONTROLER_H__

#include "commondirctrl.h"

class CMTSWContactUIHelper;


enum
{
    MTSW_DOWNLOAD_RESULT_SUCCESS        = 1,
    MTSW_DOWNLOAD_RESULT_XSI_PWD_ERROR  = 401,
    MTSW_DOWNLOAD_RESULT_SVR_URL_ERROR  = 73
};

class CMTSWContactControler : public CCommonDirCtrl
{
public:
    CMTSWContactControler();
    virtual ~CMTSWContactControler();

public:

    static CMTSWContactControler * GetControler(CDirectoryControlerPtr & pControler);

    static bool CheckContactListDataValid(ContactMetaSwitchContactList & itemData);

    static bool CheckNumberValid(const char * pszNumber, int nMaxLength = 32);

    static int GetGroupIdInAllGroup(int nIndex);

    static void GetAllGroup(VEC_CONTACT_ID & vecId);

public:
    void UpdateContacts(int nGroupId);
    bool OnBack();

    bool OnDelContactList(int nId);
    bool OnClearContact(int nId);
    bool OnEnterCopyToContactList(int nAction, int nId);

    bool AddContactList(bool bFouceAdd = false);
    bool DelContactList(int nContactId);
    bool ClearContactList(int nGroupId);
    bool SaveContactList(bool bFouceEdit = false);

    bool SortContactList();

    bool IsContactChanged();
    bool IsContactList(int nContactId);
    bool IsCurrentContactList();

    void AddCurrentIdBeforeEnterPage();


    virtual bool PreLoadContact(int nGroupId);
    virtual bool PreSearchContact();
    virtual bool LoadMenuData(int nDataIndex, int nDataCount);
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    virtual bool CancelAction(int nAction, int nDataIndex, int nId);
    virtual LRESULT OnMessage(msgObject & refMessage);

public:
    CMTSWContactUIHelper * GetMTSWUIHelper();

protected:
    bool GetContactListNumber(LIST_ATTRIBUTE & lsAttr, ContactMetaSwitchContactList * pContactList);
    bool GetExtentionNumber(LIST_ATTRIBUTE & lsAttr, ContactMetaSwitchExtention * pExtention);
    bool GetMLHGNumber(LIST_ATTRIBUTE & lsAttr, ContactMetaSwitchMLHG * pMLHG);

    bool CheckContactListValid(ContactMetaSwitchContactList & itemData);
    bool CheckChangeContactList(int nRetCode, int nAction);
    bool CheckDeleteContactList(int nRetCode);
    void OnDownLoadFinished(msgObject & refMessage);

    void RefreshAfterContactListChange();
    void ShowErrorResult(int nGroupId, int nResult);
    void ResetFailResult()
    {
        m_nFailResult = MTSW_DOWNLOAD_RESULT_SUCCESS;
    }
    bool IsResultSuccess(int nResult)
    {
        return (MTSW_DOWNLOAD_RESULT_SUCCESS == nResult);
    };
    bool IsXsiPwdError(int nResult)
    {
        return (MTSW_DOWNLOAD_RESULT_XSI_PWD_ERROR == nResult);
    };
    bool IsSvrURLInvalid(int nResult)
    {
        return (MTSW_DOWNLOAD_RESULT_SVR_URL_ERROR == nResult);
    };

#ifdef DIR_SUPPORT_BATCH_OPERATE
    bool CopyToLocalContact(int nGroupId, VEC_CONTACT_ID & vecContactId);
#endif

protected:
    bool m_bDownloading;
    bool m_bUpdating;
    int m_nFailResult;
    int m_nDownloadGroupId;
    bool m_bAddContact;

    YLList<int> m_listPerID;

};

#endif //__MTSW_CONTACT_CONTROLER_H__

#endif //IF_FEATURES_METASWITCH_DIRECTORY
