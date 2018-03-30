#ifndef __BSFT_CONTACT_CONTROLER_H__
#define __BSFT_CONTACT_CONTROLER_H__

#include "precisesearchcontrollbase.h"

enum
{
    BSFT_DOWNLOAD_RESULT_SUCCESS = 1,
    BSFT_DOWNLOAD_RESULT_XSI_PWD_ERROR = 401,
    BSFT_DOWNLOAD_RESULT_SVR_URL_ERROR = 73
};

class CBSFTContactControler : public CPreciseSearchControlBase
{
public:
    CBSFTContactControler();
    virtual ~CBSFTContactControler();

public:
    static CBSFTContactControler * GetControler(CDirectoryControlerPtr & pControler);
    static ContactBroadSoftPersonal * GetPersonalContact(SmartPtr & pDetail);
    static bool IsExitPersonal();
    static bool IsPersonal(int nContactId);
    static bool CheckPersonalDataValid(ContactBroadSoftPersonal & itemData);
    static bool CheckNumberValid(const char * pszNumber, int nMaxLength = 30);
    static int GetGroupIdInAllGroup(int nIndex);
    static void GetAllGroup(VEC_CONTACT_ID & vecId);

public:
    virtual bool IsManualSearch() const;
    virtual void OnEnterPreciseSearch();
    void UpdateContacts(int nGroupId);
    virtual bool OnBack();
    bool CheckBackToMenuList();
    bool CheckBackToSearchResult();
    bool IsBackToMenuList();
    bool IsBackToSearchResult();

    bool OnDelPersonal(int nId);
    bool OnEnterCopyToPeronal(int nContactId);
    bool OnCopyToPersonal(bool bOverwrite);

    bool AddPersonal();
    bool DelPersonal(int nContactId);
    bool SavePersonal();

    bool IsContactChanged();
    bool IsCurrentPersonal();

    virtual bool PreciseSearch();
    bool PreciseSearch(const yl::string& strSearchKey);

    bool GetContactShowNumber(SmartPtr & pDetail, yl::string & strShowNumber);

    virtual void EnterState(int nState);
    virtual void LeaveState(int nState);
    virtual void OnEnterMenu();

    virtual bool IsGroupData(SmartPtr & pDetail);

    virtual bool IsCanEnterGroupByKey(int nKey);
    virtual bool PreLoadContact(int nGroupId);

    virtual bool PreSearchContact();
#if DIR_UI_SUPPORT_LOAD_DATA_LIST
    virtual bool LoadMenuData(int nDataIndex, int nDataCount);
#endif
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    virtual void RefreshTotalCount();
#endif
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    virtual bool CancelAction(int nAction, int nDataIndex, int nId);
    virtual LRESULT OnMessage(msgObject & refMessage);

    virtual int GetBrowseItemAmount();

    bool IsHasNextPageData();
    void CleanContactLoadInfo(int nGroupId);
    bool IsViewMoreId(int nIndex);

#if DIR_UI_SUPPORT_LOAD_DATA_LIST
// UIHelper
public:
    bool LoadPreciseData(int nDataIndex, int nDataCount);
#endif

protected:

    virtual void PrepareDetailData();
    bool PrepareCommonDetail(ContactBroadSoftCommon * pContact);
    bool PrepareEnterpriseDetail(ContactBroadSoftEnterprise * pContact);
    bool PrepareCustomDetail(ContactBroadSoftCustom * pContact);
    bool PrepareGroupDetail(ContactBroadSoftGroup * pContact);
    void AddDetailData(const yl::string& strName, const yl::string& strValue);
    void AddNumberDetailData(BaseContactRecord* pContact,
                             const yl::string& strName,
                             const yl::string& strNumberId);

    bool GetNumberByType(LIST_ATTRIBUTE & lsAttr, BaseContactRecord* pContact, bool bCopy = false);
    void AddNumberByName(LIST_ATTRIBUTE & lsAttr, BaseContactRecord*pContact,
                         const yl::string& strDispName, const yl::string& strNumberId, bool bCopy);

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

    void CheckSearchType();

    virtual int GetResultGroupId();
    bool OnPreciseSearchByData(ContactBroadSoftItemData& itemData, int nGroupId);
    void OnViewMoreContact();

protected:
    bool m_bDownloading;
    bool m_bUpdating;
    bool m_bBackToSearchResult;
    int m_nFailResult;
    int m_nDownloadGroupId;

    // 搜索类型（提供给逻辑层删除Personal）
    int m_nSearchType;

    int m_nPreciseSearchGroupId;
};

#endif
