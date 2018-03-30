#ifndef __GENBAND_CONTACT_CONTROLER_H__
#define __GENBAND_CONTACT_CONTROLER_H__

#include "commondirctrl.h"
#ifndef DIR_SUPPORT_SINGLE_UIHELPER
class CGenbandContactUIHelper;
#endif

#if IF_FEATURE_GBBOOK
enum
{
    GENBAND_DOWNLOAD_RESULT_SUCCESS     = 1,
    GENBAND_DOWNLOAD_RESULT_XSI_PWD_ERROR   = 401,
    GENBAND_DOWNLOAD_RESULT_SVR_URL_ERROR   = 73
};

class CGenbandContactControler : public CCommonDirCtrl
{
public:
    CGenbandContactControler();
    virtual ~CGenbandContactControler();

public:
    static CGenbandContactControler * GetControler(CDirectoryControlerPtr & pControler);

    static bool IsExitPabGroup();
    static bool IsPabContact(int nContactId);
    static bool CheckPabContactDataValid(ContactGBBookUnit & itemData);
    static bool CheckNumberValid(const char * pszNumber, int nMaxLength = 30);
    static void GetAllGroup(VEC_CONTACT_ID & vecId);
    static ContactGBBookUnit * GetGenbandContact(SmartPtr & pDetail);

    virtual void UnInit();

    enum
    {
        PRECISE_SEARCH_STATE_NONE       = 0,        // 非高级搜索状态
        PRECISE_SEARCH_STATE_ENTER      = 1,        // 进入高级搜索状态
        PRECISE_SEARCH_STATE_WAITING    = 2,        // 高级搜索等待中
        PRECISE_SEARCH_STATE_RESULT     = 3,        // 高级搜索结果返回
    };

public:
    void UpdateContacts(int nGroupId);
    bool OnBack();

    bool OnDelPabContact(int nId);
    bool OnEnterCopyToPabContact(int nAction, int nId);
    bool OnCopyToPab(bool bOverwrite);

    bool AddPabContact();
    bool DelPabContact(int nContactId);
    bool SavePabContact();


    bool OnAddGroup();
    bool OnSaveGroup(int nGroupId);
    bool AddGroup(yl::string strGroupName);
    bool SaveGroup(int nGroupId, yl::string strGroupName);
    bool OnDelGroup(int nGroupId);
    bool DeleteGroup(int nGroupId);

    bool IsContactChanged();

    bool IsPreciseSearchResult() const;
    void EnterPreciseSearch();
    int GetPreciseSearchState() const;
    void AdjustPreciseState(int nState);
    bool PreciseSearchByMode(GBBOOK_OPERATION_TYPE eMode);
    bool PreciseSearch();

    virtual bool PreLoadContact(int nGroupId);
    virtual bool PreSearchContact();
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    virtual bool CancelAction(int nAction, int nDataIndex, int nId);
    virtual LRESULT OnMessage(msgObject & refMessage);

// UIHelper
public:
    CGenbandContactUIHelper * GetGenbandUIHelper();
    bool LoadPreciseData(int nDataIndex, int nDataCount);

protected:
    bool GetGenbandNumber(LIST_ATTRIBUTE & lsAttr, ContactGBBookUnit * pGroup);

    bool CheckPersonalValid(ContactGBBookUnit & itemData, bool bIsAddContact = false);
    bool CheckChangePabContact(int nRetCode, int nAction);
    bool CheckDeletePersonal(int nRetCode);
    void OnPreciseSearchResult(msgObject & refMessage);
    void RefreshAfterPabContactChange(msgObject & refMessage);
    void RefreshAfterPabContactChange();
    void ShowErrorResult(int nGroupId, int nResult);
    bool IsResultSuccess(int nResult)
    {
        return (GENBAND_DOWNLOAD_RESULT_SUCCESS == nResult);
    };
    bool IsSvrURLInvalid(int nResult)
    {
        return (GENBAND_DOWNLOAD_RESULT_SVR_URL_ERROR == nResult);
    };

protected:
    bool m_bDownloading;
    bool m_bUpdating;
    int m_nDownloadGroupId;

    // 高级搜索状态
    int m_nPreciseSearchState;

};
#endif

#endif
