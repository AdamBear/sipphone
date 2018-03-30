#ifndef __BSFT_CONTACT_CONTROLER_H__
#define __BSFT_CONTACT_CONTROLER_H__

#include "commondirctrl.h"
#ifndef DIR_SUPPORT_SINGLE_UIHELPER
class CBSFTContactUIHelper;
#endif


enum
{
    BSFT_DOWNLOAD_RESULT_SUCCESS        = 1,
    BSFT_DOWNLOAD_RESULT_XSI_PWD_ERROR  = 401,
    BSFT_DOWNLOAD_RESULT_SVR_URL_ERROR  = 73
};

class CBSFTContactControler : public CCommonDirCtrl
{
public:
    CBSFTContactControler();
    virtual ~CBSFTContactControler();

public:
    static CBSFTContactControler * GetControler(CDirectoryControlerPtr & pControler);
    static bool IsExitPersonal();
    static bool IsPersonal(int nContactId);
    static bool CheckPersonalDataValid(ContactBroadSoftPersonal & itemData);
    static bool CheckNumberValid(const char * pszNumber, int nMaxLength = 30);
    static int GetGroupIdInAllGroup(int nIndex);
    static void GetAllGroup(VEC_CONTACT_ID & vecId);

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

    bool OnDelPersonal(int nId);
    bool OnEnterCopyToPeronal(int nAction, int nId);
    bool OnCopyToPersonal(bool bOverwrite);

    bool AddPersonal();
    bool DelPersonal(int nContactId);
    bool SavePersonal();

    bool IsContactChanged();
    bool IsCurrentPersonal();

    bool IsPreciseSearchResult() const;
    void EnterPreciseSearch();
    int GetPreciseSearchState() const;
    void AdjustPreciseState(int nState);
    bool PreciseSearch();

    virtual bool LoadMore();
    virtual void PreLoadContactOnUIReturn(int nGroupId);
    virtual bool PreLoadContact(int nGroupId);
    virtual bool PreSearchContact();
    virtual bool LoadMenuData(int nDataIndex, int nDataCount);
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    virtual void RefreshTotalCount();
#endif
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    virtual bool CancelAction(int nAction, int nDataIndex, int nId);
    virtual LRESULT OnMessage(msgObject & refMessage);
    virtual void GetDetailInfo(int & nId, int & nIndex, int & nCount) const;
    virtual int GetLogicListGroupId();

// UIHelper
public:
    CBSFTContactUIHelper * GetBSFTUIHelper();
    bool LoadPreciseData(int nDataIndex, int nDataCount);
    void SetNeedReLoad(bool bIsReload);

protected:
    bool GetCommonNumber(LIST_ATTRIBUTE & lsAttr, ContactBroadSoftCommon * pCommon, bool bCopy = false);
    bool GetEnterpriseNumber(LIST_ATTRIBUTE & lsAttr, ContactBroadSoftEnterprise * pEnterprise,
                             bool bCopy = false);
    bool GetCustomNumber(LIST_ATTRIBUTE & lsAttr, ContactBroadSoftCustom * pCustom, bool bCopy = false);
    bool GetGroupNumber(LIST_ATTRIBUTE & lsAttr, ContactBroadSoftGroup * pGroup, bool bCopy = false);
    bool GetPersonalNumber(LIST_ATTRIBUTE & lsAttr, ContactBroadSoftPersonal * pPersonal);
#ifdef DIR_SUPPORT_BATCH_OPERATE
    bool GetContactCopyInfo(SmartPtr & pDetail, yl::string & strName, yl::string & strNumber);
#endif
    bool CheckPersonalValid(ContactBroadSoftPersonal & itemData);
    bool CheckChangePersonal(int nRetCode, int nAction);
    bool CheckDeletePersonal(int nRetCode);
    void OnDownLoadFinished(msgObject & refMessage);
    void OnPreciseSearchResult(msgObject & refMessage);
    void RefreshAfterPersonalChange();
    void ShowErrorResult(int nGroupId, int nResult);
    void ResetFailResult()
    {
        m_nFailResult = BSFT_DOWNLOAD_RESULT_SUCCESS;
    }
    bool IsResultSuccess(int nResult)
    {
        return (BSFT_DOWNLOAD_RESULT_SUCCESS == nResult);
    };
    bool IsXsiPwdError(int nResult)
    {
        return (BSFT_DOWNLOAD_RESULT_XSI_PWD_ERROR == nResult);
    };
    bool IsSvrURLInvalid(int nResult)
    {
        return (BSFT_DOWNLOAD_RESULT_SVR_URL_ERROR == nResult);
    };

    void RefreshAfterDownload(int nGroupId);

protected:
    bool m_bDownloading;
    bool m_bUpdating;
    int m_nFailResult;
    int m_nDownloadGroupId;
    int m_nPreciseSearchGroupId;
    // 是否需要重新下载
    bool m_bIsReloadContact;
    // 高级搜索状态
    int m_nPreciseSearchState;
};

#endif
