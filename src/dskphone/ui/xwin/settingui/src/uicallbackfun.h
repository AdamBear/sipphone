#ifndef __UI_CALL_BACK_FUN_H__
#define __UI_CALL_BACK_FUN_H__

#include "singletonclassdefine.h"
#include "uicallbackfundefine.h"

class CUICallBackFun
{
    DECLARE_SINGLETON_CLASS(CUICallBackFun)

public:
    void SetFunRefreshUI(Fun_RefreshUI pRefreshUI);

    void SetFunCacheDlgData(Fun_CacheDlgData pCacheDlgData);

    void SetFunRefreshSoftkey(Fun_RefreshSoftkey pRefreshSoftkey);

    void SetFunRefreshDlgTitle(Fun_RefreshDlgTitle pRefreshDlgTitle);

    void SetFunFallBackToPreviousPage(Fun_FallBackToPrevousPage pFallBackToPrevousPage);

    void SetFunBackToIdle(Fun_BackToIdle pBackToIdle);

    void SetFunDropOutCurrentPage(Fun_DropOutCurrentPage pDropOutCurrentPage);

    void SetFunDropOutSettingUI(Fun_DropOutSettingUI pDropOutSettingUI);

    void SetFunEnterPage(Fun_EnterPage pEnterPage);

    void SetFunBindDelegate(Fun_BindDelegate pBindDelegate);

    void SetFunUnBindDelegate(Fun_UnBindDelegate pUnBindDelegate);

    void SetFunPopCommonMessageBox(Fun_PopCommonMessageBox pPopCommonMessageBox);

    void SetDealWithValidData(Fun_DealWithValidData pDealWithValidData);

public:
    void RefreshUI(bool bLoadData = false, const yl::string & strPageId = "");

    void CacheDlgData(const yl::string & strPageId = "");

    void RefreshSoftkey(const yl::string & strPageId = "");

    void RefreshDlgTitle(const yl::string & strTitle, const yl::string & strPageId = "");

    void FallBackToPrevousPage(const yl::string & strPageId = "");

    void BackToIdle();

    void DropOutCurrentPage(bool bDelDlg = true, const yl::string & strPageId = "");

    void DropOutSettingUI(const yl::string & strPageId = "");

    void EnterPage(const PageOpenData & openPageData);

    void BindDelegate(CSettingUIDelegateBasePtr pSetDelegate, const yl::string & strPageId = "");

    void UnBindDelegate(const yl::string & strPageId = "");

    void PopCommonMessageBox(const yl::string & strPageId, const yl::string & strTitle,
                             const yl::string & strNote,
                             int eType, const yl::string & strCancelBtnTitle = "",
                             const yl::string & strConfirmBtnTitle = "", int nMsTime = -1,
                             int iID = -1, void * pExtraData = NULL, CMsgBoxCallBackAction * pAction = NULL);

    void DealWithValidData(CSettingUIDataValidInfo & dataValid, const yl::string & strPageId = "");

protected:
    Fun_RefreshUI           m_pFunRefreshUI;
    Fun_CacheDlgData        m_pFunCacheDlgData;
    Fun_RefreshSoftkey      m_pFunRefresSoftkey;
    Fun_RefreshDlgTitle         m_pFunRefreshDlgTitle;
    Fun_FallBackToPrevousPage m_pFunFallBackToPrevousPage;
    Fun_BackToIdle          m_pFunBackToIdle;
    Fun_DropOutCurrentPage  m_pFunDropOutCurrentPage;
    Fun_DropOutSettingUI    m_pFunDropOutSettingUI;
    Fun_EnterPage           m_pFunEnterPage;
    Fun_BindDelegate        m_pFunBindDelegate;
    Fun_UnBindDelegate      m_pFunUnBindDelegate;
    Fun_PopCommonMessageBox m_pPopCommonMessageBox;
    Fun_DealWithValidData   m_pDealWithValidData;
    Fun_SetSubTileActive    m_pFunSetSubTileActive;
};

#define g_SetUICallBackFun GET_SINGLETON_INSTANCE(CUICallBackFun)

#endif

