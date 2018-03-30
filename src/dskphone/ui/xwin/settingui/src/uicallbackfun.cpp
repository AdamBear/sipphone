#include "uicallbackfun.h"

IMPLEMENT_GETINSTANCE(CUICallBackFun)

CUICallBackFun::CUICallBackFun()
    : m_pFunRefreshUI(NULL)
    , m_pFunCacheDlgData(NULL)
    , m_pFunRefresSoftkey(NULL)
    , m_pFunRefreshDlgTitle(NULL)
    , m_pFunFallBackToPrevousPage(NULL)
    , m_pFunBackToIdle(NULL)
    , m_pFunDropOutCurrentPage(NULL)
    , m_pFunDropOutSettingUI(NULL)
    , m_pFunEnterPage(NULL)
    , m_pFunBindDelegate(NULL)
    , m_pFunUnBindDelegate(NULL)
    , m_pDealWithValidData(NULL)
    , m_pFunSetSubTileActive(NULL)
{
}

CUICallBackFun::~CUICallBackFun()
{
}

void CUICallBackFun::SetFunRefreshUI(Fun_RefreshUI pRefreshUI)
{
    m_pFunRefreshUI = pRefreshUI;
}

void CUICallBackFun::SetFunCacheDlgData(Fun_CacheDlgData pCacheDlgData)
{
    m_pFunCacheDlgData = pCacheDlgData;
}

void CUICallBackFun::SetFunRefreshSoftkey(Fun_RefreshSoftkey pRefreshSoftkey)
{
    m_pFunRefresSoftkey = pRefreshSoftkey;
}

void CUICallBackFun::SetFunRefreshDlgTitle(Fun_RefreshDlgTitle pRefreshDlgTitle)
{
    m_pFunRefreshDlgTitle = pRefreshDlgTitle;
}

void CUICallBackFun::SetFunFallBackToPreviousPage(Fun_FallBackToPrevousPage pFallBackToPrevousPage)
{
    m_pFunFallBackToPrevousPage = pFallBackToPrevousPage;
}

void CUICallBackFun::SetFunBackToIdle(Fun_BackToIdle pBackToIdle)
{
    m_pFunBackToIdle = pBackToIdle;
}

void CUICallBackFun::SetFunDropOutCurrentPage(Fun_DropOutCurrentPage pDropOutCurrentPage)
{
    m_pFunDropOutCurrentPage = pDropOutCurrentPage;
}

void CUICallBackFun::SetFunDropOutSettingUI(Fun_DropOutSettingUI pDropOutSettingUI)
{
    m_pFunDropOutSettingUI = pDropOutSettingUI;
}

void CUICallBackFun::SetFunEnterPage(Fun_EnterPage pEnterPage)
{
    m_pFunEnterPage = pEnterPage;
}

void CUICallBackFun::SetFunBindDelegate(Fun_BindDelegate pBindDelegate)
{
    m_pFunBindDelegate = pBindDelegate;
}

void CUICallBackFun::SetFunUnBindDelegate(Fun_UnBindDelegate pUnBindDelegate)
{
    m_pFunUnBindDelegate = pUnBindDelegate;
}

void CUICallBackFun::SetFunPopCommonMessageBox(Fun_PopCommonMessageBox pPopCommonMessageBox)
{
    m_pPopCommonMessageBox = pPopCommonMessageBox;
}

void CUICallBackFun::SetDealWithValidData(Fun_DealWithValidData pDealWithValidData)
{
    m_pDealWithValidData = pDealWithValidData;
}

void CUICallBackFun::RefreshUI(bool bLoadData /*= false*/, const yl::string & strPageId /*= ""*/)
{
    if (NULL != m_pFunRefreshUI)
    {
        (*m_pFunRefreshUI)(bLoadData, strPageId);
    }
}

void CUICallBackFun::CacheDlgData(const yl::string & strPageId /*= ""*/)
{
    if (NULL != m_pFunCacheDlgData)
    {
        (*m_pFunCacheDlgData)(strPageId);
    }
}

void CUICallBackFun::RefreshSoftkey(const yl::string & strPageId/* = ""*/)
{
    if (NULL != m_pFunRefresSoftkey)
    {
        (*m_pFunRefresSoftkey)(strPageId);
    }
}

void CUICallBackFun::RefreshDlgTitle(const yl::string & strTitle,
                                     const yl::string & strPageId /*= ""*/)
{
    if (NULL != m_pFunRefreshDlgTitle)
    {
        (*m_pFunRefreshDlgTitle)(strTitle, strPageId);
    }
}

void CUICallBackFun::FallBackToPrevousPage(const yl::string & strPageId /*= ""*/)
{
    if (NULL != m_pFunFallBackToPrevousPage)
    {
        (*m_pFunFallBackToPrevousPage)(strPageId);
    }
}

void CUICallBackFun::BackToIdle()
{
    if (NULL != m_pFunBackToIdle)
    {
        (*m_pFunBackToIdle)();
    }
}

void CUICallBackFun::DropOutCurrentPage(bool bDelDlg /*= true*/,
                                        const yl::string & strPageId /*= ""*/)
{
    if (NULL != m_pFunDropOutCurrentPage)
    {
        (*m_pFunDropOutCurrentPage)(bDelDlg, strPageId);
    }
}

void CUICallBackFun::DropOutSettingUI(const yl::string & strPageId /*= ""*/)
{
    if (NULL != m_pFunDropOutSettingUI)
    {
        (*m_pFunDropOutSettingUI)(strPageId);
    }
}

void CUICallBackFun::EnterPage(const PageOpenData & openPageData)
{
    if (NULL != m_pFunEnterPage)
    {
        (*m_pFunEnterPage)(openPageData);
    }
}

void CUICallBackFun::BindDelegate(CSettingUIDelegateBasePtr pSetDelegate,
                                  const yl::string & strPageId /*= ""*/)
{
    if (NULL != m_pFunBindDelegate)
    {
        (*m_pFunBindDelegate)(pSetDelegate, strPageId);
    }
}

void CUICallBackFun::UnBindDelegate(const yl::string & strPageId /*= ""*/)
{
    if (NULL != m_pFunUnBindDelegate)
    {
        (*m_pFunUnBindDelegate)(strPageId);
    }
}

void CUICallBackFun::PopCommonMessageBox(const yl::string & strPageId, const yl::string & strTitle,
        const yl::string & strNote,
        int eType, const yl::string & strCancelBtnTitle /*= ""*/,
        const yl::string & strConfirmBtnTitle /*= ""*/, int nMsTime /*= -1*/,
        int iID /*= -1*/, void * pExtraData /*= NULL*/,
        CMsgBoxCallBackAction * pAction /*= NULL*/)
{
    if (NULL != m_pPopCommonMessageBox)
    {
        (*m_pPopCommonMessageBox)(strPageId, strTitle, strNote, eType, strCancelBtnTitle,
                                  strConfirmBtnTitle, nMsTime, iID, pExtraData, pAction);
    }
}

void CUICallBackFun::DealWithValidData(CSettingUIDataValidInfo & dataValid,
                                       const yl::string & strPageId /*= ""*/)
{
    if (NULL != m_pDealWithValidData)
    {
        (*m_pDealWithValidData)(dataValid, strPageId);
    }
}

