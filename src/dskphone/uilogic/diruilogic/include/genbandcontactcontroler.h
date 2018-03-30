#ifndef __GENBAND_CONTACT_CONTROLER_H__
#define __GENBAND_CONTACT_CONTROLER_H__

#include "commondirctrl.h"

#if IF_FEATURE_GBBOOK

enum
{
    GENBAND_DOWNLOAD_RESULT_SUCCESS = 1,
    GENBAND_DOWNLOAD_RESULT_XSI_PWD_ERROR = 401,
    GENBAND_DOWNLOAD_RESULT_SVR_URL_ERROR = 73
};

#include "precisesearchcontrollbase.h"

class CGenbandContactControler : public CPreciseSearchControlBase
{
public:
    CGenbandContactControler();
    virtual ~CGenbandContactControler();

public:
    static CGenbandContactControler * GetControler(CDirectoryControlerPtr & pControler);
    static ContactGBBookUnit * GetGenbandContact(SmartPtr & pDetail);

public:
    virtual void UnInit();
    virtual void OnEnterPreciseSearch();
    void UpdateContacts(int nGroupId);

    virtual bool PreLoadContact(int nGroupId);
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool CancelAction(int nAction, int nDataIndex, int nId);
    virtual LRESULT OnMessage(msgObject & refMessage);
    virtual int GetBrowseItemAmount();
    virtual bool OnBack();
    int GetSearchMode();
    virtual bool IsGroupData(SmartPtr & pDetail);
protected:
    virtual void PrepareDetailData();
    void UpdateSearchMode(int nAction);
    virtual bool PreciseSearch();
    void OnPreciseSearchResult(msgObject & refMessage);
    void ShowErrorResult(int nGroupId, int nResult);
    void RefreshAfterPabContactChange(msgObject & refMessage);
    void RefreshAfterPabContactChange();

    bool AddPabContact();
    bool DelPabContact(int nContactId);
    void OnDelPabContact(int nContactId);
    bool SavePabContact();
    bool CheckPersonalValid(ContactGBBookUnit & itemData);
    bool CheckChangePabContact(int nRetCode, int nAction);
    bool CheckDeletePersonal(int nRetCode);
    bool OnCopyToPab(bool bOverwrite);

    void OnEnterManagerGroup(int nAction);
    bool OnAddGroup();
    bool OnSaveGroup(int nGroupId);
    bool OnDelGroup(int nGroupId);
    bool DeleteGroup(int nGroupId);
protected:
    void AddDetailData(ContactGBBookUnit* pGenbandContact, const yl::string& strDialName, const yl::string& strNumberName);
    void ClearSearchData();
protected:
    bool m_bDownloading;
    bool m_bUpdating;
    int m_nDownloadGroupId;
    int m_nSearchMode;
    int m_nDialAccountID;
};
#endif

#endif
