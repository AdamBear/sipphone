#ifndef __MTSW_CONTACT_CONTROLER_H__
#define __MTSW_CONTACT_CONTROLER_H__

#if IF_FEATURE_METASWITCH_DIRECTORY
#include "commondirctrl.h"

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
    static ContactMetaSwitchContactList* GetMetaSwitchContact(SmartPtr & pDetail);
public:
    static bool CheckNumberValid(const char * pszNumber, int nMaxLength = 32);

    static int GetGroupIdInAllGroup(int nIndex);

    static void GetAllGroup(VEC_CONTACT_ID & vecId);

public:
    void UpdateContacts(int nGroupId);

    bool IsContactCanEdit(int nContactID);

    bool OnDelContactList(int nId);
    bool OnClearContact(int nId);
    bool OnEnterCopyToContactList(int nAction, int nId);

    bool AddContactList(bool bFouceAdd = false);
    bool DelContactList(int nContactId);
    bool ClearContactList(int nGroupId);
    bool SaveContactList(bool bFouceEdit = false);

    bool SortContactList();
    bool IsContactList(int nContactId);
    bool IsCurrentContactList();

    virtual int GetBrowseItemAmount();
    virtual bool IsGroupData(SmartPtr & pDetail);
    virtual bool PreLoadContact(int nGroupId);
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    virtual bool CancelAction(int nAction, int nDataIndex, int nId);
    virtual LRESULT OnMessage(msgObject & refMessage);
    void OnLoginSuccess();
protected:
    virtual void PrepareDetailData();
    void PrepareContactlistDetail(ContactMetaSwitchContactList * pContact);
    void PrepareExtentionDetail(ContactMetaSwitchExtention * pContact);
    void PrepareMLHGDetail(ContactMetaSwitchMLHG * pContact);
    void AddDetailData(ContactMetaSwitchContactList* pContact, const yl::string& strDialName, const yl::string& strNumberName);
    void GetNumberByName(BaseContactRecord* pContact, LIST_ATTRIBUTE & lsAttr
                         , const yl::string& strName, const yl::string& strDispName);

    bool CheckContactListValid(ContactMetaSwitchContactList& itemData);
    bool CheckNumberValidByName(ContactMetaSwitchContactList& itemData, const yl::string& strName
                                , const yl::string& strDefaultError, yl::string& strError);
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

protected:
    bool m_bDownloading;
    bool m_bUpdating;
    int m_nFailResult;
    int m_nDownloadGroupId;
};

#endif //__MTSW_CONTACT_CONTROLER_H__

#endif//#if IF_FEATURE_METASWITCH_DIRECTORY
